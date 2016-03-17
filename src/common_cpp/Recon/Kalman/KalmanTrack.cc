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

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

#include <iostream>

#include "src/common_cpp/Utils/Exception.hh"

namespace MAUS {
namespace Kalman {

////////////////////////////////////////////////////////////////////////////////
  // STATE MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
  State::State(unsigned int dim, double pos) :
    _dimension(dim),
    _id(0),
    _position(pos),
    _vector(dim, 1),
    _covariance(dim, dim),
    _has_value(false) {
    }

  State::State(TMatrixD vec, TMatrixD cov, double pos) :
    _dimension(vec.GetNrows()),
    _id(0),
    _position(pos),
    _vector(vec),
    _covariance(cov),
    _has_value(true) {
      if ((_vector.GetNcols() != 1) ||
          (_covariance.GetNrows() != static_cast<int>(_dimension)) ||
          (_covariance.GetNcols() != static_cast<int>(_dimension))) {
        throw Exception(Exception::nonRecoverable,
            "Vector and covariance matrix have inconsistent dimensions",
            "Kalman::State::State()");
      }
    }

  State::State(const State& st) :
    _dimension(st._dimension),
    _id(st._id),
    _position(st._position),
    _vector(st._vector),
    _covariance(st._covariance),
    _has_value(st._has_value) {
  }

  State& State::operator=(const State& st) {
    if (this->_dimension != st._dimension) {
      throw Exception(Exception::nonRecoverable,
          "State has wrong dimensions",
          "Kalman::State::SetVector()");
    }
    this->_id = st._id;
    this->_position = st._position;
    this->_vector = st._vector;
    this->_covariance = st._covariance;
    this->_has_value = st._has_value;

    return *this;
  }

  State& State::copy(State st) { // NOTE! ID and Position stay the same!
    if (this->_dimension != st._dimension) {
      throw Exception(Exception::nonRecoverable,
          "State has wrong dimensions",
          "Kalman::State::SetVector()");
    }
    this->_vector = st._vector;
    this->_covariance = st._covariance;
    this->_has_value = st._has_value;

    return *this;
  }

  void State::SetVector(TMatrixD vec) {
    if ((vec.GetNrows() != static_cast<int>(_dimension)) || (vec.GetNcols() != 1)) {
      throw Exception(Exception::nonRecoverable,
          "State vector has wrong dimensions",
          "Kalman::State::SetVector()");
    }
    _vector = vec;
    _has_value = true;
  }

  void State::SetCovariance(TMatrixD cov) {
    if ((cov.GetNrows() != static_cast<int>(_dimension)) ||
        (cov.GetNcols() != static_cast<int>(_dimension))) {
      throw Exception(Exception::nonRecoverable,
          "Covariance matrix has wrong dimensions",
          "Kalman::State::SetCovariance()");
    }
    _covariance = cov;
    _has_value = true;
  }

////////////////////////////////////////////////////////////////////////////////
  // TRACK MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

  Track::Track(unsigned int dim, unsigned int length) :
    _dimension(dim) {
    for (unsigned int i = 0; i < length; ++i) {
      _track_vector.push_back(State(_dimension));
    }
  }

  void Track::SetState(unsigned int index, State state) {
    if (state._dimension != this->_dimension) {
     // throw Exception(Exception::nonRecoverable,
     //     "State has wrong dimensions",
     //     "Kalman::Track::SetState()");
    }
    if (index >= _track_vector.size()) {
      throw Exception(Exception::recoverable,
          "Index out of bounds of track",
          "Kalman::Track::SetState()");
    }
    _track_vector[index] = state;
  }

  void Track::Append(State state) {
    if (state._dimension != this->_dimension) {
     // throw Exception(Exception::nonRecoverable,
     //     "State has wrong dimensions",
     //     "Kalman::Track::Append()");
    }
    _track_vector.push_back(state);
  }

  void Track::DeleteState(unsigned int index) {
    if (index >= _track_vector.size()) {
      throw Exception(Exception::recoverable,
          "Index out of bounds of track",
          "Kalman::Track::SetState()");
    }
    _track_vector.erase(_track_vector.begin()+index);
  }


  void Track::Reset(const Track& similar_track) {
    _track_vector.clear();

    for (unsigned int i = 0; i < similar_track.GetLength(); ++i) {
      State new_state(_dimension, similar_track[i].GetPosition());
      new_state.SetId(similar_track[i].GetId());
      _track_vector.push_back(new_state);
    }
  }

  std::ostream& operator<<(std::ostream& out, const TMatrixD& matrix) {
      for (int i = 0; i < matrix.GetNrows(); ++i) {
          for (int j = 0; j < matrix.GetNcols(); ++j) {
              out << matrix[i][j] << " ";
          }
          out << "\n";
      }
      return out;
  }

  std::ostream& operator<<(std::ostream& out, const State& state) {
      out << "State Id: " << state.GetId() << " position: " << state.GetPosition() << "\n";
      TMatrixD vec_transposed(TMatrixD::kTransposed, state.GetVector());
      out << "Vector: " << vec_transposed;
      out << "Covariance:\n" << state.GetCovariance();
      return out;
  }


  std::ostream& operator<<(std::ostream& out, const Track& track) {
      out << "Track of length " << track.GetLength() << "\n";
      for (size_t i = 0; i < track.GetLength(); ++i) {
          out << track[i];
      }
      return out;
  }

}
} // namespace MAUS

