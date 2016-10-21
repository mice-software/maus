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

  RegisterTRefArray(
      "lr_spacepoints", &_lr_sp_trefarray_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_lr_spacepoints,
      &MAUS::DataStructure::Global::PrimaryChain::set_lr_spacepoints,
      true);

  RegisterTRefArray(
      "lr_tracks", &_lr_track_trefarray_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_lr_tracks,
      &MAUS::DataStructure::Global::PrimaryChain::set_lr_tracks,
      true);

  RegisterTRefArray(
      "tracks", &_track_trefarray_proc,
      &MAUS::DataStructure::Global::PrimaryChain::get_tracks,
      &MAUS::DataStructure::Global::PrimaryChain::set_tracks,
      true);

}
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS
