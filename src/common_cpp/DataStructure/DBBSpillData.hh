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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_DBBSPILLDATA_
#define _SRC_COMMON_CPP_DATASTRUCTURE_DBBSPILLDATA_

#include <vector>
#include <string>

#include "Utils/VersionNumber.hh"
#include "DataStructure/DBBHit.hh"

namespace MAUS {

typedef std::vector<DBBHit> DBBHitsArray;

class DBBSpillData {
  public:
    DBBSpillData();

    DBBSpillData(const DBBSpillData& dbbspill);

    DBBSpillData& operator=(const DBBSpillData& dbbspill);

    virtual ~DBBSpillData();

    /** Returns Ldc Id */
    int GetLdcId() const;

    /** Sets Ldc Id */
    void SetLdcId(int id);

    /** Returns DB BId */
    int GetDBBId() const;

    /** Sets DBB Id */
    void SetDBBId(int id);

    /** Returns Spill Number */
    int GetSpillNumber() const;

    /** Sets Spill Number */
    void SetSpillNumber(int n);

    /** Returns Spill Width */
    int GetSpillWidth() const;

    /** Sets Spill Width */
    void SetSpillWidth(int w);

    /** Returns Trigger Count */
    int GetTriggerCount() const;

    /** Sets Trigger Count */
    void SetTriggerCount(int tc);

    /** Returns Hit Count */
    int GetHitCount() const;

    /** Sets Hit Count */
    void SetHitCount(int hc);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

    /** Returns Detector */
    std::string GetDetector() const;

    /** Sets Detector */
    void SetDetector(std::string detector);

    /** Returns DBB Hit Array */
    DBBHitsArray GetDBBHitsArray() const;

    /** Get an element from DBB Hit Array (needed for PyROOT) */
    DBBHit GetDBBHitsArrayElement(size_t index) const;

    /** Get size of EMRHitsArray (needed for PyROOT) */
    size_t GetDBBHitsArraySize() const;

    /** Sets DBB Hit Array */
    void SetDBBHitsArray(DBBHitsArray ha);



    /** Returns DBB Trigger Array */
    DBBHitsArray GetDBBTriggersArray() const;

    /** Get an element from DBB Hit Array (needed for PyROOT) */
    DBBHit GetDBBTriggersArrayElement(size_t index) const;

    /** Get size of EMRDaqArray (needed for PyROOT) */
    size_t GetDBBTriggersArraySize() const;

    /** Sets DBB Hit Array */
    void SetDBBTriggersArray(DBBHitsArray ta);

    void print() {
      std::cout << "*** Spill DBB" << _dbbId << " ***\n  sn: " << _spill_number
                << "\n  sw: " << _spill_width
                << "\n  tc: " << _trigger_count << "\n  hc: "
                <<  _hit_count << "\n   hits: " << _dbb_hits.size()
                << "\n   triggers: " << _dbb_triggers.size() << std::endl;
    }

  private:

    int _ldcId;
    int _dbbId;
    int _spill_number;
    int _spill_width;
    int _trigger_count;
    int _hit_count;
    int _time_stamp;
    std::string _detector;

    DBBHitsArray _dbb_hits;
    DBBHitsArray _dbb_triggers;

    MAUS_VERSIONED_CLASS_DEF(DBBSpillData)
};
}

#endif
