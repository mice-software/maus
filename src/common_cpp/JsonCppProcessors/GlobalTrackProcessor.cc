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

#include "src/common_cpp/JsonCppProcessors/GlobalTrackProcessor.hh"

namespace MAUS {

GlobalTrackProcessor::GlobalTrackProcessor() :
    _geometry_paths_proc(new StringProcessor)
{

    RegisterValueBranch("mapper_name", &_string_proc,
                        &MAUS::recon::global::Track::get_mapper_name,
                        &MAUS::recon::global::Track::set_mapper_name,
                        true);

    RegisterValueBranch("pid", &_pid_proc,
                        &MAUS::recon::global::Track::get_pid,
                        &MAUS::recon::global::Track::set_pid, true);

    RegisterValueBranch("charge", &_int_proc,
                        &MAUS::recon::global::Track::get_charge,
                        &MAUS::recon::global::Track::set_charge, true);

    RegisterPointerReference("trackpoints",
                        &MAUS::recon::global::Track::get_trackpoints,
                        &MAUS::recon::global::Track::set_trackpoints, true); 

    RegisterValueBranch("detector_points", &_uint_proc,
                        &MAUS::recon::global::Track::get_detectorpoints,
                        &MAUS::recon::global::Track::set_detectorpoints, true);

    RegisterValueBranch("geometry_paths", &_geometry_paths_proc,
                        &MAUS::recon::global::Track::get_geometry_paths,
                        &MAUS::recon::global::Track::set_geometry_paths,
                        true);
    
    RegisterPointerReference(
        "constituent_tracks",
        &MAUS::recon::global::Track::get_constituent_tracks,
        &MAUS::recon::global::Track::set_constituent_tracks, true); 

    RegisterValueBranch("goodness_of_fit", &_double_proc,
                        &MAUS::recon::global::Track::get_goodness_of_fit,
                        &MAUS::recon::global::Track::set_goodness_of_fit, true);

}

} // ~namespace MAUS
