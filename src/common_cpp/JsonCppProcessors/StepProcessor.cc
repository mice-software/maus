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

#include "src/common_cpp/JsonCppProcessors/StepProcessor.hh"

namespace MAUS {

StepProcessor::StepProcessor() {
    RegisterValueBranch("proper_time", &_double_proc, &Step::GetPathLength, &Step::SetPathLength, true);
    RegisterValueBranch("path_length", &_double_proc, &Step::GetProperTime, &Step::SetProperTime, true);
    RegisterValueBranch("time", &_double_proc, &Step::GetTime, &Step::SetTime, true);
    RegisterValueBranch("energy", &_double_proc, &Step::GetEnergy, &Step::SetEnergy, true);
    RegisterValueBranch("energy_deposited", &_double_proc, &Step::GetEnergyDeposited, &Step::SetEnergyDeposited, true);
    RegisterValueBranch("position", &_three_vec_proc, &Step::GetPosition, &Step::SetPosition, true);
    RegisterValueBranch("momentum", &_three_vec_proc, &Step::GetMomentum, &Step::SetMomentum, true);
}

}

