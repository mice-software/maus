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

#include <string>

#include "DataStructure/DBBSpillData.hh"

namespace MAUS {

DBBSpillData::DBBSpillData()
: _ldcId(0), _dbbId(0), _spill_number(0), _spill_width(0), _trigger_count(0),
  _hit_count(0), _time_stamp(0), _detector("unknown"), _dbb_hits(0), _dbb_triggers(0) {
}

DBBSpillData::DBBSpillData(const DBBSpillData& dbbspill)
: _ldcId(0), _dbbId(0), _spill_number(0), _spill_width(0), _trigger_count(0),
  _hit_count(0), _time_stamp(0), _detector("unknown"), _dbb_hits(0), _dbb_triggers(0)  {
  *this = dbbspill;
}

DBBSpillData& DBBSpillData::operator=(const DBBSpillData& dbbspill) {
  if (&dbbspill == this) {
      return *this;
  }

  SetLdcId(dbbspill._ldcId);
  SetDBBId(dbbspill._dbbId);
  SetSpillNumber(dbbspill._spill_number);
  SetSpillWidth(dbbspill._spill_width);
  SetTriggerCount(dbbspill._trigger_count);
  SetHitCount(dbbspill._hit_count);
  SetTimeStamp(dbbspill._time_stamp);
  SetDetector(dbbspill._detector);
  SetDBBHitsArray(dbbspill._dbb_hits);
  SetDBBTriggersArray(dbbspill._dbb_triggers);

  return *this;
}

DBBSpillData::~DBBSpillData() {
//   int n = _dbb_hits.size();
//   for (int i=0; i<n; i++)
//     delete _dbb_hits[i];

  _dbb_hits.resize(0);

//   n = _dbb_triggers.size();
//   for (int i=0; i<n; i++)
//     delete _dbb_triggers[i];

  _dbb_triggers.resize(0);
}

int DBBSpillData::GetLdcId() const {
  return _ldcId;
}

void DBBSpillData::SetLdcId(int id) {
  _ldcId = id;
}

int DBBSpillData::GetDBBId() const {
  return _dbbId;
}

void DBBSpillData::SetDBBId(int id) {
  _dbbId = id;
}

int DBBSpillData::GetSpillNumber() const {
  return _spill_number;
}

void DBBSpillData::SetSpillNumber(int n) {
  _spill_number = n;
}

int DBBSpillData::GetSpillWidth() const {
  return _spill_width;
}

void DBBSpillData::SetSpillWidth(int w) {
  _spill_width = w;
}

int DBBSpillData::GetTriggerCount() const {
  return _trigger_count;
}

void DBBSpillData::SetTriggerCount(int tc) {
  _trigger_count = tc;
}

int DBBSpillData::GetHitCount() const {
  return _hit_count;
}

void DBBSpillData::SetHitCount(int hc) {
  _hit_count = hc;
}

int DBBSpillData::GetTimeStamp() const {
    return _time_stamp;
}

void DBBSpillData::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

std::string DBBSpillData::GetDetector() const {
    return _detector;
}

void DBBSpillData::SetDetector(std::string detector) {
    _detector = detector;
}

DBBHitsArray DBBSpillData::GetDBBHitsArray() const {
  return _dbb_hits;
}

DBBHit DBBSpillData::GetDBBHitsArrayElement(size_t index) const {
  return _dbb_hits[index];
}

size_t DBBSpillData::GetDBBHitsArraySize() const {
  return _dbb_hits.size();
}

void DBBSpillData::SetDBBHitsArray(DBBHitsArray ha) {
//   for (size_t i = 0; i < _dbb_hits.size(); ++i) {
//     if (_dbb_hits[i] != NULL) {
//       delete _dbb_hits[i];
//     }
//   }
  _dbb_hits.resize(0);
  _dbb_hits = ha;
}


DBBHitsArray DBBSpillData::GetDBBTriggersArray() const {
  return _dbb_triggers;
}

DBBHit DBBSpillData::GetDBBTriggersArrayElement(size_t index) const {
  return _dbb_triggers[index];
}

size_t DBBSpillData::GetDBBTriggersArraySize() const {
  return _dbb_triggers.size();
}

void DBBSpillData::SetDBBTriggersArray(DBBHitsArray ta) {
//   for (size_t i = 0; i < _dbb_triggers.size(); ++i) {
//     if (_dbb_triggers[i] != NULL) {
//       delete _dbb_triggers[i];
//     }
//   }
  _dbb_triggers.resize(0);
  _dbb_triggers = ta;
}
}



