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

#include "src/common_cpp/JsonCppProcessors/Global/SpacePointProcessor.hh"

namespace MAUS {
namespace Processor {
namespace Global {

SpacePointProcessor::SpacePointProcessor() {
  RegisterValueBranch(
      "charge", &_double_proc,
      &MAUS::DataStructure::Global::SpacePoint::get_charge,
      &MAUS::DataStructure::Global::SpacePoint::set_charge, true);

  RegisterValueBranch(
      "mapper_name", &_string_proc,
      &MAUS::DataStructure::Global::SpacePoint::get_mapper_name,
      &MAUS::DataStructure::Global::SpacePoint::set_mapper_name,
      false); // Make this not required so that existing data can still be loaded

  RegisterBaseClass(
      "basepoint", &_base_proc,
      &MAUS::DataStructure::Global::BasePoint::set_base, true);
}

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
