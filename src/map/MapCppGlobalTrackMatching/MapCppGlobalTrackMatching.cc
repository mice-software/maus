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

#include <vector>

#include "src/common_cpp/API/PyWrapMapBase.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/TrackMatching.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/legacy/Config/MiceModule.hh"

#include "src/map/MapCppGlobalTrackMatching/MapCppGlobalTrackMatching.hh"

namespace MAUS {

  PyMODINIT_FUNC init_MapCppGlobalTrackMatching(void) {
    PyWrapMapBase<MapCppGlobalTrackMatching>::PyWrapMapBaseModInit
                                    ("MapCppGlobalTrackMatching", "", "", "", "");
  }

  MapCppGlobalTrackMatching::MapCppGlobalTrackMatching()
    : MapBase<Data>("MapCppGlobalTrackMatching"), _configCheck(false) {
  }

  void MapCppGlobalTrackMatching::_birth(const std::string& argJsonConfigDocument) {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    // Check if the JSON document can be parsed, else return error only.
    _configCheck = false;
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
        throw Exceptions::Exception(Exceptions::recoverable,
                        "Failed to parse configuration",
                        "MapCppGlobalTrackMatching::birth");
    }
    _configCheck = true;
    _mapper_name = "MapCppGlobalTrackMatching";
    _pid_hypothesis_string = _configJSON["track_matching_pid_hypothesis"].asString();
    Json::Value track_matching_tolerances = _configJSON["track_matching_tolerances"];
    _matching_tolerances["TOF0"] = std::make_pair(
        track_matching_tolerances["TOF0t"].asDouble(), 10000);
    _matching_tolerances["TOF1"] = std::make_pair(
        track_matching_tolerances["TOF1x"].asDouble(),
        track_matching_tolerances["TOF1y"].asDouble());
    _matching_tolerances["TOF2"] = std::make_pair(
        track_matching_tolerances["TOF2x"].asDouble(),
        track_matching_tolerances["TOF2y"].asDouble());
    _matching_tolerances["KL"] = std::make_pair(10000,
        track_matching_tolerances["KLy"].asDouble());
    _matching_tolerances["EMR"] = std::make_pair(
        track_matching_tolerances["EMRx"].asDouble(),
        track_matching_tolerances["EMRy"].asDouble());
    _energy_loss = _configJSON["track_matching_energy_loss"].asBool();
    std::string geo_filename = _configJSON["reconstruction_geometry_filename"].asString();
    MiceModule* geo_module = new MiceModule(geo_filename);
    std::vector<const MiceModule*> tofs =
        geo_module->findModulesByPropertyString("Detector", "TOF");
    double tof12_cdt = ((tofs.at(2)->globalPosition().z() -
        tofs.at(1)->globalPosition().z())/299.791);
    _matching_tolerances["TOF12dT"] = std::make_pair(
        tof12_cdt/track_matching_tolerances["TOF12maxSpeed"].asDouble(),
        tof12_cdt/track_matching_tolerances["TOF12minSpeed"].asDouble());
    bool no_check = _configJSON["track_matching_no_single_event_check"].asBool();
    Json::Value charge_thresholds = _configJSON["track_matching_check_charge_thresholds"];
    std::map<std::string, double> no_check_thresholds;
    no_check_thresholds["TOF0"] = charge_thresholds["TOF0"].asDouble();
    no_check_thresholds["TOF1"] = charge_thresholds["TOF1"].asDouble();
    no_check_thresholds["TOF2"] = charge_thresholds["TOF2"].asDouble();
    no_check_thresholds["KL"] = charge_thresholds["KL"].asDouble();
    _no_check_settings = std::make_pair(no_check, no_check_thresholds);
  }

  void MapCppGlobalTrackMatching::_death() {
  }

  void MapCppGlobalTrackMatching::_process(Data* data_cpp) const {
    // Read string and convert to a Json object
    if (!data_cpp) {
        throw Exceptions::Exception(Exceptions::recoverable,
                        "data_cpp was NULL",
                        "MapCppGlobalTrackMatching::_process");
    }
    if (!_configCheck) {
        throw Exceptions::Exception(Exceptions::recoverable,
                        "Birth has not been successfully called",
                        "MapCppGlobalTrackMatching::_process");
    }
    const Spill* spill = data_cpp->GetSpill();

    ReconEventPArray* recon_events = spill->GetReconEvents();
    if (!recon_events) {
      return;
    }

    for (auto recon_event_iter = recon_events->begin();
         recon_event_iter != recon_events->end();
         ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      ReconEvent* recon_event = (*recon_event_iter);
      GlobalEvent* global_event = recon_event->GetGlobalEvent();

      if (global_event) {
        recon::global::TrackMatching
            track_matching(global_event, _mapper_name, _pid_hypothesis_string,
                           _matching_tolerances, 10.0, _no_check_settings, _energy_loss);
        track_matching.USTrack();
        track_matching.DSTrack();
        track_matching.throughTrack();
      }
    }
  }
} // ~MAUS
