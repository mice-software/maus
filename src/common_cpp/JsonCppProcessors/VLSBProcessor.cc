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

#include "src/common_cpp/JsonCppProcessors/VLSBProcessor.hh"

namespace MAUS {

VLSBProcessor::VLSBProcessor() {
    RegisterValueBranch
          ("detector", &_string_proc,
          &VLSB::GetDetector,
          &VLSB::SetDetector, true);
    RegisterValueBranch
          ("equip_type", &_int_proc,
          &VLSB::GetEquipType,
          &VLSB::SetEquipType, true);
    RegisterValueBranch
          ("time_stamp", &_int_proc,
          &VLSB::GetTimeStamp,
          &VLSB::SetTimeStamp, true);
    RegisterValueBranch
          ("phys_event_number", &_int_proc,
          &VLSB::GetPhysEventNumber,
          &VLSB::SetPhysEventNumber, true);
    RegisterValueBranch
          ("bank_id", &_int_proc,
          &VLSB::GetBankID,
          &VLSB::SetBankID, true);
    RegisterValueBranch
          ("adc", &_int_proc,
          &VLSB::GetADC,
          &VLSB::SetADC, true);
    RegisterValueBranch
          ("adc", &_int_proc,
          &VLSB::GetADC,
          &VLSB::SetADC, true);
    RegisterValueBranch
          ("part_event_number", &_int_proc,
          &VLSB::GetPartEventNumber,
          &VLSB::SetPartEventNumber, true);
    RegisterValueBranch
          ("channel", &_int_proc,
          &VLSB::GetChannel,
          &VLSB::SetChannel, true);
    RegisterValueBranch
          ("tdc", &_int_proc,
          &VLSB::GetTDC,
          &VLSB::SetTDC, true);
    RegisterValueBranch
          ("discriminator", &_int_proc,
          &VLSB::GetDiscriminator,
          &VLSB::SetDiscriminator, true);
    RegisterValueBranch
          ("ldc_id", &_int_proc,
          &VLSB::GetLdcId,
          &VLSB::SetLdcId, true);
}
}  // namespace MAUS

