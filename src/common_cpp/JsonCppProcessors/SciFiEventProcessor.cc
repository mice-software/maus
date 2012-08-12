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

SciFiEventProcessor::SciFiEventProcessor() : _sf_hit_array_proc(new SciFiHitProcessor),
                                             _sf_digit_array_proc(new SciFiDigitProcessor),
                                             _sf_cluster_array_proc(new SciFiClusterProcessor),
                                             _sf_spoint_array_proc(new SciFiSpacePointProcessor) {

  RegisterValueBranch("hits", &_sf_hit_array_proc,
                      &SciFiEvent::hits, &SciFiEvent::set_hits, false);
  RegisterValueBranch("digits", &_sf_digit_array_proc,
                      &SciFiEvent::digits, &SciFiEvent::set_digits, false);
  RegisterValueBranch("clusters", &_sf_cluster_array_proc,
                      &SciFiEvent::clusters, &SciFiEvent::set_clusters, false);
  RegisterValueBranch("spacepoints", &_sf_spoint_array_proc,
                      &SciFiEvent::spacepoints, &SciFiEvent::set_spacepoints, false);
}
} // ~namespace MAUS
