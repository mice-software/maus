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
    converter << " @ " << state.GetPosition() << ", " << state.GetId() << "  |  ";
    if (state.HasValue()) {
      for (unsigned int i = 0; i < state.GetDimension(); ++i) {
        converter << "(" << vec(i, 0) << " +/- " << sqrt(cov(i, i)) << "), ";
      }
      converter << "\n";
    } else {
      converter << "NO DATA\n";
    }
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
      State state = track[i];
      converter << print_state(state);
    }
    converter << "\n";
    return converter.str();
  }

  State CalculateResidual(const State& st1, const State& st2) {
    if (st1._dimension != st2._dimension) {
      throw Exception(Exception::recoverable,
          "States have different dimensions",
          "Kalman::CalculateResidual()");
    }
    TMatrixD vector = st1._vector - st2._vector;
    TMatrixD covariance = st1._covariance + st2._covariance;

    State residual(vector, covariance, st1._position);
    residual.SetId(st1._id);

    return residual;
  }

  double CalculateChiSquaredUpdate(const State& st) {
    TMatrixD rT(TMatrixD::kTransposed, st._vector);// rT.Transpose(r);
    TMatrixD RI(TMatrixD::kInverted, st._covariance);
    TMatrixD update = rT * RI * st._vector;
    return update(0, 0);
  }



  TrackFit::TrackFit(Propagator_base* prop, Measurement_base* meas) :
    _dimension(prop->GetDimension()),
    _measurement_dimension(meas->GetMeasurementDimension()),
    _propagator(prop),
    _measurement(meas),
    _seed(_dimension, _measurement_dimension),
    _data(_dimension),
    _predicted(_dimension),
    _filtered(_dimension),
    _smoothed(_dimension),
    _identity_matrix(_dimension, _dimension) {
    if (_measurement->GetDimension() != _propagator->GetDimension()) {
      throw Exception(Exception::recoverable,
          "Propagators and Measurements have conflicting dimension.",
          "Kalman::TrackFit::TrackFit()");
    }
    _identity_matrix.Zero();
    for (unsigned int i = 0; i < _dimension; ++i)
      _identity_matrix(i, i) = 1.0;
  }


  TrackFit::~TrackFit() {
    if (_propagator) {
      delete _propagator;
      _propagator = NULL;
    }
    if (_measurement) {
      delete _measurement;
      _measurement = NULL;
    }
  }


  void TrackFit::AppendFilter(State state) {
    throw Exception(Exception::nonRecoverable,
        "Functionality Not Implemented!",
        "Kalman::TrackFit::AppendFilter()");

    if (state.GetDimension() != _measurement->GetMeasurementDimension()) {
      throw Exception(Exception::recoverable,
          "State dimension does not match the measurement dimension",
          "Kalman::TrackFit::AppendFilter()");
    }
    // The good stuff goes here!
  }


  void TrackFit::Filter(bool forward) {
    _predicted.Reset(_data);
    _filtered.Reset(_data);

    int increment = (forward ? 1 : -1);
    int track_start = (forward ? 1 : _data.GetLength() - 2);
    int track_end = (forward ? _data.GetLength() : -1);

    _predicted[track_start-increment].copy(_seed);
//    _filtered[track_start-increment].copy(_seed);
    this->_filter(_data[track_start-increment],
                                        _predicted[track_start-increment],
                                        _filtered[track_start-increment]);

    for (int i = track_start; i != track_end; i += increment) {
      this->_propagate(_filtered[i-increment], _predicted[i]);

      if (_data[i].HasValue()) {
        this->_filter(_data[i], _predicted[i], _filtered[i]);
      } else {
        _filtered[i] = _predicted[i];
      }
    }
  }


  void TrackFit::Smooth(bool forward) {
    _smoothed.Reset(_data);

    int increment = (forward ? -1 : 1);
    int track_start = (forward ? (_smoothed.GetLength() - 2) : 1);
    int track_end = (forward ? -1 : (_smoothed.GetLength()));

    _smoothed[track_start-increment] = _filtered[track_start-increment];

    for (int i = track_start; i != track_end; i += increment) {
//      TMatrixD temp(GetDimension(), GetDimension());
//      TDecompLU lu(_predicted[i - increment].GetCovariance());
//      if (!lu.Decompose() || !lu.Invert(temp)) {
//        _smoothed[i] = _filtered[i];
//        _propagator->Propagate(_filtered[i - increment], _smoothed[i]);
//      } else {
        TMatrixD temp(TMatrixD::kInverted, _predicted[i - increment].GetCovariance());
        TMatrixD prop = _propagator->CalculatePropagator(_filtered[i], _filtered[i - increment]);
        TMatrixD propT(TMatrixD::kTransposed, prop);

        TMatrixD A = _filtered[i].GetCovariance() * propT * temp;
        TMatrixD AT(TMatrixD::kTransposed, A);

        TMatrixD vec = _filtered[i].GetVector() + A * (_smoothed[i-increment].GetVector() -
                                                              _predicted[i-increment].GetVector());
        TMatrixD cov = _filtered[i].GetCovariance() + A * (_smoothed[i-increment].GetCovariance() -
                                                     _predicted[i-increment].GetCovariance()) * AT;

        _smoothed[i].SetVector(vec);
        _smoothed[i].SetCovariance(cov);
//      }
    }
  }


  void TrackFit::SetSeed(State seed) {
    if (seed.GetDimension() != _dimension) {
      throw Exception(Exception::recoverable,
          "Seed dimension does not match the track fitter",
          "Kalman::TrackFit::SetSeed()");
    }
    _seed = seed;
  }

  void TrackFit::SetData(Track data_track) {
    if (data_track.GetDimension() != _measurement->GetMeasurementDimension()) {
      throw Exception(Exception::nonRecoverable,
          "Data track dimension does not match the measurement dimension",
          "Kalman::TrackFit::SetData()");
    }
    _data = data_track;
    _predicted = Track(_dimension);
    _filtered = Track(_dimension);
    _smoothed = Track(_dimension);
  }



  void TrackFit::_propagate(State& first, State& second) const {
    _propagator->Propagate(first, second);
  }

  void TrackFit::_filter(const State& data, const State& predicted, State& filtered) const {
    State measured = _measurement->Measure(predicted);
    TMatrixD V = _measurement->GetMeasurementNoise();

//    TMatrixD temp(GetMeasurementDimension(), GetMeasurementDimension());
//    TDecompLU lu(V + measured.GetCovariance());
//    if (!lu.Decompose() || !lu.Invert(temp)) {
//      filtered = predicted;
//    } else {
      TMatrixD temp(TMatrixD::kInverted, measured.GetCovariance());
      TMatrixD H = _measurement->GetMeasurementMatrix();
      TMatrixD HT(TMatrixD::kTransposed, H);// HT.Transpose(H);

      TMatrixD K = predicted.GetCovariance() * HT * temp;
      TMatrixD KT(TMatrixD::kTransposed, K);// KT.Transpose(K);

      TMatrixD pull(GetMeasurementDimension(), 1);

      TMatrixD gain = _identity_matrix - K*H;
      TMatrixD gainT(TMatrixD::kTransposed, gain);// gainT.Transpose(gain);
      TMatrixD gain_constant = K * V * KT;

      pull = data.GetVector() - measured.GetVector();

      TMatrixD temp_vec = predicted.GetVector() + K * pull;
      TMatrixD temp_cov = gain * predicted.GetCovariance() * gainT + gain_constant;

      filtered.SetVector(temp_vec);
      filtered.SetCovariance(temp_cov);
//    }
  }

  void TrackFit::_smooth(State& first, State& second) const {
  }


  void TrackFit::_inverse_filter(const State& data, const State& smoothed, State& cleaned) const {
    State measured = _measurement->Measure(smoothed);
    TMatrixD V = _measurement->GetMeasurementNoise();
    // THIS IS THE INVERSE FILTER LINE!
    V *= -1.0;

    TMatrixD temp(GetMeasurementDimension(), GetMeasurementDimension());
    TDecompLU lu(measured.GetCovariance());
    if (!lu.Decompose() || !lu.Invert(temp)) {
      cleaned = smoothed;
    } else {
      TMatrixD H = _measurement->GetMeasurementMatrix();
      TMatrixD HT(TMatrixD::kTransposed, H);

      TMatrixD K = smoothed.GetCovariance() * HT * temp;
      TMatrixD KT(TMatrixD::kTransposed, K);

      TMatrixD pull(GetMeasurementDimension(), 1);

      TMatrixD gain = _identity_matrix - K*H;
      TMatrixD gainT(TMatrixD::kTransposed, gain);
      TMatrixD gain_constant = K * V * KT;

      pull = data.GetVector() - measured.GetVector();

      TMatrixD temp_vec = smoothed.GetVector() + K * pull;
      TMatrixD temp_cov = gain * smoothed.GetCovariance() * gainT + gain_constant;

      cleaned.SetVector(temp_vec);
      cleaned.SetCovariance(temp_cov);
    }
  }


  double TrackFit::CalculateChiSquared(const Track& track) const {
    if (track.GetLength() != _data.GetLength())
      throw Exception(Exception::recoverable,
          "Supplied Track length is different to data track",
          "Kalman::TrackFit::CalculateChiSquared()");

    double chi_squared = 0.0;

    for (unsigned int i = 0; i < track.GetLength(); ++i) {
      if (_data[i]) {
        State measured = _measurement->Measure(track[i]);
        State chisq = CalculateResidual(measured, _data[i]);
        double update = CalculateChiSquaredUpdate(chisq);
        chi_squared += update;
      }
    }

    return chi_squared;
  }

  int TrackFit::GetNDF() const {
    int no_parameters = GetDimension();
    int no_measurements = 0;
    for (unsigned int i = 0; i < _data.GetLength(); ++i) {
      if (_data[i].HasValue()) {
        no_measurements += GetMeasurementDimension();
      }
    }
    return (no_measurements - no_parameters);
  }

  State TrackFit::CalculateCleanedState(unsigned int i) const {
    State smoothed = _smoothed[i];
    const State& data = _data[i];
    State cleaned(GetDimension());

    this->_inverse_filter(data, smoothed, cleaned);
    return cleaned;
  }

  State TrackFit::CalculatePull(unsigned int i) const {
    State cleaned = this->CalculateCleanedState(i);
    return CalculateResidual(cleaned, _smoothed[i]);
  }

  State TrackFit::CalculatePredictedResidual(unsigned int i) const {
    State measured = _measurement->Measure(_predicted[i]);
    return CalculateResidual(measured, _data[i]);
  }

  State TrackFit::CalculateFilteredResidual(unsigned int i) const {
    State measured = _measurement->Measure(_filtered[i]);
    return CalculateResidual(measured, _data[i]);
  }

  State TrackFit::CalculateSmoothedResidual(unsigned int i) const {
    State measured = _measurement->Measure(_smoothed[i]);
    return CalculateResidual(measured, _data[i]);
  }


  TrackFit::TrackFit(const TrackFit& tf) :
    _dimension(tf.GetDimension()),
    _propagator(0),
    _measurement(0),
    _seed(_dimension),
    _data(_dimension),
    _predicted(_dimension),
    _filtered(_dimension),
    _smoothed(_dimension),
    _identity_matrix(_measurement_dimension, _measurement_dimension) {
  }

} // namespace Kalman
} // namespace MAUS
