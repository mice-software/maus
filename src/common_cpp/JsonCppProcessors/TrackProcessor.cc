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

#include "src/common_cpp/JsonCppProcessors/TrackProcessor.hh"

namespace MAUS {

TrackProcessor::TrackProcessor() : _step_proc(new StepProcessor()) {
    RegisterValueBranch("particle_id", &_int_proc, &Track::GetParticleId, &Track::SetParticleId, true);
    RegisterValueBranch("track_id", &_int_proc, &Track::GetTrackId, &Track::SetTrackId, true);
    RegisterValueBranch("parent_track_id", &_int_proc, &Track::GetParentTrackId, &Track::SetParentTrackId, true);
    RegisterPointerBranch("steps", &_step_proc, &Track::GetSteps, &Track::SetSteps, false);
    RegisterValueBranch("initial_position", &_three_vec_proc, &Track::GetInitialPosition, &Track::SetInitialPosition, true);
    RegisterValueBranch("final_position", &_three_vec_proc, &Track::GetFinalPosition, &Track::SetFinalPosition, true);
    RegisterValueBranch("initial_momentum", &_three_vec_proc, &Track::GetInitialMomentum, &Track::SetInitialMomentum, true);
    RegisterValueBranch("final_momentum", &_three_vec_proc, &Track::GetFinalMomentum, &Track::SetFinalMomentum, true);   
}

}

