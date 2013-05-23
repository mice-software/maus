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

#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"

namespace MAUS {

MapCppTrackerRecon::MapCppTrackerRecon()
    : _spill_json(NULL), _spill_cpp(NULL) {
}

MapCppTrackerRecon::~MapCppTrackerRecon() {
  if (_spill_json != NULL) {
      delete _spill_json;
  }
  if (_spill_cpp != NULL) {
      delete _spill_cpp;
  }
}

bool MapCppTrackerRecon::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerRecon";

  try {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    Json::Value *json = Globals::GetConfigurationCards();
    _helical_pr_on  = (*json)["SciFiPRHelicalOn"].asBool();
    _straight_pr_on = (*json)["SciFiPRStraightOn"].asBool();
    _kalman_on      = (*json)["SciFiKalmanOn"].asBool();
    _size_exception = (*json)["SciFiClustExcept"].asInt();
    _min_npe        = (*json)["SciFiNPECut"].asDouble();

    MiceModule* module = Globals::GetReconstructionMiceModules();
    std::vector<const MiceModule*> modules =
      module->findModulesByPropertyString("SensitiveDetector", "SciFi");
    _geometry_map = SciFiGeometryHelper(modules).build_geometry_map();
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
  read_in_json(document);
  Spill& spill = *_spill_cpp;

  // Json::Value root = JsonWrapper::StringToJson(document);
  // SpillProcessor spill_proc;
  // Spill spill = *spill_proc.JsonToCpp(root);

  try { // ================= Reconstruction =========================
    if ( spill.GetReconEvents() ) {
    std::cerr << "Spill has " << spill.GetReconEvents()->size() << " events." << std::endl;
      for ( unsigned int k = 0; k < spill.GetReconEvents()->size(); k++ ) {
        std::cerr << "Processing event " << k << std::endl;
        SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
        // Build Clusters.
        // Squeak::mout(Squeak::info) << "Digits found " <<
        //          event->digits().size() << std::endl;
        if ( event->digits().size() ) {
          cluster_recon(*event);
        }
        // Build SpacePoints.
        if ( event->clusters().size() ) {
          spacepoint_recon(*event);
        }
        // Pattern Recognition.
        if ( event->spacepoints().size() ) {
          std::cout << "Calling Pattern Recognition, helical " << _helical_pr_on;
          std::cout << ", straight " << _straight_pr_on << std::endl;
          pattern_recognition(_helical_pr_on, _straight_pr_on, *event);
          std::cout << "Pattern Recognition complete." << std::endl;
        }
        // Kalman Track Fit.
        if ( _kalman_on ) {
          if ( event->straightprtracks().size() || event->helicalprtracks().size() ) {
            track_fit(*event);
          }
        }
/*
        if ( _kalman_on && event->spacepoints().size()==5 && event->clusters().size()==15) {
          kalman_fit(*event);
        }
*/
        print_event_info(*event);
      }
    } else {
      std::cout << "No recon events found\n";
    }
    save_to_json(spill);
  } catch(Squeal& squee) {
    squee.Print();
    // _spill_json = MAUS::CppErrorHandler::getInstance()
    //                                   ->HandleSqueal(_spill_json, squee, _classname);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["recon_failed"] = ss.str();
    (*_spill_json)["errors"] = errors;
    return writer.write(_spill_json);
  }
  return writer.write(*_spill_json);
}

void MapCppTrackerRecon::read_in_json(std::string json_data) {
  Json::Reader reader;
  Json::Value json_root;
  Json::FastWriter writer;

  if (_spill_cpp != NULL) {
    delete _spill_cpp;
    _spill_cpp = NULL;
  }

  try {
    json_root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    _spill_cpp = spill_proc.JsonToCpp(json_root);
  } catch(...) {
    std::cerr << "Bad json document" << std::endl;
    _spill_cpp = new Spill();
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _spill_cpp->GetErrors();
  }
}

void MapCppTrackerRecon::save_to_json(Spill &spill) {
    SpillProcessor spill_proc;
    if (_spill_json != NULL) {
        delete _spill_json;
        _spill_json = NULL;
    }
    _spill_json = spill_proc.CppToJson(spill, "");
}

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(_size_exception, _min_npe, _geometry_map);
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
    seed->Build<SciFiHelicalPRTrack>(evt.helicalprtracks()[track_i]);
    seeds.push_back(seed);
  }

  for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
    KalmanSeed *seed = new KalmanSeed();
    seed->Build<SciFiStraightPRTrack>(evt.straightprtracks()[track_i]);
    seeds.push_back(seed);
  }

  if ( seeds.size() ) {
    KalmanTrackFit fit;
    fit.Process(seeds, evt);
  }
}

void MapCppTrackerRecon::kalman_fit(SciFiEvent &evt) {
  std::vector<KalmanSeed*> seeds;

  KalmanSeed *the_seed = new KalmanSeed();
  the_seed->Build(evt, _helical_pr_on);

  seeds.push_back(the_seed);

  KalmanTrackFit fit;
  fit.Process(seeds, evt);
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) {
  Squeak::mout(Squeak::info) << event.digits().size() << " "
                              << event.clusters().size() << " "
                              << event.spacepoints().size() << "; "
                              << event.straightprtracks().size() << " "
                              << event.helicalprtracks().size() << "; ";
  for ( size_t track_i = 0; track_i < event.scifitracks().size(); track_i++ ) {
    Squeak::mout(Squeak::info) << " Chi2: " << event.scifitracks()[track_i]->f_chi2() << "; "
                                << " P-Value: " << event.scifitracks()[track_i]->P_value() << "; ";
  }
  Squeak::mout(Squeak::info) << std::endl;
}

} // ~namespace MAUS
