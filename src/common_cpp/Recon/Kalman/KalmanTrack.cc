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
  State::State(unsigned int dim) :
    _dimension(dim),
    _vector(dim, 1),
    _covariance(dim, dim),
    _has_value(false) {
    }

  State::State(TMatrixD vec, TMatrixD cov) :
    _dimension(vec.GetNrows()),
    _vector(vec),
    _covariance(cov),
    _has_value(true) {
      if ((_vector.GetNcols() != 1) ||
          (_covariance.GetNrows() != static_cast<int>(_dimension)) ||
          (_covariance.GetNcols() != static_cast<int>(_dimension))) {
        throw Exceptions::Exception(Exceptions::nonRecoverable,
            "Vector and covariance matrix have inconsistent dimensions",
            "Kalman::State::State()");
      }
    }

  State::State(const State& st) :
    _dimension(st._dimension),
    _vector(st._vector),
    _covariance(st._covariance),
    _has_value(st._has_value) {
  }

  State& State::operator=(const State& st) {
    this->_dimension = st._dimension;
    this->_vector.ResizeTo(st._vector.GetNrows(), st._vector.GetNcols());
    this->_vector = st._vector;
    this->_covariance.ResizeTo(st._covariance.GetNrows(), st._covariance.GetNcols());
    this->_covariance = st._covariance;
    this->_has_value = st._has_value;

    return *this;
  }

  void State::SetVector(TMatrixD vec) {
    if ((vec.GetNrows() != static_cast<int>(_dimension)) || (vec.GetNcols() != 1)) {
      throw Exceptions::Exception(Exceptions::nonRecoverable,
          "State vector has wrong dimensions",
          "Kalman::State::SetVector()");
    }
    _vector = vec;
    _has_value = true;
  }

  void State::SetCovariance(TMatrixD cov) {
    if ((cov.GetNrows() != static_cast<int>(_dimension)) ||
        (cov.GetNcols() != static_cast<int>(_dimension))) {
      throw Exceptions::Exception(Exceptions::nonRecoverable,
          "Covariance matrix has wrong dimensions",
          "Kalman::State::SetCovariance()");
    }
    _covariance = cov;
    _has_value = true;
  }

////////////////////////////////////////////////////////////////////////////////
  // TRACKPOINT MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

  TrackPoint::TrackPoint(unsigned int dim, unsigned int meas_dim, double pos, int id) :
    _predicted(dim),
    _filtered(dim),
    _smoothed(dim),
    _data(meas_dim),
    _id(id),
    _position(pos) {
      _data.SetHasValue(false);
    }

  TrackPoint::TrackPoint(unsigned int dim, double pos, State data, int id) :
    _predicted(dim),
    _filtered(dim),
    _smoothed(dim),
    _data(data),
    _id(id),
    _position(pos) {
    }

  TrackPoint::TrackPoint(State pred, State filt, State smoo, State data, double pos, int id) :
    _predicted(pred),
    _filtered(filt),
    _smoothed(smoo),
    _data(data),
    _id(id),
    _position(pos) {
    }

  TrackPoint::TrackPoint(const TrackPoint& tp) :
    _predicted(tp._predicted),
    _filtered(tp._filtered),
    _smoothed(tp._smoothed),
    _data(tp._data),
    _id(tp._id),
    _position(tp._position) {
  }

  TrackPoint& TrackPoint::operator=(const TrackPoint& tp) {
    if (this->GetDimension() != tp.GetDimension()) {
      throw Exceptions::Exception(Exceptions::nonRecoverable,
          "TrackPoint has wrong dimensions",
          "Kalman::TrackPoint::operator=()");
    }
    this->_predicted = tp._predicted;
    this->_filtered = tp._filtered;
    this->_smoothed = tp._smoothed;
    this->_data = tp._data;

    this->_id = tp._id;
    this->_position = tp._position;

    return *this;
  }

  TrackPoint& TrackPoint::copy(TrackPoint tp) { // NOTE! ID and Position stay the same!
    if (this->GetDimension() != tp.GetDimension()) {
      throw Exceptions::Exception(Exceptions::nonRecoverable,
          "TrackPoint has wrong dimensions",
          "Kalman::TrackPoint::operator=()");
    }
    this->_predicted = tp._predicted;
    this->_filtered = tp._filtered;
    this->_smoothed = tp._smoothed;
    this->_data = tp._data;

    return *this;
  }


  State& TrackPoint::GetFiltered() {
    if (_filtered.HasValue()) {
      return _filtered;
    } else {
      return _predicted;
    }
  }

  const State& TrackPoint::GetFiltered() const {
    if (_filtered.HasValue()) {
      return _filtered;
    } else {
      return _predicted;
    }
  }

  State& TrackPoint::GetSmoothed() {
    if (_smoothed.HasValue()) {
      return _smoothed;
    } else if (_filtered.HasValue()) {
      return _filtered;
    } else {
      return _predicted;
    }
  }

  const State& TrackPoint::GetSmoothed() const {
    if (_smoothed.HasValue()) {
      return _smoothed;
    } else if (_filtered.HasValue()) {
      return _filtered;
    } else {
      return _predicted;
    }
  }

