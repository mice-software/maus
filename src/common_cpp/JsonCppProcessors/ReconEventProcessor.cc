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

#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

namespace MAUS {

ReconEventProcessor::ReconEventProcessor() {
    RegisterValueBranch("part_event_number", &_int_proc,
       &ReconEvent::GetPartEventNumber, &ReconEvent::SetPartEventNumber, true);
    RegisterPointerBranch("tof_event", &_tof_proc, &ReconEvent::GetTOFEvent,
                                               &ReconEvent::SetTOFEvent, false);
    RegisterPointerBranch("sci_fi_event", &_sci_fi_proc,
                 &ReconEvent::GetSciFiEvent, &ReconEvent::SetSciFiEvent, false);
    RegisterPointerBranch("ckov_event", &_ckov_proc, &ReconEvent::GetCkovEvent,
                                              &ReconEvent::SetCkovEvent, false);
    RegisterPointerBranch("emr_event", &_emr_proc, &ReconEvent::GetEMREvent,
                                               &ReconEvent::SetEMREvent, false);
    RegisterPointerBranch("kl_event", &_kl_proc, &ReconEvent::GetKLEvent,
                                                &ReconEvent::SetKLEvent, false);
    RegisterPointerBranch("trigger_event", &_trigger_proc,
             &ReconEvent::GetTriggerEvent, &ReconEvent::SetTriggerEvent, false);
    RegisterPointerBranch("global_event", &_global_proc,
               &ReconEvent::GetGlobalEvent, &ReconEvent::SetGlobalEvent, false);
}
}

