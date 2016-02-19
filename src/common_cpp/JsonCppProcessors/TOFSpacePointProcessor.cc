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

#include "src/common_cpp/JsonCppProcessors/TOFSpacePointProcessor.hh"

namespace MAUS {

TOFSpacePointProcessor::TOFSpacePointProcessor()
    : _phys_event_number_proc(), _pixel_key_proc(), _charge_proc(),
      _station_proc(), _slabY_proc(), _slabX_proc(), _charge_product_proc(),
      _time_proc(), _detector_proc(), _part_event_number_proc(), _dt_proc() {
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &TOFSpacePoint::GetPhysEventNumber,
          &TOFSpacePoint::SetPhysEventNumber, true);
    RegisterValueBranch
          ("pixel_key", &_pixel_key_proc, &TOFSpacePoint::GetPixelKey,
          &TOFSpacePoint::SetPixelKey, true);
    RegisterValueBranch
          ("charge", &_charge_proc, &TOFSpacePoint::GetCharge,
          &TOFSpacePoint::SetCharge, true);
    RegisterValueBranch
          ("station", &_station_proc, &TOFSpacePoint::GetStation,
          &TOFSpacePoint::SetStation, true);
    RegisterValueBranch
          ("slabY", &_slabY_proc, &TOFSpacePoint::GetSlaby,
          &TOFSpacePoint::SetSlaby, true);
    RegisterValueBranch
          ("slabX", &_slabX_proc, &TOFSpacePoint::GetSlabx,
          &TOFSpacePoint::SetSlabx, true);
    RegisterValueBranch
          ("charge_product", &_charge_product_proc, &TOFSpacePoint::GetChargeProduct,
          &TOFSpacePoint::SetChargeProduct, true);
    RegisterValueBranch
          ("time", &_time_proc, &TOFSpacePoint::GetTime,
          &TOFSpacePoint::SetTime, true);
    RegisterValueBranch
          ("detector", &_detector_proc, &TOFSpacePoint::GetDetector,
          &TOFSpacePoint::SetDetector, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &TOFSpacePoint::GetPartEventNumber,
          &TOFSpacePoint::SetPartEventNumber, true);
    RegisterValueBranch
          ("dt", &_dt_proc, &TOFSpacePoint::GetDt,
          &TOFSpacePoint::SetDt, true);
    RegisterValueBranch
          ("global_x", &_global_x_proc, &TOFSpacePoint::GetGlobalPosX,
          &TOFSpacePoint::SetGlobalPosX, true);
    RegisterValueBranch
          ("global_y", &_global_y_proc, &TOFSpacePoint::GetGlobalPosY,
          &TOFSpacePoint::SetGlobalPosY, true);
    RegisterValueBranch
          ("global_z", &_global_z_proc, &TOFSpacePoint::GetGlobalPosZ,
          &TOFSpacePoint::SetGlobalPosZ, true);
    RegisterValueBranch
          ("global_x_err", &_global_x_err_proc, &TOFSpacePoint::GetGlobalPosXErr,
          &TOFSpacePoint::SetGlobalPosXErr, true);
    RegisterValueBranch
          ("global_y_err", &_global_y_err_proc, &TOFSpacePoint::GetGlobalPosYErr,
          &TOFSpacePoint::SetGlobalPosYErr, true);
    RegisterValueBranch
          ("global_z_err", &_global_z_err_proc, &TOFSpacePoint::GetGlobalPosZErr,
          &TOFSpacePoint::SetGlobalPosZErr, true);
}
}  // namespace MAUS


