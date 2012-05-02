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

#include "src/common_cpp/JsonCppProcessors/CkovBProcessor.hh"

namespace MAUS {

CkovBProcessor::CkovBProcessor()
    : _arrival_time_6_proc(), _arrival_time_7_proc(), _arrival_time_4_proc(),
      _arrival_time_5_proc(), _pulse_5_proc(), _pulse_4_proc(),
      _pulse_7_proc(), _pulse_6_proc(), _position_min_6_proc(),
      _coincidences_proc(), _total_charge_proc(), _part_event_number_proc(),
      _position_min_7_proc(), _number_of_pes_proc(), _position_min_5_proc(),
      _position_min_4_proc() {
    RegisterValueBranch
          ("arrival_time_6", &_arrival_time_6_proc, &CkovB::GetArrivalTime6,
          &CkovB::SetArrivalTime6, true);
    RegisterValueBranch
          ("arrival_time_7", &_arrival_time_7_proc, &CkovB::GetArrivalTime7,
          &CkovB::SetArrivalTime7, true);
    RegisterValueBranch
          ("arrival_time_4", &_arrival_time_4_proc, &CkovB::GetArrivalTime4,
          &CkovB::SetArrivalTime4, true);
    RegisterValueBranch
          ("arrival_time_5", &_arrival_time_5_proc, &CkovB::GetArrivalTime5,
          &CkovB::SetArrivalTime5, true);
    RegisterValueBranch
          ("pulse_5", &_pulse_5_proc, &CkovB::GetPulse5,
          &CkovB::SetPulse5, true);
    RegisterValueBranch
          ("pulse_4", &_pulse_4_proc, &CkovB::GetPulse4,
          &CkovB::SetPulse4, true);
    RegisterValueBranch
          ("pulse_7", &_pulse_7_proc, &CkovB::GetPulse7,
          &CkovB::SetPulse7, true);
    RegisterValueBranch
          ("pulse_6", &_pulse_6_proc, &CkovB::GetPulse6,
          &CkovB::SetPulse6, true);
    RegisterValueBranch
          ("position_min_6", &_position_min_6_proc, &CkovB::GetPositionMin6,
          &CkovB::SetPositionMin6, true);
    RegisterValueBranch
          ("coincidences", &_coincidences_proc, &CkovB::GetCoincidences,
          &CkovB::SetCoincidences, true);
    RegisterValueBranch
          ("total_charge", &_total_charge_proc, &CkovB::GetTotalCharge,
          &CkovB::SetTotalCharge, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &CkovB::GetPartEventNumber,
          &CkovB::SetPartEventNumber, true);
    RegisterValueBranch
          ("position_min_7", &_position_min_7_proc, &CkovB::GetPositionMin7,
          &CkovB::SetPositionMin7, true);
    RegisterValueBranch
          ("number_of_pes", &_number_of_pes_proc, &CkovB::GetNumberOfPes,
          &CkovB::SetNumberOfPes, true);
    RegisterValueBranch
          ("position_min_5", &_position_min_5_proc, &CkovB::GetPositionMin5,
          &CkovB::SetPositionMin5, true);
    RegisterValueBranch
          ("position_min_4", &_position_min_4_proc, &CkovB::GetPositionMin4,
          &CkovB::SetPositionMin4, true);
}
}  // namespace MAUS


