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

#include "src/common_cpp/JsonCppProcessors/MausSpillProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ScalarsProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRSpillDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DAQDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

namespace MAUS {

MausSpill* MausSpillProcessor::operator()(const Json::Value& jv) {
    MausSpill* ms = new MausSpill();

    if (!jv["scalars"].isNull()) {
        ms->SetScalars(ScalarsProcessor()(jv["scalars"]));
    }
    if (!jv["emr_spill_data"].isNull()) {
        ms->SetEMRSpillData(EMRSpillDataProcessor()(jv["emr_spill_data"]));
    }
    if (!jv["daq_data"].isNull()) {
        ms->SetDAQData(DAQDataProcessor()(jv["daq_data"]));
    }
    if (!jv["mc_events"].isNull()) {
        ms->SetMCEventArray(MCEventArrayProcessor()(jv["mc_events"]));
    }
    if (!jv["recon_events"].isNull()) {
        ms->SetReconEventArray(ReconEventArrayProcessor()(jv["recon_events"]));
    }

    return ms;  
}

}

