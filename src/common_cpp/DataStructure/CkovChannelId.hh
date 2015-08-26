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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CkovCHANNELID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CkovCHANNELID_HH_

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** Identifier for the channel where a hit was recorded in the Ckov
 */
class CkovChannelId {
  public:
    /** Constructor - allocate to 0 */
    CkovChannelId();

    /** Copy Constructor - copies data from channel*/
    CkovChannelId(const CkovChannelId& channel);

    /** Equality operator - copies data from channel */
    CkovChannelId& operator=(const CkovChannelId& channel);

    /** Destructor (does nothing)*/
    virtual ~CkovChannelId();

    /** Get the station that was hit, 1 or 2 */
    int GetStation() const;

    /** Set the station that was hit, 1 or 2 */
    void SetStation(int station);

    // Get the PMT id that the light was sent to 
    int GetPMT() const;

    // Set the PMT id that the light was sent to 
    void SetPMT(int PMT);

  private:
    
    int _station;
    int _pmt;

    MAUS_VERSIONED_CLASS_DEF(CkovChannelId)
};
}

#endif

