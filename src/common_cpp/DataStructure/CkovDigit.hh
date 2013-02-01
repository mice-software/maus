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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CKOVDIGIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CKOVDIGIT_

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/CkovA.hh"
#include "src/common_cpp/DataStructure/CkovB.hh"

namespace MAUS {

/** @class CkovDigit comment
 *
 *  @var A  <--description-->
 *  @var B  <--description-->
 */

class CkovDigit {
  public:
    /** Default constructor - initialises to 0/NULL */
    CkovDigit();

    /** Copy constructor - any pointers are deep copied */
    CkovDigit(const CkovDigit& _ckovdigit);

    /** Equality operator - any pointers are deep copied */
    CkovDigit& operator=(const CkovDigit& _ckovdigit);

    /** Destructor - any member pointers are deleted */
    virtual ~CkovDigit();


    /** Returns CkovA */
    CkovA GetCkovA() const;

    /** Sets CkovA */
    void SetCkovA(CkovA A);

    /** Returns CkovB */
    CkovB GetCkovB() const;

    /** Sets CkovB */
    void SetCkovB(CkovB B);

  private:
    CkovA _A;
    CkovB _B;

    MAUS_VERSIONED_CLASS_DEF(CkovDigit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_CKOVDIGIT_

