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

#include "src/common_cpp/JsonCppProcessors/DBBHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DBBSpillDataProcessor.hh"

namespace MAUS {
DBBSpillDataProcessor::DBBSpillDataProcessor() : _int_proc(), _string_proc(),
                                         _dbb_hit_proc(new DBBHitProcessor()) {
    RegisterValueBranch
          ("ldc_id", &_int_proc, &DBBSpillData::GetLdcId,
          &DBBSpillData::SetLdcId, true);
    RegisterValueBranch
          ("dbb_id", &_int_proc, &DBBSpillData::GetDBBId,
          &DBBSpillData::SetDBBId, true);
    RegisterValueBranch
          ("spill_number", &_int_proc, &DBBSpillData::GetSpillNumber,
          &DBBSpillData::SetSpillNumber, true);
    RegisterValueBranch
          ("spill_width", &_int_proc, &DBBSpillData::GetSpillWidth,
          &DBBSpillData::SetSpillWidth, true);
    RegisterValueBranch
          ("trigger_count", &_int_proc, &DBBSpillData::GetTriggerCount,
          &DBBSpillData::SetTriggerCount, true);
    RegisterValueBranch
          ("hit_count", &_int_proc, &DBBSpillData::GetHitCount,
          &DBBSpillData::SetHitCount, true);
    RegisterValueBranch
          ("time_stamp", &_int_proc, &DBBSpillData::GetTimeStamp,
          &DBBSpillData::SetTimeStamp, true);
    RegisterValueBranch
          ("detector", &_string_proc, &DBBSpillData::GetDetector,
          &DBBSpillData::SetDetector, true);
    RegisterValueBranch
          ("dbb_hits", &_dbb_hit_proc, &DBBSpillData::GetDBBHitsArray,
          &DBBSpillData::SetDBBHitsArray, true);
    RegisterValueBranch
          ("dbb_triggers", &_dbb_hit_proc,
          &DBBSpillData::GetDBBTriggersArray,
          &DBBSpillData::SetDBBTriggersArray, true);
}

}  // namespace MAUS

