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

#include "json/json.h"

#include <vector>

#include "src/map/MapCppDataSelection/MapCppDataSelection.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppDataSelection(void) {
  PyWrapMapBase<MapCppDataSelection>::PyWrapMapBaseModInit
                                      ("MapCppDataSelection", "", "", "", "");
}

MapCppDataSelection::MapCppDataSelection() : 
  MapBase<Data>("init_MapCppDataSelection") {
  // Do nothing
}

MapCppDataSelection::~MapCppDataSelection() {
  // Do nothing
}

void MapCppDataSelection::_birth(const std::string& argJsonConfigDocument) {
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
  
  // Config setup here
}

void MapCppDataSelection::_death() {
  // Do nothing
}

void MapCppDataSelection::_process(MAUS::Data* data) const {
  Spill *spill = data->GetSpill();

  if (spill->GetReconEvents() == NULL)
    return;

  if (spill->GetDaqEventType() != "physics_event")
    return;
  
  std::vector<ReconEvent*>* events = spill->GetReconEvents();
  for (size_t i = 0; i < events->size(); ++i) {
    ReconEvent* evt = events->at(i);
    std::vector<TOFSpacePoint> tof1_spoints = 
      evt->GetTOFEvent()->GetTOFEventSpacePoint().GetTOF1SpacePointArray();
    std::vector<TOFSpacePoint> tof2_spoints = 
      evt->GetTOFEvent()->GetTOFEventSpacePoint().GetTOF2SpacePointArray();
  }
}

} // ~namespace MAUS