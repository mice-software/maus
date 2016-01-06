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
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"

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

    // TOF selection
    std::vector<TOFSpacePoint> tof1_spoints = 
      evt->GetTOFEvent()->GetTOFEventSpacePoint().GetTOF1SpacePointArray();
    std::vector<TOFSpacePoint> tof2_spoints = 
      evt->GetTOFEvent()->GetTOFEventSpacePoint().GetTOF2SpacePointArray();

    bool tof1_cut = false;
    bool tof2_cut = false;
    if (tof1_spoints.size() == 1) tof1_cut = true;
    if (tof2_spoints.size() == 1) tof2_cut = true;

    // Tracker selection
    SciFiEvent* sfevt = evt->GetSciFiEvent();
    std::vector<SciFiSpacePoint*> spoints = sfevt->spacepoints();

    // Sort seed spacepoints by tracker
    std::vector<std::vector<SciFiSpacePoint*>> spoints_by_tracker =
      SciFiTools::sort_by_tracker(spoints);
    std::vector<SciFiSpacePoint*> tkus_spoints = spoints_by_tracker[0];
    std::vector<SciFiSpacePoint*> tkds_spoints = spoints_by_tracker[1];

    std::cerr << "SP in TkUS: " << tkus_spoints.size() << std::endl;
    std::cerr << "SP in TkDS: " << tkds_spoints.size() << std::endl;

    // Sort seed spacepoints further by station
    std::vector<std::vector<SciFiSpacePoint*>> tkus_spoints_by_station(5);
    SciFiTools::sort_by_station(tkus_spoints, tkus_spoints_by_station);
    std::vector<std::vector<SciFiSpacePoint*>> tkds_spoints_by_station(5);
    SciFiTools::sort_by_station(tkds_spoints, tkds_spoints_by_station);

    std::cerr << "Stations in TkUS: " << tkus_spoints_by_station.size()  << std::endl;
    std::cerr << "Stations in TkDS: " << tkds_spoints_by_station.size()  << std::endl;

    // Check there is one and only spacepoint in each tracker
    bool tkus_cut = true;
    bool tkds_cut = true;
    // Loop over stations
    std::cerr << "Tk Stat Sp Cut\n";
    for (size_t i = 0; i < tkus_spoints_by_station.size(); ++i) {
      if (tkus_spoints_by_station[i].size() != 1) tkus_cut = false;
      std::cerr << "TkUS " << i+1 << " " << tkus_spoints_by_station[i].size()
                << " " << tkus_cut << std::endl;
    }
    for (size_t i = 0; i < tkds_spoints_by_station.size(); ++i) {
      if (tkds_spoints_by_station[i].size() != 1) tkds_cut = false;
      std::cerr << "TkDS " << i+1 << " " << tkus_spoints_by_station[i].size()
                << " " << tkds_cut<< std::endl;
    }

    if ( (tof1_cut == false) || (tof2_cut == false) ||
         (tkus_cut == false) || (tkds_cut == false) ) {
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