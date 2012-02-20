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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MAUSSPILLPROCESSOR_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MAUSSPILLPROCESSOR_HH_

#include "json/value.h"

#include "src/common_cpp/JsonCppProcessors/DAQDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ScalarsProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRSpillDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/JsonCppPrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/JsonCppArrayProcessors.hh"

#include "src/common_cpp/JsonCppProcessors/JsonCppObjectProcessor.hh"
#include "src/common_cpp/DataStructure/Spill.hh"


namespace MAUS {

typedef std::vector<MCEvent*> MCEventArray;

class SpillProcessor : public JsonCppObjectProcessor<Spill> {
 public:
    SpillProcessor();

    Spill* JsonToCpp(const Json::Value& json_spill) {
        return JsonCppObjectProcessor<Spill>::JsonToCpp(json_spill);
    }
    Json::Value* CppToJson(const Spill& cpp_spill) {
        return JsonCppObjectProcessor<Spill>::CppToJson(cpp_spill);
    }
    
 private:
    Spill _spill;

    ScalarsProcessor _scal_proc;
    DAQDataProcessor _daq_proc;
    EMRSpillDataProcessor _emr_proc;
    JsonCppPointerArrayProcessor<MCEvent> _mc_array_proc;
    JsonCppPointerArrayProcessor<ReconEvent> _recon_array_proc;
    JsonCppIntProcessor _int_proc;

};

}
#endif

