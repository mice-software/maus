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

#include "src/common_cpp/JsonCppProcessors/SciFiSpacePointProcessor.hh"

namespace MAUS {

SciFiSpacePointProcessor::SciFiSpacePointProcessor()
                         : _sf_cluster_array_proc(new SciFiClusterProcessor) {

    RegisterValueBranch("used", &_bool_proc,
                        &SciFiSpacePoint::is_used,
                        &SciFiSpacePoint::set_used, true);

    RegisterValueBranch("spill", &_int_proc,
                        &SciFiSpacePoint::get_spill,
                        &SciFiSpacePoint::set_spill, true);
    RegisterValueBranch("event", &_int_proc,
                        &SciFiSpacePoint::get_event,
                        &SciFiSpacePoint::set_event, true);
    RegisterValueBranch("station", &_int_proc,
                        &SciFiSpacePoint::get_station,
                        &SciFiSpacePoint::set_station, true);
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiSpacePoint::get_tracker,
                        &SciFiSpacePoint::set_tracker, true);

    RegisterValueBranch("type", &_string_proc,
                        &SciFiSpacePoint::get_type,
                        &SciFiSpacePoint::set_type, true);

    RegisterValueBranch("npe", &_double_proc,
                        &SciFiSpacePoint::get_npe,
                        &SciFiSpacePoint::set_npe, true);
    RegisterValueBranch("chi2", &_double_proc,
                        &SciFiSpacePoint::get_chi2,
                        &SciFiSpacePoint::set_chi2, true);

    RegisterValueBranch("position", &_three_vec_proc,
                        &SciFiSpacePoint::get_position,
                        &SciFiSpacePoint::set_position, true);

    RegisterValueBranch("clusters", &_sf_cluster_array_proc,
                        &SciFiSpacePoint::get_channels, &SciFiSpacePoint::set_channels, true);
}
} // ~namespace MAUS