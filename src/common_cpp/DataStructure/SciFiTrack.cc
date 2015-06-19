/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "src/common_cpp/DataStructure/SciFiTrack.hh"

namespace MAUS {

SciFiTrack::SciFiTrack(): _tracker(-1),
                          _chi2(-1),
                          _ndf(-1),
                          _P_value(-1),
                          _charge(0),
                          _trackpoints(0),
                          _seed_position(),
                          _seed_momentum(),
                          _seed_covariance_matrix(0) {
}

SciFiTrack::SciFiTrack(const SciFiTrack &a_track): _tracker(-1),
                                                   _chi2(-1),
                                                   _ndf(-1),
                                                   _P_value(-1),
                                                   _charge(0),
                                                   _trackpoints(0),
                                                   _seed_position(),
                                                   _seed_momentum(),
                                                   _seed_covariance_matrix(0) {
  _tracker   = a_track.tracker();
  _chi2    = a_track.chi2();
  _ndf       = a_track.ndf();
  _P_value   = a_track.P_value();
  _charge    = a_track.charge();
  _algorithm_used = a_track._algorithm_used;
  _seed_position = a_track._seed_position;
  _seed_momentum = a_track._seed_momentum;

  _trackpoints.resize(a_track._trackpoints.size());
  for (size_t i = 0; i < a_track._trackpoints.size(); ++i) {
    _trackpoints[i] = new SciFiTrackPoint(*a_track._trackpoints[i]);
  }

//  _seed_state_vector.resize(a_track._seed_state_vector.size());
//  for (size_t i = 0; i < a_track._seed_state_vector.size(); ++i) {
//    _seed_state_vector[i] = a_track._seed_state_vector[i];
//  }
  _seed_covariance_matrix.resize(a_track._seed_covariance_matrix.size());
  for (size_t i = 0; i < a_track._seed_covariance_matrix.size(); ++i) {
    _seed_covariance_matrix[i] = a_track._seed_covariance_matrix[i];
  }
}

SciFiTrack& SciFiTrack::operator=(const SciFiTrack &a_track) {
  if (this == &a_track) {
    return *this;
  }
  _tracker = a_track.tracker();
  _chi2  = a_track.chi2();
  _ndf     = a_track.ndf();
  _P_value = a_track.P_value();
  _charge  = a_track.charge();
  _algorithm_used = a_track._algorithm_used;
  _seed_position = a_track._seed_position;
  _seed_momentum = a_track._seed_momentum;

  _trackpoints.resize(a_track._trackpoints.size());
  for (size_t i = 0; i < a_track._trackpoints.size(); ++i) {
    _trackpoints[i] = new SciFiTrackPoint(*a_track._trackpoints[i]);
  }

//  _seed_state_vector.resize(a_track._seed_state_vector.size());
//  for (size_t i = 0; i < a_track._seed_state_vector.size(); ++i) {
//    _seed_state_vector[i] = a_track._seed_state_vector[i];
//  }
  _seed_covariance_matrix.resize(a_track._seed_covariance_matrix.size());
  for (size_t i = 0; i < a_track._seed_covariance_matrix.size(); ++i) {
    _seed_covariance_matrix[i] = a_track._seed_covariance_matrix[i];
  }

  return *this;
}

// void SciFiTrack::set_scifitrackpoints(SciFiTrackPointPArray points) {
//   // Delete any existing track points.
//   std::vector<SciFiTrackPoint*>::iterator track_point;
//   for (track_point = _trackpoints.begin();
//        track_point!= _trackpoints.end(); ++track_point) {
//     delete (*track_point);
//   }
//
//   // Make the deep copy.
//   _trackpoints.resize(points.size());
//   for (size_t i = 0; i < points.size(); ++i) {
//     _trackpoints[i] = new SciFiTrackPoint(*points.at(i));
//   }
// }

SciFiTrack::~SciFiTrack() {
  // Delete track points in this track.
  std::vector<SciFiTrackPoint*>::iterator track_point;
  for (track_point = _trackpoints.begin();
       track_point!= _trackpoints.end(); ++track_point) {
    delete (*track_point);
  }
}

// void SciFiTrack::SetSeedState(double* state, unsigned int size) {
//   _seed_state_vector.resize(size);
//   for ( unsigned int i = 0; i < size; ++i ) {
//     _seed_state_vector[i] = state[i];
//   }
// }

void SciFiTrack::SetSeedCovariance(double* covariance, unsigned int size) {
  _seed_covariance_matrix.resize(size);
  for ( unsigned int i = 0; i < size; ++i ) {
    _seed_covariance_matrix[i] = covariance[i];
  }
}

} // ~namespace MAUS
