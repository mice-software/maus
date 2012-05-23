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

#include "src/map/MapCppSingleStationRecon/MapCppSingleStationRecon.hh"

bool MapCppSingleStationRecon::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppSingleStationRecon";

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
  // std::cout << "Looking up modules...";
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  // assert(modules.size()>0);

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  // Get the value above which an Exception is thrown
  assert(_configJSON.isMember("SciFiClustExcept"));
  ClustException = _configJSON["SciFiClustExcept"].asInt();

  return true;
}

bool MapCppSingleStationRecon::death() {
  return true;
}

std::string MapCppSingleStationRecon::process(std::string document) {
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  SESpill spill;
  // std::cout << "Start Recon" << std::endl;
/*
  try {
    root = JsonWrapper::StringToJson(document);
    digitization(spill, root);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }*/

  try { // ================= Reconstruction =========================
    root = JsonWrapper::StringToJson(document);

    if ( root["daq_event_type"].asString() == "physics_event" ) {

    digitization(spill, root);

    for ( unsigned int k = 0; k < spill.events().size(); k++ ) {
      SEEvent event = *(spill.events()[k]);
      // Build Clusters.
      if ( event.digits().size() ) {
        cluster_recon(event);
      }
      // Build SpacePoints.
      if ( event.clusters().size() ) {
        spacepoint_recon(event);
      }

      // Build Global Track.
      if ( is_good_for_track(root, event, k) ) {
        global_track_fit(root, event, k);
      }

      print_event_info(event, k);
      save_to_json(event);
      }
    } // ==========================================================
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["recon_failed"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  // std::cout << "End Recon" << std::endl;
  return writer.write(root);
}

// === The Digits vectors is filled, either by running real data ==========
// === digitization or by reading-in an already existing digits branch.====
void MapCppSingleStationRecon::digitization(SESpill &spill, Json::Value &root) {
  if ( root.isMember("daq_data") && !root["daq_data"].isNull() ) {
    Json::Value daq = root.get("daq_data", 0);
    SEDigitization digitization;
    digitization.process(spill, daq);
  } else {
    throw 0;
  }
}

void MapCppSingleStationRecon::cluster_recon(SEEvent &evt) {
  SEClusterRec clustering(ClustException, minPE);
  clustering.process(evt, modules);
}

void MapCppSingleStationRecon::spacepoint_recon(SEEvent &evt) {
  SESpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppSingleStationRecon::global_track_fit(Json::Value root, SEEvent &event, int k) {
  // assert(root["space_points"]["tof0"][k][(Json::Value::ArrayIndex)0].isMember("slabX"));
  // assert(root["space_points"]["tof1"][k][(Json::Value::ArrayIndex)0].isMember("slabX"));

  Json::Value tof0_spacepoint = root["space_points"]["tof0"][k][(Json::Value::ArrayIndex)0];
  Json::Value tof1_spacepoint = root["space_points"]["tof1"][k][(Json::Value::ArrayIndex)0];

  int tof0_slabx = tof0_spacepoint["slabX"].asInt();
  int tof0_slaby = tof0_spacepoint["slabY"].asInt();
  double tof0_time  = tof0_spacepoint["time"].asDouble();

  int tof1_slabx = tof1_spacepoint["slabX"].asInt();
  int tof1_slaby = tof1_spacepoint["slabY"].asInt();
  double tof1_time  = tof1_spacepoint["time"].asDouble();

  // std::cout << tof0_slabx << " " << tof0_slaby << " " << tof0_time << std::endl;
  // std::cout << tof1_slabx << " " << tof1_slaby << " " << tof1_time << std::endl;

  // KalmanTrackFit fit;
  // fit.process(evt);
}

void MapCppSingleStationRecon::save_to_json(SEEvent &evt) {
  Json::Value doublet_clusters;
  for ( unsigned int dig_i = 0; dig_i < evt.clusters().size(); dig_i++ ) {
    Json::Value digits_in_event;
    digits_in_event["plane"]  = evt.clusters()[dig_i]->get_plane();
    digits_in_event["channel"]= evt.clusters()[dig_i]->get_channel();
    digits_in_event["npe"]    = evt.clusters()[dig_i]->get_npe();
    digits_in_event["time"]   = evt.clusters()[dig_i]->get_time();
    doublet_clusters.append(digits_in_event);
  }
  root["doublet_clusters"]["single_station"].append(doublet_clusters);

  Json::Value digits;
  for ( unsigned int dig_i = 0; dig_i < evt.digits().size(); dig_i++ ) {
    Json::Value digits_in_event;
    digits_in_event["plane"]  = evt.digits()[dig_i]->get_plane();
    digits_in_event["channel"]= evt.digits()[dig_i]->get_channel();
    digits_in_event["npe"]   = evt.digits()[dig_i]->get_npe();
    digits_in_event["time"]  = evt.digits()[dig_i]->get_time();
    digits_in_event["adc"]   = evt.digits()[dig_i]->get_adc();
    digits.append(digits_in_event);
  }
  root["digits"]["single_station"].append(digits);

  Json::Value se_sp;
  Json::Value channels;
  for ( unsigned int evt_i = 0; evt_i < evt.spacepoints().size(); evt_i++ ) {
    Json::Value spacepoints_in_event;
    channels = Json::Value(Json::arrayValue);
    channels.clear();
    // int tracker = evt.spacepoints()[evt_i]->get_tracker();
    spacepoints_in_event["npe"]    = evt.spacepoints()[evt_i]->get_npe();
    // spacepoints_in_event["time"]   = evt.spacepoints()[evt_i]->get_time();
    spacepoints_in_event["type"]   = evt.spacepoints()[evt_i]->get_type();
    Hep3Vector pos = evt.spacepoints()[evt_i]->get_position();
    spacepoints_in_event["position"]["x"]   = pos.x();
    spacepoints_in_event["position"]["y"]   = pos.y();
    spacepoints_in_event["position"]["z"]   = pos.z();
    std::vector<SECluster*> clusters_in_spacepoint = evt.spacepoints()[evt_i]->get_channels();
    for ( unsigned int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoints_in_event["channels"] = channels;
    se_sp.append(spacepoints_in_event);
  }
  root["space_points"]["single_station"].append(se_sp);
}

bool MapCppSingleStationRecon::is_good_for_track(Json::Value root, SEEvent &event, int k) {
  int exp_events_t1 = root["space_points"]["tof1"][k].size();
  int tof1_spacepoints = 0;
  for ( int sp_i = 0; sp_i < exp_events_t1; sp_i++ ) {
    if ( !root["space_points"]["tof1"][k][sp_i].isNull() ) {
      tof1_spacepoints += 1;
    }
  }
  int exp_events_t0 = root["space_points"]["tof0"].size();
  int tof0_spacepoints = 0;
  for ( int sp_i = 0; sp_i < exp_events_t0; sp_i++ ) {
    if ( !root["space_points"]["tof0"][k][sp_i].isNull() ) {
      tof0_spacepoints += 1;
    }
  }

  if ( tof0_spacepoints == 1 && tof1_spacepoints == 1 && event.spacepoints().size() == 1 ) {
    return true;
  } else {
    return false;
  }
}

void MapCppSingleStationRecon::print_event_info(SEEvent &event, int k) {
  int exp_events_t1 = root["space_points"]["tof1"][k].size();
  int tof1_spacepoints = 0;
  for ( int sp_i = 0; sp_i < exp_events_t1; sp_i++ ) {
    if ( !root["space_points"]["tof1"][k][sp_i].isNull() ) {
      tof1_spacepoints += 1;
    }
  }
  int exp_events_t0 = root["space_points"]["tof0"].size();
  int tof0_spacepoints = 0;
  for ( int sp_i = 0; sp_i < exp_events_t0; sp_i++ ) {
    if ( !root["space_points"]["tof0"][k][sp_i].isNull() ) {
      tof0_spacepoints += 1;
    }
  }

  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << " "
            << tof0_spacepoints << " " << tof1_spacepoints << std::endl;

}

// The following two functions are added for testing purposes only
Json::Value MapCppSingleStationRecon::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppSingleStationRecon::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}
