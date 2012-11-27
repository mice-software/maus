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

#include "src/map/MapCppTrackerVirtualRecon/MapCppTrackerVirtualRecon.hh"
#include <algorithm>
#include "Riostream.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TGraphErrors.h"
#include "TDecompChol.h"
#include "TDecompSVD.h"
#include "TF1.h"

namespace MAUS {

bool MapCppTrackerVirtualRecon::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerVirtualRecon";
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful)
    return false;

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

  // Get the flags for turning straight and helical pr on or off
  assert(_configJSON.isMember("SciFiPRHelicalOn"));
  _helical_pr_on = _configJSON["SciFiPRHelicalOn"].asBool();
  assert(_configJSON.isMember("SciFiPRStraightOn"));
  _straight_pr_on = _configJSON["SciFiPRStraightOn"].asBool();

  return true;
}

bool MapCppTrackerVirtualRecon::death() {
  return true;
}

std::string MapCppTrackerVirtualRecon::process(std::string document) {
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  SciFiSpill spill;
  SciFiSpill spill2;

  //================first looks at spill from Recon====================
  try {root = JsonWrapper::StringToJson(document);
    Json::Value input_digits = root["recon_events"];
    Json::Value mc_info = root["mc_events"];
    fill_digits_vector(input_digits, spill);
    fill_virtualhits_vector(mc_info, spill2);
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
        std::cout << "This event has: " << event.spacepoints().size() <<"spacepoints\n";
        pattern_recognition(_helical_pr_on, _straight_pr_on, event);
        std::cout << "Pattern Recognition complete." << std::endl;
      }
      // Kalman Track Fit.
      if ( event.straightprtracks().size() || event.helicalprtracks().size() ) {
        track_fit(event);
      }

      print_event_info_recon(event);
      save_to_json_recon(event, k);
    }
  } catch(...) {
    return "";
  }
  // =================Looks at Virtual Hits=================================
  num_event_spill2 = 0;
  num_event_spill2 = spill2.events().size();

  try { // ================= Reconstruction =========================
    for ( unsigned int k = 0; k < spill2.events().size(); k++ ) {
      SciFiEvent event = *(spill2.events()[k]);
      if ( event.spacepoints().size() ) {
        pattern_recognition(_helical_pr_on, _straight_pr_on, event);
      }
      print_event_info_mc(event);
      save_to_json_mc(event, k);
    }  // ==========================================================
  } catch(...) {
    return "";
  }
  return writer.write(root);
}

// fill our virtual hits
void MapCppTrackerVirtualRecon::fill_virtualhits_vector(Json::Value &virtualhits,
                                                        MAUS::SciFiSpill &a_spill) {
  // vector of virtual hits
  for ( unsigned int event_i = 0; event_i < virtualhits.size(); event_i++ ) {
    SciFiEvent* an_event = new SciFiEvent();
    Json::Value _virtualhits = virtualhits[event_i]["virtual_hits"];
    for ( unsigned int j = 0; j < _virtualhits.size(); j++ ) {
      Json::Value hit;
      hit = _virtualhits[j];
      int  spill, event;
      double x, y, z;
      x = hit["position"]["x"].asDouble();
      y = hit["position"]["y"].asDouble();
      z = hit["position"]["z"].asDouble();
      spill   = hit["phys_evt_num"].asInt();
      event   = hit["part_evt_num"].asInt();
      ThreeVector position(x, y, z);
      SciFiSpacePoint* scifi_sp = new SciFiSpacePoint();
      find_tracker_station(*scifi_sp, static_cast<double>(z));
      scifi_sp->set_spill(spill);
      scifi_sp->set_event(event);
      scifi_sp->set_position(position);
      scifi_sp->set_used(false);
      scifi_sp->set_type("triplet");
      an_event->add_spacepoint(scifi_sp);
    } // ends loop over virtual hits in the event
    a_spill.add_event(an_event);
  } // ends loop over events
}

void MapCppTrackerVirtualRecon::find_tracker_station(SciFiSpacePoint &spacepoint,
                                                     double z_value) {
  if (z_value > -5800 && z_value < -4600) {
    spacepoint.set_tracker(0);
    if (z_value > -4700 && z_value < -4600)
      spacepoint.set_station(1);
    else if (z_value >-4900 && z_value < -4800)
      spacepoint.set_station(2);
    else if (z_value > -5150 && z_value< -5050)
      spacepoint.set_station(3);
    else if (z_value> -5450 && z_value< -5350)
      spacepoint.set_station(4);
    else
      spacepoint.set_station(5);
  }
  if (z_value > -983.5 && z_value < 216.5) {
    spacepoint.set_tracker(1);
    if ( z_value > 116.5 && z_value < 216.5 )
      spacepoint.set_station(5);
    if ( z_value > -233.5 && z_value < -133.5)
      spacepoint.set_station(4);
    if ( z_value > -533.5 && z_value < -433.5)
      spacepoint.set_station(3);
    if ( z_value > -783.5 && z_value < -683.5)
      spacepoint.set_station(2);
    if (z_value > -983.5 && z_value < -883.5)
      spacepoint.set_station(1);
  }
}


void MapCppTrackerVirtualRecon::fill_digits_vector(Json::Value &digits, SciFiSpill &a_spill) {
  for ( unsigned int event_i = 0; event_i < digits.size(); event_i++ ) {
    SciFiEvent* an_event = new SciFiEvent();
    Json::Value digits_tracker0 = digits[event_i]["sci_fi_event"]["sci_fi_digits"]["tracker0"];
    Json::Value digits_tracker1 = digits[event_i]["sci_fi_event"]["sci_fi_digits"]["tracker1"];
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
      // temp stuff
      double x, y, z, px, py, pz;
      x = digit["true_position"]["x"].asDouble();
      y = digit["true_position"]["y"].asDouble();
      z = digit["true_position"]["z"].asDouble();
      px = digit["true_momentum"]["x"].asDouble();
      py = digit["true_momentum"]["y"].asDouble();
      pz = digit["true_momentum"]["z"].asDouble();
      Hep3Vector position(x, y, z);
      Hep3Vector momentum(px, py, pz);
      a_digit->set_true_position(position);
      a_digit->set_true_momentum(momentum);
      // temp stuff
      an_event->add_digit(a_digit);
    } // ends loop over digits in the event
    a_spill.add_event(an_event);
  } // ends loop over events.
}
void MapCppTrackerVirtualRecon::fit(std::vector<SciFiSpacePoint*> spacepoints,
                             double &x_const, double &x_slope,
                             double &y_const, double &y_slope) {
#ifdef __CINT__
  gSystem->Load("libMatrix");
#endif
  const Int_t nrVar  = 2;
  // const Int_t nrPnts = 5;
  int nrPnts = spacepoints.size();
  double az[nrPnts];
  double ax[nrPnts];
  double ay[nrPnts];
  double ae[nrPnts];
  for ( int i = 0; i < nrPnts; ++i ) {
    az[i] = spacepoints[i]->get_position().z();
    ax[i] = spacepoints[i]->get_position().x();
    ay[i] = spacepoints[i]->get_position().y();
    ae[i] = 0.9;
  }

  TVectorD z;
  z.Use(nrPnts, az);
  TVectorD x;
  x.Use(nrPnts, ax);
  TVectorD y;
  y.Use(nrPnts, ay);
  TVectorD e;
  e.Use(nrPnts, ae);

  TMatrixD A(nrPnts, nrVar);
  TMatrixDColumn(A, 0) = 1.0;
  TMatrixDColumn(A, 1) = z;
  TMatrixD B(nrPnts, nrVar);
  TMatrixDColumn(B, 0) = 1.0;
  TMatrixDColumn(B, 1) = y;

  const TVectorD solve_x = NormalEqn(A, x, e);
  solve_x.Print();
  x_const = solve_x(0);
  x_slope = solve_x(1);

  const TVectorD solve_y = NormalEqn(A, y, e);
  solve_y.Print();
  y_const = solve_y(0);
  y_slope = solve_y(1);
}

void MapCppTrackerVirtualRecon::cluster_recon(SciFiEvent &evt) {
  SciFiClusterRec clustering(ClustException, minPE, modules);
  clustering.process(evt);
}

