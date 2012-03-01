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

#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"

namespace MAUS {

HitProcessor::HitProcessor() {
    RegisterValueBranch
             ("track_id", &_int_proc, &Hit::GetTrackId, &Hit::SetTrackId, true);
    RegisterValueBranch
    ("particle_id", &_int_proc, &Hit::GetParticleId, &Hit::SetParticleId, true);
    RegisterValueBranch
              ("energy", &_double_proc, &Hit::GetEnergy, &Hit::SetEnergy, true);
    RegisterValueBranch
              ("charge", &_double_proc, &Hit::GetCharge, &Hit::SetCharge, true);
    RegisterValueBranch
                    ("time", &_double_proc, &Hit::GetTime, &Hit::SetTime, true);
    RegisterValueBranch("energy_deposited", &_double_proc,
                      &Hit::GetEnergyDeposited, &Hit::SetEnergyDeposited, true);
    RegisterValueBranch
     ("position", &_three_vec_proc, &Hit::GetPosition, &Hit::SetPosition, true);
    RegisterValueBranch
     ("momentum", &_three_vec_proc, &Hit::GetMomentum, &Hit::SetMomentum, true);
}
}

