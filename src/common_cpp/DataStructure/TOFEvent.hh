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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENT_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class TOFEvent comment
 *
 */

class TOFEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFEvent();

    /** Copy constructor - any pointers are deep copied */
    TOFEvent(const TOFEvent& _tofevent);

    /** Equality operator - any pointers are deep copied */
    TOFEvent& operator=(const TOFEvent& _tofevent);

    /** Destructor - any member pointers are deleted */
    ~TOFEvent();


  private:

    ClassDef(TOFEvent, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENT_

