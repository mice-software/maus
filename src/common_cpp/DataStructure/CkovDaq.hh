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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CKOVDAQ_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CKOVDAQ_

#include <vector>

#include "Rtypes.h"  // ROOT

#include "src/common_cpp/DataStructure/V1731.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<V1731> V1731Array;

/** @class CkovDaq comment
 *
 *  @var V1731  <--description-->
 */

class CkovDaq {
  public:
    /** Default constructor - initialises to 0/NULL */
    CkovDaq();

    /** Copy constructor - any pointers are deep copied */
    CkovDaq(const CkovDaq& _ckovdaq);

    /** Equality operator - any pointers are deep copied */
    CkovDaq& operator=(const CkovDaq& _ckovdaq);

    /** Destructor - any member pointers are deleted */
    virtual ~CkovDaq();


    /** Returns V1731Array */
    V1731Array GetV1731Array() const;

    /** Get an element from V1731Array (needed for PyROOT) */
    V1731 GetV1731ArrayElement(size_t index) const;

    /** Get size of V1731Array (needed for PyROOT) */
    size_t GetV1731ArraySize() const;

    /** Sets V1731Array */
    void SetV1731Array(V1731Array V1731);

  private:
    V1731Array _V1731;

    ClassDef(CkovDaq, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_CKOVDAQ_

