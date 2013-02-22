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
      _detector_proc(), _part_event_number_proc() {
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
}
}  // namespace MAUS


