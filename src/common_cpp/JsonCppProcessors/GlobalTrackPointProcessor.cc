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

GlobalTrackPointProcessor::GlobalTrackPointProcessor() {
    RegisterValueBranch("mapper_name", &_string_proc,
                        &MAUS::recon::global::TrackPoint::get_mapper_name,
                        &MAUS::recon::global::TrackPoint::set_mapper_name,
                        true);
    RegisterValueBranch("charge", &_double_proc,
                        &MAUS::recon::global::TrackPoint::get_charge,
                        &MAUS::recon::global::TrackPoint::set_charge, true);

    RegisterValueBranch("position", &_tlorentz_vec_proc,
                        &MAUS::recon::global::TrackPoint::get_position,
                        &MAUS::recon::global::TrackPoint::set_position, true);

    RegisterValueBranch("position_error", &_tlorentz_vec_proc,
                        &MAUS::recon::global::TrackPoint::get_position_error,
                        &MAUS::recon::global::TrackPoint::set_position_error,
                        true);

    RegisterValueBranch("momentum", &_tlorentz_vec_proc,
                        &MAUS::recon::global::TrackPoint::get_momentum,
                        &MAUS::recon::global::TrackPoint::set_momentum, true);

    RegisterValueBranch("momentum_error", &_tlorentz_vec_proc,
                        &MAUS::recon::global::TrackPoint::get_momentum_error,
                        &MAUS::recon::global::TrackPoint::set_momentum_error,
                        true);

    RegisterValueBranch("detector", &_detector_enum_proc,
                        &MAUS::recon::global::TrackPoint::get_detector,
                        &MAUS::recon::global::TrackPoint::set_detector, true);

    RegisterValueBranch("geometry_path", &_string_proc,
                        &MAUS::recon::global::TrackPoint::get_geometry_path,
                        &MAUS::recon::global::TrackPoint::set_geometry_path,
                        true);

    RegisterTRef<MAUS::recon::global::SpacePoint>(
        "spacepoint",
        &MAUS::recon::global::TrackPoint::get_spacepoint_tref,
        &MAUS::recon::global::TrackPoint::set_spacepoint_tref,
        true);
}

} // ~namespace MAUS
