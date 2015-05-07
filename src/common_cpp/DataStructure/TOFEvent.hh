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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENT_

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/TOFEventDigit.hh"
#include "src/common_cpp/DataStructure/TOFEventSlabHit.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"

namespace MAUS {

/** @class TOFEvent comment
 *
 *  @var tof_slab_hits  <--description-->
 *  @var tof_space_points  <--description-->
 *  @var tof_digits  <--description-->
 */

class TOFEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFEvent();

    /** Copy constructor - any pointers are deep copied */
    TOFEvent(const TOFEvent& _tofevent);

    /** Equality operator - any pointers are deep copied */
    TOFEvent& operator=(const TOFEvent& _tofevent);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFEvent();


    /** Returns TOFEventSlabHit */
    TOFEventSlabHit GetTOFEventSlabHit() const;

    /** Returns TOFEventSlabHit pointer*/
    TOFEventSlabHit* GetTOFEventSlabHitPtr();

    /** Sets TOFEventSlabHit */
    void SetTOFEventSlabHit(TOFEventSlabHit tof_slab_hits);

    /** Returns TOFEventSpacePoint */
    TOFEventSpacePoint GetTOFEventSpacePoint() const;

    /** Returns TOFEventSpacePoint pointer*/
    TOFEventSpacePoint* GetTOFEventSpacePointPtr();

    /** Sets TOFEventSpacePoint */
    void SetTOFEventSpacePoint(TOFEventSpacePoint tof_space_points);

    /** Returns TOFEventDigit */
    TOFEventDigit GetTOFEventDigit() const;

    /** Returns TOFEventDigit pointer*/
    TOFEventDigit* GetTOFEventDigitPtr();

    /** Sets TOFEventDigit */
    void SetTOFEventDigit(TOFEventDigit tof_digits);

  private:
    TOFEventSlabHit _tof_slab_hits;
    TOFEventSpacePoint _tof_space_points;
    TOFEventDigit _tof_digits;

    MAUS_VERSIONED_CLASS_DEF(TOFEvent)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFEVENT_

