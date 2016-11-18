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

#include <string>

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectMapProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/Global/EnumeratorProcessors.hh"

#include "src/common_cpp/DataStructure/Global/PrimaryChain.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALPRIMARYCHAINPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALPRIMARYCHAINPROCESSOR_HH_

namespace MAUS {
namespace Processor {
namespace Global {

/** @class PrimaryChainProcessor processor for
 *  MAUS::DataStructure::Global::PrimaryChain
 *  @author Jan Greis, University of Warwick
 */

class PrimaryChainProcessor
    : public ObjectProcessor<MAUS::DataStructure::Global::PrimaryChain> {
 public:
  /** Constructor - registers the branch structure */
  PrimaryChainProcessor();

 private:
  StringProcessor _string_proc;
  EnumChainTypeProcessor _chain_type_proc;
  TRefArrayProcessor _track_trefarray_proc;
};
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS

#endif
