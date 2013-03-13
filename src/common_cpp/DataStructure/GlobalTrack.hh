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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACK_HH_

#include "Rtypes.h" // ROOT

#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"

namespace MAUS {

class GlobalTrack {
  public:
    /** Initialise to 0/NULL */
    GlobalTrack();

    /** Copy constructor - deep copies steps */
    GlobalTrack(const GlobalTrack& track);

    /** Equality operator - deep copies steps */
    GlobalTrack& operator=(const GlobalTrack& track);

    /** Destructor - frees memory allocated to steps */
    virtual ~GlobalTrack();

    /** Get the array of the track points */
    GlobalTrackPointArray track_points() const { return track_points_; };

    /** Set the array of the track points */
    void set_track_points(GlobalTrackPointArray points)
    {
      track_points_ = points;
    };

  private:
    GlobalTrackPointArray track_points_;

    MAUS_VERSIONED_CLASS_DEF(GlobalTrack);
};

typedef std::vector<GlobalTrack> GlobalTrackArray;
}

#endif
