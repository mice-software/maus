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
#include "src/common_cpp/JsonCppProcessors/TRefArrayProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiSpacePointProcessor.hh"

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFISTRAIGHTPRTRACKPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFISTRAIGHTPRTRACKPROCESSOR_HH_

namespace MAUS {

/** @class SciFiStraightPRTrackProcessor processor for SciFiStraightPRTrack */
class SciFiStraightPRTrackProcessor : public ObjectProcessor<SciFiStraightPRTrack> {
 public:
    /** Constructor - registers the branch structure */
    SciFiStraightPRTrackProcessor();

 private:
    IntProcessor _int_proc;
    ThreeVectorProcessor _threevector_proc;
    DoubleProcessor _double_proc;
    TRefArrayProcessor _spoint_tref_proc;
};
} // ~namespace MAUS

#endif
