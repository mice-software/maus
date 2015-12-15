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
  _pr_track = new TRef();
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

  _seed_covariance_matrix.resize(a_track._seed_covariance_matrix.size());
  for (size_t i = 0; i < a_track._seed_covariance_matrix.size(); ++i) {
    _seed_covariance_matrix[i] = a_track._seed_covariance_matrix[i];
  }

  _pr_track = new TRef(*a_track.pr_track());
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

  _seed_covariance_matrix.resize(a_track._seed_covariance_matrix.size());
  for (size_t i = 0; i < a_track._seed_covariance_matrix.size(); ++i) {
    _seed_covariance_matrix[i] = a_track._seed_covariance_matrix[i];
  }
  if (_pr_track) delete _pr_track;
  _pr_track = new TRef(*a_track.pr_track());

  return *this;
}


SciFiTrack::~SciFiTrack() {
  // Delete track points in this track.
  std::vector<SciFiTrackPoint*>::iterator track_point;
  for (track_point = _trackpoints.begin();
       track_point!= _trackpoints.end(); ++track_point) {
    delete (*track_point);
  }

  delete _pr_track;
}


void SciFiTrack::SetSeedCovariance(double* covariance, unsigned int size) {
  _seed_covariance_matrix.resize(size);
  for ( unsigned int i = 0; i < size; ++i ) {
    _seed_covariance_matrix[i] = covariance[i];
  }
}

int SciFiTrack::GetRating() const {
  SciFiStraightPRTrack* pr_track = static_cast<SciFiStraightPRTrack*>(_pr_track->GetObject());

  int number_spacepoints = pr_track->get_num_points();

  bool good_pval = (this->P_value() > 0.05);


  int rating = 0;

  if (number_spacepoints == 5 &&
      good_pval) {
    rating = 1;
  } else if (good_pval) {
    rating = 2;
  } else {
    rating = 5;
  }

  return rating;
}

} // ~namespace MAUS
