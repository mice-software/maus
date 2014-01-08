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

#include "src/common_cpp/JsonCppProcessors/EMRBarProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRPlaneHitProcessor.hh"

namespace MAUS {

EMRPlaneHitProcessor::EMRPlaneHitProcessor()
      : _hit_arr_proc(new EMRBarProcessor()) {
    RegisterValueBranch
          ("plane", &_int_proc, &EMRPlaneHit::GetPlane,
          &EMRPlaneHit::SetPlane, true);
    RegisterValueBranch
          ("orientation", &_int_proc, &EMRPlaneHit::GetOrientation,
          &EMRPlaneHit::SetOrientation, true);
    RegisterValueBranch
          ("emr_bars", &_hit_arr_proc, &EMRPlaneHit::GetEMRBarArray,
          &EMRPlaneHit::SetEMRBarArray, true);
    RegisterValueBranch
          ("charge", &_int_proc, &EMRPlaneHit::GetCharge,
          &EMRPlaneHit::SetCharge, true);
    RegisterValueBranch
          ("time", &_int_proc, &EMRPlaneHit::GetTime,
          &EMRPlaneHit::SetTime, true);
    RegisterValueBranch
          ("spill", &_int_proc, &EMRPlaneHit::GetSpill,
          &EMRPlaneHit::SetSpill, true);
    RegisterValueBranch
          ("trigger", &_int_proc, &EMRPlaneHit::GetTrigger,
          &EMRPlaneHit::SetTrigger, true);
    RegisterValueBranch
          ("delta_t", &_int_proc, &EMRPlaneHit::GetDeltaT,
          &EMRPlaneHit::SetDeltaT, true);
}
}  // namespace MAUS


