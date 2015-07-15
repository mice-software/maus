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

SpillProcessor::SpillProcessor() :_mc_array_proc(new MCEventProcessor()),
                                  _recon_array_proc(new ReconEventProcessor()),
                                  _error_proc(new StringProcessor()) {
    RegisterPointerBranch
         ("test_branch", &_test_branch_proc, &Spill::GetTestBranch,
          &Spill::SetTestBranch, false);
    RegisterPointerBranch
         ("scalars", &_scal_proc, &Spill::GetScalars, &Spill::SetScalars, false);
    RegisterPointerBranch
         ("daq_data", &_daq_proc, &Spill::GetDAQData, &Spill::SetDAQData, false);
    RegisterPointerBranch("mc_events", &_mc_array_proc, &Spill::GetMCEvents,
                                                    &Spill::SetMCEvents, false);
    RegisterPointerBranch("recon_events", &_recon_array_proc,
                          &Spill::GetReconEvents, &Spill::SetReconEvents, false);
    RegisterPointerBranch("emr_spill_data", &_emr_proc,
                          &Spill::GetEMRSpillData, &Spill::SetEMRSpillData, false);
    RegisterValueBranch("spill_number", &_int_proc, &Spill::GetSpillNumber,
                                                  &Spill::SetSpillNumber, true);
    RegisterValueBranch("run_number", &_int_proc, &Spill::GetRunNumber,
                                                  &Spill::SetRunNumber, true);
    RegisterValueBranch("daq_event_type", &_string_proc, &Spill::GetDaqEventType,
                                                  &Spill::SetDaqEventType, true);
    RegisterValueBranch("errors", &_error_proc, &Spill::GetErrors,
                                                  &Spill::SetErrors, false);
    RegisterConstantBranch("maus_event_type", Json::Value("Spill"), true);
}
}

