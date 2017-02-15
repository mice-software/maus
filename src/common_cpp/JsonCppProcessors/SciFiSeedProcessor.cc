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

#include "src/common_cpp/JsonCppProcessors/SciFiSeedProcessor.hh"

namespace MAUS {

SciFiSeedProcessor::SciFiSeedProcessor()
                         : _vector_proc(new DoubleProcessor) {
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiSeed::getTracker,
                        &SciFiSeed::setTracker, true);
    RegisterValueBranch("state_vector", &_vector_proc,
                        &SciFiSeed::getStateVector_vector,
                        &SciFiSeed::setStateVector_vector, false);
    RegisterValueBranch("covariance", &_vector_proc,
                        &SciFiSeed::getCovariance_vector,
                        &SciFiSeed::setCovariance_vector, false);
    RegisterValueBranch("algorithm", &_int_proc,
                        &SciFiSeed::getAlgorithm,
                        &SciFiSeed::setAlgorithm, false);
    ObjectProcessor<SciFiSeed>::RegisterTRef("pr_track",
        &SciFiSeed::getPRTrackTobject, &SciFiSeed::setPRTrackTobject, false);
}
} // ~namespace MAUS
