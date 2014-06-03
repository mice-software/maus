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

#include "src/common_cpp/JsonCppProcessors/SciFiHelicalPRTrackProcessor.hh"

namespace MAUS {

SciFiHelicalPRTrackProcessor::SciFiHelicalPRTrackProcessor()
                   : _double_array_proc(new DoubleProcessor) {

    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiHelicalPRTrack::get_tracker,
                        &SciFiHelicalPRTrack::set_tracker, true);
    RegisterValueBranch("num_points", &_int_proc,
                        &SciFiHelicalPRTrack::get_num_points,
                        &SciFiHelicalPRTrack::set_num_points, true);
    RegisterValueBranch("charge", &_int_proc,
                        &SciFiHelicalPRTrack::get_charge,
                        &SciFiHelicalPRTrack::set_charge, true);

    RegisterValueBranch("x0", &_double_proc,
                        &SciFiHelicalPRTrack::get_x0,
                        &SciFiHelicalPRTrack::set_x0, true);
    RegisterValueBranch("y0", &_double_proc,
                        &SciFiHelicalPRTrack::get_y0,
                        &SciFiHelicalPRTrack::set_y0, true);
    RegisterValueBranch("z0", &_double_proc,
                        &SciFiHelicalPRTrack::get_z0,
                        &SciFiHelicalPRTrack::set_z0, true);
    RegisterValueBranch("phi0", &_double_proc,
                        &SciFiHelicalPRTrack::get_phi0,
                        &SciFiHelicalPRTrack::set_phi0, true);
    RegisterValueBranch("psi0", &_double_proc,
                        &SciFiHelicalPRTrack::get_psi0,
                        &SciFiHelicalPRTrack::set_psi0, true);
    RegisterValueBranch("dsdz", &_double_proc,
                        &SciFiHelicalPRTrack::get_dsdz,
                        &SciFiHelicalPRTrack::set_dsdz, true);
    RegisterValueBranch("R", &_double_proc,
                        &SciFiHelicalPRTrack::get_R,
                        &SciFiHelicalPRTrack::set_R, true);
    RegisterValueBranch("line_sz_c", &_double_proc,
                        &SciFiHelicalPRTrack::get_line_sz_c,
                        &SciFiHelicalPRTrack::set_line_sz_c, true);
    RegisterValueBranch("line_sz_chisq", &_double_proc,
                        &SciFiHelicalPRTrack::get_line_sz_chisq,
                        &SciFiHelicalPRTrack::set_line_sz_chisq, true);
    RegisterValueBranch("circle_x0", &_double_proc,
                        &SciFiHelicalPRTrack::get_circle_x0,
                        &SciFiHelicalPRTrack::set_circle_x0, true);
    RegisterValueBranch("circle_y0", &_double_proc,
                        &SciFiHelicalPRTrack::get_circle_y0,
                        &SciFiHelicalPRTrack::set_circle_y0, true);
    RegisterValueBranch("circle_chisq", &_double_proc,
                        &SciFiHelicalPRTrack::get_circle_chisq,
                        &SciFiHelicalPRTrack::set_circle_chisq, true);
    RegisterValueBranch("chisq", &_double_proc,
                        &SciFiHelicalPRTrack::get_chisq,
                        &SciFiHelicalPRTrack::set_chisq, true);
    RegisterValueBranch("chisq_dof", &_double_proc,
                        &SciFiHelicalPRTrack::get_chisq_dof,
                        &SciFiHelicalPRTrack::set_chisq_dof, true);

    RegisterIgnoredBranch("spacepoints", false);
//    RegisterValueBranch("spacepoints", &_sf_spoint_array_proc,
//                        &SciFiHelicalPRTrack::get_spacepoints,
//                        &SciFiHelicalPRTrack::set_spacepoints, true);
    RegisterValueBranch("phi", &_double_array_proc,
                        &SciFiHelicalPRTrack::get_phi,
                        &SciFiHelicalPRTrack::set_phi, false);
}
} // ~namespace MAUS
