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

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/reduce/ReduceCppTracker/ReduceCppTracker.hh"

bool ReduceCppTracker::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "ReduceCppTracker";
  _filename = "cosmics.root";
  _nSpills = 0;
  // tree->Branch("event","Event",&event,64000,1);

  // Efficiencies: initialise arrays.
  for ( int j = 0; j < 3; ++j ) {
    _plane_array[j] = j;
  }
  for ( int i = 0; i < 214; ++i ) {
    _channel_array[i] = i;
  }
  //
  // Canvases
  //
  // TCanvas *c1 = new TCanvas("c1", "Plane Profile (tracker 0)", 200, 10, 700, 500);
  // TCanvas *c2 = new TCanvas("c2", "Plane Profile (tracker 1)", 200, 10, 700, 500);
  TCanvas *c1 = new TCanvas("c1", "SpacePoints", 200, 10, 700, 500);
  // TCanvas *c4 = new TCanvas("c4", "Cluster NPE", 200, 10, 700, 500);
  // TCanvas *c5 = new TCanvas("c5", "Digits Time", 200, 10, 700, 500);

  // c1->Divide(3, 5);
  // c2->Divide(3, 5);
  c1->Divide(7, 2);
  // c4->Divide(3, 1);
  // c5->Divide(1, 1);

  gStyle->SetLabelSize(0.07, "xyz");
  gStyle->SetTitleSize(0.07, "xy");
  gStyle->SetTitleOffset(0.6, "x");
  gStyle->SetTitleOffset(0.4, "y");

  //
  // Histograms
  //
  triplets = new TH2F("triplets", "Current Spill (x, y)", 300, -150, 150, 300, -150, 150);
  duplets  = new TH2F("duplets",  "Current Spill (x, y)", 300, -150, 150, 300, -150, 150);
  triplets_copy = new TH2F("triplets_copy", "All Spills (x, y)", 300, -150, 150, 300, -150, 150);
  duplets_copy  = new TH2F("duplets_copy", "All Spills (x, y)", 300, -150, 150, 300, -150, 150);
