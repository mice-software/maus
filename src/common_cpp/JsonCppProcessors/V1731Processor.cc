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

#include "src/common_cpp/JsonCppProcessors/V1731Processor.hh"

namespace MAUS {

V1731Processor::V1731Processor()
    : _ldc_id_proc(), _samples_proc(new IntProcessor), _charge_mm_proc(),
      _equip_type_proc(), _phys_event_number_proc(), _charge_pm_proc(),
      _arrival_time_proc(), _channel_key_proc(), _position_min_proc(),
      _pulse_area_proc(), _max_pos_proc(), _trigger_time_tag_proc(),
      _time_stamp_proc(), _detector_proc(), _part_event_number_proc(),
      _geo_proc(), _pedestal_proc(), _channel_proc() {
    RegisterValueBranch
          ("ldc_id", &_ldc_id_proc, &V1731::GetLdcId,
          &V1731::SetLdcId, true);
    RegisterValueBranch
          ("samples", &_samples_proc, &V1731::GetSampleArray,
          &V1731::SetSampleArray, true);
    RegisterValueBranch
          ("charge_mm", &_charge_mm_proc, &V1731::GetChargeMm,
          &V1731::SetChargeMm, true);
    RegisterValueBranch
          ("equip_type", &_equip_type_proc, &V1731::GetEquipType,
          &V1731::SetEquipType, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &V1731::GetPhysEventNumber,
          &V1731::SetPhysEventNumber, true);
    RegisterValueBranch
          ("charge_pm", &_charge_pm_proc, &V1731::GetChargePm,
          &V1731::SetChargePm, true);
    RegisterValueBranch
          ("arrival_time", &_arrival_time_proc, &V1731::GetArrivalTime,
          &V1731::SetArrivalTime, true);
    RegisterValueBranch
          ("channel_key", &_channel_key_proc, &V1731::GetChannelKey,
          &V1731::SetChannelKey, true);
    RegisterValueBranch
          ("position_min", &_position_min_proc, &V1731::GetPositionMin,
          &V1731::SetPositionMin, true);
    RegisterValueBranch
          ("pulse_area", &_pulse_area_proc, &V1731::GetPulseArea,
          &V1731::SetPulseArea, true);
    RegisterValueBranch
          ("max_pos", &_max_pos_proc, &V1731::GetMaxPos,
          &V1731::SetMaxPos, true);
    RegisterValueBranch
          ("trigger_time_tag", &_trigger_time_tag_proc, &V1731::GetTriggerTimeTag,
          &V1731::SetTriggerTimeTag, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &V1731::GetTimeStamp,
          &V1731::SetTimeStamp, true);
    RegisterValueBranch
          ("detector", &_detector_proc, &V1731::GetDetector,
          &V1731::SetDetector, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &V1731::GetPartEventNumber,
          &V1731::SetPartEventNumber, true);
    RegisterValueBranch
          ("geo", &_geo_proc, &V1731::GetGeo,
          &V1731::SetGeo, true);
    RegisterValueBranch
          ("pedestal", &_pedestal_proc, &V1731::GetPedestal,
          &V1731::SetPedestal, true);
    RegisterValueBranch
          ("channel", &_channel_proc, &V1731::GetChannel,
          &V1731::SetChannel, true);
}
}  // namespace MAUS


