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

#include "src/map/MapCppGlobalTrackMatching/MapCppGlobalTrackMatching.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "TProcessID.h"
#include "src/common_cpp/Recon/Global/MCTruthTools.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"

#include <ctime>

namespace MAUS {

  PyMODINIT_FUNC init_MapCppGlobalTrackMatching(void) {
    PyWrapMapBase<MAUS::MapCppGlobalTrackMatching>::PyWrapMapBaseModInit
                                    ("MapCppGlobalTrackMatching", "", "", "", "");
  }

  MapCppGlobalTrackMatching::MapCppGlobalTrackMatching()
    : MapBase<Data>("MapCppGlobalTrackMatching"), _configCheck(false) {
  }

  void MapCppGlobalTrackMatching::_birth(const std::string& argJsonConfigDocument) {
    // Check if the JSON document can be parsed, else return error only.
    _configCheck = false;
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to parse configuration",
                              "MapCppGlobalTrackMatching::birth");
    }
    _configCheck = true;
    _classname = "MapCppGlobalTrackMatching";
    std::cerr << "TIME: " << time(0) << "\n";
  }

  void MapCppGlobalTrackMatching::_death() {
    std::cerr << "TIME: " << time(0) << "\n";
  }

  void MapCppGlobalTrackMatching::_process(MAUS::Data* data_cpp) const {
    // Read string and convert to a Json object
    if (!data_cpp) {
        throw MAUS::Exception(Exception::recoverable,
                              "data_cpp was NULL",
                              "MapCppGlobalTrackMatching::_process");
    }
    if (!_configCheck) {
        throw MAUS::Exception(Exception::recoverable,
                              "Birth has not been successfully called",
                              "MapCppGlobalTrackMatching::_process");
    }
    const MAUS::Spill* spill = data_cpp->GetSpill();

    
    //~ std::map<MAUS::DataStructure::Global::DetectorPoint, bool> mc_detectors = MCTruthTools::GetMCDetectors(spill->GetMCEvents()->at(0));
    //~ std::map<MAUS::DataStructure::Global::DetectorPoint, bool> recon_detectors = GlobalTools::GetMCDetectors(spill->GetReconEvents()->at(0)->GetGlobalEvent());
    //~ std::cerr << "U|V|TOF0 |Chk|TOF1 |Tracker0   |Tracker1   |TOF2 |K|E\n";
    //~ for (int i = 0; i < 27; i++) {
      //~ std::cerr << mc_detectors[static_cast<MAUS::DataStructure::Global::DetectorPoint>(i)] << " ";
    //~ }
    //~ std::cerr << "\n";
    //~ for (int i = 0; i < 27; i++) {
      //~ std::cerr << recon_detectors[static_cast<MAUS::DataStructure::Global::DetectorPoint>(i)] << " ";
    //~ }
    //~ std::cerr << "\n";

    
    MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();
    if (!recon_events) {
      return;
    }
    std::cerr << "Number of Recon Events: " << recon_events->size() << "\n";
    MAUS::ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
   recon_event_iter != recon_events->end();
   ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      MAUS::ReconEvent* recon_event = (*recon_event_iter);
      MAUS::GlobalEvent* global_event = recon_event->GetGlobalEvent();

      if (global_event) {
        MAUS::recon::global::TrackMatching track_matching;
        track_matching.USTrack(global_event, _classname);
        track_matching.DSTrack(global_event, _classname);
        track_matching.throughTrack(global_event, _classname);
      }
    }
  }
} // ~MAUS
