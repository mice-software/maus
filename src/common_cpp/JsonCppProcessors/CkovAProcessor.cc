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

#include "src/common_cpp/JsonCppProcessors/CkovAProcessor.hh"

namespace MAUS {

CkovAProcessor::CkovAProcessor()
    : _position_min_0_proc(), _arrival_time_2_proc(), _arrival_time_3_proc(),
      _arrival_time_0_proc(), _arrival_time_1_proc(), _pulse_1_proc(),
      _pulse_0_proc(), _pulse_3_proc(), _pulse_2_proc(), _coincidences_proc(),
      _position_min_2_proc(), _position_min_3_proc(), _total_charge_proc(),
      _position_min_1_proc(), _part_event_number_proc(), _number_of_pes_proc() {
    RegisterValueBranch
          ("position_min_0", &_position_min_0_proc, &CkovA::GetPositionMin0,
          &CkovA::SetPositionMin0, true);
    RegisterValueBranch
          ("arrival_time_2", &_arrival_time_2_proc, &CkovA::GetArrivalTime2,
          &CkovA::SetArrivalTime2, true);
    RegisterValueBranch
          ("arrival_time_3", &_arrival_time_3_proc, &CkovA::GetArrivalTime3,
          &CkovA::SetArrivalTime3, true);
    RegisterValueBranch
          ("arrival_time_0", &_arrival_time_0_proc, &CkovA::GetArrivalTime0,
          &CkovA::SetArrivalTime0, true);
    RegisterValueBranch
          ("arrival_time_1", &_arrival_time_1_proc, &CkovA::GetArrivalTime1,
          &CkovA::SetArrivalTime1, true);
    RegisterValueBranch
          ("pulse_1", &_pulse_1_proc, &CkovA::GetPulse1,
          &CkovA::SetPulse1, true);
    RegisterValueBranch
          ("pulse_0", &_pulse_0_proc, &CkovA::GetPulse0,
          &CkovA::SetPulse0, true);
    RegisterValueBranch
          ("pulse_3", &_pulse_3_proc, &CkovA::GetPulse3,
          &CkovA::SetPulse3, true);
    RegisterValueBranch
          ("pulse_2", &_pulse_2_proc, &CkovA::GetPulse2,
          &CkovA::SetPulse2, true);
    RegisterValueBranch
          ("coincidences", &_coincidences_proc, &CkovA::GetCoincidences,
          &CkovA::SetCoincidences, true);
    RegisterValueBranch
          ("position_min_2", &_position_min_2_proc, &CkovA::GetPositionMin2,
          &CkovA::SetPositionMin2, true);
    RegisterValueBranch
          ("position_min_3", &_position_min_3_proc, &CkovA::GetPositionMin3,
          &CkovA::SetPositionMin3, true);
    RegisterValueBranch
          ("total_charge", &_total_charge_proc, &CkovA::GetTotalCharge,
          &CkovA::SetTotalCharge, true);
    RegisterValueBranch
          ("position_min_1", &_position_min_1_proc, &CkovA::GetPositionMin1,
          &CkovA::SetPositionMin1, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &CkovA::GetPartEventNumber,
          &CkovA::SetPartEventNumber, true);
    RegisterValueBranch
          ("number_of_pes", &_number_of_pes_proc, &CkovA::GetNumberOfPes,
          &CkovA::SetNumberOfPes, true);
}
}  // namespace MAUS


