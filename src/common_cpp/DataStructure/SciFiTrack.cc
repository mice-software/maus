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
                          _f_chi2(-1),
                          _s_chi2(-1),
                          _ndf(-1),
                          _P_value(-1),
                          _trackpoints(0) {
  // _scifitrackpoints.resize(0);
}

SciFiTrack::SciFiTrack(const SciFiTrack &a_track): _tracker(-1),
                                                   _f_chi2(-1),
                                                   _s_chi2(-1),
                                                   _ndf(-1),
                                                   _P_value(-1),
                                                   _trackpoints(0) {
  _tracker   = a_track.tracker();
  _f_chi2    = a_track.f_chi2();
  _s_chi2    = a_track.s_chi2();
  _ndf       = a_track.ndf();
  _P_value   = a_track.P_value();
  _trackpoints = a_track.scifitrackpoints();
}

SciFiTrack::SciFiTrack(const KalmanTrack *kalman_track): _tracker(-1),
                                                  _f_chi2(-1),
                                                  _s_chi2(-1),
                                                  _ndf(-1),
                                                  _P_value(-1) {
  _tracker = kalman_track->tracker();
  _f_chi2    = kalman_track->f_chi2();
  _s_chi2    = kalman_track->s_chi2();
  _ndf     = kalman_track->ndf();
  _P_value = kalman_track->P_value();
}

SciFiTrack& SciFiTrack::operator=(const SciFiTrack &a_track) {
    if (this == &a_track) {
        return *this;
    }
    _tracker = a_track.tracker();
    _f_chi2  = a_track.f_chi2();
    _s_chi2  = a_track.s_chi2();
    _ndf     = a_track.ndf();
    _P_value = a_track.P_value();
    _trackpoints = a_track.scifitrackpoints();
    return *this;
}

SciFiTrack::~SciFiTrack() {
  // Delete track points in this track.
  std::vector<SciFiTrackPoint*>::iterator track_point;
  for (track_point = _trackpoints.begin();
       track_point!= _trackpoints.end(); ++track_point) {
    delete (*track_point);
  }
}

} // ~namespace MAUS
