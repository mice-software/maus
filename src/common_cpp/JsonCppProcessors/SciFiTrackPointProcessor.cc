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

#include "src/common_cpp/JsonCppProcessors/SciFiTrackPointProcessor.hh"

namespace MAUS {

SciFiTrackPointProcessor::SciFiTrackPointProcessor(): _matrix_proc(new DoubleProcessor) {
    RegisterValueBranch("spill", &_int_proc,
                        &SciFiTrackPoint::spill,
                        &SciFiTrackPoint::set_spill, false);
    RegisterValueBranch("event", &_int_proc,
                        &SciFiTrackPoint::event,
                        &SciFiTrackPoint::set_event, false);
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiTrackPoint::tracker,
                        &SciFiTrackPoint::set_tracker, false);
    RegisterValueBranch("station", &_int_proc,
                        &SciFiTrackPoint::station,
                        &SciFiTrackPoint::set_station, false);
    RegisterValueBranch("plane", &_int_proc,
                        &SciFiTrackPoint::plane,
                        &SciFiTrackPoint::set_plane, false);
    RegisterValueBranch("channel", &_double_proc,
                        &SciFiTrackPoint::channel,
                        &SciFiTrackPoint::set_channel, false);
    RegisterValueBranch("f_chi2", &_double_proc,
                        &SciFiTrackPoint::f_chi2,
                        &SciFiTrackPoint::set_f_chi2, false);
    RegisterValueBranch("s_chi2", &_double_proc,
                        &SciFiTrackPoint::s_chi2,
                        &SciFiTrackPoint::set_s_chi2, false);
    RegisterValueBranch("pos", &_threevector_proc,
                        &SciFiTrackPoint::pos,
                        &SciFiTrackPoint::set_pos, false);
    RegisterValueBranch("mom", &_threevector_proc,
                        &SciFiTrackPoint::mom,
                        &SciFiTrackPoint::set_mom, false);
    RegisterValueBranch("mc_x", &_double_proc,
                        &SciFiTrackPoint::mc_x,
                        &SciFiTrackPoint::set_mc_x, false);
    RegisterValueBranch("mc_px", &_double_proc,
                        &SciFiTrackPoint::mc_px,
                        &SciFiTrackPoint::set_mc_px, false);
    RegisterValueBranch("mc_y", &_double_proc,
                        &SciFiTrackPoint::mc_y,
                        &SciFiTrackPoint::set_mc_y, false);
    RegisterValueBranch("mc_py", &_double_proc,
                        &SciFiTrackPoint::mc_py,
                        &SciFiTrackPoint::set_mc_py, false);
    RegisterValueBranch("mc_pz", &_double_proc,
                        &SciFiTrackPoint::mc_pz,
                        &SciFiTrackPoint::set_mc_pz, false);
    RegisterValueBranch("pull", &_double_proc,
                        &SciFiTrackPoint::pull,
                        &SciFiTrackPoint::set_pull, false);
    RegisterValueBranch("residual", &_double_proc,
                        &SciFiTrackPoint::residual,
                        &SciFiTrackPoint::set_residual, false);
    RegisterValueBranch("s_residual", &_double_proc,
                        &SciFiTrackPoint::smoothed_residual,
                        &SciFiTrackPoint::set_smoothed_residual, false);
    RegisterValueBranch("covariance", &_matrix_proc,
                        &SciFiTrackPoint::covariance,
                        &SciFiTrackPoint::set_covariance, false);
    RegisterTRefArray("clusters", &_cluster_tref_proc,
                      &SciFiTrackPoint::get_clusters, &SciFiTrackPoint::set_clusters, true);
}
} // ~namespace MAUS
