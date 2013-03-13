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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "DataStructure/GlobalTrack.hh"

namespace MAUS {

/** @class GlobalEvent comment
 *
 */

class GlobalEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    GlobalEvent();

    /** Copy constructor - any pointers are deep copied */
    GlobalEvent(const GlobalEvent& global_event);

    /** Equality operator - any pointers are deep copied */
    GlobalEvent& operator=(const GlobalEvent& global_event);

    /** Destructor - any member pointers are deleted */
    virtual ~GlobalEvent();

    GlobalTrackArray raw_tracks() const {
      return raw_tracks_;
    }

    void set_raw_tracks(GlobalTrackArray raw_tracks) {
      raw_tracks_ = raw_tracks;
    }

    GlobalTrackArray tracks() const {
      return raw_tracks_;
    }

    void set_tracks(GlobalTrackArray tracks) {
      tracks_ = tracks;
    }

  private:
    GlobalTrackArray raw_tracks_;
    GlobalTrackArray tracks_;

    MAUS_VERSIONED_CLASS_DEF(GlobalEvent);
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALEVENT_

