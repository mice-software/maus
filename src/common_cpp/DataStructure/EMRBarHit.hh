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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRBARHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRBARHIT_

#include "Utils/VersionNumber.hh"

namespace MAUS {

/** @class EMRBarHit comment
 *
 *  @var tot  <-- time over threshold measurement-->
 *  @var deltat  <-- time of a hit minus trigger hit time measured by DBB -->
 */

class EMRBarHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    EMRBarHit();

    /** Copy constructor - any pointers are deep copied */
    EMRBarHit(const EMRBarHit& _emrbarhit);

    /** Equality operator - any pointers are deep copied */
    EMRBarHit& operator=(const EMRBarHit& _emrbarhit);

    /** Destructor - any member pointers are deleted */
    virtual ~EMRBarHit();

    /** Returns Time Over Threshold */
    int GetTot() const;

    /** Sets Time Over Threshold */
    void SetTot(int tot);

    /** Returns time from closest trigger */
    int GetDeltaT() const;

    /** Sets time from closest trigger */
    void SetDeltaT(int deltat);

  private:
    int _tot;
    int _deltat;

    MAUS_VERSIONED_CLASS_DEF(EMRBarHit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_EMRBARHIT_

