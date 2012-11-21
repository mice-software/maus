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

#include "src/common_cpp/JsonCppProcessors/SciFiStraightPRTrackProcessor.hh"

namespace MAUS {

SciFiStraightPRTrackProcessor::SciFiStraightPRTrackProcessor()
                         : _sf_spoint_array_proc(new SciFiSpacePointProcessor) {

    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiStraightPRTrack::get_tracker,
                        &SciFiStraightPRTrack::set_tracker, true);
    RegisterValueBranch("num_points", &_int_proc,
                        &SciFiStraightPRTrack::get_num_points,
                        &SciFiStraightPRTrack::set_num_points, true);

    RegisterValueBranch("x0", &_double_proc,
                        &SciFiStraightPRTrack::get_x0,
                        &SciFiStraightPRTrack::set_x0, true);
    RegisterValueBranch("mx", &_double_proc,
                        &SciFiStraightPRTrack::get_mx,
                        &SciFiStraightPRTrack::set_mx, true);
    RegisterValueBranch("x_chisq", &_double_proc,
                        &SciFiStraightPRTrack::get_x_chisq,
                        &SciFiStraightPRTrack::set_x_chisq, true);
    RegisterValueBranch("y0", &_double_proc,
                        &SciFiStraightPRTrack::get_y0,
                        &SciFiStraightPRTrack::set_y0, true);
    RegisterValueBranch("my", &_double_proc,
                        &SciFiStraightPRTrack::get_my,
                        &SciFiStraightPRTrack::set_my, true);
    RegisterValueBranch("y_chisq", &_double_proc,
                        &SciFiStraightPRTrack::get_y_chisq,
                        &SciFiStraightPRTrack::set_y_chisq, true);

    RegisterValueBranch("spacepoints", &_sf_spoint_array_proc,
                        &SciFiStraightPRTrack::get_spacepoints,
                        &SciFiStraightPRTrack::set_spacepoints, true);
}
} // ~namespace MAUS