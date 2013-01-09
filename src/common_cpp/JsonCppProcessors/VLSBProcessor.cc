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

VLSBProcessor::VLSBProcessor()
    : _detector_proc(), _equip_type_proc(), _time_stamp_proc(),
      _phys_event_number_proc(), _bank_id_proc(), _adc_proc(),
      _part_event_number_proc(), _channel_proc(), _tdc_proc(),
      _discriminator_proc(), _ldc_id_proc() {
    RegisterValueBranch
          ("detector", &_detector_proc, &VLSB::GetDetector,
          &VLSB::SetDetector, true);
    RegisterValueBranch
          ("equip_type", &_equip_type_proc, &VLSB::GetEquipType,
          &VLSB::SetEquipType, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &VLSB::GetTimeStamp,
          &VLSB::SetTimeStamp, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &VLSB::GetPhysEventNumber,
          &VLSB::SetPhysEventNumber, true);
    RegisterValueBranch
          ("bank_id", &_bank_id_proc, &VLSB::GetBankID,
          &VLSB::SetBankID, true);
    RegisterValueBranch
          ("adc", &_adc_proc, &VLSB::GetADC,
          &VLSB::SetADC, true);
    RegisterValueBranch
          ("adc", &_adc_proc, &VLSB::GetADC,
          &VLSB::SetADC, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &VLSB::GetPartEventNumber,
          &VLSB::SetPartEventNumber, true);
    RegisterValueBranch
          ("channel", &_channel_proc, &VLSB::GetChannel,
          &VLSB::SetChannel, true);
    RegisterValueBranch
          ("tdc", &_tdc_proc, &VLSB::GetTDC,
          &VLSB::SetTDC, true);
    RegisterValueBranch
          ("discriminator", &_discriminator_proc, &VLSB::GetDiscriminator,
          &VLSB::SetDiscriminator, true);
    RegisterValueBranch
          ("ldc_id", &_ldc_id_proc, &VLSB::GetLdcId,
          &VLSB::SetLdcId, true);
}
}  // namespace MAUS

