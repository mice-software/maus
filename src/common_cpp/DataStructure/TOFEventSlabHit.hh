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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTSLABHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTSLABHIT_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/TOFSlabHit.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<TOFSlabHit> TOF0SlabHitArray;
typedef std::vector<TOFSlabHit> TOF2SlabHitArray;
typedef std::vector<TOFSlabHit> TOF1SlabHitArray;

/** @class TOFEventSlabHit comment
 *
 *  @var tof1  <--description-->
 *  @var tof0  <--description-->
 *  @var tof2  <--description-->
 */

class TOFEventSlabHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFEventSlabHit();

    /** Copy constructor - any pointers are deep copied */
    TOFEventSlabHit(const TOFEventSlabHit& _tofeventslabhit);

    /** Equality operator - any pointers are deep copied */
    TOFEventSlabHit& operator=(const TOFEventSlabHit& _tofeventslabhit);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFEventSlabHit();


    /** Returns TOF1SlabHitArray */
    TOF1SlabHitArray GetTOF1SlabHitArray() const;

    /** Returns TOF1SlabHitArray pointer*/
    TOF1SlabHitArray* GetTOF1SlabHitArrayPtr();

    /** Get an element from TOF1SlabHitArray (needed for PyROOT) */
    TOFSlabHit GetTOF1SlabHitArrayElement(size_t index) const;

    /** Get size of TOF1SlabHitArray (needed for PyROOT) */
    size_t GetTOF1SlabHitArraySize() const;

    /** Sets TOF1SlabHitArray */
    void SetTOF1SlabHitArray(TOF1SlabHitArray tof1);

    /** Returns TOF0SlabHitArray */
    TOF0SlabHitArray GetTOF0SlabHitArray() const;

    /** Returns TOF0SlabHitArray pointer*/
    TOF0SlabHitArray* GetTOF0SlabHitArrayPtr();

    /** Get an element from TOF0SlabHitArray (needed for PyROOT) */
    TOFSlabHit GetTOF0SlabHitArrayElement(size_t index) const;

    /** Get size of TOF0SlabHitArray (needed for PyROOT) */
    size_t GetTOF0SlabHitArraySize() const;

    /** Sets TOF0SlabHitArray */
    void SetTOF0SlabHitArray(TOF0SlabHitArray tof0);

    /** Returns TOF2SlabHitArray */
    TOF2SlabHitArray GetTOF2SlabHitArray() const;

    /** Returns TOF2SlabHitArray pointer*/
    TOF2SlabHitArray* GetTOF2SlabHitArrayPtr();

    /** Get an element from TOF2SlabHitArray (needed for PyROOT) */
    TOFSlabHit GetTOF2SlabHitArrayElement(size_t index) const;

    /** Get size of TOF2SlabHitArray (needed for PyROOT) */
    size_t GetTOF2SlabHitArraySize() const;

    /** Sets TOF2SlabHitArray */
    void SetTOF2SlabHitArray(TOF2SlabHitArray tof2);

  private:
    TOF1SlabHitArray _tof1;
    TOF0SlabHitArray _tof0;
    TOF2SlabHitArray _tof2;

    MAUS_VERSIONED_CLASS_DEF(TOFEventSlabHit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTSLABHIT_

