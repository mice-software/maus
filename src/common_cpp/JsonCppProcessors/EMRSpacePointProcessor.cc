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

#include "src/common_cpp/JsonCppProcessors/EMRSpacePointProcessor.hh"

namespace MAUS {

EMRSpacePointProcessor::EMRSpacePointProcessor() {
    RegisterValueBranch
          ("pos", &_threevector_proc, &EMRSpacePoint::GetPosition,
          &EMRSpacePoint::SetPosition, true);
    RegisterValueBranch
          ("gpos", &_threevector_proc, &EMRSpacePoint::GetGlobalPosition,
          &EMRSpacePoint::SetGlobalPosition, true);
    RegisterValueBranch
          ("pos_err", &_threevector_proc, &EMRSpacePoint::GetPositionErrors,
          &EMRSpacePoint::SetPositionErrors, true);
    RegisterValueBranch
          ("ch", &_int_proc, &EMRSpacePoint::GetChannel,
          &EMRSpacePoint::SetChannel, true);
    RegisterValueBranch
          ("time", &_double_proc, &EMRSpacePoint::GetTime,
          &EMRSpacePoint::SetTime, true);
    RegisterValueBranch
          ("deltat", &_double_proc, &EMRSpacePoint::GetDeltaT,
          &EMRSpacePoint::SetDeltaT, true);
    RegisterValueBranch
          ("charge_ma", &_double_proc, &EMRSpacePoint::GetChargeMA,
          &EMRSpacePoint::SetChargeMA, true);
    RegisterValueBranch
          ("charge_sa", &_double_proc, &EMRSpacePoint::GetChargeSA,
          &EMRSpacePoint::SetChargeSA, true);
}
}  // namespace MAUS
