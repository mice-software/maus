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

namespace MAUS {

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
        if ( event->spacepoints().size() == 1 ) {
          Json::Value json_event = set_up_json(root, k, event);
          if ( is_good_for_tracking(json_event) ) {
            KalmanTrackFitSS fit;
            fit.process(json_event);
          }
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
      std::vector<SEDigit*> digits_in_cluster = clusters_in_spacepoint[cl]->get_digits();
      Json::Value digits;
      for ( unsigned int dig = 0; dig < digits_in_cluster.size(); ++dig ) {
        Json::Value digit;
        digit["adc"] = digits_in_cluster[dig]->get_adc();
        digit["npe"] = digits_in_cluster[dig]->get_npe();
        digits.append(digit);
      }
      cluster["digits"] = digits;
      channels.append(cluster);
    }
    spacepoint["channels"] = channels;
    sci_fi_space_points.append(spacepoint);
  }

  //
  // Save everything in data structrure tree.
  //
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["single_station"]
                                               = sci_fi_digits;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["single_station"]
                                               = sci_fi_clusters;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["single_station"]
                                               = sci_fi_space_points;
}

Json::Value MapCppSingleStationRecon::set_up_json(Json::Value &root, int event_i, SEEvent *event) {
  Json::Value tof0_sps = root["recon_events"][event_i]["tof_event"]["tof_space_points"]["tof0"];
  Json::Value tof1_sps = root["recon_events"][event_i]["tof_event"]["tof_space_points"]["tof1"];
  int numb_sp_tof_0 = tof0_sps.size();
  int numb_sp_tof_1 = tof1_sps.size();

  Json::Value json_event;

  int tof1_spill, tof1_event, tof0_spill, tof0_event, ss_spill, ss_event;

  bool found = false;
  int index_value;
  double tof1_slabx, tof1_slaby, tof1_time, tof0_slabx, tof0_slaby, tof0_time;
  if ( numb_sp_tof_1 == 1 ) {
    // Get TOF1 slab hits.
    tof1_slabx = tof1_sps[(Json::Value::ArrayIndex)0]["slabX"].asDouble();
    tof1_slaby = tof1_sps[(Json::Value::ArrayIndex)0]["slabY"].asDouble();
    tof1_time  = tof1_sps[(Json::Value::ArrayIndex)0]["time"].asDouble();
    tof1_spill = tof1_sps[(Json::Value::ArrayIndex)0]["phys_event_number"].asDouble();
    tof1_event = tof1_sps[(Json::Value::ArrayIndex)0]["part_event_number"].asDouble();
    // Find good TOF0 hit.
    for ( int i = 0; i < numb_sp_tof_0; ++i ) {
      tof0_time = tof0_sps[i]["time"].asDouble();
      tof0_spill = tof0_sps[i]["phys_event_number"].asDouble();
      tof0_event = tof0_sps[i]["part_event_number"].asDouble();
      double deltaT = tof1_time-tof0_time;
      if ( deltaT > 24.78 ) {
        found = true;
        index_value = i;
      }
    }
  }

  if ( found ) {
    tof0_slabx = tof0_sps[(Json::Value::ArrayIndex)index_value]["slabX"].asDouble();
    tof0_slaby = tof0_sps[(Json::Value::ArrayIndex)index_value]["slabY"].asDouble();
    json_event["TOF0"]["slabX"] = tof0_slabx;
    json_event["TOF0"]["slabY"] = tof0_slaby;
    json_event["TOF1"]["slabX"] = tof1_slabx;
    json_event["TOF1"]["slabY"] = tof1_slaby;
    json_event["TOF1"]["time"]  = tof1_time;
    json_event["TOF0"]["time"]  = tof0_time;
  } else {
    return json_event;
  }

  std::vector<SECluster*> clusters = event->spacepoints()[0]->get_channels();
  int size = clusters.size();
  for ( int clust_i = 0; clust_i < size; ++clust_i ) {
    if ( clusters[clust_i]->get_plane() == 0 )
      json_event["SS"]["Plane0"] = clusters[clust_i]->get_alpha();
    if ( clusters[clust_i]->get_plane() == 1 )
      json_event["SS"]["Plane1"] = clusters[clust_i]->get_alpha();
    if ( clusters[clust_i]->get_plane() == 2 )
      json_event["SS"]["Plane2"] = clusters[clust_i]->get_alpha();
  }
  int numb_spacepoints = event->spacepoints().size();
  json_event["number_ss_spacepoints"] = numb_spacepoints;
  Hep3Vector position = event->spacepoints()[0]->get_position();
  json_event["SS"]["position"]["x"] = position.x();
  json_event["SS"]["position"]["y"] = position.y();
  ss_spill = event->spacepoints()[0]->get_spill();
  ss_event = event->spacepoints()[0]->get_event();

  std::ofstream out2("part_event.txt", std::ios::out | std::ios::app);
  out2  << tof0_spill << " " << tof1_spill << " " << ss_spill <<  " "
        << tof0_event << " " << tof1_event << " " << ss_event << "\n";
  out2.close();

  return json_event;
}

void MapCppSingleStationRecon::reconstruct_tofs(Json::Value &root, int event_i,
                                                Hep3Vector &tof0, Hep3Vector &tof1,
                                                Hep3Vector &PR_pos, Hep3Vector &PR_mom,
                                                bool &success) {}

bool MapCppSingleStationRecon::is_good_for_tracking(Json::Value json_event) {
  bool goodness = false;

  if ( json_event.isMember("TOF0") && json_event.isMember("TOF1") &&
       json_event["SS"].isMember("Plane0") && json_event["SS"].isMember("Plane1") &&
       json_event["SS"].isMember("Plane2") ) {
    goodness = true;
  }

  double time_tof_0 = json_event["TOF0"]["time"].asDouble();
  double time_tof_1 = json_event["TOF1"]["time"].asDouble();
  double deltat = time_tof_1 - time_tof_0;

  if ( deltat < 24.78 ) {
    goodness = false;
  }

  return goodness;
}

void MapCppSingleStationRecon::print_event_info(SEEvent *event, int k) {
  std::cout << event->digits().size() << " "
            << event->clusters().size() << " "
            << event->spacepoints().size() << std::endl;
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
}
