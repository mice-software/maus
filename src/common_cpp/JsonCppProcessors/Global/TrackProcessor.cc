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

TrackProcessor::TrackProcessor()
  : _geometry_paths_proc(new StringProcessor),
    _pid_logL_values_proc(
        new MAUS::Processor::Global::PIDLogLPairProcessor()) {

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
        "track_points",
        &_track_point_trefarray_proc,
        &MAUS::DataStructure::Global::Track::get_track_points,
        &MAUS::DataStructure::Global::Track::set_track_points, true);

    RegisterValueBranch(
        "detector_points", &_uint_proc,
        &MAUS::DataStructure::Global::Track::get_detectorpoints,
        &MAUS::DataStructure::Global::Track::set_detectorpoints, true);

    RegisterValueBranch(
        "geometry_paths", &_geometry_paths_proc,
        &MAUS::DataStructure::Global::Track::get_geometry_paths,
        &MAUS::DataStructure::Global::Track::set_geometry_paths,
        true);

    RegisterValueBranch(
        "pid_logL_values", &_pid_logL_values_proc,
        &MAUS::DataStructure::Global::Track::get_pid_logL_values,
        &MAUS::DataStructure::Global::Track::set_pid_logL_values,
        true);

    RegisterTRefArray(
        "constituent_tracks",
        &_track_trefarray_proc,
        &MAUS::DataStructure::Global::Track::get_constituent_tracks,
        &MAUS::DataStructure::Global::Track::set_constituent_tracks,
        true);

    RegisterValueBranch(
        "emr_range_primary", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_emr_range_primary,
        &MAUS::DataStructure::Global::Track::set_emr_range_primary, true);

    RegisterValueBranch(
        "emr_range_secondary", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_emr_range_secondary,
        &MAUS::DataStructure::Global::Track::set_emr_range_secondary, true);

    RegisterValueBranch(
        "emr_plane_density", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_emr_plane_density,
        &MAUS::DataStructure::Global::Track::set_emr_plane_density, true);

    RegisterValueBranch(
        "pid_confidence_level", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_pid_confidence_level,
        &MAUS::DataStructure::Global::Track::set_pid_confidence_level, true);

    RegisterValueBranch(
        "goodness_of_fit", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_goodness_of_fit,
        &MAUS::DataStructure::Global::Track::set_goodness_of_fit, true);

    RegisterValueBranch(
        "p_value", &_double_proc,
        &MAUS::DataStructure::Global::Track::get_p_value,
        &MAUS::DataStructure::Global::Track::set_p_value, false);

    RegisterValueBranch(
        "tracker_clusters", &_uint_proc,
        &MAUS::DataStructure::Global::Track::get_tracker_clusters,
        &MAUS::DataStructure::Global::Track::set_tracker_clusters, false);
}

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
