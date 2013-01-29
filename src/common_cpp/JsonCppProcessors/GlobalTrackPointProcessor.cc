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

#include "src/common_cpp/JsonCppProcessors/GlobalTrackPointProcessor.hh"

namespace MAUS {

GlobalTrackPointProcessor::GlobalTrackPointProcessor()
                         : int_processor_(), double_processor_(),
                           three_vector_processor_() {

    RegisterValueBranch("time", &double_processor_,
                        &GlobalTrackPoint::time,
                        &GlobalTrackPoint::set_time, true);
    RegisterValueBranch("energy", &double_processor_,
                        &GlobalTrackPoint::energy,
                        &GlobalTrackPoint::set_energy, true);
    RegisterValueBranch("momentum", &three_vector_processor_,
                        &GlobalTrackPoint::momentum,
                        &GlobalTrackPoint::set_momentum, true);
    RegisterValueBranch("position", &three_vector_processor_,
                        &GlobalTrackPoint::position,
                        &GlobalTrackPoint::set_position, true);
    RegisterValueBranch("detector_id", &int_processor_,
                        &GlobalTrackPoint::detector_id,
                        &GlobalTrackPoint::set_detector_id, true);
    RegisterValueBranch("particle_id", &int_processor_,
                        &GlobalTrackPoint::particle_id,
                        &GlobalTrackPoint::set_particle_id, true);
}
} // ~namespace MAUS
