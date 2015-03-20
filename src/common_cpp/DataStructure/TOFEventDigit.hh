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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTDIGIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTDIGIT_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/TOFDigit.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<TOFDigit> TOF0DigitArray;
typedef std::vector<TOFDigit> TOF2DigitArray;
typedef std::vector<TOFDigit> TOF1DigitArray;

/** @class TOFEventDigit comment
 *
 *  @var tof1  <--description-->
 *  @var tof0  <--description-->
 *  @var tof2  <--description-->
 */

class TOFEventDigit {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFEventDigit();

    /** Copy constructor - any pointers are deep copied */
    TOFEventDigit(const TOFEventDigit& _tofeventdigit);

    /** Equality operator - any pointers are deep copied */
    TOFEventDigit& operator=(const TOFEventDigit& _tofeventdigit);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFEventDigit();


    /** Returns TOF1DigitArray */
    TOF1DigitArray GetTOF1DigitArray() const;

    /** Returns TOF1DigitArray pointer*/
    TOF1DigitArray* GetTOF1DigitArrayPtr();

    /** Get an element from TOF1DigitArray (needed for PyROOT) */
    TOFDigit GetTOF1DigitArrayElement(size_t index) const;

    /** Get size of TOF1DigitArray (needed for PyROOT) */
    size_t GetTOF1DigitArraySize() const;

    /** Sets TOF1DigitArray */
    void SetTOF1DigitArray(TOF1DigitArray tof1);

    /** Returns TOF0DigitArray */
    TOF0DigitArray GetTOF0DigitArray() const;

    /** Returns TOF0DigitArray pointer*/
    TOF0DigitArray* GetTOF0DigitArrayPtr();

    /** Get an element from TOF0DigitArray (needed for PyROOT) */
    TOFDigit GetTOF0DigitArrayElement(size_t index) const;

    /** Get size of TOF0DigitArray (needed for PyROOT) */
    size_t GetTOF0DigitArraySize() const;

    /** Sets TOF0DigitArray */
    void SetTOF0DigitArray(TOF0DigitArray tof0);

    /** Returns TOF2DigitArray */
    TOF2DigitArray GetTOF2DigitArray() const;

    /** Returns TOF2DigitArray pointer*/
    TOF2DigitArray* GetTOF2DigitArrayPtr();

    /** Get an element from TOF2DigitArray (needed for PyROOT) */
    TOFDigit GetTOF2DigitArrayElement(size_t index) const;

    /** Get size of TOF2DigitArray (needed for PyROOT) */
    size_t GetTOF2DigitArraySize() const;

    /** Sets TOF2DigitArray */
    void SetTOF2DigitArray(TOF2DigitArray tof2);

  private:
    TOF1DigitArray _tof1;
    TOF0DigitArray _tof0;
    TOF2DigitArray _tof2;

    MAUS_VERSIONED_CLASS_DEF(TOFEventDigit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTDIGIT_

