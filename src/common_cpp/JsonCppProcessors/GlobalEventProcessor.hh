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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALEVENTPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALEVENTPROCESSOR_

#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"

#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/Global/SpacePointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/Global/TrackPointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/Global/TrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/Global/PrimaryChainProcessor.hh"

namespace MAUS {

/** @class GlobalEventProcessor Conversions for GlobalEvent between
 * C++ and Json
 */

class GlobalEventProcessor : public ObjectProcessor<GlobalEvent> {
 public:
  /** Set up processors and register branches
   *
   *  Everything else is handled by the base class
   */
  GlobalEventProcessor();
  
 private:
  
  PointerArrayProcessor<MAUS::DataStructure::Global::SpacePoint>
  _global_space_point_array_proc;
  
  PointerArrayProcessor<MAUS::DataStructure::Global::TrackPoint>
  _global_track_point_array_proc;
  
  PointerArrayProcessor<MAUS::DataStructure::Global::Track>
  _global_track_array_proc;
  
  PointerArrayProcessor<MAUS::DataStructure::Global::PrimaryChain>
  _global_primary_chain_array_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_GLOBALEVENTPROCESSOR_

