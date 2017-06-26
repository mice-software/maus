/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"


#include <iostream>
#include <sstream>

#include "src/common_cpp/Utils/Exception.hh"

#include "TDecompLU.h"


namespace MAUS {
namespace Kalman {

  std::string print_state(const State& state, const char* detail) {
    std::ostringstream converter("");
    TMatrixD vec = state.GetVector();
    TMatrixD cov = state.GetCovariance();

    if (detail)
      converter << detail;
    converter << " ";
    for (unsigned int i = 0; i < state.GetDimension(); ++i) {
      converter << "(" << vec(i, 0) << " +/- " << sqrt(cov(i, i)) << "), ";
    }
    converter << "\n";
    return converter.str();
  }


  std::string print_trackpoint(const TrackPoint& tp, const char* detail) {
    std::ostringstream converter("");

    if (detail) {
      converter << detail << " @ ";
    } else {
      converter << "TrackPoint @ ";
    }
    converter << tp.GetPosition() << ", " << tp.GetId() << '\n';
    if (tp.HasData()) {
      converter << print_state(tp.GetData(), "DATA");
    } else {
      converter << "NO DATA\n";
    }

    converter << print_state(tp.GetPredicted(), "Predicted");
    converter << print_state(tp.GetFiltered(), "Filtered ");
    converter << print_state(tp.GetSmoothed(), "Smoothed ") << '\n';

    return converter.str();
  }


  std::string print_track(const Track& track, const char* name) {
    unsigned int num = track.GetLength();
    std::ostringstream converter("");
    if (name) {
      converter << "Printing " << name << " (" << num << ")\n";
    } else {
      converter << "Printing Track (" << num << ")\n";
    }

    for (unsigned int i = 0; i < num; ++i) {
      TrackPoint tp = track[i];
      converter << print_trackpoint(tp);
    }
    converter << "\n";
    return converter.str();
  }

  State CalculateResidual(const State& st1, const State& st2) {
    if (st1._dimension != st2._dimension) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "States have different dimensions",
          "Kalman::CalculateResidual()");
    }
    TMatrixD vector = st1._vector - st2._vector;
    TMatrixD covariance = st1._covariance + st2._covariance;

    State residual(vector, covariance);

    return residual;
  }

  State CalculateFilteredResidual(const State& data, const State& measured) {
    if (data._dimension != measured._dimension) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "States have different dimensions",
          "Kalman::CalculateFilteredResidual()");
    }
    TMatrixD vector = data._vector - measured._vector;
    TMatrixD covariance = data._covariance - measured._covariance;

    State residual(vector, covariance);

    return residual;
  }

  State CalculateChiSquaredResidual(const State& data, const State& measured) {
    if (data._dimension != measured._dimension) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "States have different dimensions",
          "Kalman::CalculateFilteredResidual()");
    }
    TMatrixD vector = data._vector - measured._vector;
    State residual(vector, data._covariance);
    return residual;
  }

  double CalculateChiSquaredUpdate(const State& st) {
    TMatrixD rT(TMatrixD::kTransposed, st._vector);// rT.Transpose(r);
    TMatrixD RI(TMatrixD::kInverted, st._covariance);
    TMatrixD update = rT * RI * st._vector;
    return update(0, 0);
  }



  TrackFit::TrackFit(Propagator_base* prop) :
    _dimension(prop->GetDimension()),
    _propagator(prop),
    _measurements(),
    _seed(_dimension),
    _track(_dimension),
    _identity_matrix(_dimension, _dimension) {

    _identity_matrix.Zero();
    for (unsigned int i = 0; i < _dimension; ++i)
      _identity_matrix(i, i) = 1.0;
  }


  TrackFit::~TrackFit() {
    if (_propagator) {
      delete _propagator;
      _propagator = NULL;
    }
    for (std::map<int, Measurement_base*>::iterator it = _measurements.begin();
                                                                it != _measurements.end(); ++it ) {
      if (it->second) {
        delete it->second;
      }
    }
    _measurements.clear();
  }


  void TrackFit::AppendFilter(TrackPoint tp) {
    _track.Append(tp);
    int length = _track.GetLength();

    if (length == 1) {
      _track[0].SetPredicted(_seed);
    } else {
      this->_propagate(_track[length-2], _track[length-1]);
    }

    this->_filter(_track[length-1]);
  }


  void TrackFit::Filter(bool forward) {
    int increment = (forward ? 1 : -1);
    int track_start = (forward ? 1 : _track.GetLength() - 2);
    int track_end = (forward ? _track.GetLength() : -1);

    _track[track_start-increment].SetPredicted(_seed);

    this->_filter(_track[track_start-increment]);

    for (int i = track_start; i != track_end; i += increment) {
      this->_propagate(_track[i-increment], _track[i]);
      this->_filter(_track[i]);
    }
  }


  void TrackFit::Smooth(bool forward) {
    int increment = (forward ? -1 : 1);
    int track_start = (forward ? (_track.GetLength() - 2) : 1);
    int track_end = (forward ? -1 : (_track.GetLength()));

    _track[track_start-increment].SetSmoothed(_track[track_start-increment].GetFiltered());

    for (int i = track_start; i != track_end; i += increment) {
      TMatrixD inv_cov(TMatrixD::kInverted, _track[i - increment].GetPredicted().GetCovariance());
      TMatrixD prop = _propagator->CalculatePropagator(_track[i], _track[i - increment]);
      TMatrixD propT(TMatrixD::kTransposed, prop);

      TMatrixD A = _track[i].GetFiltered().GetCovariance() * propT * inv_cov;
      TMatrixD AT(TMatrixD::kTransposed, A);

      TMatrixD vec = _track[i].GetFiltered().GetVector() + A *
                                                   (_track[i-increment].GetSmoothed().GetVector() -
                                                   _track[i-increment].GetPredicted().GetVector());

      TMatrixD temp = A * (_track[i-increment].GetSmoothed().GetCovariance() -
                                          _track[i-increment].GetPredicted().GetCovariance()) * AT;
      TMatrixD cov = _track[i].GetFiltered().GetCovariance() + temp;

      _track[i].SetSmoothed(State(vec, cov));
    }
  }


  void TrackFit::SetSeed(State seed) {
    if (seed.GetDimension() != _dimension) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Seed dimension does not match the track fitter",
          "Kalman::TrackFit::SetSeed()");
    }
    _seed = seed;
  }

  void TrackFit::SetTrack(Track track) {
    if (track.GetDimension() != _dimension) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Track dimension does not match the track fitter",
          "Kalman::TrackFit::SetTrack()");
    }
    _track = track;
  }



