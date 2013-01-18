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

#include "src/common_cpp/JsonCppProcessors/SciFiTrackProcessor.hh"

namespace MAUS {

SciFiTrackProcessor::SciFiTrackProcessor() {
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiTrack::get_tracker,
                        &SciFiTrack::set_tracker, true);
    RegisterValueBranch("chi2", &_double_proc,
                        &SciFiTrack::get_chi2,
                        &SciFiTrack::set_chi2, true);
    RegisterValueBranch("ndf", &_int_proc,
                        &SciFiTrack::get_ndf,
                        &SciFiTrack::set_ndf, true);
    RegisterValueBranch("P_value", &_double_proc,
                        &SciFiTrack::get_P_value,
                        &SciFiTrack::set_P_value, true);
}
} // ~namespace MAUS
