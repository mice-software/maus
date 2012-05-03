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

#include "src/common_cpp/JsonCppProcessors/V1290Processor.hh"

namespace MAUS {

V1290Processor::V1290Processor()
    : _bunch_id_proc(), _ldc_id_proc(), _equip_type_proc(),
      _phys_event_number_proc(), _trailing_time_proc(), _channel_key_proc(),
      _leading_time_proc(), _trigger_time_tag_proc(), _time_stamp_proc(),
      _detector_proc(), _part_event_number_proc(), _geo_proc(),
      _channel_proc() {
    RegisterValueBranch
          ("bunch_id", &_bunch_id_proc, &V1290::GetBunchId,
          &V1290::SetBunchId, true);
    RegisterValueBranch
          ("ldc_id", &_ldc_id_proc, &V1290::GetLdcId,
          &V1290::SetLdcId, true);
    RegisterValueBranch
          ("equip_type", &_equip_type_proc, &V1290::GetEquipType,
          &V1290::SetEquipType, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &V1290::GetPhysEventNumber,
          &V1290::SetPhysEventNumber, true);
    RegisterValueBranch
          ("trailing_time", &_trailing_time_proc, &V1290::GetTrailingTime,
          &V1290::SetTrailingTime, true);
    RegisterValueBranch
          ("channel_key", &_channel_key_proc, &V1290::GetChannelKey,
          &V1290::SetChannelKey, false);
    RegisterValueBranch
          ("leading_time", &_leading_time_proc, &V1290::GetLeadingTime,
          &V1290::SetLeadingTime, true);
    RegisterValueBranch
          ("trigger_time_tag", &_trigger_time_tag_proc, &V1290::GetTriggerTimeTag,
          &V1290::SetTriggerTimeTag, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &V1290::GetTimeStamp,
          &V1290::SetTimeStamp, true);
    RegisterValueBranch
          ("detector", &_detector_proc, &V1290::GetDetector,
          &V1290::SetDetector, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &V1290::GetPartEventNumber,
          &V1290::SetPartEventNumber, true);
    RegisterValueBranch
          ("geo", &_geo_proc, &V1290::GetGeo,
          &V1290::SetGeo, true);
    RegisterValueBranch
          ("channel", &_channel_proc, &V1290::GetChannel,
          &V1290::SetChannel, true);
}
}  // namespace MAUS


