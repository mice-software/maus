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
#include "src/common_cpp/JsonCppProcessors/SciFiSpacePointProcessor.hh"

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFIHELICALPRTRACKPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFIHELICALPRTRACKPROCESSOR_HH_

namespace MAUS {

/** @class SciFiHelicalPRTrackProcessor processor for SciFiHelicalPRTrack */
class SciFiHelicalPRTrackProcessor : public ObjectProcessor<SciFiHelicalPRTrack> {
 public:
    /** Constructor - registers the branch structure */
    SciFiHelicalPRTrackProcessor();

 private:
    IntProcessor _int_proc;
    DoubleProcessor _double_proc;
    ValueArrayProcessor<double> _double_array_proc;
    ValueArrayProcessor<SciFiSpacePoint> _sf_spoint_array_proc;
};
} // ~namespace MAUS

#endif
