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

/** @class SciFiBasePRTrack Base class for scifi pattern recogntion tracks.  */

#ifndef  _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIBASEPRTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIBASEPRTRACK_HH_

// C++ headers
#include <vector>

// ROOT headers
#include "TRefArray.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

class SciFiBasePRTrack {
  public:
    /** Default constructor */
    SciFiBasePRTrack();

    /** Default destructor */
    virtual ~SciFiBasePRTrack();

    /** Get the vector holding pointers to the spacepoints used by the track */
    TRefArray* get_spacepoints() const { return _spoints; }

    /** Set the vector holding pointers to the spacepoints
     * used by the track from a TRefArray */
    void set_spacepoints(TRefArray* spoints) { _spoints = spoints; }

   /** Return a vector holding pointers to the spacepoints
     * (requires extracting them from the TRefArray holder 
     */
    SciFiSpacePointPArray get_spacepoints_pointers();

    /** Set the vector holding pointers to the spacepoints
     *  used by the track from a vector  of spacepoint pointers */
    void set_spacepoints_pointers(SciFiSpacePointPArray spoints);

  protected:
    TRefArray* _spoints;

  MAUS_VERSIONED_CLASS_DEF(SciFiBasePRTrack)
};

typedef std::vector<SciFiBasePRTrack*> SciFiBasePRTrackPArray;
typedef std::vector<SciFiBasePRTrack> SciFiBasePRTrackArray;

} // ~namespace MAUS

#endif