void MapCppTrackerVirtualRecon::spacepoint_recon(SciFiEvent &evt) {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerVirtualRecon::pattern_recognition(const bool helical_pr_on,
                                                    const bool straight_pr_on, SciFiEvent &evt) {
  PatternRecognition pr1;
  pr1.process(helical_pr_on, straight_pr_on, evt);
}

void MapCppTrackerVirtualRecon::track_fit(SciFiEvent &evt) {
  KalmanTrackFit fit;
  if ( evt.helicalprtracks().size() )
    fit.process(evt.helicalprtracks());
  if ( evt.straightprtracks().size() )
    fit.process(evt.straightprtracks());
}

void MapCppTrackerVirtualRecon::save_to_json_recon(SciFiEvent &evt, int event_i) {
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
/*
      std::vector<SciFiDigit*> digits_in_cluster = clusters_in_spacepoint[cl]->get_digits();
      Json::Value digits;
      for ( unsigned int dig = 0; dig < digits_in_cluster.size(); ++dig ) {
        Json::Value digit;
        digit["adc"] = digits_in_cluster[dig]->get_adc();
        digit["tdc"] = digits_in_cluster[dig]->get_tdc();
        digit["npe"] = digits_in_cluster[dig]->get_npe();
        digits.append(digit);
      }
      cluster["digits"] = digits;
*/
      channels.append(cluster);
    }
    spacepoint["channels"] = channels;
    if ( tracker == 0 ) sci_fi_space_points_tracker0.append(spacepoint);
    if ( tracker == 1 ) sci_fi_space_points_tracker1.append(spacepoint);
  }
  // ------- TRACKS ----------------------------------------------------
  // Straight tracks
  Json::Value s_tracks_tracker0;
  Json::Value s_tracks_tracker1;
  for ( unsigned int track_i = 0; track_i < evt.straightprtracks().size(); track_i++ ) {
    Json::Value track;
    track["num_points"] = evt.straightprtracks()[track_i]->get_num_points();
    track["x0"] = evt.straightprtracks()[track_i]->get_x0();
    track["y0"] = evt.straightprtracks()[track_i]->get_y0();
    track["mx"] = evt.straightprtracks()[track_i]->get_mx();
    track["my"] = evt.straightprtracks()[track_i]->get_my();
    track["x_chisq"] = evt.straightprtracks()[track_i]->get_x_chisq();
    track["y_chisq"] = evt.straightprtracks()[track_i]->get_y_chisq();
    int tracker = evt.straightprtracks()[track_i]->get_tracker();
    track["tracker"] = tracker;
    if ( tracker == 0 ) {
      s_tracks_tracker0.append(track);
    } else if ( tracker == 1 ) {
      s_tracks_tracker1.append(track);
    }
  }

  // Helical Tracks.
  Json::Value h_tracks_tracker0;
  Json::Value h_tracks_tracker1;
  for ( unsigned int track_i = 0; track_i < evt.helicalprtracks().size(); track_i++ ) {
    Json::Value track;
    track["num_points"] = evt.helicalprtracks()[track_i]->get_num_points();
    track["R"]          = evt.helicalprtracks()[track_i]->get_R();
    track["dsdz"]       = evt.helicalprtracks()[track_i]->get_dsdz();
    track["Phi_0"]      = evt.helicalprtracks()[track_i]->get_phi0();
    track["starting_point"]["x"] = evt.helicalprtracks()[track_i]->get_x0();
    track["starting_point"]["y"] = evt.helicalprtracks()[track_i]->get_y0();
    track["starting_point"]["z"] = evt.helicalprtracks()[track_i]->get_z0();
    int tracker = evt.helicalprtracks()[track_i]->get_tracker();
    track["tracker"] = tracker;
    if ( tracker == 0 ) {
      h_tracks_tracker0.append(track);
    } else if ( tracker == 1 ) {
      h_tracks_tracker1.append(track);
    }
  }
  //
  // Save everything in data structrure tree.
  //
  // Tracker 0 -------------------------------------------------------------------
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["tracker0"]
                                               = sci_fi_clusters_tracker0;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["tracker0"]
                                               = sci_fi_space_points_tracker0;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_pr_tracks"]["straight"]["tracker0"]
                                               = s_tracks_tracker0;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_pr_tracks"]["helical"]["tracker0"]
                                               = h_tracks_tracker0;
  // Tracker 1 -------------------------------------------------------------------
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["tracker1"]
                                               = sci_fi_clusters_tracker1;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["tracker1"]
                                               = sci_fi_space_points_tracker1;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_pr_tracks"]["straight"]["tracker1"]
                                               = s_tracks_tracker1;
  root["recon_events"][event_i]["sci_fi_event"]["sci_fi_pr_tracks"]["helical"]["tracker1"]
                                               = h_tracks_tracker1;
}

void MapCppTrackerVirtualRecon::print_event_info_recon(SciFiEvent &event) {
  std::cout << event.digits().size() << " "
            << event.clusters().size() << " "
            << event.spacepoints().size() << "; "
            << event.straightprtracks().size() << " "
            << event.helicalprtracks().size() << " " << std::endl;
}

