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

bool MapCppTrackerRecon::death() {
  return true;
}

std::string MapCppTrackerRecon::process(std::string document) {
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  SciFiSpill spill;

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
  }
  try { // ================= Reconstruction =========================
    for ( unsigned int k = 0; k < spill.events().size(); k++ ) {
      SciFiEvent event = *(spill.events()[k]);
      // Build Clusters.
      if ( event.digits().size() ) {
        cluster_recon(event);
      }
      // Build SpacePoints.
      if ( event.clusters().size() ) {
        spacepoint_recon(event);
      }
      // Pattern Recognition.
      if ( event.spacepoints().size() ) {
        std::cout << "Calling Pattern Recognition..." << std::endl;
        pattern_recognition(event);
      }
      // Kalman Track Fit.
      if ( event.straightprtracks().size() ) {
        track_fit(event);
      }
      print_event_info(event);
      save_to_json(event, k);
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

// === The Digits vectors is filled, either by running real data ==========
// === digitization or by reading-in an already existing digits branch.====
void MapCppTrackerRecon::digitization(SciFiSpill &spill, Json::Value &root) {
  if ( root.isMember("daq_data") && !root["daq_data"].isNull() ) {
    Json::Value daq = root.get("daq_data", 0);
    RealDataDigitization real;
    real.process(spill, daq);
  } else if ( root.isMember("mc_events") ) {
    // Json::Value digits = root.get("tracker_digits", 0);
    Json::Value digits = root["recon_events"];
    fill_digits_vector(digits, spill);
  } else {
    throw 0;
  }
}

void MapCppTrackerRecon::fill_digits_vector(Json::Value &mc_event, SciFiSpill &a_spill) {
  int number_events = mc_event.size();
  for ( unsigned int event_i = 0; event_i < number_events; event_i++ ) {
    SciFiEvent* an_event = new SciFiEvent();
    // Json::Value digits;
    Json::Value digits_tracker0 = mc_event[event_i]["sci_fi_event"]["sci_fi_digits"]["tracker0"];
    Json::Value digits_tracker1 = mc_event[event_i]["sci_fi_event"]["sci_fi_digits"]["tracker1"];
    Json::Value digits_merged = digits_tracker0;
    for ( unsigned int idig = 0; idig < digits_tracker1.size(); ++idig ) {
      digits_merged[digits_merged.size()] = digits_tracker1[idig];
    }
    for ( unsigned int j = 0; j < digits_merged.size(); j++ ) {
      Json::Value digit;
      digit = digits_merged[j];
      int tracker, station, plane, channel;
      double npe, time;
      int spill = 99;
      int event = 99;
      tracker = digit["tracker"].asInt();
      station = digit["station"].asInt();
      plane   = digit["plane"].asInt();
      channel = digit["channel"].asInt();
      npe     = digit["npe"].asDouble();
      time    = digit["time"].asInt();
      SciFiDigit* a_digit = new SciFiDigit(spill, event,
                                           tracker, station, plane, channel,
                                           npe, time);
      an_event->add_digit(a_digit);
    } // ends loop over digits in the event
    a_spill.add_event(an_event);
  } // ends loop over events.
}

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(ClustException, minPE);
  clustering.process(evt, modules);
}

void MapCppTrackerRecon::spacepoint_recon(SciFiEvent &evt) {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerRecon::pattern_recognition(SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(evt);
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) {
  KalmanTrackFit fit;
  fit.process(evt);
}

void MapCppTrackerRecon::save_to_json(SciFiEvent &evt, int event_i) {
  // ------- DIGITS -------------------------------------------------------
  Json::Value sci_fi_digits_tracker0;
  Json::Value sci_fi_digits_tracker1;
  for ( unsigned int dig_i = 0; dig_i < evt.digits().size(); dig_i++ ) {
    Json::Value digit;
    int tracker = evt.digits()[dig_i]->get_tracker();
    digit["tracker"]= tracker;
    digit["station"]= evt.digits()[dig_i]->get_station();
    digit["plane"]  = evt.digits()[dig_i]->get_plane();
    digit["channel"]= evt.digits()[dig_i]->get_channel();
    digit["npe"]    = evt.digits()[dig_i]->get_npe();
    digit["time"]   = evt.digits()[dig_i]->get_time();
    if ( tracker == 0 ) sci_fi_digits_tracker0.append(digit);
    if ( tracker == 1 ) sci_fi_digits_tracker1.append(digit);
  }
  // ------- CLUSTERS -------------------------------------------------------
  Json::Value sci_fi_clusters_tracker0;
  Json::Value sci_fi_clusters_tracker1;
  for ( unsigned int clust_i = 0; clust_i < evt.clusters().size(); clust_i++ ) {
    Json::Value cluster;
    int tracker = evt.clusters()[clust_i]->get_tracker();
    cluster["tracker"]= tracker;
    cluster["station"]= evt.clusters()[clust_i]->get_station();
    cluster["plane"]  = evt.clusters()[clust_i]->get_plane();
    cluster["channel"]= evt.clusters()[clust_i]->get_channel();
    cluster["npe"]    = evt.clusters()[clust_i]->get_npe();
    cluster["time"]   = evt.clusters()[clust_i]->get_time();
    cluster["phys_evt_num"]   = evt.clusters()[clust_i]->get_spill();
    cluster["part_evt_num"]   = evt.clusters()[clust_i]->get_event();
    if ( tracker == 0 ) sci_fi_clusters_tracker0.append(cluster);
    if ( tracker == 1 ) sci_fi_clusters_tracker1.append(cluster);
  }
  // ------- SPACEPOINTS ----------------------------------------------------
  Json::Value sci_fi_space_points_tracker0;
  Json::Value sci_fi_space_points_tracker1;
  for ( unsigned int sp_i = 0; sp_i < evt.spacepoints().size(); sp_i++ ) {
    Json::Value spacepoint;
    Json::Value channels;
    channels = Json::Value(Json::arrayValue);
    spacepoint["npe"]    = evt.spacepoints()[sp_i]->get_npe();
    // spacepoints_in_event["time"]   = evt.spacepoints()[evt_i]->get_time();
    int tracker = evt.spacepoints()[sp_i]->get_tracker();
    spacepoint["tracker"]= tracker;
    spacepoint["station"]= evt.spacepoints()[sp_i]->get_station();
    spacepoint["type"]   = evt.spacepoints()[sp_i]->get_type();
    spacepoint["phys_event_number"] = evt.spacepoints()[sp_i]->get_spill();
    spacepoint["part_event_number"] = evt.spacepoints()[sp_i]->get_event();
    Hep3Vector pos = evt.spacepoints()[sp_i]->get_position();
    spacepoint["position"]["x"]   = pos.x();
    spacepoint["position"]["y"]   = pos.y();
    spacepoint["position"]["z"]   = pos.z();
    std::vector<SciFiCluster*> clusters_in_spacepoint = evt.spacepoints()[sp_i]->get_channels();
    for ( unsigned int cl = 0; cl < clusters_in_spacepoint.size(); cl++ ) {
      Json::Value cluster;
      cluster["plane_number"] = clusters_in_spacepoint[cl]->get_plane();
      cluster["channel_number"] = clusters_in_spacepoint[cl]->get_channel();
      cluster["npe"] = clusters_in_spacepoint[cl]->get_npe();
      channels.append(cluster);
    }
    spacepoint["channels"] = channels;
    if ( tracker == 0 ) sci_fi_space_points_tracker0.append(spacepoint);
    if ( tracker == 1 ) sci_fi_space_points_tracker1.append(spacepoint);
  }
  // ------- TRACKS ----------------------------------------------------
  Json::Value tracks_tracker0;
  Json::Value tracks_tracker1;
  for ( unsigned int track_i = 0; track_i < evt.straightprtracks().size(); track_i++ ) {
    Json::Value track;
    track["num_points"] = evt.straightprtracks()[track_i].get_num_points();
    track["x0"] = evt.straightprtracks()[track_i].get_x0();
    track["y0"] = evt.straightprtracks()[track_i].get_y0();
    track["mx"] = evt.straightprtracks()[track_i].get_mx();
    track["my"] = evt.straightprtracks()[track_i].get_my();
    track["x_chisq"] = evt.straightprtracks()[track_i].get_x_chisq();
    track["y_chisq"] = evt.straightprtracks()[track_i].get_y_chisq();
    int tracker = evt.straightprtracks()[track_i].get_tracker();
    track["tracker"] = tracker;
    if ( tracker == 0 ) {
      tracks_tracker0.append(track);
    } else if ( tracker == 1 ) {
      tracks_tracker1.append(track);
    }
  }
  // ------ Helical Tracks -----------------------------------------------
  Json::Value h_tracks_tracker0;
  Json::Value h_tracks_tracker1;
  for ( unsigned int track_i = 0; track_i < evt.prtracks().size(); track_i++ ) {
    Json::Value a_track;
    a_track["num_points"] = evt.prtracks()[track_i].get_num_points();
    a_track["R"]          = evt.prtracks()[track_i].get_R();
    a_track["dzds"]       = evt.prtracks()[track_i].get_dzds();
    a_track["Phi_0"]      = evt.prtracks()[track_i].get_phi0();
    a_track["starting_point"]["x"] = evt.prtracks()[track_i].get_x0();
    a_track["starting_point"]["y"] = evt.prtracks()[track_i].get_y0();
    a_track["starting_point"]["z"] = evt.prtracks()[track_i].get_z0();
    if ( evt.prtracks()[track_i].get_tracker() == 0 ) {
      h_tracks_tracker0.append(a_track);
    } else if ( evt.prtracks()[track_i].get_tracker() == 1 ) {
      h_tracks_tracker1.append(a_track);
    }
  }

  //
  // Save everything in data structrure tree.
  //
  // Tracker 0 -------------------------------------------------------------------
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["tracker0"]
                                               = sci_fi_digits_tracker0;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["tracker0"]
                                               = sci_fi_clusters_tracker0;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["tracker0"]
                                               = sci_fi_space_points_tracker0;
  // root["recon_events"][event_i]["sci_fi_event"]["sci_fi_tracks"]["tracker0"]
  //                                             = tracks_tracker0;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_tracks"]["tracker0"]
                                               = h_tracks_tracker0;
  // Tracker 1 -------------------------------------------------------------------
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["tracker1"]
                                               = sci_fi_digits_tracker1;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["tracker1"]
                                               = sci_fi_clusters_tracker1;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["tracker1"]
                                               = sci_fi_space_points_tracker1;
  // root["recon_events"][event_i]["sci_fi_event"]["sci_fi_tracks"]["tracker1"]
  //                                             = tracks_tracker1;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_tracks"]["tracker1"]
                                               = h_tracks_tracker1;
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << " "
            << event.straightprtracks().size() << " " << std::endl;
}

// The following two functions are added for testing purposes only
Json::Value MapCppTrackerRecon::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppTrackerRecon::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}
