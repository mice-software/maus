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

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ScalarsProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRSpillDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DAQDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

#include <iostream>

namespace MAUS {

Spill* SpillProcessor::operator()(const Json::Value& jv) {
    Spill* ms = new Spill();

    if (!jv["scalars"].isNull()) {
        ms->SetScalars(*ScalarsProcessor()(jv["scalars"]));
    }
    if (!jv["emr_spill_data"].isNull()) {
        ms->SetEMRSpillData(*EMRSpillDataProcessor()(jv["emr_spill_data"]));
    }
    if (!jv["daq_data"].isNull()) {
        ms->SetDAQData(*DAQDataProcessor()(jv["daq_data"]));
    }
    if (!jv["mc_events"].isNull()) {
        ms->SetMCEventArray(*MCEventArrayProcessor()(jv["mc_events"]));
    }
    if (!jv["recon_events"].isNull()) {
        ms->SetReconEventArray(*ReconEventArrayProcessor()(jv["recon_events"]));
    }

    return ms;  
}

Json::Value* SpillProcessor::operator()(const Spill& cpp_spill) {
        Json::Value* json_spill = new Json::Value();

        std::cerr << "SPILL PROCESSOR 0 " << this << std::endl;
        (*json_spill)["scalars"] = ScalarsProcessor()((cpp_spill.GetScalars()));
        std::cerr << "SPILL PROCESSOR a" << std::endl;
        (*json_spill)["emr_spill_data"] = EMRSpillDataProcessor()((cpp_spill.GetEMRSpillData()));
        std::cerr << "SPILL PROCESSOR b" << std::endl;
        (*json_spill)["daq_data"] = DAQDataProcessor()((cpp_spill.GetDAQData()));
        std::cerr << "SPILL PROCESSOR c" << std::endl;
        (*json_spill)["mc_events"] = MCEventArrayProcessor()((cpp_spill.GetMCEventArray()));
        std::cerr << "SPILL PROCESSOR d" << std::endl;
        (*json_spill)["recon_events"] = ReconEventArrayProcessor()((cpp_spill.GetReconEventArray()));
        std::cerr << "SPILL PROCESSOR e" << std::endl;

    return json_spill;  
}

}

