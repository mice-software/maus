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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EpicsInterface_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EpicsInterface_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/ParticleTrigger.hh"

namespace MAUS {

/** @class EpicsInterface comment
 *
 *  @var ldc_id  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var time_stamp  <--description-->
 *  @var geo  <--description-->
 *  @var pv_id  <--description-->
 *  @var pv_value  <--description-->
 */

class EpicsInterface {
  public:
    /** Default constructor - initialises to 0/NULL */
    EpicsInterface();

    /** Copy constructor - any pointers are deep copied */
    EpicsInterface(const EpicsInterface& ei);

    /** Equality operator - any pointers are deep copied */
    EpicsInterface& operator=(const EpicsInterface& ei);

    /** Destructor - any member pointers are deleted */
    virtual ~EpicsInterface();


    /** Returns LdcId */
    int GetLdcId() const;

    /** Sets LdcId */
    void SetLdcId(int ldc_id);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns Geo */
    int GetGeo() const;

    /** Sets Geo */
    void SetGeo(int geo);

    /** Adds a Hall probe value */
    void AddHallProbe(int pvId, double v);

    /** Get the number of PV recorded */
    size_t GetHallProbeArraySize() {return _pv_id.size();}

    /** Get the vector of PV Id numbers */
    std::vector<int> GetPvIdArray() {return _pv_id;}

    /** Get a pointer to the vector of PVs Id numbers */
    std::vector<int>* GetPvIdArrayPtr() {return &_pv_id;}

    /** Set the vector of PV Id numbers */
    void SetPvIdArray(std::vector<int> v) {_pv_id = v;}

    /** Get the vector of PV values */
    std::vector<double> GetPvValueArray() {return _pv_value;}

    /** Get a pointer the vector of PV values */
    std::vector<double>* GetPvValueArrayPtr() {return &_pv_value;}

    /** Set the vector of PV values */
    void SetPvValueArray(std::vector<double> v) {_pv_value = v;}

    /** Get the PV Id number */
    int GetPvId(int i)  {return _pv_id[i];}

    /** Get the PV value */
    double GetPvValue(int i) const {return _pv_value[i];}

  private:
    int _ldc_id;
    int _phys_event_number;
    int _geo;
    std::vector<int> _pv_id;
    std::vector<double> _pv_value;

    MAUS_VERSIONED_CLASS_DEF(EpicsInterface)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_EpicsInterface_

