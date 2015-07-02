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

#include "src/common_cpp/JsonCppProcessors/PrimaryProcessor.hh"

namespace MAUS {

PrimaryProcessor::PrimaryProcessor() {
    RegisterValueBranch("particle_id", &_int_proc, &Primary::GetParticleId,
                                                 &Primary::SetParticleId, true);
    RegisterValueBranch("random_seed", &_int_proc, &Primary::GetRandomSeed,
                                                 &Primary::SetRandomSeed, true);
    RegisterValueBranch("time", &_double_proc, &Primary::GetTime,
                                                       &Primary::SetTime, true);
    RegisterValueBranch("energy", &_double_proc, &Primary::GetEnergy,
                                                     &Primary::SetEnergy, true);
    RegisterValueBranch("position", &_three_vec_proc, &Primary::GetPosition,
                                                   &Primary::SetPosition, true);
    RegisterValueBranch("momentum", &_three_vec_proc, &Primary::GetMomentum,
                                                   &Primary::SetMomentum, true);
    RegisterValueBranch("spin", &_three_vec_proc, &Primary::GetSpin,
                                                   &Primary::SetSpin, false);
}
}

