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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFCHANNELID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFCHANNELID_HH_

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** Identifier for the channel where a hit was recorded in the TOF
 */
class TOFChannelId {
  public:
    /** Constructor - allocate to 0 */
    TOFChannelId();

    /** Copy Constructor - copies data from channel*/
    TOFChannelId(const TOFChannelId& channel);

    /** Equality operator - copies data from channel */
    TOFChannelId& operator=(const TOFChannelId& channel);

    /** Destructor (does nothing)*/
    virtual ~TOFChannelId();

    /** Get the slab that was hit */
    int GetSlab() const;

    /** Set the slab that was hit */
    void SetSlab(int slab);

    /** Get the plane (vertical or horizontal) that was hit */
    int GetPlane() const;

    /** Set the plane (vertical or horizontal) that was hit */
    void SetPlane(int plane);

    /** Get the station that was hit */
    int GetStation() const;

    /** Set the station that was hit */
    void SetStation(int station);

  private:
    int _slab;
    int _plane;
    int _station;

    MAUS_VERSIONED_CLASS_DEF(TOFChannelId)
};
}

#endif

