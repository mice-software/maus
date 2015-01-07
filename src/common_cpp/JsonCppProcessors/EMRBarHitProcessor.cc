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

#include "src/common_cpp/JsonCppProcessors/EMRBarHitProcessor.hh"

namespace MAUS {

EMRBarHitProcessor::EMRBarHitProcessor() {
    RegisterValueBranch
          ("tot", &_int_proc, &EMRBarHit::GetTot,
          &EMRBarHit::SetTot, true);
    RegisterValueBranch
          ("delta_t", &_int_proc, &EMRBarHit::GetDeltaT,
          &EMRBarHit::SetDeltaT, true);
    RegisterValueBranch
          ("hittime", &_int_proc, &EMRBarHit::GetHitTime,
          &EMRBarHit::SetHitTime, true);
    RegisterValueBranch
          ("x", &_double_proc, &EMRBarHit::GetX,
          &EMRBarHit::SetX, true);
    RegisterValueBranch
          ("y", &_double_proc, &EMRBarHit::GetY,
          &EMRBarHit::SetY, true);
    RegisterValueBranch
          ("z", &_double_proc, &EMRBarHit::GetZ,
          &EMRBarHit::SetZ, true);
    RegisterValueBranch
          ("charge_corrected", &_double_proc, &EMRBarHit::GetChargeCorrected,
          &EMRBarHit::SetChargeCorrected, true);
    RegisterValueBranch
          ("total_charge_corrected", &_double_proc, &EMRBarHit::GetTotalChargeCorrected,
          &EMRBarHit::SetTotalChargeCorrected, true);
}
}  // namespace MAUS

