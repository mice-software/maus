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

#include "src/common_cpp/JsonCppProcessors/KLCellHitProcessor.hh"

namespace MAUS {

KLCellHitProcessor::KLCellHitProcessor()
    : _cell_proc(), _phys_event_number_proc(),
      _charge_proc(), _charge_product_proc(),
      _detector_proc(), _part_event_number_proc(),
      _global_pos_x_proc(), _global_pos_y_proc(), _global_pos_z_proc(),
      _local_pos_x_proc(), _local_pos_y_proc(), _local_pos_z_proc(),
      _error_x_proc(), _error_y_proc(), _error_z_proc(), _flag_proc() {
    RegisterValueBranch
          ("cell", &_cell_proc, &KLCellHit::GetCell,
          &KLCellHit::SetCell, true);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc, &KLCellHit::GetPhysEventNumber,
          &KLCellHit::SetPhysEventNumber, true);
    RegisterValueBranch
          ("charge", &_charge_proc, &KLCellHit::GetCharge,
          &KLCellHit::SetCharge, true);
    RegisterValueBranch
         ("charge_product", &_charge_product_proc, &KLCellHit::GetChargeProduct,
          &KLCellHit::SetChargeProduct, true);
    RegisterValueBranch
          ("detector", &_detector_proc, &KLCellHit::GetDetector,
          &KLCellHit::SetDetector, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc, &KLCellHit::GetPartEventNumber,
          &KLCellHit::SetPartEventNumber, true);
    RegisterValueBranch
          ("global_pos_x", &_global_pos_x_proc, &KLCellHit::GetGlobalPosX,
          &KLCellHit::SetGlobalPosX, true);
    RegisterValueBranch
          ("global_pos_y", &_global_pos_y_proc, &KLCellHit::GetGlobalPosY,
          &KLCellHit::SetGlobalPosY, true);
    RegisterValueBranch
          ("global_pos_z", &_global_pos_z_proc, &KLCellHit::GetGlobalPosZ,
          &KLCellHit::SetGlobalPosZ, true);
    RegisterValueBranch
          ("local_pos_x", &_local_pos_x_proc, &KLCellHit::GetLocalPosX,
          &KLCellHit::SetLocalPosX, true);
    RegisterValueBranch
          ("local_pos_y", &_local_pos_y_proc, &KLCellHit::GetLocalPosY,
          &KLCellHit::SetLocalPosY, true);
    RegisterValueBranch
          ("local_pos_z", &_local_pos_z_proc, &KLCellHit::GetLocalPosZ,
          &KLCellHit::SetLocalPosZ, true);
    RegisterValueBranch
          ("err_x", &_error_x_proc, &KLCellHit::GetErrorX,
          &KLCellHit::SetErrorX, true);
    RegisterValueBranch
          ("err_y", &_error_y_proc, &KLCellHit::GetErrorY,
          &KLCellHit::SetErrorY, true);
    RegisterValueBranch
          ("err_z", &_error_z_proc, &KLCellHit::GetErrorZ,
          &KLCellHit::SetErrorZ, true);
    RegisterValueBranch
          ("flag", &_flag_proc, &KLCellHit::GetFlag,
          &KLCellHit::SetFlag, true);
}
}  // namespace MAUS


