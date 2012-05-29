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

  try { // ================= Reconstruction =========================
    root = JsonWrapper::StringToJson(document);

    if ( root["daq_event_type"].asString() == "physics_event" ) {

      digitization(spill, root);

      for ( unsigned int k = 0; k < spill.events().size(); k++ ) {
        SEEvent *event = (spill.events()[k]);
        // Build Clusters.
        if ( event->digits().size() ) {
          cluster_recon(event);
        }
        // Build SpacePoints.
        if ( event->clusters().size() ) {
          spacepoint_recon(event);
        }

        print_event_info(event, k);
        save_to_json(event, k);
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

void MapCppSingleStationRecon::cluster_recon(SEEvent *evt) {
  SEClusterRec clustering(ClustException, minPE);
  clustering.process(evt, modules);
}

void MapCppSingleStationRecon::spacepoint_recon(SEEvent *evt) {
  SESpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppSingleStationRecon::save_to_json(SEEvent *evt, int event_i) {
  // Get current event entry from the array.
  // Json::Value recon_event = JsonWrapper::GetItem(root["recon_events"],
  //                                                event_i,
  //                                                JsonWrapper::arrayValue);

  //
  // Append sci_fi_digits, sci_fi_clusters and sci_fi_space_points.
  // This is the single_station detector.
  //
  // ------- DIGITS -------------------------------------------------------
  Json::Value sci_fi_digits;
  for ( unsigned int dig_i = 0; dig_i < evt->digits().size(); dig_i++ ) {
    Json::Value digit;
    digit["plane"]  = evt->digits()[dig_i]->get_plane();
    digit["channel"]= evt->digits()[dig_i]->get_channel();
    digit["npe"]   = evt->digits()[dig_i]->get_npe();
    digit["time"]  = evt->digits()[dig_i]->get_time();
    // digits_in_event["adc"]   = evt->digits()[dig_i]->get_adc();
    digit["phys_evt_num"]   = evt->digits()[dig_i]->get_spill();
    digit["part_evt_num"]   = evt->digits()[dig_i]->get_event();
    sci_fi_digits.append(digit);
  }
  // ------- CLUSTERS -------------------------------------------------------
  Json::Value sci_fi_clusters;
  for ( unsigned int clust_i = 0; clust_i < evt->clusters().size(); clust_i++ ) {
    Json::Value cluster;
    cluster["plane"]  = evt->clusters()[clust_i]->get_plane();
    cluster["channel"]= evt->clusters()[clust_i]->get_channel();
    cluster["npe"]    = evt->clusters()[clust_i]->get_npe();
    cluster["time"]   = evt->clusters()[clust_i]->get_time();
    cluster["phys_evt_num"]   = evt->clusters()[clust_i]->get_spill();
    cluster["part_evt_num"]   = evt->clusters()[clust_i]->get_event();
    sci_fi_clusters.append(cluster);
  }
  // ------- SPACEPOINTS ----------------------------------------------------
  Json::Value sci_fi_space_points;
  for ( unsigned int sp_i = 0; sp_i < evt->spacepoints().size(); sp_i++ ) {
    Json::Value spacepoint;
    Json::Value channels;
    channels = Json::Value(Json::arrayValue);
    spacepoint["npe"]    = evt->spacepoints()[sp_i]->get_npe();
    // spacepoints_in_event["time"]   = evt.spacepoints()[evt_i]->get_time();
    spacepoint["type"]   = evt->spacepoints()[sp_i]->get_type();
    spacepoint["phys_event_number"] = evt->spacepoints()[sp_i]->get_spill();
    spacepoint["part_event_number"] = evt->spacepoints()[sp_i]->get_event();
    Hep3Vector pos = evt->spacepoints()[sp_i]->get_position();
    spacepoint["position"]["x"]   = pos.x();
    spacepoint["position"]["y"]   = pos.y();
    spacepoint["position"]["z"]   = pos.z();
    std::vector<SECluster*> clusters_in_spacepoint = evt->spacepoints()[sp_i]->get_channels();
    for ( unsigned int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoint["channels"] = channels;
    sci_fi_space_points.append(spacepoint);
  }

  //
  // Save everything in data structrure tree.
  //
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["single_station"].append(sci_fi_digits);
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["single_station"].append(sci_fi_clusters);
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["single_station"].append(sci_fi_space_points);
}
/*
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
*/
void MapCppSingleStationRecon::print_event_info(SEEvent *event, int k) {
  int exp_events = root["recon_events"].size();
/*
  // root["recon_events"][0]["tof_event"]["tof_space_points"]["tof0"]
   int tof1_spacepoints = 0;
  for ( int evt_i = 0; evt_i < exp_events; evt_i++ ) {
    if ( !root["recon_events"][evt_i]["tof_event"]["tof_space_points"]["tof0"][0].isNull() ) {
      tof1_spacepoints += 1;
    }
  }

  int tof0_spacepoints = 0;
  for ( int sp_i = 0; sp_i < exp_events_t0; sp_i++ ) {
    if ( !root["space_points"]["tof0"][k][sp_i].isNull() ) {
      tof0_spacepoints += 1;
    }
  }
*/
  std::cout << event->digits().size() << " "
            << event->clusters().size() << " "
            << event->spacepoints().size() << " "
            << exp_events << std::endl;
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