/*
  triplets->SetMarkerStyle(20);
  triplets->SetMarkerColor(kBlue);
  triplets_copy->SetMarkerStyle(20);
  triplets_copy->SetMarkerColor(kBlue);
  duplets->SetMarkerStyle(20);
  duplets->SetMarkerColor(kRed);
  duplets_copy->SetMarkerStyle(20);
  duplets_copy->SetMarkerColor(kRed);
*/
/*
  // Clusters
  _hist_plane0 = new TH1F("_hist_plane0", "plane 0", 215, 0, 214);
  _hist_plane1 = new TH1F("_hist_plane1", "plane 1", 215, 0, 214);
  _hist_plane2 = new TH1F("_hist_plane2", "plane 2", 215, 0, 214);
  _chan_sum = new TH1F("_hist_plane2", "Sum Of Channel Numb", 641, 0, 640);
  _npe_plane0 = new TH1F("_npe_plane0", "plane 0", 31, 0, 30);
  _npe_plane1 = new TH1F("_npe_plane1", "plane 1", 31, 0, 30);
  _npe_plane2 = new TH1F("_npe_plane2", "plane 2", 31, 0, 30);

  // Digits
  _adc_plane0 = new TH1F("_adc_plane0", "plane 0 (ADC-PEDESTAL)", 256, 0, 255);
  _adc_plane1 = new TH1F("_adc_plane1", "plane 1 (ADC-PEDESTAL)", 256, 0, 255);
  _adc_plane2 = new TH1F("_adc_plane2", "plane 2 (ADC-PEDESTAL)", 256, 0, 255);
  _dig_npe_plane0 = new TH1F("_dig_npe_plane0", "plane 0 (NPE)", 31, 0, 30);
  _dig_npe_plane1 = new TH1F("_dig_npe_plane1", "plane 1 (NPE)", 31, 0, 30);
  _dig_npe_plane2 = new TH1F("_dig_npe_plane2", "plane 2 (NPE)", 31, 0, 30);
*/
  //
  // Trees
  //
  _unpacked.SetNameTitle("daq", "daq");
  _unpacked.Branch("adc", &_adc, "adc/I");
  _unpacked.Branch("tdc", &_tdc, "tdc/I");
  _unpacked.Branch("bank", &_bank, "bank/I");
  _unpacked.Branch("chan", &_chan, "chan/I");
  _unpacked.Branch("board", &_board, "board/I");

  _digits.SetNameTitle("digits", "digits");
  _digits.Branch("tracker", &_tracker_dig, "tracker/I");
  _digits.Branch("station", &_station_dig, "station/I");
  _digits.Branch("plane", &_plane_dig, "plane/I");
  _digits.Branch("channel", &_channel_dig, "channel/D");
  _digits.Branch("npe", &_npe_dig, "npe/D");
  _digits.Branch("time", &_time, "time/D");

  _doublet_clusters.SetNameTitle("clusters", "clusters");
  _doublet_clusters.Branch("tracker", &_tracker, "tracker/I");
  _doublet_clusters.Branch("station", &_station, "station/I");
  _doublet_clusters.Branch("plane", &_plane, "plane/I");
  _doublet_clusters.Branch("channel", &_channel, "channel/D");
  _doublet_clusters.Branch("npe", &_npe, "npe/D");

  _spacepoints.SetNameTitle("spacepoints", "spacepoints");
  _spacepoints.Branch("pe", &_pe, "pe/D");
  _spacepoints.Branch("x", &_x, "x/D");
  _spacepoints.Branch("y", &_y, "y/D");
  _spacepoints.Branch("z", &_z, "z/D");
  _spacepoints.Branch("tracker", &_tracker_sp, "tracker/I");
  _spacepoints.Branch("station", &_station_sp, "station/I");
  _spacepoints.Branch("type", &_type, "type/I");
