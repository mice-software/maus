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

#include "src/common_cpp/JsonCppProcessors/Pmt0Processor.hh"

namespace MAUS {

Pmt0Processor::Pmt0Processor()
    : _raw_time_proc(), _charge_proc(), _leading_time_proc(), _time_proc(),
      _tof_key_proc(), _trigger_request_leading_time_proc() {
    RegisterValueBranch
          ("raw_time", &_raw_time_proc, &Pmt0::GetRawTime,
          &Pmt0::SetRawTime, true);
    RegisterValueBranch
          ("charge", &_charge_proc, &Pmt0::GetCharge,
          &Pmt0::SetCharge, true);
    RegisterValueBranch
          ("leading_time", &_leading_time_proc, &Pmt0::GetLeadingTime,
          &Pmt0::SetLeadingTime, true);
    RegisterValueBranch
          ("time", &_time_proc, &Pmt0::GetTime,
          &Pmt0::SetTime, false);
    RegisterValueBranch
          ("tof_key", &_tof_key_proc, &Pmt0::GetTofKey,
          &Pmt0::SetTofKey, true);
    RegisterValueBranch
          ("trigger_request_leading_time", &_trigger_request_leading_time_proc, &Pmt0::GetTriggerRequestLeadingTime,
          &Pmt0::SetTriggerRequestLeadingTime, true);
}
}  // namespace MAUS


