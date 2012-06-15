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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SPECIALVIRTUALCHANNELID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SPECIALVIRTUALCHANNELID_HH_

#include "Rtypes.h" // ROOT

namespace MAUS {

/** Identifier for the channel where a hit was recorded in the SpecialVirtual
 */
class SpecialVirtualChannelId {
  public:
    /** Constructor - allocate to 0 */
    SpecialVirtualChannelId();

    /** Copy Constructor - copies data from channel*/
    SpecialVirtualChannelId(const SpecialVirtualChannelId& channel);

    /** Equality operator - copies data from channel */
    SpecialVirtualChannelId& operator=(const SpecialVirtualChannelId& channel);

    /** Destructor (does nothing)*/
    virtual ~SpecialVirtualChannelId();

    /** Get index of the station that was hit */
    int GetStation() const;

    /** Set index of the station that was hit */
    void SetStation(int station);

  private:
    int _station;

    ClassDef(SpecialVirtualChannelId, 1)
};
}

#endif

