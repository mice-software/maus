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

#include "src/common_cpp/JsonCppProcessors/Global/PrimaryChainProcessor.hh"

namespace MAUS {
namespace Processor {
namespace Global {

PrimaryChainProcessor::PrimaryChainProcessor() {

  RegisterValueBranch(
      "mapper_name", &_string_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_mapper_name,
      &MAUS::DataStructure::Global::PrimaryChain::set_mapper_name,
      true);

  RegisterValueBranch(
      "type", &_chain_type_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_chain_type,
      &MAUS::DataStructure::Global::PrimaryChain::set_chain_type, true);

  RegisterValueBranch(
      "child_multiplicity", &_chain_child_multiplicity_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_multiplicity,
      &MAUS::DataStructure::Global::PrimaryChain::set_multiplicity, true);

  RegisterTRef(
      "us_daughter",
      &MAUS::DataStructure::Global::PrimaryChain::get_us_daughter,
      &MAUS::DataStructure::Global::PrimaryChain::set_us_daughter,
      false);

  RegisterTRef(
      "ds_daughter",
      &MAUS::DataStructure::Global::PrimaryChain::get_ds_daughter,
      &MAUS::DataStructure::Global::PrimaryChain::set_us_daughter,
      false);

  RegisterTRefArray(
      "tracks", &_track_trefarray_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_tracks,
      &MAUS::DataStructure::Global::PrimaryChain::set_tracks,
      true);

}
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
