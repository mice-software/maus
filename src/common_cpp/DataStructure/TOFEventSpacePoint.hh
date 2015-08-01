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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTSPACEPOINT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTSPACEPOINT_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"

namespace MAUS {
// Needed for ROOT
typedef std::vector<TOFSpacePoint> TOF0SpacePointArray;
typedef std::vector<TOFSpacePoint> TOF1SpacePointArray;
typedef std::vector<TOFSpacePoint> TOF2SpacePointArray;

/** @class TOFEventSpacePoint comment
 *
 *  @var tof0  <--description-->
 *  @var tof1  <--description-->
 *  @var tof2  <--description-->
 */

class TOFEventSpacePoint {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFEventSpacePoint();

    /** Copy constructor - any pointers are deep copied */
    TOFEventSpacePoint(const TOFEventSpacePoint& _tofeventspacepoint);

    /** Equality operator - any pointers are deep copied */
    TOFEventSpacePoint& operator=(const TOFEventSpacePoint& _tofeventspacepoint);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFEventSpacePoint();

    /** Returns TOF0SpacePointArray */
    TOF0SpacePointArray GetTOF0SpacePointArray() const;

    /** Returns TOF0SpacePointArray */
    TOF0SpacePointArray* GetTOF0SpacePointArrayPtr();

    /** Get an element from TOF0SpacePointArray (needed for PyROOT) */
    TOFSpacePoint GetTOF0SpacePointArrayElement(size_t index) const;

    /** Get size of TOF0SpacePointArray (needed for PyROOT) */
    size_t GetTOF0SpacePointArraySize() const;

    /** Sets TOF0SpacePointArray */
    void SetTOF0SpacePointArray(TOF1SpacePointArray tof1);

    /** Returns TOF1SpacePointArray */
    TOF1SpacePointArray GetTOF1SpacePointArray() const;

    /** Returns TOF1SpacePointArray */
    TOF1SpacePointArray* GetTOF1SpacePointArrayPtr();

    /** Get an element from TOF1SpacePointArray (needed for PyROOT) */
    TOFSpacePoint GetTOF1SpacePointArrayElement(size_t index) const;

    /** Get size of TOF1SpacePointArray (needed for PyROOT) */
    size_t GetTOF1SpacePointArraySize() const;

    /** Sets TOF1SpacePointArray */
    void SetTOF1SpacePointArray(TOF1SpacePointArray tof1);

    /** Returns TOF2SpacePointArray */
    TOF2SpacePointArray GetTOF2SpacePointArray() const;

    /** Returns TOF2SpacePointArray */
    TOF2SpacePointArray* GetTOF2SpacePointArrayPtr();

    /** Get an element from TOF2SpacePointArray (needed for PyROOT) */
    TOFSpacePoint GetTOF2SpacePointArrayElement(size_t index) const;

    /** Get size of TOF2SpacePointArray (needed for PyROOT) */
    size_t GetTOF2SpacePointArraySize() const;

    /** Sets TOF2SpacePointArray */
    void SetTOF2SpacePointArray(TOF2SpacePointArray tof2);

  private:
    TOF0SpacePointArray _tof0;
    TOF1SpacePointArray _tof1;
    TOF2SpacePointArray _tof2;

    MAUS_VERSIONED_CLASS_DEF(TOFEventSpacePoint)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENTSPACEPOINT_

