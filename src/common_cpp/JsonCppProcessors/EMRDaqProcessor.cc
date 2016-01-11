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

#include "src/common_cpp/JsonCppProcessors/EMRDaqProcessor.hh"

namespace MAUS {

EMRDaqProcessor::EMRDaqProcessor()
    : _V1731_proc(new ValueArrayProcessor<V1731>(new V1731Processor)),
      _dbb_proc(new DBBSpillDataProcessor) {
    RegisterValueBranch // zero suppression removes V1724 arrays
          ("V1731", &_V1731_proc, &EMRDaq::GetV1731PartEventArray,
          &EMRDaq::SetV1731PartEventArray, false);
    RegisterValueBranch
          ("dbb", &_dbb_proc, &EMRDaq::GetDBBArray,
          &EMRDaq::SetDBBArray, false);
}
}  // namespace MAUS
