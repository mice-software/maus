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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPILLDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPILLDATA_HH_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRPlaneHit.hh"

namespace MAUS {

typedef std::vector<EMRPlaneHit*> EMRPlaneHitArray;

/** @class EMREvent comment
 *  Class for EMR digits on the spill level
 */

class EMRSpillData {
  public:
    /** Default constructor - initialises to 0/NULL */
    EMRSpillData();

    /** Copy constructor - any pointers are deep copied */
    EMRSpillData(const EMRSpillData& emrspilldata);

    /** Equality operator - any pointers are deep copied */
    EMRSpillData& operator=(const EMRSpillData& emrspilldata);

    /** Destructor - any member pointers are deleted */
    virtual ~EMRSpillData();

    /** Returns  */
    EMRPlaneHitArray GetEMRPlaneHitArray() const;

    /** Sets  */
    void SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehitarray);

  private:
    EMRPlaneHitArray _emrplanehitarray;

    MAUS_VERSIONED_CLASS_DEF(EMRSpillData)
};
}

#endif
