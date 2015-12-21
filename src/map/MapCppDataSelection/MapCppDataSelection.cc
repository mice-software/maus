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
  
  std::vector<ReconEvent*>& events = (*spill->GetReconEvents());
  std::cerr << "Total # of ReconEvents in spill: " << events.size() << "\n";
  
  // Identify the events to be removed
  std::vector<int> events_to_remove;
  for (size_t i = 0; i < events.size(); ++i) {
    ReconEvent* evt = events[i];
    std::vector<TOFSpacePoint> tof1_spoints = 
      evt->GetTOFEvent()->GetTOFEventSpacePoint().GetTOF1SpacePointArray();
    std::vector<TOFSpacePoint> tof2_spoints = 
      evt->GetTOFEvent()->GetTOFEventSpacePoint().GetTOF2SpacePointArray();
      
    bool tof1_cut = false;
    bool tof2_cut = false;
    if (tof1_spoints.size() == 1) tof1_cut = true;
    if (tof2_spoints.size() == 1) tof2_cut = true;
    
    std::cerr << "Spoints in TOF1 and TOF2: " << tof1_spoints.size()
              << " " << tof2_spoints.size() << std::endl;
    if ( (tof1_cut == false) || (tof2_cut == false) ) {
      events_to_remove.push_back(i);
    }
  }
  
  // Now remove the selected events
  int nEventsRemoved = 0;
  for (size_t i = 0; i < events_to_remove.size(); ++i) {
    events.erase(events.begin() + events_to_remove[i] - nEventsRemoved);
    std::cerr << "Removed event " << events_to_remove[i] - nEventsRemoved 
              << std::endl;
    ++nEventsRemoved;
  }
  
  std::cerr << "# of ReconEvents retained: " << events.size() << std::endl;
  std::cerr << "# of Spill ReconEvents retained: "
            << spill->GetReconEvents()->size() << std::endl;
}

} // ~namespace MAUS