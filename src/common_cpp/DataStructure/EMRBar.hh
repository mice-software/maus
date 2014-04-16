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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRBAR_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRBAR_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRBarHit.hh"

namespace MAUS {

typedef std::vector<EMRBarHit> EMRBarHitArray;

/** @class EMRBar comment
 *
 *  @var bar  <-- bar id -->
 *  @var emrbarhitarray  <-- array of hit in the plane -->
 */

class EMRBar {
  public:
    /** Default constructor - initialises to 0/NULL **/
    EMRBar();

    /** Copy constructor - any pointers are deep copied **/
    EMRBar(const EMRBar& _emrbar);

    /** Equality operator - any pointers are deep copied **/
    EMRBar& operator=(const EMRBar& _emrbar);

    /** Destructor - any member pointers are deleted **/
    virtual ~EMRBar();

    /** Returns Bar **/
    int GetBar() const;

    /** Sets Bar **/
    void SetBar(int bar);

    /** Returns number of hits **/
    int GetNHits() const;

    /** Returns  **/
    EMRBarHitArray GetEMRBarHitArray() const;

    /** Sets  **/
    void SetEMRBarHitArray(EMRBarHitArray emrbarhitarray);

    /** Add one EMRBarHit **/
    void AddBarHit(EMRBarHit bHit);

  private:
    int _bar;
    EMRBarHitArray _emrbarhitarray;

    MAUS_VERSIONED_CLASS_DEF(EMRBar)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_EMRBAR_

