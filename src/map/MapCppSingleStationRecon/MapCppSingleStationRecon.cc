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
#include <iostream>
#include <fstream>

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
        //
        Hep3Vector tof0, tof1, PR_pos, PR_mom;
        Hep3Vector ss(666., 666., 666.);
        // double tof0_x, tof0_y, tof1_x, tof1_y;
        // double tof0_time, tof1_time;
        bool success = false;
        if ( event->spacepoints().size() == 1 ) {
          std::vector<SECluster*> clusters = event->spacepoints()[0]->get_channels();
          // std::cerr << "Clusters size: " << clusters.size() << "\n";
          int size = clusters.size();
          for ( int clust_i = 0; clust_i < size; ++clust_i ) {
            // std::cerr << clust_i << "\n";
            if ( clusters[clust_i]->get_plane() == 0 ) ss.setX(clusters[clust_i]->get_alpha());
            if ( clusters[clust_i]->get_plane() == 1 ) ss.setY(clusters[clust_i]->get_alpha());
            if ( clusters[clust_i]->get_plane() == 2 ) ss.setZ(clusters[clust_i]->get_alpha());
          }
          reconstruct_tofs(root, k, tof0, tof1, PR_pos, PR_mom, success);
        }
        if ( success ) {
          std::cerr << "TOF0 " << tof0(0) << " " << tof0(1) << "\n"
                    << "TOF1 " << tof1(0) << " " << tof1(1) << "\n";
          std::cerr << "SS position " << event->spacepoints()[0]->get_position().x() << " "
                    << event->spacepoints()[0]->get_position().y() << "\n";

          std::ofstream out2("detectors.txt", std::ios::out | std::ios::app);
                 out2 << tof0(0)    << " " << tof0(1) << " " << tof1(0)    << " " << tof1(1) << " "
                 << event->spacepoints()[0]->get_position().x() << " "
                 << event->spacepoints()[0]->get_position().y() << "\n";
                 out2.close();


          std::cerr << "Starting Global Recon" << std::endl;
          KalmanTrackFitSS fit;
          // fit.process(tof0, ss, tof1, PR_pos, PR_mom);
          // ++eff_counter;
        }
        //
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

void MapCppSingleStationRecon::reconstruct_tofs(Json::Value &root, int event_i,
                                                Hep3Vector &tof0, Hep3Vector &tof1,
                                                Hep3Vector &PR_pos, Hep3Vector &PR_mom,
                                                bool &success) {
  // Define some constants.
  static const int tof0_num_slabs = 10;
  static const int tof1_num_slabs = 7;
  static const double tof0_a = 40.; // mm
  static const double tof1_a = 60.; // mm
  // assert(root.isMember("recon_events"));
  // assert(root["recon_events"][event_i].isMember("tof_event"));
  Json::Value tof0_sps = root["recon_events"][event_i]["tof_event"]["tof_space_points"]["tof0"];
  Json::Value tof1_sps = root["recon_events"][event_i]["tof_event"]["tof_space_points"]["tof1"];
  int numb_sp_tof_0 = tof0_sps.size();
  int numb_sp_tof_1 = tof1_sps.size();

  std::cerr << "Number of spacepoints " << numb_sp_tof_0 << " " << numb_sp_tof_1 << "\n";

  // Run elementar TOF Recon.
  double tof0_slabx, tof0_slaby;
  double tof1_slabx, tof1_slaby;
  double tof0_x, tof0_y, tof1_x, tof1_y;
  double tof0_time, tof1_time;
  bool found = false;
  int index_value;

  if ( numb_sp_tof_1 == 1 ) {
    // Get TOF1 slab hits.
    // std::cout << tof1_sps[(Json::Value::ArrayIndex)0] << std::endl;
    tof1_slabx = tof1_sps[(Json::Value::ArrayIndex)0]["slabX"].asDouble();
    tof1_slaby = tof1_sps[(Json::Value::ArrayIndex)0]["slabY"].asDouble();
    tof1_time  = tof1_sps[(Json::Value::ArrayIndex)0]["time"].asDouble();
    // Find good TOF0 hit.
    for ( int i = 0; i < numb_sp_tof_0; ++i ) {
      tof0_time = tof0_sps[i]["time"].asDouble();
      if ( tof0_time > -50. && tof0_time < 0.0 ) {
        found = true;
        index_value = i;
      }
    }
  }

  if ( found ) {
    // Do TOF reconstruction
    tof0_slabx = tof0_sps[(Json::Value::ArrayIndex)index_value]["slabX"].asDouble();
    tof0_slaby = tof0_sps[(Json::Value::ArrayIndex)index_value]["slabY"].asDouble();

    // tof0_time         = tof0_sps[(Json::Value::ArrayIndex)0]["time"].asDouble();
    // tof1_time         = tof1_sps[(Json::Value::ArrayIndex)0]["time"].asDouble();

    tof0_x = -(tof0_slaby - (tof0_num_slabs - 1.)/2.)*tof0_a;
    tof0_y = (tof0_slabx - (tof0_num_slabs - 1.)/2.)*tof0_a;
    Hep3Vector tof0_sp(tof0_x, tof0_y, tof0_time);

    tof1_x = -(tof1_slaby - (tof1_num_slabs - 1.)/2.)*tof1_a;
    tof1_y = (tof1_slabx - (tof1_num_slabs - 1.)/2.)*tof1_a;
    Hep3Vector tof1_sp(tof1_x, tof1_y, tof1_time);

    tof0 = tof0_sp;
    tof1 = tof1_sp;
    success = true;
    // std::cerr << "TOF0 pos (mm): " << tof0_x << " " << tof0_y << "\n"
    //          << "TOF1 pos (mm): " << tof1_x << " " << tof1_y << "\n";
  }
  // Basic PR
  double tof0_z = 0.; // mm
  double tof1_z = 3924.55+3899.55; // mm
  const double m_pi = 139.570; // MeV/c2
  double pr_x = tof0_x; // mm
  double pr_y = tof0_y; // mm
  double pr_z = tof0_z;

  double delta_x = (tof1_x-tof0_x); // mm
  double delta_y = (tof1_y-tof0_y); // mm
  double delta_z = tof1_z-tof0_z;       // mm
  double pr_mx   = delta_x/delta_z;
  double pr_my   = delta_y/delta_z;
  double pr_pz   = m_pi*delta_z/abs(tof0_time);

/*
  double pr_px   = m_pi*delta_x/abs(tof0_time);
  double pr_py   = m_pi*delta_y/abs(tof0_time);
  double pr_pz   = m_pi*delta_z/abs(tof0_time);
*/
  // Build track fit input parameters.
  tof0.setX(tof0_slabx);
  tof0.setY(tof0_slaby);
  tof0.setZ(tof0_time);

  tof1.setX(tof1_slabx);
  tof1.setY(tof1_slaby);
  tof1.setZ(tof1_time);

  PR_pos.setX(pr_x);
  PR_pos.setY(pr_y);
  PR_pos.setZ(0.0);

  PR_mom.setX(pr_mx);
  PR_mom.setY(pr_my);
  PR_mom.setZ(pr_pz);
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
