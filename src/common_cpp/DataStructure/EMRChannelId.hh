
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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRChannelID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRChannelID_HH_

#include "Rtypes.h" // ROOT

namespace MAUS {

/** Identifier for the channel where a hit was recorded in the EMR
 */
class EMRChannelId {
  public:
    /** Constructor - allocate to 0 */
    EMRChannelId()
    :_bar(-1) {}

    /** Copy Constructor - copies data from channel*/
    EMRChannelId(const EMRChannelId& channel);

    /** Equality operator - copies data from channel */
    EMRChannelId& operator=(const EMRChannelId& channel);

    /** Destructor (does nothing)*/
    virtual ~EMRChannelId() {}

    /** Get the bar that was hit */
    int GetBar() const  {return _bar;}

    /** Set the bar that was hit */
    void SetBar(int bar) {_bar = bar;}

//     /** Get the bar that was hit */
//     int GetPlane() const  {return _plane;}
//
//     /** Set the bar that was hit */
//     void SetPlane(int p) {_plane = p;}

  private:
    int _bar;
//     int _plane;

    ClassDef(EMRChannelId, 1)
};
}

#endif
