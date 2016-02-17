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

#include "src/common_cpp/JsonCppProcessors/TOFSlabHitProcessor.hh"

namespace MAUS {

TOFSlabHitProcessor::TOFSlabHitProcessor()
    : _slab_proc(), _phys_event_number_proc(), _raw_time_proc(),
      _charge_proc(), _plane_proc(), _charge_product_proc(), _time_proc(),
      _station_proc(), _detector_proc(), _part_event_number_proc(),
      _pmt1_proc(), _pmt0_proc() {
    RegisterValueBranch
          ("slab", &_slab_proc, &TOFSlabHit::GetSlab,
          &TOFSlabHit::SetSlab, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &TOFSlabHit::GetPhysEventNumber,
          &TOFSlabHit::SetPhysEventNumber, true);
    RegisterValueBranch
          ("raw_time", &_raw_time_proc, &TOFSlabHit::GetRawTime,
          &TOFSlabHit::SetRawTime, true);
    RegisterValueBranch
          ("charge", &_charge_proc, &TOFSlabHit::GetCharge,
          &TOFSlabHit::SetCharge, true);
    RegisterValueBranch
          ("plane", &_plane_proc, &TOFSlabHit::GetPlane,
          &TOFSlabHit::SetPlane, true);
    RegisterValueBranch
          ("charge_product", &_charge_product_proc, &TOFSlabHit::GetChargeProduct,
          &TOFSlabHit::SetChargeProduct, true);
    RegisterValueBranch
          ("time", &_time_proc, &TOFSlabHit::GetTime,
          &TOFSlabHit::SetTime, false);
    RegisterValueBranch
          ("station", &_station_proc, &TOFSlabHit::GetStation,
          &TOFSlabHit::SetStation, true);
    RegisterValueBranch
          ("detector", &_detector_proc, &TOFSlabHit::GetDetector,
          &TOFSlabHit::SetDetector, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &TOFSlabHit::GetPartEventNumber,
          &TOFSlabHit::SetPartEventNumber, true);
    RegisterValueBranch
          ("pmt1", &_pmt1_proc, &TOFSlabHit::GetPmt1,
          &TOFSlabHit::SetPmt1, true);
    RegisterValueBranch
          ("pmt0", &_pmt0_proc, &TOFSlabHit::GetPmt0,
          &TOFSlabHit::SetPmt0, true);
    RegisterValueBranch
          ("global_x", &_global_x_proc, &TOFSlabHit::GetGlobalPosX,
          &TOFSlabHit::SetGlobalPosX, true);
    RegisterValueBranch
          ("global_y", &_global_y_proc, &TOFSlabHit::GetGlobalPosY,
          &TOFSlabHit::SetGlobalPosY, true);
    RegisterValueBranch
          ("global_z", &_global_z_proc, &TOFSlabHit::GetGlobalPosZ,
          &TOFSlabHit::SetGlobalPosZ, true);
    RegisterValueBranch
          ("global_x_err", &_global_x_err_proc, &TOFSlabHit::GetGlobalPosXErr,
          &TOFSlabHit::SetGlobalPosXErr, true);
    RegisterValueBranch
          ("global_y_err", &_global_y_err_proc, &TOFSlabHit::GetGlobalPosYErr,
          &TOFSlabHit::SetGlobalPosYErr, true);
    RegisterValueBranch
          ("global_z_err", &_global_z_err_proc, &TOFSlabHit::GetGlobalPosZErr,
          &TOFSlabHit::SetGlobalPosZErr, true);
    RegisterValueBranch
          ("horizontal_slab", &_horizontal_slab_proc, &TOFSlabHit::IsHorizontal,
          &TOFSlabHit::SetHorizontal, true);
    RegisterValueBranch
          ("vertical_slab", &_vertical_slab_proc, &TOFSlabHit::IsVertical,
          &TOFSlabHit::SetVertical, true);
}
}  // namespace MAUS


