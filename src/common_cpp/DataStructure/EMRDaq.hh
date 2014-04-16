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


#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRDAQ_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRDAQ_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/V1731.hh"
#include "DataStructure/DBBSpillData.hh"

namespace MAUS {

typedef std::vector<V1731>           V1731HitArray;
typedef std::vector<V1731HitArray>   V1731PartEventArray;
typedef std::vector<DBBSpillData>    DBBArray;

class EMRDaq {
  public:
    /** Default constructor - initialises to 0/NULL */
    EMRDaq();

    /** Copy constructor - any pointers are deep copied */
    EMRDaq(const EMRDaq& _emrdaq);

    /** Equality operator - any pointers are deep copied */
    EMRDaq& operator=(const EMRDaq& _emrdaq);

    /** Destructor - any member pointers are deleted */
    virtual ~EMRDaq();

    /** Returns V1731PartEventArray */
    V1731PartEventArray GetV1731PartEventArray() const;

    /** Returns V1731PartEventArray size */
    size_t GetV1731NumPartEvents() const;

    /** Get an element from V1731Array (needed for PyROOT) */
    V1731HitArray GetV1731PartEvent(size_t index) const;

    /** Get size of V1731Array (needed for PyROOT) */
    size_t GetV1731PartEventArraySize(size_t index) const;

    /** Sets V1731Array */
    void SetV1731PartEventArray(V1731PartEventArray V1731);

    /** Returns DBB Spill */
    DBBArray GetDBBArray() const;

    /** Get an element from V1731Array (needed for PyROOT) */
    DBBSpillData GetDBBArrayElement(size_t index) const;

    /** Get size of V1731Array (needed for PyROOT) */
    size_t GetDBBArraySize() const;

    /** Sets Dbb Spill */
    void SetDBBArray(DBBArray s);

  private:
    V1731PartEventArray _V1731;
    DBBArray            _dbb;

    MAUS_VERSIONED_CLASS_DEF(EMRDaq)
};
}
#endif

