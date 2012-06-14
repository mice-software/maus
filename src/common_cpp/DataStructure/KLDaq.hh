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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLDAQ_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLDAQ_

#include <vector>

#include "Rtypes.h"  // ROOT

#include "src/common_cpp/DataStructure/V1724.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<V1724> V1724Array;

/** @class KLDaq comment
 *
 *  @var V1724  <--description-->
 */

class KLDaq {
  public:
    /** Default constructor - initialises to 0/NULL */
    KLDaq();

    /** Copy constructor - any pointers are deep copied */
    KLDaq(const KLDaq& _kldaq);

    /** Equality operator - any pointers are deep copied */
    KLDaq& operator=(const KLDaq& _kldaq);

    /** Destructor - any member pointers are deleted */
    virtual ~KLDaq();


    /** Returns V1724Array */
    V1724Array GetV1724Array() const;

    /** Get an element from V1724Array (needed for PyROOT) */
    V1724 GetV1724ArrayElement(size_t index) const;

    /** Get size of V1724Array (needed for PyROOT) */
    size_t GetV1724ArraySize() const;

    /** Sets V1724Array */
    void SetV1724Array(V1724Array V1724);

  private:
    V1724Array _V1724;

    ClassDef(KLDaq, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_KLDAQ_

