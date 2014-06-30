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

#include "src/common_cpp/JsonCppProcessors/EMRBarHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRBarProcessor.hh"

namespace MAUS {

EMRBarProcessor::EMRBarProcessor() : _hit_arr_proc(new EMRBarHitProcessor()) {
    RegisterValueBranch
          ("bar", &_int_proc, &EMRBar::GetBar,
          &EMRBar::SetBar, true);
    RegisterValueBranch
          ("emr_bar_hits", &_hit_arr_proc, &EMRBar::GetEMRBarHitArray,
          &EMRBar::SetEMRBarHitArray, true);
}
}  // namespace MAUS


