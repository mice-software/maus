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

#include "src/common_cpp/JsonCppProcessors/EMRTrackPointProcessor.hh"

namespace MAUS {

EMRTrackPointProcessor::EMRTrackPointProcessor() {
    RegisterValueBranch
          ("pos", &_threevector_proc, &EMRTrackPoint::GetPosition,
           &EMRTrackPoint::SetPosition, true);
    RegisterValueBranch
          ("gpos", &_threevector_proc, &EMRTrackPoint::GetGlobalPosition,
           &EMRTrackPoint::SetGlobalPosition, true);
    RegisterValueBranch
          ("pos_err", &_threevector_proc, &EMRTrackPoint::GetPositionErrors,
           &EMRTrackPoint::SetPositionErrors, true);
    RegisterValueBranch
          ("ch", &_int_proc, &EMRTrackPoint::GetChannel,
           &EMRTrackPoint::SetChannel, true);
    RegisterValueBranch
          ("resx", &_double_proc, &EMRTrackPoint::GetResidualX,
           &EMRTrackPoint::SetResidualX, true);
    RegisterValueBranch
          ("resy", &_double_proc, &EMRTrackPoint::GetResidualY,
           &EMRTrackPoint::SetResidualY, true);
    RegisterValueBranch
          ("chi2", &_double_proc, &EMRTrackPoint::GetChi2,
           &EMRTrackPoint::SetChi2, true);
}
}  // namespace MAUS
