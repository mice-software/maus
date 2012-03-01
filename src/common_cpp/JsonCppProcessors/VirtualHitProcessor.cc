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

#include "src/common_cpp/JsonCppProcessors/VirtualHitProcessor.hh"

namespace MAUS {

VirtualHitProcessor::VirtualHitProcessor() {
    RegisterValueBranch("station_id", &_int_proc, &VirtualHit::GetStationId,
                                               &VirtualHit::SetStationId, true);
    RegisterValueBranch("particle_id", &_int_proc, &VirtualHit::GetParticleId,
                                              &VirtualHit::SetParticleId, true);
    RegisterValueBranch("track_id", &_int_proc, &VirtualHit::GetTrackId,
                                                 &VirtualHit::SetTrackId, true);
    RegisterValueBranch("time", &_double_proc, &VirtualHit::GetTime,
                                                    &VirtualHit::SetTime, true);
    RegisterValueBranch("mass", &_double_proc, &VirtualHit::GetMass,
                                                    &VirtualHit::SetMass, true);
    RegisterValueBranch("charge", &_double_proc, &VirtualHit::GetCharge,
                                                  &VirtualHit::SetCharge, true);
    RegisterValueBranch("proper_time", &_double_proc,
                  &VirtualHit::GetProperTime, &VirtualHit::SetProperTime, true);
    RegisterValueBranch("path_length", &_double_proc,
                  &VirtualHit::GetPathLength, &VirtualHit::SetPathLength, true);
    RegisterValueBranch("position", &_three_vec_proc, &VirtualHit::GetPosition,
                                                &VirtualHit::SetPosition, true);
    RegisterValueBranch("momentum", &_three_vec_proc, &VirtualHit::GetMomentum,
                                                &VirtualHit::SetMomentum, true);
    RegisterValueBranch("b_field", &_three_vec_proc, &VirtualHit::GetBField,
                                                  &VirtualHit::SetBField, true);
    RegisterValueBranch("e_field", &_three_vec_proc, &VirtualHit::GetEField,
                                                  &VirtualHit::SetEField, true);
}
}

