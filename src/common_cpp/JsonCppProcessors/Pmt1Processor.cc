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

#include "src/common_cpp/JsonCppProcessors/Pmt1Processor.hh"

namespace MAUS {

Pmt1Processor::Pmt1Processor()
    : _raw_time_proc(), _charge_proc(), _leading_time_proc(), _time_proc(),
      _tof_key_proc(), _trigger_request_leading_time_proc() {
    RegisterValueBranch
          ("raw_time", &_raw_time_proc, &Pmt1::GetRawTime,
          &Pmt1::SetRawTime, true);
    RegisterValueBranch
          ("charge", &_charge_proc, &Pmt1::GetCharge,
          &Pmt1::SetCharge, true);
    RegisterValueBranch
          ("leading_time", &_leading_time_proc, &Pmt1::GetLeadingTime,
          &Pmt1::SetLeadingTime, true);
    RegisterValueBranch
          ("time", &_time_proc, &Pmt1::GetTime,
          &Pmt1::SetTime, false);
    RegisterValueBranch
          ("tof_key", &_tof_key_proc, &Pmt1::GetTofKey,
          &Pmt1::SetTofKey, true);
    RegisterValueBranch
          ("trigger_request_leading_time", &_trigger_request_leading_time_proc, &Pmt1::GetTriggerRequestLeadingTime,
          &Pmt1::SetTriggerRequestLeadingTime, true);
}
}  // namespace MAUS


