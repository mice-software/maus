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

#include "src/common_cpp/JsonCppProcessors/V1724Processor.hh"

namespace MAUS {

V1724Processor::V1724Processor()
    : _ldc_id_proc(), _charge_mm_proc(), _equip_type_proc(),
      _channel_key_proc(), _charge_pm_proc(), _phys_event_number_proc(),
      _position_max_proc(), _time_stamp_proc(), _trigger_time_tag_proc(),
      _detector_proc(), _part_event_number_proc(), _geo_proc(),
      _pedestal_proc(), _channel_proc() {
    RegisterValueBranch
          ("ldc_id", &_ldc_id_proc, &V1724::GetLdcId,
          &V1724::SetLdcId, true);
    RegisterValueBranch
          ("charge_mm", &_charge_mm_proc, &V1724::GetChargeMm,
          &V1724::SetChargeMm, true);
    RegisterValueBranch
          ("equip_type", &_equip_type_proc, &V1724::GetEquipType,
          &V1724::SetEquipType, true);
    RegisterValueBranch
          ("channel_key", &_channel_key_proc, &V1724::GetChannelKey,
          &V1724::SetChannelKey, true);
    RegisterValueBranch
          ("charge_pm", &_charge_pm_proc, &V1724::GetChargePm,
          &V1724::SetChargePm, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &V1724::GetPhysEventNumber,
          &V1724::SetPhysEventNumber, true);
    RegisterValueBranch
          ("position_max", &_position_max_proc, &V1724::GetPositionMax,
          &V1724::SetPositionMax, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &V1724::GetTimeStamp,
          &V1724::SetTimeStamp, true);
    RegisterValueBranch
          ("trigger_time_tag", &_trigger_time_tag_proc, &V1724::GetTriggerTimeTag,
          &V1724::SetTriggerTimeTag, true);
    RegisterValueBranch
          ("detector", &_detector_proc, &V1724::GetDetector,
          &V1724::SetDetector, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &V1724::GetPartEventNumber,
          &V1724::SetPartEventNumber, true);
    RegisterValueBranch
          ("geo", &_geo_proc, &V1724::GetGeo,
          &V1724::SetGeo, true);
    RegisterValueBranch
          ("pedestal", &_pedestal_proc, &V1724::GetPedestal,
          &V1724::SetPedestal, true);
    RegisterValueBranch
          ("channel", &_channel_proc, &V1724::GetChannel,
          &V1724::SetChannel, true);
}
}  // namespace MAUS


