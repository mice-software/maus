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

#include <iostream>
#include <sstream>

#include "TDecompLU.h"


namespace MAUS {
namespace Kalman {


  std::string print_state( const State& state, const char* detail ) { 
    std::ostringstream converter("");
    TMatrixD vec = state.GetVector();

    if ( detail )
      converter << detail;
    converter << " @ " << state.GetPosition() << ", " << state.GetId() << "  |  ";
    for ( unsigned int i = 0; i < state.GetDimension(); ++i ) {
      converter << vec(i, 0) << "    ";
    }
    converter << 1.0/vec(vec.GetNrows()-1, 0);
    converter << "\n";
    return converter.str();
  }


  std::string print_track( const Track& track, const char* name) { 
    unsigned int num = track.GetLength();
    std::ostringstream converter("");
    if ( name )
      converter << "Printing " << name << "\n";
    else
      converter << "TRACK:\n";

    for ( unsigned int i = 0; i < num; ++i ) {
      State state = track[i];
      converter << print_state(state);
    }
    converter << "\n";
    return converter.str();
  }

  TrackFit::TrackFit(Propagator_base* prop, Measurement_base* meas) :
    _fitter_status(TrackFit::Initialised),
    _dimension(prop->GetDimension()),
    _measurement_dimension(meas->GetMeasurementDimension()),
    _propagator(prop),
    _measurement(meas),
    _seed(_dimension),
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
    throw Exception(Exception::recoverable,
        "Functionality Not Implemented!",
        "Kalman::TrackFit::AppendFilter()");

    if (state.GetDimension() != _measurement->GetMeasurementDimension()) {
      throw Exception(Exception::recoverable,
          "State dimension does not match the measurement dimension",
          "Kalman::TrackFit::AppendFilter()");
    }


    // The good stuff goes here!

    _fitter_status = TrackFit::Filtered;
  }


  void TrackFit::Filter(bool forward) {
    _predicted.Reset(_data);
    _filtered.Reset(_data);

    int increment = ( forward ? 1 : -1 );
    int track_start = ( forward ? 1 : _data.GetLength() - 2 );
    int track_end = ( forward ? _data.GetLength() : -1 );

    _predicted[track_start-increment] = _seed;
    _filtered[track_start-increment] = _seed;

    for (int i = track_start; i != track_end; i += increment) {
      _propagator->Propagate(_filtered[i-increment], _predicted[i]);

      if (_data[i].HasValue()) {

        State measured = _measurement->Measure(_predicted[i]);

        TMatrixD temp(GetMeasurementDimension(), GetMeasurementDimension());
//        temp = measured.GetCovariance();
        TDecompLU lu(measured.GetCovariance());
        if (!lu.Decompose()) {
          std::cerr << "Covariance too small, skipping...\n";
          _filtered[i] = _predicted[i];
          continue;
        }
        lu.Invert(temp);
//        temp.Invert();

        TMatrixD H(GetMeasurementDimension(), GetDimension());
        TMatrixD HT(GetDimension(), GetMeasurementDimension());
        TMatrixD K(GetDimension(), GetMeasurementDimension());
        TMatrixD pull(GetMeasurementDimension(), 1);

        H = _measurement->GetMeasurementMatrix();
        HT.Transpose(H);

        K = _predicted[i].GetCovariance() * HT * temp;

        pull = _data[i].GetVector() - measured.GetVector();

        TMatrixD temp_vec( GetDimension(), 1 );
        TMatrixD temp_cov( GetDimension(), GetDimension() );

        temp_vec = _predicted[i].GetVector() + K * pull;
        temp_cov = ( _identity_matrix - K * H ) * _predicted[i].GetCovariance();

        _filtered[i].SetVector( temp_vec );
        _filtered[i].SetCovariance( temp_cov );
      } else {
        std::cerr << "NO DATA FOUND IN STATE\n";
        _filtered[i] = _predicted[i];
      }
    }
    _fitter_status = TrackFit::Filtered;
  }


  void TrackFit::Smooth(bool forward) {
    _smoothed.Reset(_filtered);

    int increment = ( forward ? -1 : 1 );
    int track_start = ( forward ? (_smoothed.GetLength() - 2) : 1 );
    int track_end = ( forward ? -1 : (_smoothed.GetLength()) );

    _smoothed[track_start-increment] = _filtered[track_start-increment];

    for (int i = track_start; i != track_end; i += increment ) {
      TMatrixD temp(GetDimension(), GetDimension());
      TDecompLU lu(_predicted[i - increment].GetCovariance());
      if (!lu.Decompose() || !lu.Invert(temp)) {
        std::cerr << "Covariance too small, skipping...\n";
        _smoothed[i] = _filtered[i];
        continue;
      }

      TMatrixD prop = _propagator->CalculatePropagator(_filtered[i], _filtered[i - increment]);
      TMatrixD propT(GetDimension(), GetDimension()); propT.Transpose(prop);

      TMatrixD A = _filtered[i].GetCovariance() * propT * temp;
      TMatrixD AT(GetDimension(), GetDimension()); AT.Transpose(A);

      TMatrixD vec = _filtered[i].GetVector() + A * ( _smoothed[i-increment].GetVector() - _predicted[i-increment].GetVector() );
      TMatrixD cov = _filtered[i].GetCovariance() + A * ( _smoothed[i-increment].GetCovariance() - _predicted[i-increment].GetCovariance() ) * AT;

      _smoothed[i].SetVector( vec );
      _smoothed[i].SetCovariance( cov );
    }

    _fitter_status = TrackFit::Smoothed;
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

  TrackFit::TrackFit(const TrackFit& tf) :
    _fitter_status(TrackFit::Initialised),
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
