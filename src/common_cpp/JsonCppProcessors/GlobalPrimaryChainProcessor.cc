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

#include "src/common_cpp/JsonCppProcessors/GlobalPrimaryChainProcessor.hh"

namespace MAUS {

GlobalPrimaryChainProcessor::GlobalPrimaryChainProcessor() :
    _treftrackpair_array_proc(new TRefTrackPairProcessor())
{

  RegisterValueBranch("mapper_name", &_string_proc,
                      &MAUS::recon::global::PrimaryChain::get_mapper_name,
                      &MAUS::recon::global::PrimaryChain::set_mapper_name,
                      true);
  
  RegisterPointerBranch("track_parent_pairs", &_treftrackpair_array_proc,
                        &MAUS::GlobalPrimaryChain::get_track_parent_pairs,
                        &MAUS::GlobalPrimaryChain::set_track_parent_pairs,
                        true);
  
  RegisterValueBranch(
      "goodness_of_fit", &_double_proc,
      &MAUS::recon::global::PrimaryChain::get_goodness_of_fit,
      &MAUS::recon::global::PrimaryChain::set_goodness_of_fit, true);
  
}

} // ~namespace MAUS
