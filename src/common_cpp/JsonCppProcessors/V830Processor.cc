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

#include "src/common_cpp/JsonCppProcessors/V830Processor.hh"

namespace MAUS {

V830Processor::V830Processor()
    : _ldc_id_proc(), _equip_type_proc(), _channels_proc(),
      _phys_event_number_proc(), _time_stamp_proc(), _geo_proc() {
    RegisterValueBranch
          ("ldc_id", &_ldc_id_proc, &V830::GetLdcId,
          &V830::SetLdcId, true);
    RegisterValueBranch
          ("equip_type", &_equip_type_proc, &V830::GetEquipType,
          &V830::SetEquipType, true);
    RegisterValueBranch
          ("channels", &_channels_proc, &V830::GetChannels,
          &V830::SetChannels, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &V830::GetPhysEventNumber,
          &V830::SetPhysEventNumber, true);
    RegisterValueBranch
          ("time_stamp", &_time_stamp_proc, &V830::GetTimeStamp,
          &V830::SetTimeStamp, true);
    RegisterValueBranch
          ("geo", &_geo_proc, &V830::GetGeo,
          &V830::SetGeo, true);
}
}  // namespace MAUS


