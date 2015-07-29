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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENTDIGIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENTDIGIT_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/KLDigit.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<KLDigit> KLDigitArray;

/** @class KLEventDigit comment
 *
 *  @var kl  <--description-->
 */

class KLEventDigit {
  public:
    /** Default constructor - initialises to 0/NULL */
    KLEventDigit();

    /** Copy constructor - any pointers are deep copied */
    KLEventDigit(const KLEventDigit& _kleventdigit);

    /** Equality operator - any pointers are deep copied */
    KLEventDigit& operator=(const KLEventDigit& _kleventdigit);

    /** Destructor - any member pointers are deleted */
    virtual ~KLEventDigit();


    /** Returns KLDigitArray */
    KLDigitArray GetKLDigitArray() const;

    /** Returns KLDigitArray */
    KLDigitArray* GetKLDigitArrayPtr();

    /** Get an element from KLDigitArray (needed for PyROOT) */
    KLDigit GetKLDigitArrayElement(size_t index) const;

    /** Get size of KLDigitArray (needed for PyROOT) */
    size_t GetKLDigitArraySize() const;

    /** Sets KLDigitArray */
    void SetKLDigitArray(KLDigitArray kl);

  private:
    KLDigitArray _kl;

    MAUS_VERSIONED_CLASS_DEF(KLEventDigit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENTDIGIT_

