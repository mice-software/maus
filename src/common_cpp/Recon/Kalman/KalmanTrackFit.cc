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

#include "src/common_cpp/Utils/Exception.hh"


namespace MAUS {
namespace Kalman {

  TrackFit::TrackFit(Propagator_base* prop, Measurement_base meas) :
    _fitter_status(TrackFit::Initialised),
    _dimension(prop.GetDimension()),
    _propagator(prop),
    _measurement(meas),
    _seed(_dimension),
    _data(_dimension),
    _predicted(_dimension),
    _filtered(_dimension),
    _smoothed(_dimension),
    _identity_matrix(_dimension, _dimension) {
    if (_measurement.GetDimension() != _propagator.GetDimension()) {
      throw Exception(Exception::Recoverable,
          "Propagators and Measurements have conflicting dimension."
          "Kalman::TrackFit::TrackFit()");
    }
    _identity_matrix.Zero();
    for (unsigned int i = 0; i < _dimension; ++i)
      _identity_matrix(i, i) = 1.0;
  }

  TrackFit::~Trackfit() {
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
    throw Exception(Exception::Recoverable,
        "Functionality Not Implemented!",
        "Kalman::TrackFit::AppendFilter()")

    if (state.GetDimension() != _measurement->GetMeasurementDimension()) {
      throw Exception(Exception::Recoverable,
          "State dimension does not match the measurement dimension",
          "Kalman::TrackFit::AppendFilter()");
    }


    // The good stuff goes here!

    _fitter_status = TrackFit::Filtered;
  }


  void TrackFit::Filter() {
    _predicted.Reset(_data);
    _filtered.Reset(_data);

    _predicted[0] = _seed;
    _filtered[0] = _seed;

    for (unsigned int i = 1; i < _data.Length(); ++i) {
      _propagator.Propagate(_filtered[i-1], _predicted[i]);

      if (_data[i].HasValue()) {

        TMatrixD H = measurement.GetMeasurementMatrix(_predicted[index]);
        TMatrixD HT; HT.Transpose(H);

        State measured = measurement->Measure(_predicted[index]);
        TMatrixD temp = measured.GetCovariance();
        temp.Invert();

        TMatrixD K = _predicted[i].GetCovariance() * HT * temp;

        TMatrixD pull = _data[i].GetVector - measured.GetVector();

        _filtered[i].SetVector( _predicted[i].GetState() + K * pull );
        _filtered[i].SetCovariance( ( _identity_matrix - H * K ) * _predicted[i].getCovariance() );
      } else {
        _filtered[i] = _predicted[i];
      }
    }

    _fitter_status = TrackFit::Filtered;
  }


  void TrackFit::Smooth() {
    _smoothed.Reset(_filtered);

    unsigned int last_state = _smoothed.Length() - 1;
    _smoothed[last_state] = _filtered[last_state];

    for (unsigned int i = (last_state - 1); i >= 0; --i ) {

      TMatrixD prop = _propagator->GetPropagator(_filtered[i], _filtered[i+1]);
      TMatrixD protT; propT.Transpose(prop);
      TMatrixD A = _filtered[i].GetCovariance() * propT _predicted[i+1].GetCovariance().Invert();
      TMatrixD AT; AT.Transpose(A);

      _smoothed[i].SetVector( _filtered[i].GetCovaraince() + A * ( _smoothed[i+1].GetVector() - _predicted[i+1].GetVector() ) );
      smoothed[i].SetCovariance( _filtered[i].GetCovariance() + A * ( _smoothed[i+1].GetCovariance() - _predicted[i+1].GetCovariance() ) * AT );
    }

    _fitter_status = TrackFit::Smoothed;
  }


  void TrackFit::SetSeed(State seed) {
    if (seed.GetDimension() != _dimension) {
      throw Exception(Exception::Recoverable,
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



//  void TrackFit::Filter(unsigned int index) {
//    TMatrixD I = TMatrix::kUnit;
//    TMatrixD pred_cov = _predicted[index].GetCovariance();
//    TMatrixD H = measurement.GetMeasurementMatrix();
//    TMatrixD HT; HT.Transpose(H);
//
//    State measured = measurement->Measure(_predicted[index]);
//    TMatrixD temp = measured.GetCovariance();
//    temp.Invert();
//
//    TMatrixD K = pred_cov * HT * temp;
//
//    TMatrixD new_vec = _predicted[index].GetVector() + K * ( _data[index].GetVector() - measured.GetVector() );
//    TMatrixD new_cov = ( I - K * H ) * pred_cov;
//
//    _filtered[index].SetVector(new_vec);
//    _filtered[index].SetCovariance(new_cov);
//  }



} // namespace Kalman
} // namespace MAUS
