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
    RegisterValueBranch("chi2", &_double_proc,
                        &SciFiTrackPoint::chi2,
                        &SciFiTrackPoint::set_chi2, false);
    RegisterValueBranch("pos", &_threevector_proc,
                        &SciFiTrackPoint::pos,
                        &SciFiTrackPoint::set_pos, false);
    RegisterValueBranch("mom", &_threevector_proc,
                        &SciFiTrackPoint::mom,
                        &SciFiTrackPoint::set_mom, false);
    RegisterValueBranch("gradient", &_threevector_proc,
                        &SciFiTrackPoint::gradient,
                        &SciFiTrackPoint::set_gradient, false);
    RegisterValueBranch("pos_err", &_threevector_proc,
                        &SciFiTrackPoint::pos_error,
                        &SciFiTrackPoint::set_pos_error, false);
    RegisterValueBranch("mom_err", &_threevector_proc,
                        &SciFiTrackPoint::mom_error,
                        &SciFiTrackPoint::set_mom_error, false);
    RegisterValueBranch("gradient_err", &_threevector_proc,
                        &SciFiTrackPoint::gradient_error,
                        &SciFiTrackPoint::set_gradient_error, false);
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
    RegisterValueBranch("errors", &_matrix_proc,
                        &SciFiTrackPoint::errors,
                        &SciFiTrackPoint::set_errors, false);
    RegisterValueBranch("has_data", &_bool_proc,
                        &SciFiTrackPoint::has_data,
                        &SciFiTrackPoint::set_has_data, false);
}
} // ~namespace MAUS
