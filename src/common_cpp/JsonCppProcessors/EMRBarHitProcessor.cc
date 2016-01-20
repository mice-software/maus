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
          ("ch", &_int_proc, &EMRBarHit::GetChannel,
          &EMRBarHit::SetChannel, true);
    RegisterValueBranch
          ("tot", &_int_proc, &EMRBarHit::GetTot,
          &EMRBarHit::SetTot, true);
    RegisterValueBranch
          ("time", &_int_proc, &EMRBarHit::GetTime,
          &EMRBarHit::SetTime, true);
    RegisterValueBranch
          ("deltat", &_int_proc, &EMRBarHit::GetDeltaT,
          &EMRBarHit::SetDeltaT, true);
}
}  // namespace MAUS
