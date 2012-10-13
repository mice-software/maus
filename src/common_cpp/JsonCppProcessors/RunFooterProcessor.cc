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

#include "src/common_cpp/JsonCppProcessors/RunFooterProcessor.hh"

namespace MAUS {

RunFooterProcessor::RunFooterProcessor()
    : _int_proc() {
    RegisterValueBranch("run_number", &_int_proc,
                        &RunFooter::GetRunNumber,
                        &RunFooter::SetRunNumber, true);
    RegisterConstantBranch("maus_event_type", Json::Value("RunFooter"), true);
}
}  // namespace MAUS


