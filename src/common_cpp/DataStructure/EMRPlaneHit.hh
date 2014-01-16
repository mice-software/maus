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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRPLANEHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRPLANEHIT_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRBar.hh"

namespace MAUS {

typedef std::vector<EMRBar *> EMRBarArray;

/** @class EMRPlaneHit comment
 *
 *  @var plane        <-- plane id / DBB id / fADC id -->
 *  @var orientation  <-- plane orientation, can be 0 or 1 -->
 *  @var emrbararray  <-- array of bars in the plane / 59 elements -->
 *  @var charge       <-- fADC charge -->
 *  @var time         <-- trigger time w.r.t. start of the spill recorded by DBB -->
 *  @var spill        <-- spill count recorded by DBB-->
 *  @var trigger      <-- trigger count recorded by DBB-->
 *  @var deltat       <-- time of a hit in fADC minus trigger hit time measured by fADC -->
 */

class EMRPlaneHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    EMRPlaneHit();

    /** Copy constructor - any pointers are deep copied */
    EMRPlaneHit(const EMRPlaneHit& _emrbarhit);

    /** Equality operator - any pointers are deep copied */
    EMRPlaneHit& operator=(const EMRPlaneHit& _emrbarhit);

    /** Destructor - any member pointers are deleted */
    virtual ~EMRPlaneHit();

    /** Returns  */
    int GetPlane() const;

    /** Sets  */
    void SetPlane(int plane);

    /** Returns  */
    EMRBarArray GetEMRBarArray() const;

    /** Sets  */
    void SetEMRBarArray(EMRBarArray emrbararray);

    /** Returns  */
    int GetCharge() const;

    /** Sets  */
    void SetCharge(int charge);

    /** Returns  */
    int GetTime() const;

    /** Sets  */
    void SetTime(int time);

    /** Returns  */
    int GetSpill() const;

    /** Sets  */
    void SetSpill(int spill);

    /** Returns  */
    int GetTrigger() const;

    /** Sets  */
    void SetTrigger(int trigger);

    /** Returns  */
    int GetDeltaT() const;

    /** Sets  */
    void SetDeltaT(int deltat);

    /** Returns  */
    int GetOrientation() const;

    /** Sets  */
    void SetOrientation(int orient);

  private:
    int _plane;
    int _orientation;
    EMRBarArray _emrbararray;
    int _charge;
    int _time;
    int _spill;
    int _trigger;
    int _deltat;

    MAUS_VERSIONED_CLASS_DEF(EMRPlaneHit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_EMRPLANEHIT_

