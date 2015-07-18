/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */

#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"

#include "src/map/MapCppReconSetup/MapCppReconSetup.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppReconSetup(void) {
  PyWrapMapBase<MAUS::MapCppReconSetup>::PyWrapMapBaseModInit
                                            ("MapCppReconSetup", "", "", "", "");
}

MapCppReconSetup::MapCppReconSetup()
  : MapBase<MAUS::Data>("MapCppReconSetup") {
}

void MapCppReconSetup::_birth(const std::string& argJsonConfigDocument) {}

void MapCppReconSetup::_death()  {}

void MapCppReconSetup::_process(MAUS::Data *data) const {
  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();
  if (spill->GetDAQData() == NULL)
    return;

  if (spill->GetDaqEventType() != "physics_event")
    return;

  TriggerArray *tr_data = spill->GetDAQData()->GetTriggerArrayPtr();

  // get number of triggers in this spill
  int nTrigs = tr_data->size();

  // get size of recon event buffer
  ReconEventPArray *recEvts =  new ReconEventPArray();
  int recPartEvents = spill->GetReconEventSize();

  // if no recon events, resize recon event array to hold events=num.triggers
  for (int iPe = 0; iPe < nTrigs; iPe++)
    recEvts->push_back(new ReconEvent);

  // std::cerr << "nTriggers = " << nTrigs << std::endl;
  for (int xPE = 0; xPE < nTrigs; xPE++) {
    int evNumber = (tr_data->at(xPE))->GetV1290Array()[0].GetPartEventNumber();
    recEvts->at(xPE)->SetPartEventNumber(evNumber);

    recEvts->at(xPE)->SetTOFEvent(new TOFEvent());
    recEvts->at(xPE)->SetTriggerEvent(new TriggerEvent());
    recEvts->at(xPE)->SetSciFiEvent(new SciFiEvent());
    recEvts->at(xPE)->SetCkovEvent(new CkovEvent());
    recEvts->at(xPE)->SetKLEvent(new KLEvent());
    recEvts->at(xPE)->SetEMREvent(new EMREvent());
    recEvts->at(xPE)->SetGlobalEvent(new GlobalEvent());
  }
  spill->SetReconEvents(recEvts);
}
}
