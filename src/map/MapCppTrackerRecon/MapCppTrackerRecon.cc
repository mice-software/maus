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

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
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

  try {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    Json::Value *json = Globals::GetConfigurationCards();
    _helical_pr_on = (*json)["SciFiPRHelicalOn"].asBool();
    _straight_pr_on = (*json)["SciFiPRStraightOn"].asBool();
    return true;
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

bool MapCppTrackerRecon::death() {
  return true;
}

std::string MapCppTrackerRecon::process(std::string document) {
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
          cluster_recon(*event);
        }
        // Build SpacePoints.
        if ( event->clusters().size() ) {
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
  } catch(Squeal& squee) {
    squee.Print();
    root = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(root, squee, _classname);
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
  root = *spill_proc.CppToJson(spill, "");
}

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering;
  clustering.initialise();
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
  std::vector<KalmanSeed*> seeds;
  size_t number_helical_tracks  = evt.helicalprtracks().size();
  size_t number_straight_tracks = evt.straightprtracks().size();

  for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
    KalmanSeed *seed = new KalmanSeed();
    seed->Build(evt.helicalprtracks()[track_i]);
    seeds.push_back(seed);
  }

  for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
    KalmanSeed *seed = new KalmanSeed();
    seed->Build(evt.straightprtracks()[track_i]);
    seeds.push_back(seed);
  }

  if ( seeds.size() ) {
    KalmanTrackFit fit;
    fit.Process(seeds, evt);
  }
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << "; "
            << event.straightprtracks().size() << " "
            << event.helicalprtracks().size() << " " << std::endl;
}

} // ~namespace MAUS
