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

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"

#include "src/common_cpp/DataStructure/GlobalPrimaryChain.hh"

#include "src/common_cpp/JsonCppProcessors/TRefTrackPairProcessor.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALPRIMARYCHAINPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALPRIMARYCHAINPROCESSOR_HH_

namespace MAUS {

/** @class GlobalPrimaryChainProcessor processor for
 *  MAUS::recon::global::PrimaryChain */
class GlobalPrimaryChainProcessor :
      public ObjectProcessor<MAUS::recon::global::PrimaryChain> {
 public:
  /** Constructor - registers the branch structure */
  GlobalPrimaryChainProcessor();

 private:
  DoubleProcessor _double_proc;
  StringProcessor _string_proc;
  PointerArrayProcessor<MAUS::recon::global::TRefTrackPair>
  _treftrackpair_array_proc;
  
};
} // ~namespace MAUS

#endif
