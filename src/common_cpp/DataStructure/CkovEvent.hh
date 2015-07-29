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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CKOVEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CKOVEVENT_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/CkovDigit.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<CkovDigit> CkovDigitArray;

/** @class CkovEvent comment
 *
 *  @var ckov_digits  <--description-->
 */

class CkovEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    CkovEvent();

    /** Copy constructor - any pointers are deep copied */
    CkovEvent(const CkovEvent& _ckovevent);

    /** Equality operator - any pointers are deep copied */
    CkovEvent& operator=(const CkovEvent& _ckovevent);

    /** Destructor - any member pointers are deleted */
    virtual ~CkovEvent();


    /** Returns CkovDigitArray */
    CkovDigitArray GetCkovDigitArray() const;

    /** Returns CkovDigitArray Pointer */
    CkovDigitArray* GetCkovDigitArrayPtr();

    /** Get an element from CkovDigitArray (needed for PyROOT) */
    CkovDigit GetCkovDigitArrayElement(size_t index) const;

    /** Get size of CkovDigitArray (needed for PyROOT) */
    size_t GetCkovDigitArraySize() const;

    /** Sets CkovDigitArray */
    void SetCkovDigitArray(CkovDigitArray ckov_digits);

  private:
    CkovDigitArray _ckov_digits;

    MAUS_VERSIONED_CLASS_DEF(CkovEvent)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_CKOVEVENT_

