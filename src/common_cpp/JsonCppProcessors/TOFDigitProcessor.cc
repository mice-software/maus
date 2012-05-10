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

#include "src/common_cpp/JsonCppProcessors/TOFDigitProcessor.hh"

namespace MAUS {

TOFDigitProcessor::TOFDigitProcessor()
    : _pmt_proc(), _trigger_leading_time_proc(),
      _trigger_request_leading_time_proc(), _trigger_trailing_time_proc(),
      _trigger_request_trailing_time_proc(), _charge_mm_proc(),
      _trailing_time_proc(), _plane_proc(), _charge_pm_proc(),
      _phys_event_number_proc(), _station_proc(), _leading_time_proc(),
      _time_stamp_proc(), _tof_key_proc(), _trigger_time_tag_proc(),
      _part_event_number_proc(), _slab_proc() {
    RegisterValueBranch
          ("pmt", &_pmt_proc, &TOFDigit::GetPmt,
          &TOFDigit::SetPmt, true);
    RegisterValueBranch
          ("trigger_leading_time", &_trigger_leading_time_proc,
          &TOFDigit::GetTriggerLeadingTime,
          &TOFDigit::SetTriggerLeadingTime, true);
    RegisterValueBranch
          ("trigger_request_leading_time", &_trigger_request_leading_time_proc,
          &TOFDigit::GetTriggerRequestLeadingTime,
          &TOFDigit::SetTriggerRequestLeadingTime, true);
    RegisterValueBranch
          ("trigger_trailing_time", &_trigger_trailing_time_proc,
          &TOFDigit::GetTriggerTrailingTime,
          &TOFDigit::SetTriggerTrailingTime, true);
    RegisterValueBranch
          ("trigger_request_trailing_time", &_trigger_request_trailing_time_proc,
          &TOFDigit::GetTriggerRequestTrailingTime,
          &TOFDigit::SetTriggerRequestTrailingTime, true);
    RegisterValueBranch
          ("charge_mm", &_charge_mm_proc, &TOFDigit::GetChargeMm,
          &TOFDigit::SetChargeMm, false);
    RegisterValueBranch
          ("trailing_time", &_trailing_time_proc, &TOFDigit::GetTrailingTime,
          &TOFDigit::SetTrailingTime, true);
    RegisterValueBranch
          ("plane", &_plane_proc, &TOFDigit::GetPlane,
          &TOFDigit::SetPlane, true);
    RegisterValueBranch
          ("charge_pm", &_charge_pm_proc, &TOFDigit::GetChargePm,
          &TOFDigit::SetChargePm, false);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc,
          &TOFDigit::GetPhysEventNumber,
          &TOFDigit::SetPhysEventNumber, true);
    RegisterValueBranch
          ("station", &_station_proc, &TOFDigit::GetStation,
          &TOFDigit::SetStation, true);
    RegisterValueBranch
          ("leading_time", &_leading_time_proc, &TOFDigit::GetLeadingTime,
          &TOFDigit::SetLeadingTime, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &TOFDigit::GetTimeStamp,
          &TOFDigit::SetTimeStamp, true);
    RegisterValueBranch
          ("tof_key", &_tof_key_proc, &TOFDigit::GetTofKey,
          &TOFDigit::SetTofKey, true);
    RegisterValueBranch
          ("trigger_time_tag", &_trigger_time_tag_proc,
          &TOFDigit::GetTriggerTimeTag,
          &TOFDigit::SetTriggerTimeTag, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc,
          &TOFDigit::GetPartEventNumber,
          &TOFDigit::SetPartEventNumber, true);
    RegisterValueBranch
          ("slab", &_slab_proc, &TOFDigit::GetSlab,
          &TOFDigit::SetSlab, true);
}
}  // namespace MAUS


