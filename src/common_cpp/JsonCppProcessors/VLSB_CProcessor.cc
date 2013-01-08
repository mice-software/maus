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

#include "src/common_cpp/JsonCppProcessors/VLSB_CProcessor.hh"

namespace MAUS {

VLSB_CProcessor::VLSB_CProcessor()
    : _detector_proc(), _equip_type_proc(), _time_stamp_proc(),
      _phys_event_number_proc(), _bank_id_proc(), _adc_proc(),
      _part_event_number_proc(), _channel_proc(), _tdc_proc(),
      _discriminator_proc(), _ldc_id_proc(), _geo_proc() {
    RegisterValueBranch
          ("detector", &_detector_proc, &VLSB_C::GetDetector,
          &VLSB_C::SetDetector, true);
    RegisterValueBranch
          ("equip_type", &_equip_type_proc, &VLSB_C::GetEquipType,
          &VLSB_C::SetEquipType, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &VLSB_C::GetTimeStamp,
          &VLSB_C::SetTimeStamp, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &VLSB_C::GetPhysEventNumber,
          &VLSB_C::SetPhysEventNumber, true);
    RegisterValueBranch
          ("bank_id", &_bank_id_proc, &VLSB_C::GetBankID,
          &VLSB_C::SetBankID, true);
    RegisterValueBranch
          ("adc", &_adc_proc, &VLSB_C::GetADC,
          &VLSB_C::SetADC, true);
    RegisterValueBranch
          ("adc", &_adc_proc, &VLSB_C::GetADC,
          &VLSB_C::SetADC, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &VLSB_C::GetPartEventNumber,
          &VLSB_C::SetPartEventNumber, true);
    RegisterValueBranch
          ("channel", &_channel_proc, &VLSB_C::GetChannel,
          &VLSB_C::SetChannel, true);
    RegisterValueBranch
          ("tdc", &_tdc_proc, &VLSB_C::GetTDC,
          &VLSB_C::SetTDC, true);
    RegisterValueBranch
          ("discriminator", &_discriminator_proc, &VLSB_C::GetDiscriminator,
          &VLSB_C::SetDiscriminator, true);
    RegisterValueBranch
          ("ldc_id", &_ldc_id_proc, &VLSB_C::GetLdcId,
          &VLSB_C::SetLdcId, true);
    RegisterValueBranch
          ("geo", &_geo_proc, &VLSB_C::GetGeo,
          &VLSB_C::SetGeo, true);
}
}  // namespace MAUS

