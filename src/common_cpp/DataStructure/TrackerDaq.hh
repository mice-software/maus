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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TRACKERDAQ_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TRACKERDAQ_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/VLSB.hh"
#include "src/common_cpp/DataStructure/VLSB_C.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<VLSB> VLSBArray;
typedef std::vector<VLSB_C> VLSB_CArray;
/** @class TrackerDaq comment
 *
 *  @var VLSB  <--description-->
 */

class TrackerDaq {
  public:
    /** Default constructor - initialises to 0/NULL */
    TrackerDaq();

    /** Copy constructor - any pointers are deep copied */
    TrackerDaq(const TrackerDaq& _TrackerDaq);

    /** Equality operator - any pointers are deep copied */
    TrackerDaq& operator=(const TrackerDaq& _TrackerDaq);

    /** Destructor - any member pointers are deleted */
    virtual ~TrackerDaq();

    /** Returns VLSBArray */
    VLSBArray GetVLSBArray() const;

    /** Get an element from VLSBArray (needed for PyROOT) */
    VLSB GetVLSBArrayElement(size_t index) const;

    /** Get size of VLSBArray (needed for PyROOT) */
    size_t GetVLSBArraySize() const;

    /** Sets VLSBArray */
    void SetVLSBArray(VLSBArray vlsb);

    /** Returns VLSB_CArray */
    VLSB_CArray GetVLSB_CArray() const;

    /** Get an element from VLSBArray (needed for PyROOT) */
    VLSB_C GetVLSB_CArrayElement(size_t index) const;

    /** Get size of VLSBArray (needed for PyROOT) */
    size_t GetVLSB_CArraySize() const;

    /** Sets VLSBArray */
    void SetVLSB_CArray(VLSB_CArray vlsb_c);

  private:
    VLSBArray   _VLSB;
    VLSB_CArray _VLSB_C;

    MAUS_VERSIONED_CLASS_DEF(TrackerDaq)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TRACKERDAQ_

