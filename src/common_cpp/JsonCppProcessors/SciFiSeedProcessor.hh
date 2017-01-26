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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFISEEDPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFISEEDPROCESSOR_HH_

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ThreeVectorProcessor.hh"

#include "src/common_cpp/DataStructure/SciFiSeed.hh"

namespace MAUS {

/** @class SciFiSeedProcessor processor for SciFiSeed */
class SciFiSeedProcessor : public ObjectProcessor<SciFiSeed> {
 public:
    SciFiSeedProcessor();

 private:
    IntProcessor _int_proc;
    ValueArrayProcessor<double> _vector_proc;
};
} // ~namespace MAUS

#endif
