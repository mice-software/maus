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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENTCELLHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENTCELLHIT_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/KLCellHit.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<KLCellHit> KLCellHitArray;

/** @class KLEventCellHit comment
 *
 *  @var kl  <--description-->
 */

class KLEventCellHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    KLEventCellHit();

    /** Copy constructor - any pointers are deep copied */
    KLEventCellHit(const KLEventCellHit& _kleventcellhit);

    /** Equality operator - any pointers are deep copied */
    KLEventCellHit& operator=(const KLEventCellHit& _kleventcellhit);

    /** Destructor - any member pointers are deleted */
    virtual ~KLEventCellHit();


    /** Returns KLCellHitArray */
    KLCellHitArray GetKLCellHitArray() const;

    /** Returns KLCellHitArray */
    KLCellHitArray* GetKLCellHitArrayPtr();

    /** Get an element from KLCellHitArray (needed for PyROOT) */
    KLCellHit GetKLCellHitArrayElement(size_t index) const;

    /** Get size of KLCellHitArray (needed for PyROOT) */
    size_t GetKLCellHitArraySize() const;

    /** Sets KLCellHitArray */
    void SetKLCellHitArray(KLCellHitArray kl);

  private:
    KLCellHitArray _kl;

    MAUS_VERSIONED_CLASS_DEF(KLEventCellHit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENTSLABHIT_

