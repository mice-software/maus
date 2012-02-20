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

#include <iostream>

#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

SpillProcessor::SpillProcessor() :_mc_array_proc(new MCEventProcessor()), _recon_array_proc(new ReconEventProcessor()) {
    AddPointerBranch("scalars", &_scal_proc, &Spill::GetScalars, &Spill::SetScalars, true);
    AddPointerBranch("daq_data", &_daq_proc, &Spill::GetDAQData, &Spill::SetDAQData, true);
    AddPointerBranch("emr_spill_data", &_emr_proc, &Spill::GetEMRSpillData, &Spill::SetEMRSpillData, true);
    AddPointerBranch("mc_events", &_mc_array_proc, &Spill::GetMCEvents, &Spill::SetMCEvents, false);
    AddPointerBranch("recon_events", &_recon_array_proc, &Spill::GetReconEvents, &Spill::SetReconEvents, true);
    AddValueBranch("spill_number", &_int_proc, &Spill::GetSpillNumber, &Spill::SetSpillNumber, true);
/*

    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &scal, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &emr, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &rec, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
    JsonCppObjectProcessor<Spill>::add_branch<DAQData>(std::string("daq_data"), &mc, a_spill, &Spill::GetDAQData, &Spill::SetDAQData, true);
*/
}

}