////////////////////////////////////////////////////////////////////////////////
  // TRACK MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

  Track::Track(unsigned int dim, unsigned int length) :
    _dimension(dim) {
    for (unsigned int i = 0; i < length; ++i) {
      _track_vector.push_back(TrackPoint(_dimension, _dimension, 0.0));
    }
  }

  void Track::SetTrackPoint(unsigned int index, TrackPoint tp) {
    if (tp.GetDimension() != this->_dimension) {
      throw Exceptions::Exception(Exceptions::nonRecoverable,
          "TrackPoint has wrong dimensions",
          "Kalman::Track::SetTrackPoint()");
    }
    if (index >= _track_vector.size()) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Index out of bounds of track",
          "Kalman::Track::SetTrackPoint()");
    }
    _track_vector[index] = tp;
  }

  void Track::Append(TrackPoint tp) {
    if (tp.GetDimension() != this->_dimension) {
      throw Exceptions::Exception(Exceptions::nonRecoverable,
          "TrackPoint has wrong dimensions",
          "Kalman::Track::Append()");
    }
    _track_vector.push_back(tp);
  }

  void Track::DeleteTrackPoint(unsigned int index) {
    if (index >= _track_vector.size()) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Index out of bounds of track",
          "Kalman::Track::SetState()");
    }
    _track_vector.erase(_track_vector.begin()+index);
  }


  void Track::Reset(const Track& similar_track) {
    _track_vector.clear();

    for (unsigned int i = 0; i < similar_track.GetLength(); ++i) {
      TrackPoint new_tp(_dimension, similar_track[i].GetData().GetDimension(),
                                                                   similar_track[i].GetPosition());
      new_tp.SetId(similar_track[i].GetId());
      _track_vector.push_back(new_tp);
    }
  }
}
} // namespace MAUS

std::ostream& operator<<(std::ostream& out, const TMatrixD& matrix) {
    for (int i = 0; i < matrix.GetNrows(); ++i) {
        for (int j = 0; j < matrix.GetNcols(); ++j) {
            out.width(12);
            out << std::right << matrix(i, j) << " ";
        }
        out << "\n";
    }
    return out;
}

std::ostream& operator<<(std::ostream& out,
                         const MAUS::Kalman::State& state) {
    out << state.GetVector() << "\n";
    out << state.GetCovariance();
    return out;
}

std::ostream& operator<<(std::ostream& out,
                         const MAUS::Kalman::TrackPoint& track_point) {
    out << "Track point at " << track_point.GetPosition() << "\n"
        << "    data:\n" << track_point.GetData() << "\n"
        << "    filtered:\n" << track_point.GetFiltered() << "\n"
        << "    smoothed:\n" << track_point.GetSmoothed();
    return out;
}


std::ostream& operator<<(std::ostream& out,
                         const MAUS::Kalman::Track& track) {
    out << "Track with " << track.GetLength() << " points\n";
    for (size_t i = 0; i < track.GetLength(); ++i)
        out << track[i] << std::endl;
    return out;
}


