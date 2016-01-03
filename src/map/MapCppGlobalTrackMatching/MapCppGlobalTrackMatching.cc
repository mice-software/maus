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


namespace MAUS {

  PyMODINIT_FUNC init_MapCppGlobalTrackMatching(void) {
    PyWrapMapBase<MAUS::MapCppGlobalTrackMatching>::PyWrapMapBaseModInit
                                    ("MapCppGlobalTrackMatching", "", "", "", "");
  }

  MapCppGlobalTrackMatching::MapCppGlobalTrackMatching()
    : MapBase<Data>("MapCppGlobalTrackMatching"), _configCheck(false) {
  }

  MapCppGlobalTrackMatching::~MapCppGlobalTrackMatching() {
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
    _classname = "MapCppGlobalTrackMatching-Through";
  }

  void MapCppGlobalTrackMatching::_death() {
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

    MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();

    if (!recon_events) {
      return;
    }

    MAUS::ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
	 recon_event_iter != recon_events->end();
	 ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      MAUS::ReconEvent* recon_event = (*recon_event_iter);
      if (recon_event->GetGlobalEvent()) {
	MAUS::GlobalEvent* global_event = recon_event->GetGlobalEvent();
	MAUS::recon::global::TrackMatching track_matching;
	track_matching.FormTracks(global_event, _classname);
      }
    }
  }
} // ~MAUS
