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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class SciFiEvent comment
 *
 */

class SciFiEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    SciFiEvent();

    /** Copy constructor - any pointers are deep copied */
    SciFiEvent(const SciFiEvent& _scifievent);

    /** Equality operator - any pointers are deep copied */
    SciFiEvent& operator=(const SciFiEvent& _scifievent);

    /** Destructor - any member pointers are deleted */
    virtual ~SciFiEvent();


  private:

    ClassDef(SciFiEvent, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIEVENT_

