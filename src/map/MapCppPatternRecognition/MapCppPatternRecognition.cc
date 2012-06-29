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

#include "src/map/MapCppPatternRecognition/MapCppPatternRecognition.hh"

bool MapCppPatternRecognition::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppPatternRecognition";

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
  MiceModule* _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  ClustException = _configJSON["SciFiClustExcept"].asInt();

  return true;
}

bool MapCppPatternRecognition::death() {
  return true;
}

std::string MapCppPatternRecognition::process(std::string document) {
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  SciFiSpill spill;

  try {
    root = JsonWrapper::StringToJson(document);
    Json::Value spacepoints_array = JsonWrapper::GetProperty(root, "spacepoints",
                                                              JsonWrapper::arrayValue);
    // Json::Value spacepoints_value = JsonWrapper::GetProperty(root, "space_points",
    //                                                        JsonWrapper::objectValue);

    // Json::Value spacepoints_array = JsonWrapper::GetProperty(spacepoints_value, "tracker1",
    //                                                         JsonWrapper::arrayValue);

    // assert(spacepoints_array.isArray());
    make_SciFiSpill(spacepoints_array, spill);
    std::cout<< "number of events in spill is " << spill.events().size() <<std::endl;
    // digitization(spill, root);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  try { // ================= Reconstruction =========================
    for ( unsigned int k = 0; k < spill.events().size(); k++ ) {
      SciFiEvent event = *(spill.events()[k]);
      // Pattern Recognition.
      if ( event.spacepoints().size() ) {
        std::cout << "Calling Pattern Recognition..." << std::endl;
        if (event.spacepoints().size() < 3) {
          std::ofstream out1("params_recon.txt", std::ios::out | std::ios::app);
          out1 << "Less than 3 spacepoints in event" << std::endl;
        }
        std::ofstream out("circle.txt", std::ios::out | std::ios::app);
        for ( int l = 0; l < event.spacepoints().size(); ++l ) {
          out << event.spacepoints()[l]->get_position().x() << "\t";
        }
        for ( int l = 0; l < event.spacepoints().size(); ++l ) {
          out << event.spacepoints()[l]->get_position().y() << "\t";
        }
        out << std::endl;
        pattern_recognition(event);
      }

      print_event_info(event);
      save_to_json(event);
    }  // ==========================================================
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

void MapCppPatternRecognition::make_SciFiSpill(Json::Value spacepoint_array, SciFiSpill &spill) {
  for ( unsigned int i = 0; i < spacepoint_array.size(); i++ ) {
    SciFiEvent* an_event = new SciFiEvent();
    Json::Value event;
    event = spacepoint_array[i];
    for ( unsigned int j = 0; j < event.size(); j++ ) {
      Json::Value spacepoint;
      spacepoint = event[j];
      int tracker, station;
      CLHEP::Hep3Vector position;
      tracker  = spacepoint["tracker"].asInt();
      // tracker -= 1;
      station  = spacepoint["station"].asInt();
      double _x, _y, _z;
      // Protection against improper input not being of form 0.0, where the decimal is important!
      if ( spacepoint["position"]["x"].isInt() || spacepoint["position"]["y"].isInt()
           || spacepoint["position"]["z"].isInt() ) {
        if ( spacepoint["position"]["x"].isInt() ) {
          int tmpx = spacepoint["position"]["x"].asInt();
          _x = static_cast<double>(tmpx);
          _x +=.0001;
          double _y = spacepoint["position"]["y"].asDouble();
        } else if ( spacepoint["position"]["y"].isInt() ) {
          int tmpy = spacepoint["position"]["y"].asInt();
          _y = static_cast<double>(tmpy);
          _y +=.0001;
          double _x = spacepoint["position"]["x"].asDouble();
        } else if ( spacepoint["position"]["x"].isInt() && spacepoint["position"]["y"].isInt() ) {
          int tmpx = spacepoint["position"]["x"].asInt();
          _x = static_cast<double>(tmpx);
          _x +=.0001;
          int tmpy = spacepoint["position"]["y"].asInt();
          _y = static_cast<double>(tmpy);
          _y +=.0001;
        }
        _z = spacepoint["position"]["z"].asDouble();
        CLHEP::Hep3Vector pos(_x, _y, _z);
        position = pos;
      } else {
      position = JsonWrapper::JsonToThreeVector(spacepoint["position"]);
      std::cout << position <<std::endl;
      }
      // creates spacepoint pointer and initializes used condition to false
      SciFiSpacePoint* a_spacepoint = new SciFiSpacePoint();
      a_spacepoint->set_position(position);
      a_spacepoint->set_tracker(tracker);
      a_spacepoint->set_station(station);
      an_event->add_spacepoint(a_spacepoint);
     }     // ends loop over spacepoints in the event
    spill.add_event(an_event);
  } // ends loop over events.
}

void MapCppPatternRecognition::pattern_recognition(SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(evt);
}

void MapCppPatternRecognition::save_to_json(SciFiEvent &evt) {
  Json::Value sp_tracker0;
  Json::Value sp_tracker1;
  Json::Value channels;
  for ( unsigned int evt_i = 0; evt_i < evt.spacepoints().size(); evt_i++ ) {
    Json::Value spacepoints_in_event;
    channels = Json::Value(Json::arrayValue);
    channels.clear();
    int tracker = evt.spacepoints()[evt_i]->get_tracker();
    spacepoints_in_event["tracker"]= tracker;
    spacepoints_in_event["station"]= evt.spacepoints()[evt_i]->get_station();
    // spacepoints_in_event["npe"]    = evt.spacepoints()[evt_i]->get_npe();
    // spacepoints_in_event["time"]   = evt.spacepoints()[evt_i]->get_time();
    // spacepoints_in_event["type"]   = evt.spacepoints()[evt_i]->get_type();
    Hep3Vector pos = evt.spacepoints()[evt_i]->get_position();
    spacepoints_in_event["position"]["x"]   = pos.x();
    spacepoints_in_event["position"]["y"]   = pos.y();
    spacepoints_in_event["position"]["z"]   = pos.z();
    /* std::vector<SciFiCluster*> clusters_in_spacepoint = evt.spacepoints()[evt_i]->get_channels();
     for ( unsigned int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoints_in_event["channels"] = channels; */
    if ( tracker == 0 ) {
      sp_tracker0.append(spacepoints_in_event);
    }
    if ( tracker == 1 ) {
      sp_tracker1.append(spacepoints_in_event);
    }
  }
  root["space_points"]["tracker1"].append(sp_tracker0);
  root["space_points"]["tracker2"].append(sp_tracker1);

  // Helical Tracks.
  Json::Value h_tracks_tracker0;
  Json::Value h_tracks_tracker1;
  for ( unsigned int track_i = 0; track_i < evt.helicalprtracks().size(); track_i++ ) {
    Json::Value a_track;
    a_track["num_points"] = evt.helicalprtracks()[track_i].get_num_points();
    a_track["R"]          = evt.helicalprtracks()[track_i].get_R();
    a_track["dzds"]       = evt.helicalprtracks()[track_i].get_dzds();
    a_track["Phi_0"]      = evt.helicalprtracks()[track_i].get_phi0();
    a_track["starting_point"]["x"] = evt.helicalprtracks()[track_i].get_x0();
    a_track["starting_point"]["y"] = evt.helicalprtracks()[track_i].get_y0();
    a_track["starting_point"]["z"] = evt.helicalprtracks()[track_i].get_z0();
    if ( evt.helicalprtracks()[track_i].get_tracker() == 0 ) {
      h_tracks_tracker0.append(a_track);
    } else if ( evt.helicalprtracks()[track_i].get_tracker() == 1 ) {
      h_tracks_tracker1.append(a_track);
    }
  }
  root["tracks"]["tracker1"].append(h_tracks_tracker0);
  root["tracks"]["tracker2"].append(h_tracks_tracker1);
}

void MapCppPatternRecognition::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << " "
            << event.helicalprtracks().size() << " "
            << std::endl;
}

// The following two functions are added for testing purposes only
Json::Value MapCppPatternRecognition::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppPatternRecognition::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}
