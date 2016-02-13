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

#include "src/common_cpp/JsonCppProcessors/Global/PIDLogLPairProcessor.hh"

namespace MAUS {
namespace Processor {
namespace Global {

PIDLogLPairProcessor::PIDLogLPairProcessor() {

  RegisterValueBranch(
        "pid", &_int_proc,
        &MAUS::DataStructure::Global::PIDLogLPair::get_PID,
        &MAUS::DataStructure::Global::PIDLogLPair::set_PID, true);

  RegisterValueBranch(
        "logL", &_double_proc,
        &MAUS::DataStructure::Global::PIDLogLPair::get_logL,
        &MAUS::DataStructure::Global::PIDLogLPair::set_logL, true);
}
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS


