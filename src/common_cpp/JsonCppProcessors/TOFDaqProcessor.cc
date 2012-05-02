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

#include "src/common_cpp/JsonCppProcessors/TOFDaqProcessor.hh"

namespace MAUS {

TOFDaqProcessor::TOFDaqProcessor()
    : _V1724_proc(new V1724Processor), _V1290_proc(new V1290Processor) {
    RegisterValueBranch
          ("V1724", &_V1724_proc, &TOFDaq::GetV1724Array,
          &TOFDaq::SetV1724Array, true);
    RegisterValueBranch
          ("V1290", &_V1290_proc, &TOFDaq::GetV1290Array,
          &TOFDaq::SetV1290Array, true);
}
}  // namespace MAUS


