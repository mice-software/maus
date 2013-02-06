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

#include "src/common_cpp/JsonCppProcessors/Global/TrackProcessor.hh"

namespace MAUS {
namespace Processor {
namespace Global {

TrackProcessor::TrackProcessor() :
    _geometry_paths_proc(new StringProcessor)
{

    RegisterValueBranch(
        "mapper_name", &_string_proc,
        &MAUS::DataStructure::Global::Track::get_mapper_name,
        &MAUS::DataStructure::Global::Track::set_mapper_name,
        true);

    RegisterValueBranch(
        "pid", &_pid_proc,
        &MAUS::DataStructure::Global::Track::get_pid,
        &MAUS::DataStructure::Global::Track::set_pid, true);
    
    RegisterValueBranch(
        "charge", &_int_proc,
        &MAUS::DataStructure::Global::Track::get_charge,
        &MAUS::DataStructure::Global::Track::set_charge, true);

    RegisterTRefArray(
        "trackpoints",
        &_trackpoint_trefarray_proc,
        &MAUS::DataStructure::Global::Track::get_trackpoints,
        &MAUS::DataStructure::Global::Track::set_trackpoints, true); 

    RegisterValueBranch(
        "detector_points", &_uint_proc,
        &MAUS::DataStructure::Global::Track::get_detectorpoints,
        &MAUS::DataStructure::Global::Track::set_detectorpoints, true);

    RegisterValueBranch(
        "geometry_paths", &_geometry_paths_proc,
        &MAUS::DataStructure::Global::Track::get_geometry_paths,
        &MAUS::DataStructure::Global::Track::set_geometry_paths,
        true);
    
    RegisterTRefArray(
        "constituent_tracks",
        &_track_trefarray_proc,
        &MAUS::DataStructure::Global::Track::get_constituent_tracks,
        &MAUS::DataStructure::Global::Track::set_constituent_tracks,
        true); 

    RegisterValueBranch(
        "goodness_of_fit", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_goodness_of_fit,
        &MAUS::DataStructure::Global::Track::set_goodness_of_fit, true);

}

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
