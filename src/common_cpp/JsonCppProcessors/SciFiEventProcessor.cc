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

#include "src/common_cpp/JsonCppProcessors/SciFiEventProcessor.hh"

namespace MAUS {

SciFiEventProcessor::SciFiEventProcessor()
                    : _sf_digit_array_proc(new SciFiDigitProcessor),
                      _sf_cluster_array_proc(new SciFiClusterProcessor),
                      _sf_spoint_array_proc(new SciFiSpacePointProcessor),
                      _sf_sprtrk_array_proc(new SciFiStraightPRTrackProcessor),
                      _sf_hprtrk_array_proc(new SciFiHelicalPRTrackProcessor),
                      _sf_trk_array_proc(new SciFiTrackProcessor),
                      _double_proc() {
  RegisterValueBranch("digits", &_sf_digit_array_proc,
                      &SciFiEvent::digits, &SciFiEvent::set_digits, false);
  RegisterValueBranch("clusters", &_sf_cluster_array_proc,
                      &SciFiEvent::clusters, &SciFiEvent::set_clusters, false);
  RegisterValueBranch("spacepoints", &_sf_spoint_array_proc,
                      &SciFiEvent::spacepoints, &SciFiEvent::set_spacepoints, false);
  RegisterValueBranch("straight_pr_tracks", &_sf_sprtrk_array_proc,
                      &SciFiEvent::straightprtracks, &SciFiEvent::set_straightprtrack, false);
  RegisterValueBranch("helical_pr_tracks", &_sf_hprtrk_array_proc,
                      &SciFiEvent::helicalprtracks, &SciFiEvent::set_helicalprtrack, false);
  RegisterValueBranch("tracks", &_sf_trk_array_proc,
                      &SciFiEvent::scifitracks, &SciFiEvent::set_scifitracks, false);
  RegisterValueBranch("mean_field_up", &_double_proc,
                      &SciFiEvent::get_mean_field_up,
                      &SciFiEvent::set_mean_field_up, false);
  RegisterValueBranch("mean_field_down", &_double_proc,
                      &SciFiEvent::get_mean_field_down,
                      &SciFiEvent::set_mean_field_down, false);
  RegisterValueBranch("variance_field_up", &_double_proc,
                      &SciFiEvent::get_variance_field_up,
                      &SciFiEvent::set_variance_field_up, false);
  RegisterValueBranch("variance_field_down", &_double_proc,
                      &SciFiEvent::get_variance_field_down,
                      &SciFiEvent::set_variance_field_down, false);
  RegisterValueBranch("range_field_up", &_double_proc,
                      &SciFiEvent::get_range_field_up,
                      &SciFiEvent::set_range_field_up, false);
  RegisterValueBranch("range_field_down", &_double_proc,
                      &SciFiEvent::get_range_field_down,
                      &SciFiEvent::set_range_field_down, false);
}
} // ~namespace MAUS