void MapCppTrackerVirtualRecon::print_event_info_mc(MAUS::SciFiEvent &event) {
    std::cout << event.spacepoints().size() << "; "
              << event.straightprtracks().size() << " "
              << event.helicalprtracks().size() << " " << std::endl;
}

void MapCppTrackerVirtualRecon::save_to_json_mc(MAUS::SciFiEvent &evt, int event_i) {
    std::ofstream out_file("virtual_mc_output.json", std::ios::out | std::ios::app);
    if (event_i ==0)
      {  out_file << "{";
        out_file << "\"daq_event_type\": \"physics_event\",";
        out_file <<"\"recon_events\": [";
      } else {
        out_file <<",";
      } // was }, -but wrong?
    out_file << "{"; // added this, removed it from if statement above //
    out_file << "\"part_event_number\":" << event_i << ",";
    out_file << "\"sci_fi_event\":{";
    out_file << "\"sci_fi_pr_tracks\":{";
    // ------- TRACKS ----------------------------------------------------
    //=========== Straight tracks: Needs to be filled here=============
    out_file <<"\"straight\":{";
    int tracker0_hits_s = 0;
    if (evt.straightprtracks().size() == 0) {
      out_file << "\"tracker0\":null,";
      out_file << "\"tracker1\":null},";
    }
    // Only look at tracks in tracker 0 //
    for (unsigned int track_i_s = 0; track_i_s < evt.straightprtracks().size(); track_i_s++) {
      int tracker_s = evt.straightprtracks()[track_i_s]->get_tracker();
      std::cout << evt.straightprtracks()[track_i_s]->get_tracker();
      if (tracker_s ==0) {
        tracker0_hits_s++;
        if (tracker0_hits_s == 1) {
            out_file << "\"tracker0\":[";
            out_file << "{";
          }
        Json::Value track;
        track["mx"] = evt.straightprtracks()[track_i_s]->get_mx();
        track["my"] = evt.straightprtracks()[track_i_s]->get_my();
        track["num_points"] = evt.straightprtracks()[track_i_s]->get_num_points();
        track["x0"] = evt.straightprtracks()[track_i_s]->get_x0();
        track["x_chisq"] = evt.straightprtracks()[track_i_s]->get_x_chisq();
        track["y0"] = evt.straightprtracks()[track_i_s]->get_y0();
        track["y_chisq"] = evt.straightprtracks()[track_i_s]->get_y_chisq();
        out_file << "\"mx\":" <<evt.straightprtracks()[track_i_s]->get_mx() << ",";
        out_file << "\"my\":" <<evt.straightprtracks()[track_i_s]->get_my() << ",";
        out_file << "\"num_points\":" <<evt.straightprtracks()[track_i_s]->get_num_points() << ",";
        out_file << "\"x0\":" <<evt.straightprtracks()[track_i_s]->get_x0() << ",";
        out_file << "\"x_chisq\":" << evt.straightprtracks()[track_i_s]->get_x_chisq() << ",";
        out_file << "\"y0\":" <<evt.straightprtracks()[track_i_s]->get_y0() << ",";
        out_file << "\"y_chisq\":" <<evt.straightprtracks()[track_i_s]->get_y_chisq();
        out_file << "}]" << ",";
      }
    if (tracker0_hits_s == 0)
      out_file << "\"tracker0\":null,";
    }

    /////////////////////// TRACKER 1 TRACKS //////////////////
    int tracker1_hits_s = 0;
    for (unsigned int track_i_s = 0; track_i_s <evt.straightprtracks().size(); track_i_s++) {
      int tracker = evt.straightprtracks()[track_i_s]->get_tracker();
      if (tracker == 1) {
        tracker1_hits_s++;
        if (tracker1_hits_s == 1) {
          out_file << "\"tracker1\":[";
          out_file << "{";
        }
        Json::Value track;
        track["mx"] = evt.straightprtracks()[track_i_s]->get_mx();
        track["my"] = evt.straightprtracks()[track_i_s]->get_my();
        track["num_points"] = evt.straightprtracks()[track_i_s]->get_num_points();
        track["x0"] = evt.straightprtracks()[track_i_s]->get_x0();
        track["x_chisq"] = evt.straightprtracks()[track_i_s]->get_x_chisq();
        track["y0"] = evt.straightprtracks()[track_i_s]->get_y0();
        track["y_chisq"] = evt.straightprtracks()[track_i_s]->get_y_chisq();
        out_file << "\"mx\":" << evt.straightprtracks()[track_i_s]->get_mx() << ",";
        out_file << "\"my\":" <<evt.straightprtracks()[track_i_s]->get_my() << ",";
        out_file << "\"num_points\":" <<evt.straightprtracks()[track_i_s]->get_num_points() <<",";
        out_file << "\"x0\":" <<evt.straightprtracks()[track_i_s]->get_x0() << ",";
        out_file << "\"x_chisq\":" <<evt.straightprtracks()[track_i_s]->get_x_chisq() << ",";
        out_file << "\"y0\":" << evt.straightprtracks()[track_i_s]->get_y0() << ",";
        out_file << "\"y_chisq\":" << evt.straightprtracks()[track_i_s]->get_y_chisq();
        out_file << "}]" << "},";
      }
        if (tracker1_hits_s == 0)
          out_file << "\"tracker1\":null}";
    }


    // =================HELICAL TRACKS==================
    out_file <<"\"helical\":{";
    int tracker0_hits = 0;
    if (evt.helicalprtracks().size() ==0) {
      out_file << "\"tracker0\":null,";
      out_file << "\"tracker1\":null}},";
    }

    // Only look at tracks in tracker0
    for ( unsigned int track_i = 0; track_i < evt.helicalprtracks().size(); track_i++ ) {
      int tracker = evt.helicalprtracks()[track_i]->get_tracker();
      std::cout << evt.helicalprtracks()[track_i]->get_tracker();
      if (tracker ==0) {
        tracker0_hits++;
        if  (tracker0_hits ==1) {
          out_file << "\"tracker0\":[";
          out_file << "{";
        }
        Json::Value track;
        track["num_points"] = evt.helicalprtracks()[track_i]->get_num_points();
        track["R"]          = evt.helicalprtracks()[track_i]->get_R();
        track["dsdz"]       = evt.helicalprtracks()[track_i]->get_dsdz();
        track["Phi_0"]      = evt.helicalprtracks()[track_i]->get_phi0();
        track["starting_point"]["x"] = evt.helicalprtracks()[track_i]->get_x0();
        track["starting_point"]["y"] = evt.helicalprtracks()[track_i]->get_y0();
        track["starting_point"]["z"] = evt.helicalprtracks()[track_i]->get_z0();

        out_file << "\"num_points\":" << evt.helicalprtracks()[track_i]->get_num_points() << ",";
        out_file << "\"R\":" <<evt.helicalprtracks()[track_i]->get_R() <<",";
        out_file << "\"dsdz\":" <<evt.helicalprtracks()[track_i]->get_dsdz() << ",";
        out_file << "\"Phi_0\":" <<evt.helicalprtracks()[track_i]->get_phi0() <<",";
        out_file << "\"x\":" << evt.helicalprtracks()[track_i]->get_x0() << ",";
        out_file << "\"y\":" <<evt.helicalprtracks()[track_i]->get_y0() << ",";
        out_file << "\"z\":" <<evt.helicalprtracks()[track_i]->get_z0() << "}]" << ",";
      }
      if (tracker0_hits == 0)
        out_file << "\"tracker0\":null,";
    }

    // TRACKER 1 TRACKS:
    int tracker1_hits = 0;
    for ( unsigned int track_i = 0; track_i < evt.helicalprtracks().size(); track_i++ ) {
      int tracker = evt.helicalprtracks()[track_i]->get_tracker();
      if (tracker ==1) {
        tracker1_hits++;
        if  (tracker1_hits ==1) {
          out_file << "\"tracker1\":[";
          out_file << "{";
         }
        Json::Value track;
        track["num_points"] = evt.helicalprtracks()[track_i]->get_num_points();
        track["R"]          = evt.helicalprtracks()[track_i]->get_R();
        track["dsdz"]       = evt.helicalprtracks()[track_i]->get_dsdz();
        track["Phi_0"]      = evt.helicalprtracks()[track_i]->get_phi0();
        track["starting_point"]["x"] = evt.helicalprtracks()[track_i]->get_x0();
        track["starting_point"]["y"] = evt.helicalprtracks()[track_i]->get_y0();
        track["starting_point"]["z"] = evt.helicalprtracks()[track_i]->get_z0();

        out_file << "\"num_points\":" <<evt.helicalprtracks()[track_i]->get_num_points() <<",";
        out_file << "\"R\":" <<evt.helicalprtracks()[track_i]->get_R() <<",";
        out_file << "\"dsdz\":" <<evt.helicalprtracks()[track_i]->get_dsdz()  <<",";
        out_file << "\"Phi_0\":" <<evt.helicalprtracks()[track_i]->get_phi0()  <<",";
        out_file << "\"x\":" <<evt.helicalprtracks()[track_i]->get_x0()  <<",";
        out_file << "\"y\":" <<evt.helicalprtracks()[track_i]->get_y0()  <<",";
        out_file << "\"z\":" <<evt.helicalprtracks()[track_i]->get_z0() << "}]}},";
      }
      if (tracker1_hits == 0) out_file << "\"tracker1\":null}},";
    }
    out_file <<"\"sci_fi_spacepoints\":{";
    if (evt.spacepoints().size() ==0) {
      out_file << "\"tracker0\":null,";
      out_file << "\"tracker1\":null},";
    }

    // ------- SPACEPOINTS --TRACKER 0------------------------------------------
    int spacepoint_tracker_0 = 0;
    int spacepoint_tracker_1 =0;

    for ( unsigned int sp_i = 0; sp_i < evt.spacepoints().size(); sp_i++ ) {
      int tracker = evt.spacepoints()[sp_i]->get_tracker();
      // Json::Value spacepoint;
      if (tracker ==0)
        spacepoint_tracker_0++;
      else
        spacepoint_tracker_1++;
    }

    int spacepoint_0 =0;
    for ( unsigned int sp_i = 0; sp_i < evt.spacepoints().size(); sp_i++ ) {
      // spacepoints_in_event["time"]   = evt.spacepoints()[evt_i]->get_time();
      int tracker = evt.spacepoints()[sp_i]->get_tracker();
      if (tracker == 0) {
        spacepoint_0++;
        if  (spacepoint_0 ==1) {
          out_file << "\"tracker0\":[";
        }
        Hep3Vector pos = evt.spacepoints()[sp_i]->get_position();
        out_file << "{";
        out_file << "\"npe\":0" << ",";
        out_file << "\"part_event_number\":" << evt.spacepoints()[sp_i]->get_event() << ",";
        out_file << "\"phys_event_number\":" << evt.spacepoints()[sp_i]->get_spill() << ",";
        out_file << "\"position\":{";
        out_file << "\"x\":" << pos.x() << ",";
        out_file << "\"y\":" <<pos.y()  <<",";
        out_file << "\"z\":" <<pos.z()<< "}"  <<",";
        out_file << "\"station\":" << evt.spacepoints()[sp_i]->get_station()  <<",";
        out_file << "\"tracker\":" << tracker  <<",";
        if (spacepoint_0 == spacepoint_tracker_0) {
          out_file << "\"type\":\"triplet\"}],";
        } else {
          out_file << "\"type\":\"triplet\"},";
        }
      }
      if (spacepoint_tracker_0 ==0) {
        out_file << "\"tracker0\":null,";
      }
    }

    int spacepoint_1 = 0;
    for ( unsigned int sp_i = 0; sp_i < evt.spacepoints().size(); sp_i++ ) {
      int tracker = evt.spacepoints()[sp_i]->get_tracker();
      if (tracker == 1) {
        spacepoint_1++;
        if  (spacepoint_1 ==1) {
          out_file << "\"tracker1\":[";
        }
        Hep3Vector pos = evt.spacepoints()[sp_i]->get_position();
        out_file << "{";
        out_file << "\"npe\":0" << ",";
        out_file << "\"part_event_number\":" << evt.spacepoints()[sp_i]->get_event() << ",";
        out_file << "\"phys_event_number\":" << evt.spacepoints()[sp_i]->get_spill() << ",";
        out_file << "\"position\":{";
        out_file << "\"x\":" << pos.x()  <<",";
        out_file << "\"y\":" <<pos.y()  <<",";
        out_file << "\"z\":" <<pos.z()<< "}"  <<",";
        out_file << "\"station\":" << evt.spacepoints()[sp_i]->get_station()  <<",";
        out_file << "\"tracker\":" << tracker <<",";
        // for last spacepoint
        if (spacepoint_1 == spacepoint_tracker_1) {
          out_file << "\"type\":\"triplet\"}]}}}";// was }]}}
        } else {
          out_file << "\"type\":\"triplet\"},";
        }
      }
      if (spacepoint_tracker_1 == 0) {
        out_file << "\"tracker1\":null}}},";
      }
    }
    if (num_event_spill2-1 == event_i) {
      out_file << "]}\n"; // was other way
    }
    out_file.close();
  }
}
