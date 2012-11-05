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

#include <algorithm>

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"

#include "Riostream.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TGraphErrors.h"
#include "TDecompChol.h"
#include "TDecompSVD.h"
#include "TF1.h"

namespace MAUS {

bool MapCppTrackerRecon::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerRecon";

  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // Get the tracker modules.
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  std::cerr << "Using geomtry: " << filename << std::endl;
  MiceModule* _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  ClustException = _configJSON["SciFiClustExcept"].asInt();

  // Get the flags for turning straight and helical pr on or off
  assert(_configJSON.isMember("SciFiPRHelicalOn"));
  _helical_pr_on = _configJSON["SciFiPRHelicalOn"].asBool();
  assert(_configJSON.isMember("SciFiPRStraightOn"));
  _straight_pr_on = _configJSON["SciFiPRStraightOn"].asBool();

  return true;
}

bool MapCppTrackerRecon::death() {
  return true;
}

std::string MapCppTrackerRecon::process(std::string document) {
  std::cout << "Reconstructing tracker data\n";
  Json::FastWriter writer;

  // Read in json data
  Spill spill;
  bool success = read_in_json(document, spill);
  if (!success)
    return writer.write(root);

  try { // ================= Reconstruction =========================
    if ( spill.GetReconEvents() ) {
      for ( unsigned int k = 0; k < spill.GetReconEvents()->size(); k++ ) {
        SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
        // Build Clusters.
        if ( event->digits().size() ) {
          std::cout << "Building clusters\n";
          cluster_recon(*event);
        }
        // Build SpacePoints.
        if ( event->clusters().size() ) {
          std::cout << "Building spacepoints\n";
          spacepoint_recon(*event);
        }
        // Pattern Recognition.
        if ( event->spacepoints().size() ) {
          std::cout << "Calling Pattern Recognition..." << std::endl;
          pattern_recognition(_helical_pr_on, _straight_pr_on, *event);
          std::cout << "Pattern Recognition complete." << std::endl;
        }
        // Kalman Track Fit.
         if ( event->straightprtracks().size() || event->helicalprtracks().size() ) {
          track_fit(*event);
         }

        print_event_info(*event);
      }
    } else {
      std::cout << "No recon events found\n";
    }
    save_to_json(spill);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["recon_failed"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  return writer.write(root);
}

bool MapCppTrackerRecon::read_in_json(std::string json_data, Spill &spill) {
  Json::Reader reader;
  Json::FastWriter writer;

  try {
    root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    spill = *spill_proc.JsonToCpp(root);
    return true;
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    writer.write(root);
    return false;
  }
}

void MapCppTrackerRecon::save_to_json(Spill &spill) {
  SpillProcessor spill_proc;
  root = *spill_proc.CppToJson(spill);
}

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(ClustException, minPE, modules);
  clustering.process(evt);
}

void MapCppTrackerRecon::spacepoint_recon(SciFiEvent &evt) {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerRecon::pattern_recognition(const bool helical_pr_on, const bool straight_pr_on,
                                             SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(helical_pr_on, straight_pr_on, evt);
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) {
  KalmanTrackFit fit;
  if ( evt.helicalprtracks().size() )
    fit.process(evt.helicalprtracks());
  if ( evt.straightprtracks().size() )
    fit.process(evt.straightprtracks());
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << "; "
            << event.straightprtracks().size() << " "
            << event.helicalprtracks().size() << " " << std::endl;
}

} // ~namespace MAUS

