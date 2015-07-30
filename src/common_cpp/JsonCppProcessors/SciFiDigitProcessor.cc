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

#include "src/common_cpp/JsonCppProcessors/SciFiDigitProcessor.hh"

namespace MAUS {

SciFiDigitProcessor::SciFiDigitProcessor() {
    RegisterValueBranch("spill", &_int_proc,
                        &SciFiDigit::get_spill,
                        &SciFiDigit::set_spill, true);
    RegisterValueBranch("event", &_int_proc,
                        &SciFiDigit::get_event,
                        &SciFiDigit::set_event, true);
    RegisterValueBranch("station", &_int_proc,
                        &SciFiDigit::get_station,
                        &SciFiDigit::set_station, true);
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiDigit::get_tracker,
                        &SciFiDigit::set_tracker, true);
    RegisterValueBranch("plane", &_int_proc,
                        &SciFiDigit::get_plane,
                        &SciFiDigit::set_plane, true);
    RegisterValueBranch("channel", &_int_proc,
                        &SciFiDigit::get_channel,
                        &SciFiDigit::set_channel, true);
    RegisterValueBranch("npe", &_double_proc,
                        &SciFiDigit::get_npe,
                        &SciFiDigit::set_npe, true);
    RegisterValueBranch("time", &_double_proc,
                        &SciFiDigit::get_time,
                        &SciFiDigit::set_time, true);
    RegisterValueBranch("adc", &_int_proc,
                        &SciFiDigit::get_adc,
                        &SciFiDigit::set_adc, false);
    RegisterValueBranch("used", &_bool_proc,
                        &SciFiDigit::is_used,
                        &SciFiDigit::set_used, true);
}
}
