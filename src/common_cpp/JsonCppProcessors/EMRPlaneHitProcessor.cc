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

#include "src/common_cpp/JsonCppProcessors/EMRPlaneHitProcessor.hh"

namespace MAUS {

EMRPlaneHitProcessor::EMRPlaneHitProcessor()
      : _bar_hit_array_proc(new EMRBarHitProcessor()),
	_int_array_proc(new IntProcessor()) {
    RegisterValueBranch
          ("bar_hits", &_bar_hit_array_proc, &EMRPlaneHit::GetEMRBarHitArray,
          &EMRPlaneHit::SetEMRBarHitArray, true);
    RegisterValueBranch
          ("plane", &_int_proc, &EMRPlaneHit::GetPlane,
          &EMRPlaneHit::SetPlane, true);
    RegisterValueBranch
          ("orientation", &_int_proc, &EMRPlaneHit::GetOrientation,
          &EMRPlaneHit::SetOrientation, true);
    RegisterValueBranch
          ("time", &_int_proc, &EMRPlaneHit::GetTime,
          &EMRPlaneHit::SetTime, true);
    RegisterValueBranch
          ("deltat", &_int_proc, &EMRPlaneHit::GetDeltaT,
          &EMRPlaneHit::SetDeltaT, true);
    RegisterValueBranch
          ("charge", &_double_proc, &EMRPlaneHit::GetCharge,
          &EMRPlaneHit::SetCharge, true);
    RegisterValueBranch
          ("pedestal_area", &_double_proc, &EMRPlaneHit::GetPedestalArea,
          &EMRPlaneHit::SetPedestalArea, true);
    RegisterValueBranch
          ("samples", &_int_array_proc, &EMRPlaneHit::GetSampleArray,
          &EMRPlaneHit::SetSampleArray, true);
}
}  // namespace MAUS
