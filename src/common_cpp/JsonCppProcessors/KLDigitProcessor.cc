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

#include "src/common_cpp/JsonCppProcessors/KLDigitProcessor.hh"

namespace MAUS {

KLDigitProcessor::KLDigitProcessor()
    : _pmt_proc(), _charge_mm_proc(), _charge_pm_proc(), _phys_event_number_proc(),
      _kl_key_proc(), _part_event_number_proc(), _cell_proc(), _position_max_proc() {

    RegisterValueBranch
          ("pmt", &_pmt_proc, &KLDigit::GetPmt,
          &KLDigit::SetPmt, true);
    RegisterValueBranch
          ("charge_mm", &_charge_mm_proc, &KLDigit::GetChargeMm,
          &KLDigit::SetChargeMm, false);
    RegisterValueBranch
          ("charge_pm", &_charge_pm_proc, &KLDigit::GetChargePm,
          &KLDigit::SetChargePm, false);
    RegisterValueBranch
          ("phys_event_number", &_phys_event_number_proc,
          &KLDigit::GetPhysEventNumber,
          &KLDigit::SetPhysEventNumber, true);
    RegisterValueBranch
          ("kl_key", &_kl_key_proc, &KLDigit::GetKlKey,
          &KLDigit::SetKlKey, true);
    RegisterValueBranch
          ("part_event_number", &_part_event_number_proc,
          &KLDigit::GetPartEventNumber,
          &KLDigit::SetPartEventNumber, true);
    RegisterValueBranch
          ("cell", &_cell_proc, &KLDigit::GetCell,
          &KLDigit::SetCell, true);
    RegisterValueBranch
          ("position_max", &_position_max_proc, &KLDigit::GetPositionMax,
          &KLDigit::SetPositionMax, false);
}
}  // namespace MAUS
