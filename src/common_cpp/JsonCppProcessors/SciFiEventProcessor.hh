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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFIEVENTPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFIEVENTPROCESSOR_

#include "src/common_cpp/JsonCppProcessors/SciFiDigitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiClusterProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiSpacePointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiStraightPRTrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiHelicalPRTrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiSeedProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiTrackProcessor.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiSeed.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"

#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"

namespace MAUS {

/** @class SciFiEventProcessor Conversions for SciFiEvent between C++ and Json 
 *
 */

class SciFiEventProcessor : public ObjectProcessor<SciFiEvent> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    SciFiEventProcessor();

  private:
    PointerArrayProcessor<SciFiDigit>           _sf_digit_array_proc;
    PointerArrayProcessor<SciFiCluster>         _sf_cluster_array_proc;
    PointerArrayProcessor<SciFiSpacePoint>      _sf_spoint_array_proc;
    PointerArrayProcessor<SciFiStraightPRTrack> _sf_sprtrk_array_proc;
    PointerArrayProcessor<SciFiHelicalPRTrack>  _sf_hprtrk_array_proc;
    PointerArrayProcessor<SciFiSeed>            _sf_seed_array_proc;
    PointerArrayProcessor<SciFiTrack>           _sf_trk_array_proc;
    DoubleProcessor                             _double_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFIEVENTPROCESSOR_
