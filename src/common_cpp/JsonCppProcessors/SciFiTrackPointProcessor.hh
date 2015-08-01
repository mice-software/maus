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
#include "src/common_cpp/JsonCppProcessors/ThreeVectorProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiClusterProcessor.hh"

#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFITRACKPOINTPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFITRACKPOINTPROCESSOR_HH_

namespace MAUS {

/** @class SciFiTrackPointProcessor processor for SciFiTrackPoint */
class SciFiTrackPointProcessor : public ObjectProcessor<SciFiTrackPoint> {
 public:
    SciFiTrackPointProcessor();

 private:
    BoolProcessor _bool_proc;
    IntProcessor _int_proc;
    DoubleProcessor _double_proc;
    ThreeVectorProcessor _threevector_proc;
    ValueArrayProcessor<double> _matrix_proc;
    // SciFiClusterProcessor _cluster_tref_proc;
};
} // ~namespace MAUS

#endif
