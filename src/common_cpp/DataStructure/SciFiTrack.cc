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

SciFiTrack::SciFiTrack(): _tracker(-1), _chi2(-1), _ndf(-1), _P_value(-1) {}

SciFiTrack::SciFiTrack(const SciFiTrack &a_track): _tracker(-1),
                                                   _chi2(-1),
                                                   _ndf(-1),
                                                   _P_value(-1) {
  _tracker = a_track.get_tracker();
  _chi2    = a_track.get_chi2();
  _ndf     = a_track.get_ndf();
  _P_value = a_track.get_P_value();
}

SciFiTrack::SciFiTrack(const KalmanTrack *kalman_track): _tracker(-1),
                                                  _chi2(-1),
                                                  _ndf(-1),
                                                  _P_value(-1) {
  _tracker = kalman_track->get_tracker();
  _chi2    = kalman_track->get_chi2();
  _ndf     = kalman_track->get_ndf();
  _P_value = kalman_track->get_P_value();
}

SciFiTrack& SciFiTrack::operator=(const SciFiTrack &a_track) {
    if (this == &a_track) {
        return *this;
    }
    _tracker = a_track.get_tracker();
    _chi2    = a_track.get_chi2();
    _ndf     = a_track.get_ndf();
    _P_value = a_track.get_P_value();
    return *this;
}

SciFiTrack::~SciFiTrack() {

}

} // ~namespace MAUS