/*
  _spacepointscopy.SetNameTitle("spacepoints_copy", "spacepoints_copy");
  _spacepointscopy.Branch("pe", &_pe, "pe/D");
  _spacepointscopy.Branch("x", &_x, "x/D");
  _spacepointscopy.Branch("y", &_y, "y/D");
  _spacepointscopy.Branch("z", &_z, "z/D");
  _spacepointscopy.Branch("tracker", &_tracker_sp, "tracker/I");
  _spacepointscopy.Branch("station", &_station_sp, "station/I");
  _spacepointscopy.Branch("type", &_type, "type/I");
*/
/*
  c1->SetFillColor(21);
  c1->GetFrame()->SetFillColor(42);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

  c2->SetFillColor(21);
  c2->GetFrame()->SetFillColor(42);
  c2->GetFrame()->SetBorderSize(6);
  c2->GetFrame()->SetBorderMode(-1);

  gStyle->SetLabelSize(0.07, "xyz");
  gStyle->SetTitleSize(0.07, "xy");
  gStyle->SetTitleOffset(0.6, "x");
  gStyle->SetTitleOffset(0.4, "y");
*/
  // JsonCpp setup
  Json::Value configJSON;
  try {
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    // this will contain the configuration
    return true;
  } catch(Squeal squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

std::string  ReduceCppTracker::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;

  Squeak::activateCout(1);
  // Check if the JSON document can be parsed, else return error only
  try {
    root = JsonWrapper::StringToJson(document);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  try {
    if ( is_physics_daq_event(root) ) {
      unpacked_data_tree(root);

      // Find number of recon events.
      int n_events = root["recon_events"].size();
      for ( int event_i = 0; event_i < n_events; ++event_i ) {
        _this_is_tracker0_event = false;
        _this_is_tracker1_event = false;
        Json::Value event = root["recon_events"][event_i];
        if ( event.isNull() ) continue;
        // std::cerr << "Merging..." << std::endl;
        merge_tracker_events(event);
        if ( !is_good_event() ) continue;
        // std::cerr << "Digits..." << std::endl;
        digits_tree();

        // std::cerr << "Clusters..." << std::endl;
        doublet_clusters_tree();

        // std::cerr << "Spacepoints..." << std::endl;
        spacepoints_tree();
        // _spacepoints.Reset();
        // draw_spacepoints(root);

        // compute_stations_efficiencies(event);
        json_daq.clear();
        json_digits.clear();
        json_clusters.clear();
        json_spacepoints.clear();
        // save_efficiency(root);
      }
    }
  } catch(Squeal squee) {
     Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

  _nSpills++;

  if ( !(_nSpills%100) ) {
    save();
  }

  // display_histograms();
  return document;
}

bool ReduceCppTracker::death()  {
  save();
  return true;
}

void ReduceCppTracker::save()  {
  std::ofstream file1;
  file1.open("efficiency_fibres.txt");
  for ( int plane_i = 0; plane_i < 30; ++plane_i ) {
    for ( int fibre_j = 0; fibre_j < 214; ++fibre_j ) {
      file1 << plane_i << " " << fibre_j << " "
            << _map_planes_total[plane_i][fibre_j] << " "
            << _map_planes_hit[plane_i][fibre_j] << "\n";
    }
  }
  file1.close();
/*
    file4.open("efficiency_station.txt");
    file4 << 0 << " " << _plane_0_hits << " " << _plane_0_counter << "\n";
    file4 << 1 << " " << _plane_1_hits << " " << _plane_1_counter << "\n";
    file4 << 2 << " " << _plane_2_hits << " " << _plane_2_counter << "\n";
    file4.close();
*/

  TFile root_file("cosmics.root", "RECREATE");
  _unpacked.Write();
  _digits.Write();
  _doublet_clusters.Write();
  _spacepoints.Write();
  root_file.Close();
}

bool ReduceCppTracker::is_physics_daq_event(Json::Value root) {
  if (root.isMember("daq_data") &&
      !root["daq_data"].isNull() &&
      ( root["daq_data"].isMember("tracker1") || root["daq_data"].isMember("tracker2") ) &&
      root["daq_event_type"].asString() == "physics_event") {
    return true;
  } else {
    return false;
  }
}

bool ReduceCppTracker::is_good_event() {
  int numb_spacepoints = json_spacepoints.size();
  bool st1_tracker0 = false;
  bool st5_tracker0 = false;
  bool st1_tracker1 = false;
  bool st5_tracker1 = false;
  for ( int sp_i = 0; sp_i < numb_spacepoints; ++sp_i ) {
    int tracker = json_spacepoints[sp_i]["tracker"].asInt();
    int station = json_spacepoints[sp_i]["station"].asInt();
    if ( station == 1 && tracker == 0 ) st1_tracker0 = true;
    if ( station == 5 && tracker == 0 ) st5_tracker0 = true;
    if ( station == 1 && tracker == 1 ) st1_tracker1 = true;
    if ( station == 5 && tracker == 1 ) st5_tracker1 = true;
  }
  if ( st1_tracker0 && st5_tracker0 ) {
    _this_is_tracker0_event = true;
  } else if ( st1_tracker1 && st5_tracker1 )  {
    _this_is_tracker1_event = true;
  }

  if ( _this_is_tracker0_event || _this_is_tracker1_event ) {
    return true;
  } else {
    return false;
  }
}

void ReduceCppTracker::merge_tracker_events(Json::Value &event) {
  // --------------------------------------------------------------
  //
  // Collect digits.
  //
  Json::Value digits = event["sci_fi_event"]["sci_fi_digits"];
  if ( digits.isMember("tracker0") && !digits["tracker0"].isNull() ) {
    json_digits = JsonWrapper::GetProperty(digits,
                                           "tracker0",
                                           JsonWrapper::arrayValue);
  }
  if ( digits.isMember("tracker1") && !digits["tracker1"].isNull() ) {
    Json::Value tracker1_digits = JsonWrapper::GetProperty(digits,
                                                           "tracker1",
                                                           JsonWrapper::arrayValue);
    for ( unsigned int idig = 0; idig < tracker1_digits.size(); ++idig ) {
      json_digits[json_digits.size()] = tracker1_digits[idig];
    }
  }
  // --------------------------------------------------------------
  //
  // Collect clusters.
  //
  Json::Value clusters = event["sci_fi_event"]["sci_fi_clusters"];
  if ( clusters.isMember("tracker0") && !clusters["tracker0"].isNull() ) {
    json_clusters = JsonWrapper::GetProperty(clusters,
                                             "tracker0" ,
                                             JsonWrapper::arrayValue);
  }
  if ( clusters.isMember("tracker1") && !clusters["tracker1"].isNull() ) {
    Json::Value tracker1_clusters = JsonWrapper::GetProperty(clusters,
                                                             "tracker1",
                                                             JsonWrapper::arrayValue);
    for ( unsigned int iclust = 0; iclust < tracker1_clusters.size(); ++iclust ) {
      json_clusters[json_clusters.size()] = tracker1_clusters[iclust];
    }
  }
  // --------------------------------------------------------------
  //
  // Collect spacepoints.
  //
  Json::Value spacepoints = event["sci_fi_event"]["sci_fi_space_points"];
  if ( spacepoints.isMember("tracker0") && !spacepoints["tracker0"].isNull() ) {
    json_spacepoints = JsonWrapper::GetProperty(spacepoints,
                                                "tracker0" ,
                                                JsonWrapper::arrayValue);
  }
  if ( spacepoints.isMember("tracker1") && !spacepoints["tracker1"].isNull() ) {
    Json::Value tracker1_spacepoints = JsonWrapper::GetProperty(spacepoints,
                                                                "tracker1",
                                                                JsonWrapper::arrayValue);
    for ( unsigned int i_sp = 0; i_sp < tracker1_spacepoints.size(); ++i_sp ) {
      json_spacepoints[json_spacepoints.size()] = tracker1_spacepoints[i_sp];
    }
  }
  // --------------------------------------------------------------
  std::cerr << "Sizes: " << json_digits.size() << " "
                         << json_clusters.size() << " " << json_spacepoints.size() << std::endl;
}

void ReduceCppTracker::compute_stations_efficiencies(Json::Value root) {
  int numb_spacepoints = json_spacepoints.size();
  for ( int sp_j = 0; sp_j < numb_spacepoints; sp_j++ ) {
    // looping over spacepoints in an event.
    bool plane_0_is_hit = false;
    bool plane_1_is_hit = false;
    bool plane_2_is_hit = false;
    int chan_0 = -10.;
    int chan_1 = -10.;
    int chan_2 = -10.;
    int plane_0_id = 99;
    int plane_1_id = 99;
    int plane_2_id = 99;

    int tracker = json_spacepoints[sp_j]["tracker"].asInt();
    int station = json_spacepoints[sp_j]["station"].asInt();

    int numb_clusters = json_spacepoints[sp_j]["channels"].size();

    for ( int clust_k = 0; clust_k < numb_clusters; ++clust_k ) {
      if ( json_spacepoints[sp_j]["channels"][clust_k]["plane_number"].asInt() == 0 ) {
        plane_0_id = 15*tracker + 3*(station-1) + (0);
        plane_0_is_hit = true;
        chan_0 = json_spacepoints[sp_j]["channels"][clust_k]["channel_number"].asDouble();
      }
      if ( json_spacepoints[sp_j]["channels"][clust_k]["plane_number"].asInt() == 1 ) {
        plane_1_id = 15*tracker + 3*(station-1) + (1);
        plane_1_is_hit = true;
        chan_1 = json_spacepoints[sp_j]["channels"][clust_k]["channel_number"].asDouble();
      }
      if ( json_spacepoints[sp_j]["channels"][clust_k]["plane_number"].asInt() == 2 ) {
        plane_2_id = 15*tracker + 3*(station-1) + (2);
        plane_2_is_hit = true;
        chan_2 = json_spacepoints[sp_j]["channels"][clust_k]["channel_number"].asDouble();
      }
    }

    // Plane 0 efficiencies.
    if ( plane_1_is_hit && plane_2_is_hit ) {
      if ( plane_0_is_hit ) {
        _map_planes_total[plane_0_id][chan_0] += 1;
        _map_planes_hit[plane_0_id][chan_0]   += 1;
        // _plane_0_map[static_cast<int>(chan_0+0.5)][0] += 1;
        // _plane_0_map[static_cast<int>(chan_0+0.5)][1] += 1;
      } else {
        int chan = 318-chan_1-chan_2;
        _map_planes_total[plane_0_id][chan] += 1;
      }
    }
    // Plane 1 efficiencies.
    if ( plane_0_is_hit && plane_2_is_hit ) {
      if ( plane_1_is_hit ) {
        _map_planes_total[plane_1_id][chan_1] += 1;
        _map_planes_hit[plane_1_id][chan_1]   += 1;
      } else {
        int chan = 318-chan_0-chan_2;
        _map_planes_total[plane_1_id][chan] += 1;
      }
    }
    // Plane 2 efficiencies.
    if ( plane_0_is_hit && plane_1_is_hit ) {
      if ( plane_2_is_hit ) {
        _map_planes_total[plane_2_id][chan_2] += 1;
        _map_planes_hit[plane_2_id][chan_2]   += 1;
      } else {
        int chan = 318-chan_0-chan_1;
        _map_planes_total[plane_2_id][chan] += 1;
      }
    }
  }
}

void ReduceCppTracker::draw_spacepoints(Json::Value root) {
/*
  int n_events = root["recon_events"].size();

  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    // Process tracker 0.
    Json::Value spacepoints_tracker0 = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"],
          "tracker0" ,
          JsonWrapper::arrayValue);
    if ( spacepoints_tracker0.isNull() ) continue;
    int numb_spacepoints_tracker0 = spacepoints_tracker0.size();
    for ( int sp_j = 0; sp_j < numb_spacepoints_tracker0; sp_j++ ) {
        if ( spacepoints_tracker0[sp_j].isNull() ) continue;
        _x =  spacepoints_tracker0[sp_j]["position"]["x"].asDouble();
        _y =  spacepoints_tracker0[sp_j]["position"]["y"].asDouble();
        _z =  spacepoints_tracker0[sp_j]["position"]["z"].asDouble();
        _pe = spacepoints_tracker0[sp_j]["npe"].asDouble();
        _tracker_sp = spacepoints_tracker0[sp_j]["tracker"].asInt();
        _station_sp = spacepoints_tracker0[sp_j]["station"].asInt();
        std::string type = spacepoints_tracker0[sp_j]["type"].asString();
        if ( type == "triplet" ) {
          _type = 3;
        }
        if ( type == "duplet" ) {
          _type = 2;
        }
        _spacepoints.Fill();
        _spacepointscopy.Fill();
    }
    // Process tracker 1.
    if ( root["recon_events"][event_i]["sci_fi_event"]
             ["sci_fi_space_points"]["tracker1"].isNull() ) continue;
    Json::Value spacepoints_tracker1 = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"],
          "tracker1" ,
          JsonWrapper::arrayValue);
    int numb_spacepoints_tracker1 = spacepoints_tracker1.size();
    for ( int sp_j = 0; sp_j < numb_spacepoints_tracker1; sp_j++ ) {
        _x =  spacepoints_tracker1[sp_j]["position"]["x"].asDouble();
        _y =  spacepoints_tracker1[sp_j]["position"]["y"].asDouble();
        _z =  spacepoints_tracker1[sp_j]["position"]["z"].asDouble();
        _pe = spacepoints_tracker1[sp_j]["npe"].asDouble();
        _tracker_sp = spacepoints_tracker1[sp_j]["tracker"].asInt();
        _station_sp = spacepoints_tracker1[sp_j]["station"].asInt();
        std::string type = spacepoints_tracker1[sp_j]["type"].asString();
        if ( type == "triplet" ) {
          _type = 3;
        }
        if ( type == "duplet" ) {
          _type = 2;
        }
        _spacepoints.Fill();
        _spacepointscopy.Fill();
    }
  }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("SpacePoints branch is corrupted!"),
  //        "ReduceCppSingleStation::draw_spacepoints"));
  // }
*/
}

void ReduceCppTracker::spacepoints_tree() {
  Json::Value spacepoint_event = json_spacepoints;
  int numb_spacepoints = spacepoint_event.size();

  double chan_sum = 0;
  for ( int sp_j = 0; sp_j < numb_spacepoints; sp_j++ ) {
    Json::Value a_spacepoint = spacepoint_event[sp_j];
    if ( a_spacepoint.isNull() ) continue;
      _x =  a_spacepoint["position"]["x"].asDouble();
      _y =  a_spacepoint["position"]["y"].asDouble();
      _z =  a_spacepoint["position"]["z"].asDouble();
      _pe = a_spacepoint["npe"].asDouble();
      _tracker_sp = a_spacepoint["tracker"].asInt();
      _station_sp = a_spacepoint["station"].asInt();
      std::string type = a_spacepoint["type"].asString();
      if ( type == "triplet" ) {
        _type = 3;
      }
      if ( type == "duplet" ) {
        _type = 2;
      }
      _spacepoints.Fill();
/*
      if ( _plane == 0 ) {
        _hist_plane0->Fill(_channel);
        _npe_plane0->Fill(_npe);
      } else if ( _plane == 1 ) {
        _hist_plane1->Fill(_channel);
        _npe_plane1->Fill(_npe);
      } else if ( _plane == 2 ) {
        _hist_plane2->Fill(_channel);
        _npe_plane2->Fill(_npe);
      }
*/
  }
  // _chan_sum->Fill(chan_sum);
  // }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("Digits branch is corrupted!"),
  //        "ReduceCppSingleStation::digits_histograms"));
  // }
}

void ReduceCppTracker::doublet_clusters_tree() {
  Json::Value cluster_event = json_clusters;
  // if ( cluster_event.isNull() ) continue;
  int numb_clusters = cluster_event.size();

  // double chan_sum = 0;
  for ( int clust_j = 0; clust_j < numb_clusters; clust_j++ ) {
    Json::Value a_cluster = cluster_event[clust_j];
    if ( a_cluster.isNull() ) continue;
    _tracker = a_cluster["tracker"].asInt();
    _station = a_cluster["station"].asInt();
    _plane   = a_cluster["plane"].asInt();
    _channel = a_cluster["channel"].asDouble();
    _npe     = a_cluster["npe"].asDouble();
    _doublet_clusters.Fill();
      // chan_sum += _channel;
/*
      if ( _plane == 0 ) {
        _hist_plane0->Fill(_channel);
        _npe_plane0->Fill(_npe);
      } else if ( _plane == 1 ) {
        _hist_plane1->Fill(_channel);
        _npe_plane1->Fill(_npe);
      } else if ( _plane == 2 ) {
        _hist_plane2->Fill(_channel);
        _npe_plane2->Fill(_npe);
      } */
  }
  // _chan_sum->Fill(chan_sum);
  // }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("Digits branch is corrupted!"),
  //        "ReduceCppSingleStation::digits_histograms"));
  // }
}

void ReduceCppTracker::digits_tree() {
  Json::Value digit_event = json_digits;

  int numb_digits = digit_event.size();
  assert(digit_event.isArray());
  double chan_sum = 0;
  for ( int digit_j = 0; digit_j < numb_digits; digit_j++ ) {
    Json::Value a_digit = digit_event[digit_j];
    if ( a_digit.isNull() ) continue;
    assert(a_digit.isMember("tracker"));
    assert(a_digit.isMember("station"));
    assert(a_digit.isMember("plane"));
    assert(a_digit.isMember("channel"));
    assert(a_digit.isMember("npe"));
    assert(a_digit.isMember("time"));
    _tracker_dig   = a_digit["tracker"].asInt();
    _station_dig   = a_digit["station"].asInt();
    _plane_dig   = a_digit["plane"].asInt();
    _channel_dig = a_digit["channel"].asDouble();
    _npe_dig     = a_digit["npe"].asDouble();
    _time        = a_digit["time"].asDouble();
    _digits.Fill();
/*      if ( _plane_dig == 0 ) {
        _adc_plane0->Fill(_adc_dig);
        _dig_npe_plane0->Fill(_npe_dig);
      } else if ( _plane_dig == 1 ) {
        _adc_plane1->Fill(_adc_dig);
        _dig_npe_plane1->Fill(_npe_dig);
      } else if ( _plane_dig == 2 ) {
        _adc_plane2->Fill(_adc_dig);
        _dig_npe_plane2->Fill(_npe_dig);
      } */
  }
  // }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("Digits branch is corrupted!"),
  //        "ReduceCppSingleStation::digits_histograms"));
  // }
}

void ReduceCppTracker::unpacked_data_tree(Json::Value root) {
  Json::Value daq_data = JsonWrapper::GetProperty(root,
                                                  "daq_data",
                                                  JsonWrapper::objectValue);
  int n_daq_events = daq_data["tracker1"].size();
  Json::Value tracker2 = daq_data["tracker2"];
  // Loop over events.
  for (int event_i = 0; event_i < n_daq_events; event_i++) {
    if ( daq_data["tracker1"][event_i].isNull() ) continue;
    Json::Value daq = daq_data["tracker1"][event_i]["VLSB_C"];
    if ( !daq_data["tracker2"][event_i].isNull() ) {
      for ( int i = 0; i < daq_data["tracker2"][event_i]["VLSB_C"].size(); ++i ) {
        daq[daq.size()] = daq_data["tracker2"][event_i]["VLSB_C"][i];
      }
    }
    json_daq.append(daq);
  }

  int n_events = json_daq.size();
  // Loop over events.
  for (int event_i = 0; event_i < n_events; event_i++) {
    Json::Value daq_event = JsonWrapper::GetItem(json_daq,
                                                 event_i,
                                                 JsonWrapper::arrayValue);
    if ( daq_event.isNull() ) continue;
    int number_channels_within = daq_event.size();
    // std::cerr << number_channels_within << std::endl;
    for ( int hit_i = 0; hit_i < number_channels_within; hit_i++ ) {
      Json::Value a_hit = daq_event[(Json::Value::ArrayIndex)hit_i];
      if ( a_hit.isNull() ) continue;
      assert(a_hit.isMember("tdc"));
      assert(a_hit.isMember("adc"));
      assert(a_hit.isMember("bank"));
      assert(a_hit.isMember("channel"));
      assert(a_hit.isMember("geo"));
      assert(a_hit.isMember("discriminator"));
      _tdc  = a_hit["tdc"].asInt();
      _adc  = a_hit["adc"].asInt();
      _bank = a_hit["bank"].asInt();
      _chan = a_hit["channel"].asInt();
      _board =a_hit["geo"].asInt()-1;
      _unpacked.Fill();
/*      int dicrim = a_hit["discriminator"].asInt();
      if ( dicrim != 0 ) {
        std::cerr << "*************** DISCRIMINATOR != 0 ***************" << std::endl;
      }
      _unpacked.Fill();
      // SciFiEvent_DAQ *daq = new SciFiEvent_DAQ(_bank,7,_chan);
      // event->set_daq(daq);
*/
    }
  }
}

/*
void ReduceCppTracker::show_efficiency(Json::Value const &root) {
  int numb_spacepoints = json_spacepoints.size();

  bool station_hit[2][6] = { {false, false, false, false, false, false},
                             {false, false, false, false, false, false}};

  for ( int sp_i = 0; sp_i < numb_spacepoints; sp_i++ ) {
    // Fill station number.
    int tracker = json_spacepoints[sp_i]["tracker"].asInt();
    int station = json_spacepoints[sp_i]["station"].asInt();
    //_station = station;

    station_hit[tracker][station]=true;
    // Fill type.
    std::string type = spacepoints_tracker0[sp_i]["type"].asString();
    if ( type == "triplet" ) {
      _type = 3;
    }
    if ( type == "duplet" ) {
      _type = 2;
    }
    // _x = spacepoints_tracker0[sp_i]["position"]["x"].asDouble();
    // _y = spacepoints_tracker0[sp_i]["position"]["y"].asDouble();
    // _z = spacepoints_tracker0[sp_i]["position"]["z"].asDouble();
    // _spacepoints.Fill();
  }

    // Fill EVENT tree.
    for ( int tr = 0; tr < 2; tr++ ) {
    int hit_counter = 0;
    if ( station_hit[tr][1] && station_hit[tr][5] ) {
      for ( int i = 2; i < 5; i++ ) {
        if ( station_hit[tr][i] )
          hit_counter+=1;
        }
        _station_hits = hit_counter+2;
        _tracker_event = tr;
        _events.Fill();
      }
    }
  } // ends loop over particle events
}
*/

void ReduceCppTracker::display_histograms() {
/*
  TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));

  triplets->SetMarkerStyle(20);
  triplets->SetMarkerColor(kBlue);
  triplets_copy->SetMarkerStyle(20);
  triplets_copy->SetMarkerColor(kBlue);
  duplets->SetMarkerStyle(20);
  duplets->SetMarkerColor(kRed);
  duplets_copy->SetMarkerStyle(20);
  duplets_copy->SetMarkerColor(kRed);

  c1->cd(1);
  _spacepoints.Draw("x:y>>duplets", "type==2 && tracker==0");
  duplets->Draw("same");
  _spacepoints.Draw("x:y>>triplets", "type==3 && tracker==0", "same");
  triplets->Draw("same");
  duplets->Reset();
  triplets->Reset();
  // draw all stations...
  c1->cd(2);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==1");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==1", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(3);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==2");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==2", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(4);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==3");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==3", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(5);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==4");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==4", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(6);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==5");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==5", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
*/
/*
  c1->cd(7);
  _spacepointscopy.Draw("type", "tracker==0");
*/
/*
  c1->cd(8);
  _spacepoints.Draw("x:y>>duplets", "type==2 && tracker==1");
  duplets->Draw("same");
  _spacepoints.Draw("x:y>>triplets", "type==3 && tracker==1", "same");
  triplets->Draw("same");

  c1->cd(9);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==1");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==1", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(10);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==2");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==2", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(11);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==3");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==3", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(12);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==4");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==4", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->cd(13);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==5");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==5", "same");
  triplets_copy->Draw("same");
  duplets_copy->Reset();
  triplets_copy->Reset();
  c1->Update();
*/
/*  c1->cd(14);
  _spacepointscopy.Draw("type", "tracker==1");
  c1->Update();
  triplets_copy->Draw("same");

  c1->cd(14);
  _spacepointscopy.Draw("type", "tracker==1");
*/
  // c1->Update();
}