//  void TrackFit::_propagate(State& first, State& second) const {
  void TrackFit::_propagate(const TrackPoint& first, TrackPoint& second) const {
    _propagator->Propagate(first, second);
  }

//  void TrackFit::_filter(const State& data, const State& predicted, State& filtered) const {
  void TrackFit::_filter(TrackPoint& tp) const {
    if (tp.HasData()) {
      const State& data = tp.GetData();
      const State& predicted = tp.GetPredicted();

      State measured = _measurements.at(tp.GetId())->Measure(predicted);
      State pull = CalculateResidual(data, measured);
      TMatrixD V = data.GetCovariance();
      TMatrixD H = _measurements.at(tp.GetId())->GetMeasurementMatrix();
      TMatrixD HT(TMatrixD::kTransposed, H);

      TMatrixD cov_inv(TMatrixD::kInverted, pull.GetCovariance());

      TMatrixD K = predicted.GetCovariance() * HT * cov_inv;
      TMatrixD KT(TMatrixD::kTransposed, K);

      TMatrixD gain = _identity_matrix - K*H;
      TMatrixD gainT(TMatrixD::kTransposed, gain);
      TMatrixD gain_constant = K * V * KT;

      TMatrixD temp_vec = predicted.GetVector() + K * pull.GetVector();
      TMatrixD temp_cov = gain * predicted.GetCovariance() * gainT + gain_constant;

      tp.SetFiltered(State(temp_vec, temp_cov));
    } else {
      tp.SetFiltered(tp.GetPredicted());
    }
  }

  void TrackFit::_smooth(TrackPoint& first, TrackPoint& second) const {
  }


  State TrackFit::_inverse_filter(const TrackPoint& tp) const {
    State data = tp.GetData();
    const State& smoothed = tp.GetSmoothed();
    State cleaned = State(_dimension);

    // THIS IS THE INVERSE FILTER LINE!
    data.SetCovariance(-1.0*data.GetCovariance());

    State measured = _measurements.at(tp.GetId())->Measure(smoothed);
    State pull = CalculateResidual(data, measured);
    TMatrixD V = data.GetCovariance();
    TMatrixD H = _measurements.at(tp.GetId())->GetMeasurementMatrix();
    TMatrixD HT(TMatrixD::kTransposed, H);

    TMatrixD cov_inv(TMatrixD::kInverted, pull.GetCovariance());

//    // THIS IS THE INVERSE FILTER LINE!
//    V *= -1.0;

    TMatrixD K = smoothed.GetCovariance() * HT * cov_inv;
    TMatrixD KT(TMatrixD::kTransposed, K);

    TMatrixD gain = _identity_matrix - K*H;
    TMatrixD gainT(TMatrixD::kTransposed, gain);
    TMatrixD gain_constant = K * V * KT;

    TMatrixD temp_vec = smoothed.GetVector() + K * pull.GetVector();
    TMatrixD temp_cov = gain * smoothed.GetCovariance() * gainT + gain_constant;

    return State(temp_vec, temp_cov);
  }


  double TrackFit::CalculateSmoothedChiSquared() const {
    double chi_squared = 0.0;
    for (unsigned int i = 0; i < _track.GetLength(); ++i) {
      if (_track[i].HasData()) {
        State measured = _measurements.at(_track[i].GetId())->Measure(_track[i].GetSmoothed());
        State chisq = Kalman::CalculateFilteredResidual(_track[i].GetData(), measured);
        double update = CalculateChiSquaredUpdate(chisq);
        chi_squared += update;
      }
    }
    return chi_squared;
  }

  double TrackFit::CalculateChiSquared() const {
    double chi_squared = 0.0;
    for (unsigned int i = 0; i < _track.GetLength(); ++i) {
      if (_track[i].HasData()) {
        State measured = _measurements.at(_track[i].GetId())->Measure(_track[i].GetSmoothed());
        State chisq = Kalman::CalculateChiSquaredResidual(_track[i].GetData(), measured);
        double update = CalculateChiSquaredUpdate(chisq);
        chi_squared += update;
      }
    }
    return chi_squared;
  }

  double TrackFit::CalculatePullChiSquared() const {
    double chi_squared = 0.0;
    for (unsigned int i = 0; i < _track.GetLength(); ++i) {
      if (_track[i].HasData()) {
        State chisq = this->CalculatePull(i);
        double update = CalculateChiSquaredUpdate(chisq);
        chi_squared += update;
      }
    }
    return chi_squared;
  }


  int TrackFit::GetNDF() const {
    int no_parameters = GetDimension();
    int no_measurements = 0;
    for (unsigned int i = 0; i < _track.GetLength(); ++i) {
      if (_track[i].HasData()) {
        no_measurements += _measurements.at(_track[i].GetId())->GetMeasurementDimension();
      }
    }
    return (no_measurements - no_parameters);
  }

  int TrackFit::GetNumberMeasurements() const {
    int no_measurements = 0;
    for (unsigned int i = 0; i < _track.GetLength(); ++i) {
      if (_track[i].HasData()) {
        no_measurements += _measurements.at(_track[i].GetId())->GetMeasurementDimension();
      }
    }
    return no_measurements;
  }

  State TrackFit::CalculateCleanedState(unsigned int i) const {
    return this->_inverse_filter(_track[i]);
  }

  State TrackFit::CalculatePull(unsigned int i) const {
    State cleaned = this->CalculateCleanedState(i);
    State measured = _measurements.at(_track[i].GetId())->Measure(cleaned);
    return Kalman::CalculateResidual(_track[i].GetData(), measured);
//    return Kalman::CalculateChiSquaredResidual(_track[i].GetData(), measured);
  }

  State TrackFit::CalculatePredictedResidual(unsigned int i) const {
    State measured = _measurements.at(_track[i].GetId())->Measure(_track[i].GetPredicted());
    return CalculateResidual(measured, _track[i].GetData());
  }

  State TrackFit::CalculateFilteredResidual(unsigned int i) const {
    State measured = _measurements.at(_track[i].GetId())->Measure(_track[i].GetFiltered());
    return Kalman::CalculateFilteredResidual(_track[i].GetData(), measured);
  }

  State TrackFit::CalculateSmoothedResidual(unsigned int i) const {
    State measured = _measurements.at(_track[i].GetId())->Measure(_track[i].GetSmoothed());
    return Kalman::CalculateFilteredResidual(_track[i].GetData(), measured);
  }


  TrackFit::TrackFit(const TrackFit& tf) :
    _dimension(tf.GetDimension()),
    _propagator(0),
    _measurements(),
    _seed(_dimension),
    _track(_dimension),
    _identity_matrix(_measurement_dimension, _measurement_dimension) {
  }

} // namespace Kalman
} // namespace MAUS
