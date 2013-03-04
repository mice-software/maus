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

PrimaryChainProcessor::PrimaryChainProcessor()
    : _treftrackpair_array_proc(new TRefTrackPairProcessor()) {

  RegisterValueBranch(
      "mapper_name", &_string_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_mapper_name,
      &MAUS::DataStructure::Global::PrimaryChain::set_mapper_name,
      true);

  RegisterPointerBranch(
      "track_parent_pairs", &_treftrackpair_array_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_track_parent_pairs,
      &MAUS::DataStructure::Global::PrimaryChain::set_track_parent_pairs,
      true);

  RegisterValueBranch(
      "goodness_of_fit", &_double_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_goodness_of_fit,
      &MAUS::DataStructure::Global::PrimaryChain::set_goodness_of_fit, true);

  RegisterTRefArray(
      "preceeding_pchains",
      &_primary_chain_trefarray_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_parent_primary_chains,
      &MAUS::DataStructure::Global::PrimaryChain::set_parent_primary_chains,
      true);
}
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
