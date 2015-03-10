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
#include "src/common_cpp/JsonCppProcessors/Global/EnumeratorProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/Global/PIDLogLPairProcessor.hh"

#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALTRACKPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALTRACKPROCESSOR_HH_

namespace MAUS {
namespace Processor {
namespace Global {

/** @class TrackProcessor processor for
 *  MAUS::DataStructure::Global::Track
 *  @author Ian Taylor, University of Warwick
 *  @date 2013/03/01
 */
class TrackProcessor
    : public ObjectProcessor<MAUS::DataStructure::Global::Track> {
 public:
  /** Constructor - registers the branch structure */
  TrackProcessor();

 private:
  DoubleProcessor _double_proc;
  StringProcessor _string_proc;
  EnumPIDProcessor _pid_proc;
  IntProcessor _int_proc;
  UIntProcessor _uint_proc;
  ValueArrayProcessor<std::string> _geometry_paths_proc;
  ValueArrayProcessor<MAUS::DataStructure::Global::PIDLogLPair> _pid_logL_values_proc;
  TRefArrayProcessor _track_trefarray_proc;
  TRefArrayProcessor _track_point_trefarray_proc;
};
} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS

#endif
