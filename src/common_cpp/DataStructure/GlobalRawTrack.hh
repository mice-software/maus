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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALRAWTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALRAWTRACK_HH_

#include "Rtypes.h" // ROOT

#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"

namespace MAUS {

class GlobalRawTrack {
  public:
    /** Initialise to 0/NULL */
    GlobalRawTrack();

    /** Copy constructor - deep copies steps */
    GlobalRawTrack(const GlobalRawTrack& track);

    /** Equality operator - deep copies steps */
    GlobalRawTrack& operator=(const GlobalRawTrack& track);

    /** Destructor - frees memory allocated to steps */
    virtual ~GlobalRawTrack();

    /** Get the array of the track points */
    GlobalTrackPointArray track_points() const { return track_points_; };

    /** Set the array of the track points */
    void set_track_points(GlobalTrackPointArray points) { track_points_ = points; };

  private:
    GlobalTrackPointArray track_points_;

    ClassDef(GlobalRawTrack, 1)
};

typedef std::vector<GlobalRawTrack> GlobalRawTrackArray;
}

#endif
