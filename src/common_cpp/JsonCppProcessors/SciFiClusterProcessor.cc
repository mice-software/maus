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

#include "src/common_cpp/JsonCppProcessors/SciFiClusterProcessor.hh"

namespace MAUS {

SciFiClusterProcessor::SciFiClusterProcessor() {
    RegisterValueBranch("spill", &_int_proc,
                        &SciFiCluster::get_spill,
                        &SciFiCluster::set_spill, true);
    RegisterValueBranch("event", &_int_proc,
                        &SciFiCluster::get_event,
                        &SciFiCluster::set_event, true);
    RegisterValueBranch("station", &_int_proc,
                        &SciFiCluster::get_station,
                        &SciFiCluster::set_station, true);
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiCluster::get_tracker,
                        &SciFiCluster::set_tracker, true);
    RegisterValueBranch("plane", &_int_proc,
                        &SciFiCluster::get_plane,
                        &SciFiCluster::set_plane, true);
    RegisterValueBranch("id", &_int_proc,
                        &SciFiCluster::get_id,
                        &SciFiCluster::set_id, true);

    RegisterValueBranch("npe", &_double_proc,
                        &SciFiCluster::get_npe,
                        &SciFiCluster::set_npe, true);
    RegisterValueBranch("time", &_double_proc,
                        &SciFiCluster::get_time,
                        &SciFiCluster::set_time, true);
    RegisterValueBranch("channel_w", &_double_proc,
                        &SciFiCluster::get_channel,
                        &SciFiCluster::set_channel, true);
    RegisterValueBranch("alpha", &_double_proc,
                        &SciFiCluster::get_alpha,
                        &SciFiCluster::set_alpha, true);

    RegisterValueBranch("used", &_bool_proc,
                        &SciFiCluster::is_used,
                        &SciFiCluster::set_used, true);

    RegisterValueBranch("direction", &_three_vec_proc,
                        &SciFiCluster::get_direction,
                        &SciFiCluster::set_direction, true);
    RegisterValueBranch("position", &_three_vec_proc,
                        &SciFiCluster::get_position,
                        &SciFiCluster::set_position, true);

    RegisterTRefArray("digits", &_digit_tref_proc,
                      &SciFiCluster::get_digits,
                      &SciFiCluster::set_digits, true);
}

} // ~namespace MAUS
