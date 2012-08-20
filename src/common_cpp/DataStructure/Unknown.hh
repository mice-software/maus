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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_UNKNOWN_
#define _SRC_COMMON_CPP_DATASTRUCTURE_UNKNOWN_

#include <vector>

#include "Rtypes.h"  // ROOT

#include "src/common_cpp/DataStructure/V1290.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<V1290> V1290Array;

/** @class Unknown comment
 *
 *  @var V1290  <--description-->
 */

class Unknown {
  public:
    /** Default constructor - initialises to 0/NULL */
    Unknown();

    /** Copy constructor - any pointers are deep copied */
    Unknown(const Unknown& _unknown);

    /** Equality operator - any pointers are deep copied */
    Unknown& operator=(const Unknown& _unknown);

    /** Destructor - any member pointers are deleted */
    virtual ~Unknown();


    /** Returns V1290Array */
    V1290Array GetV1290Array() const;

    /** Get an element from V1290Array (needed for PyROOT) */
    V1290 GetV1290ArrayElement(size_t index) const;

    /** Get size of V1290Array (needed for PyROOT) */
    size_t GetV1290ArraySize() const;

    /** Sets V1290Array */
    void SetV1290Array(V1290Array V1290);

  private:
    V1290Array _V1290;

    ClassDef(Unknown, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_UNKNOWN_

