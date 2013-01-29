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

// MAUS headers
#include "src/common_cpp/DataStructure/GlobalRawTrack.hh"

namespace MAUS {

// Constructors
GlobalRawTrack::GlobalRawTrack() : track_points_() { }

// Destructor
GlobalRawTrack::~GlobalRawTrack() {}

GlobalRawTrack::GlobalRawTrack(const GlobalRawTrack &track)
  : track_points_(track.track_points_) {
}

// Assignment operator
GlobalRawTrack &GlobalRawTrack::operator=(const GlobalRawTrack &track) {
  if (this == &track) {
      return *this;
  }
  set_track_points(track.track_points());
  return *this;
}

} // ~namespace MAUS
