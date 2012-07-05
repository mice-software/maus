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
  TCanvas *c3 = new TCanvas("c3", "SpacePoints", 200, 10, 700, 500);
  // TCanvas *c4 = new TCanvas("c4", "Cluster NPE", 200, 10, 700, 500);
  // TCanvas *c5 = new TCanvas("c5", "Digits Time", 200, 10, 700, 500);

  // c1->Divide(3, 5);
  // c2->Divide(3, 5);
  c3->Divide(7, 2);
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
  triplets->SetMarkerStyle(20);
  triplets->SetMarkerColor(kBlue);
  triplets_copy->SetMarkerStyle(20);
  triplets_copy->SetMarkerColor(kBlue);
  duplets->SetMarkerStyle(20);
  duplets->SetMarkerColor(kRed);
  duplets_copy->SetMarkerStyle(20);
  duplets_copy->SetMarkerColor(kRed);

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
  _digits.Branch("adc", &_adc_dig, "adc/I");

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
  _spacepoints.Branch("tracker", &_tracker_cop, "tracker/I");
  _spacepoints.Branch("station", &_station_cop, "station/I");
  _spacepoints.Branch("type", &_type, "type/I");

  _spacepointscopy.SetNameTitle("spacepoints_copy", "spacepoints_copy");
  _spacepointscopy.Branch("pe", &_pe, "pe/D");
  _spacepointscopy.Branch("x", &_x, "x/D");
  _spacepointscopy.Branch("y", &_y, "y/D");
  _spacepointscopy.Branch("z", &_z, "z/D");
  _spacepointscopy.Branch("tracker", &_tracker_cop, "tracker/I");
  _spacepointscopy.Branch("station", &_station_cop, "station/I");
  _spacepointscopy.Branch("type", &_type, "type/I");
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
  // TFile hfile("Event.root","RECREATE");
  // TFile root_file("cosmics.root","RECREATE");
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;

  std::ofstream file1;
  std::ofstream file2;
  std::ofstream file3;
  std::ofstream file4;

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
      unpacked_data_histograms(root);

      digits_histograms(root);

      doublet_clusters_histograms(root);

      _spacepoints.Reset();
      draw_spacepoints(root);

      compute_stations_efficiencies(root);
      // save_efficiency(root);
    }
  } catch(Squeal squee) {
     Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }
  // tree->Fill();
  _nSpills++;
  // hfile.Close();
  if (!(_nSpills%100)) {
    file1.open("efficiency_plane0.txt");
    file2.open("efficiency_plane1.txt");
    file3.open("efficiency_plane2.txt");
    for ( int i = 0; i < 214; ++i ) {
      file1 << i << " " << _plane_0_map[i][0] << " " << _plane_0_map[i][1] << "\n";
      file2 << i << " " << _plane_1_map[i][0] << " " << _plane_1_map[i][1] << "\n";
      file3 << i << " " << _plane_2_map[i][0] << " " << _plane_2_map[i][1] << "\n";
    }
    file1.close();
    file2.close();
    file3.close();
    file4.open("efficiency_station.txt");
    file4 << 0 << " " << _plane_0_hits << " " << _plane_0_counter << "\n";
    file4 << 1 << " " << _plane_1_hits << " " << _plane_1_counter << "\n";
    file4 << 2 << " " << _plane_2_hits << " " << _plane_2_counter << "\n";
    file4.close();

    save();
  }

  display_histograms();
  return document;
}

bool ReduceCppTracker::death()  {
  return true;
}

void ReduceCppTracker::save()  {
  TFile root_file("cosmics.root", "RECREATE");
  _unpacked.Write();
  _digits.Write();
  _doublet_clusters.Write();
  // _spacepoints.Write();
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

void ReduceCppTracker::compute_stations_efficiencies(Json::Value root) {
  int n_events = root["recon_events"].size();

  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( root["recon_events"][event_i]["sci_fi_event"]
             ["sci_fi_space_points"]["tracker0"].isNull() )
      continue;
    Json::Value spacepoints = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"],
          "tracker0" ,
          JsonWrapper::arrayValue);

    int numb_spacepoints = spacepoints.size();
    for ( int sp_j = 0; sp_j < numb_spacepoints; sp_j++ ) {
      // looping over spacepoints in an event.
      bool plane_0_is_hit = false;
      bool plane_1_is_hit = false;
      bool plane_2_is_hit = false;
      double chan_0 = -10.;
      double chan_1 = -10.;
      double chan_2 = -10.;

      int numb_clusters = spacepoints[sp_j]["channels"].size();
      for ( int clust_k = 0; clust_k < numb_clusters; ++clust_k ) {
        if ( spacepoints[sp_j]["channels"][clust_k]["plane_number"].asInt() == 0 ) {
          plane_0_is_hit = true;
          chan_0 = spacepoints[sp_j]["channels"][clust_k]["channel_number"].asDouble();
        }
        if ( spacepoints[sp_j]["channels"][clust_k]["plane_number"].asInt() == 1 ) {
          plane_1_is_hit = true;
          chan_1 = spacepoints[sp_j]["channels"][clust_k]["channel_number"].asDouble();
        }
        if ( spacepoints[sp_j]["channels"][clust_k]["plane_number"].asInt() == 2 ) {
          plane_2_is_hit = true;
          chan_2 = spacepoints[sp_j]["channels"][clust_k]["channel_number"].asDouble();
        }
      }
      // Plane 0 efficiencies.
      if ( plane_1_is_hit && plane_2_is_hit ) {
        _plane_0_counter += 1;
        if ( plane_0_is_hit ) {
          _plane_0_hits += 1;
          _plane_0_map[static_cast<int>(chan_0+0.5)][0] += 1;
          _plane_0_map[static_cast<int>(chan_0+0.5)][1] += 1;
        } else {
          int chan = 318-chan_1-chan_2;
          _plane_0_map[chan][1] += 1;
        }
      }
      // Plane 1 efficiencies.
      if ( plane_0_is_hit && plane_2_is_hit ) {
        _plane_1_counter += 1;
        if ( plane_1_is_hit ) {
          _plane_1_hits += 1;
          _plane_1_map[static_cast<int>(chan_1+0.5)][0] += 1;
          _plane_1_map[static_cast<int>(chan_1+0.5)][1] += 1;
        } else {
          int chan = 318-chan_0-chan_2;
          _plane_1_map[chan][1] += 1;
        }
      }
      // Plane 2 efficiencies.
      if ( plane_0_is_hit && plane_1_is_hit ) {
        _plane_2_counter += 1;
        if ( plane_2_is_hit ) {
          _plane_2_hits += 1;
          _plane_2_map[static_cast<int>(chan_2+0.5)][0] += 1;
          _plane_2_map[static_cast<int>(chan_2+0.5)][1] += 1;
        } else {
          int chan = 318-chan_0-chan_1;
          _plane_2_map[chan][1] += 1;
        }
      }
    }
  }
}

void ReduceCppTracker::draw_spacepoints(Json::Value root) {
  int n_events = root["recon_events"].size();

  // root["recon_events"][5]["sci_fi_event"]["sci_fi_space_points"]["single_station"][0];
  // std::cerr << n_events << std::endl;
  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    // Process tracker 0.
    if ( root["recon_events"][event_i]["sci_fi_event"]
             ["sci_fi_space_points"]["tracker0"].isNull() ) continue;
    Json::Value spacepoints_tracker0 = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"],
          "tracker0" ,
          JsonWrapper::arrayValue);
    // std::cerr << spacepoints.size() << std::endl;
    int numb_spacepoints_tracker0 = spacepoints_tracker0.size();
    for ( int sp_j = 0; sp_j < numb_spacepoints_tracker0; sp_j++ ) {
        _x =  spacepoints_tracker0[sp_j]["position"]["x"].asDouble();
        _y =  spacepoints_tracker0[sp_j]["position"]["y"].asDouble();
        _z =  spacepoints_tracker0[sp_j]["position"]["z"].asDouble();
        _pe = spacepoints_tracker0[sp_j]["npe"].asDouble();
        _tracker_cop = spacepoints_tracker0[sp_j]["tracker"].asInt();
        _station_cop = spacepoints_tracker0[sp_j]["station"].asInt();
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
    // std::cerr << spacepoints.size() << std::endl;
    int numb_spacepoints_tracker1 = spacepoints_tracker1.size();
    for ( int sp_j = 0; sp_j < numb_spacepoints_tracker1; sp_j++ ) {
        _x =  spacepoints_tracker1[sp_j]["position"]["x"].asDouble();
        _y =  spacepoints_tracker1[sp_j]["position"]["y"].asDouble();
        _z =  spacepoints_tracker1[sp_j]["position"]["z"].asDouble();
        _pe = spacepoints_tracker1[sp_j]["npe"].asDouble();
        _tracker_cop = spacepoints_tracker1[sp_j]["tracker"].asInt();
        _station_cop = spacepoints_tracker1[sp_j]["station"].asInt();
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
}

void ReduceCppTracker::doublet_clusters_histograms(Json::Value root) {
  int n_events = root["recon_events"].size();

  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( root["recon_events"][event_i]["sci_fi_event"]
             ["sci_fi_clusters"]["tracker0"].isNull() ) continue;
    Json::Value clusters = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"],
          "tracker0" ,
          JsonWrapper::arrayValue);

    int numb_clusters = clusters.size();

    double chan_sum = 0;
    for ( int clust_j = 0; clust_j < numb_clusters; clust_j++ ) {
      _plane   = clusters[clust_j]["plane"].asInt();
      _channel = clusters[clust_j]["channel"].asDouble();
      _npe     = clusters[clust_j]["npe"].asDouble();
      _doublet_clusters.Fill();
      chan_sum += _channel;
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
    }
    _chan_sum->Fill(chan_sum);
  }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("Digits branch is corrupted!"),
  //        "ReduceCppSingleStation::digits_histograms"));
  // }
}

void ReduceCppTracker::digits_histograms(Json::Value root) {
  int n_events = root["recon_events"].size();

  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    Json::Value tracker1 = root["recon_events"][event_i]["sci_fi_event"]
                               ["sci_fi_digits"]["tracker1"];
    // if ( tracker0.isNull() ) continue;
    Json::Value digits = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"],
          "tracker0" ,
          JsonWrapper::arrayValue);

    for ( unsigned int idig = 0; idig < tracker1.size(); ++idig ) {
      digits[digits.size()] = tracker1[idig];
    }

    int numb_digits = digits.size();

    double chan_sum = 0;
    for ( int digit_j = 0; digit_j < numb_digits; digit_j++ ) {
      _tracker_dig   = digits[digit_j]["tracker"].asInt();
      _station_dig   = digits[digit_j]["station"].asInt();
      _plane_dig   = digits[digit_j]["plane"].asInt();
      _channel_dig = digits[digit_j]["channel"].asDouble();
      _npe_dig     = digits[digit_j]["npe"].asDouble();
      _adc_dig     = digits[digit_j]["adc"].asInt();
      _time        = digits[digit_j]["time"].asDouble();
      _digits.Fill();
      if ( _plane_dig == 0 ) {
        _adc_plane0->Fill(_adc_dig);
        _dig_npe_plane0->Fill(_npe_dig);
      } else if ( _plane_dig == 1 ) {
        _adc_plane1->Fill(_adc_dig);
        _dig_npe_plane1->Fill(_npe_dig);
      } else if ( _plane_dig == 2 ) {
        _adc_plane2->Fill(_adc_dig);
        _dig_npe_plane2->Fill(_npe_dig);
      }
    }
  }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("Digits branch is corrupted!"),
  //        "ReduceCppSingleStation::digits_histograms"));
  // }
}
/*
void ReduceCppTracker::show_light_yield(Json::Value const &root) {
  int n_events = root["recon_events"].size();

  for (int event_i = 0; event_i < n_events; event_i++) {
    Json::Value spacepoints_tracker0 = root["recon_events"][event_i]["sci_fi_event"]
                                           ["sci_fi_space_points"]["tracker0"];

    Json::Value spacepoints_tracker1 = root["recon_events"][event_i]["sci_fi_event"]
                                           ["sci_fi_space_points"]["tracker1"];

    int n_sp_tracker0 = spacepoints_tracker0.size();
    int n_sp_tracker1 = spacepoints_tracker1.size();

    for ( int sp_i = 0; sp_i < n_sp_tracker0; sp_i++ ) {
      int n_digits = spacepoints_tracker0[sp_i]["channels"].size();
      for ( int digit_j = 0; digit_j < n_digits; digit_j++ ) {
        _npe = spacepoints_tracker0[sp_i]["channels"][digit_j]["npe"].asDouble();
        _tracker_dig = 0;
        _digits.Fill();
      }
    }

    for ( int sp_i = 0; sp_i < n_sp_tracker1; sp_i++ ) {
      int n_digits = spacepoints_tracker1[sp_i]["channels"].size();
      for ( int digit_j = 0; digit_j < n_digits; digit_j++ ) {
        _npe = spacepoints_tracker1[sp_i]["channels"][digit_j]["npe"].asDouble();
        _tracker_dig = 1;
        _digits.Fill();
      }
    }
  }
}
*/
void ReduceCppTracker::unpacked_data_histograms(Json::Value root) {
/*  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    // Json::Value tracker0 = root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["tracker0"];
    Json::Value tracker1 = root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["tracker1"];
    // if ( tracker0.isNull() ) continue;
    Json::Value digits = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"],
          "tracker0" ,
          JsonWrapper::arrayValue);

    for ( unsigned int idig = 0; idig < tracker1.size(); ++idig ) {
      digits[digits.size()] = tracker1[idig];
    }



*/
  Json::Value daq_data = JsonWrapper::GetProperty(root,
                                                  "daq_data",
                                                  JsonWrapper::objectValue);
  int n_events = daq_data["tracker1"].size();
  // Loop over events.
  for (int event_i = 0; event_i < n_events; event_i++) {
    if ( daq_data["tracker1"][event_i].isNull() ) continue;
    Json::Value i_PartEvent = daq_data["tracker1"][event_i];
    int number_channels_within = i_PartEvent["VLSB_C"].size();
    for ( int i = 0; i < number_channels_within; i++ ) {
      _tdc  = i_PartEvent["VLSB_C"][i]["tdc"].asInt();
      _adc  = i_PartEvent["VLSB_C"][i]["adc"].asInt();
      _bank = i_PartEvent["VLSB_C"][i]["bank_id"].asInt();
      _chan = i_PartEvent["VLSB_C"][i]["channel"].asInt();
      int dicrim = _chan = i_PartEvent["VLSB_C"][i]["discriminator"].asInt();
      if ( dicrim != 0 ) {
        std::cerr << "*************** DISCRIMINATOR != 0 ***************" << std::endl;
      }
      _unpacked.Fill();
      // SciFiEvent_DAQ *daq = new SciFiEvent_DAQ(_bank,7,_chan);
      // event->set_daq(daq);
    }
  }
}

/*
void ReduceCppTracker::show_efficiency(Json::Value const &root) {
  int n_events = root["recon_events"].size();

  for (int event_i = 0; event_i < n_events; event_i++) {
    Json::Value spacepoints_tracker0 = root["recon_events"][event_i]["sci_fi_event"]
                                           ["sci_fi_space_points"]["tracker0"];

    Json::Value spacepoints_tracker1 = root["recon_events"][event_i]["sci_fi_event"]
                                           ["sci_fi_space_points"]["tracker1"];

    int n_sp_tracker0 = spacepoints_tracker0.size();
    int n_sp_tracker1 = spacepoints_tracker1.size();

    bool station_hit[2][6] = { {false, false, false, false, false, false},
                               {false, false, false, false, false, false}};

    for ( int sp_i = 0; sp_i < n_sp_tracker0; sp_i++ ) {
      // Fill station number.
      int station = spacepoints_tracker0[sp_i]["station"].asInt();
      _station = station;
      station_hit[0][station]=true;
      // Fill type.
      std::string type = spacepoints_tracker0[sp_i]["type"].asString();
      if ( type == "triplet" ) {
        _type = 3;
      }
      if ( type == "duplet" ) {
        _type = 2;
      }
      _tracker = 0;
      _x = spacepoints_tracker0[sp_i]["position"]["x"].asDouble();
      _y = spacepoints_tracker0[sp_i]["position"]["y"].asDouble();
      _z = spacepoints_tracker0[sp_i]["position"]["z"].asDouble();
      _spacepoints.Fill();
    }

    for ( int sp_i = 0; sp_i < n_sp_tracker1; sp_i++ ) {
      // Fill station number.
      int station = spacepoints_tracker1[sp_i]["station"].asInt();
      _station = station;
      station_hit[1][station]=true;
      // Fill type.
      std::string type = spacepoints_tracker1[sp_i]["type"].asString();
      if ( type == "triplet" ) {
        _type = 3;
      }
      if ( type == "duplet" ) {
        _type = 2;
      }
      _tracker = 1;
      _x = spacepoints_tracker1[sp_i]["position"]["x"].asDouble();
      _y = spacepoints_tracker1[sp_i]["position"]["y"].asDouble();
      _z = spacepoints_tracker1[sp_i]["position"]["z"].asDouble();
      _spacepoints.Fill();
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
  // TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));
  // TCanvas *c2 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c2"));
  TCanvas *c3 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c3"));
  // TCanvas *c4 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c4"));
  // TCanvas *c5 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c5"));
/*
  c1->cd(1);
  _digits.Draw("channel","tracker==0 && station==1 && plane==0");
  c1->cd(2);
  _digits.Draw("channel","tracker==0 && station==1 && plane==1");
  c1->cd(3);
  _digits.Draw("channel","tracker==0 && station==1 && plane==2");
  c1->cd(4);
  _digits.Draw("channel","tracker==0 && station==2 && plane==0");
  c1->cd(5);
  _digits.Draw("channel","tracker==0 && station==2 && plane==1");
  c1->cd(6);
  _digits.Draw("channel","tracker==0 && station==2 && plane==2");
  c1->cd(7);
  _digits.Draw("channel","tracker==0 && station==3 && plane==0");
  c1->cd(8);
  _digits.Draw("channel","tracker==0 && station==3 && plane==1");
  c1->cd(9);
  _digits.Draw("channel","tracker==0 && station==3 && plane==2");
  c1->cd(10);
  _digits.Draw("channel","tracker==0 && station==4 && plane==0");
  c1->cd(11);
  _digits.Draw("channel","tracker==0 && station==4 && plane==1");
  c1->cd(12);
  _digits.Draw("channel","tracker==0 && station==4 && plane==2");
  c1->cd(13);
  _digits.Draw("channel","tracker==0 && station==5 && plane==0");
  c1->cd(14);
  _digits.Draw("channel","tracker==0 && station==5 && plane==1");
  c1->cd(15);
  _digits.Draw("channel","tracker==0 && station==5 && plane==2");
  c1->Update();

  c2->cd(1);
  _digits.Draw("channel","tracker==1 && station==1 && plane==0");
  c2->cd(2);
  _digits.Draw("channel","tracker==1 && station==1 && plane==1");
  c2->cd(3);
  _digits.Draw("channel","tracker==1 && station==1 && plane==2");
  c2->cd(4);
  _digits.Draw("channel","tracker==1 && station==2 && plane==0");
  c2->cd(5);
  _digits.Draw("channel","tracker==1 && station==2 && plane==1");
  c2->cd(6);
  _digits.Draw("channel","tracker==1 && station==2 && plane==2");
  c2->cd(7);
  _digits.Draw("channel","tracker==1 && station==3 && plane==0");
  c2->cd(8);
  _digits.Draw("channel","tracker==1 && station==3 && plane==1");
  c2->cd(9);
  _digits.Draw("channel","tracker==1 && station==3 && plane==2");
  c2->cd(10);
  _digits.Draw("channel","tracker==1 && station==4 && plane==0");
  c2->cd(11);
  _digits.Draw("channel","tracker==1 && station==4 && plane==1");
  c2->cd(12);
  _digits.Draw("channel","tracker==1 && station==4 && plane==2");
  c2->cd(13);
  _digits.Draw("channel","tracker==1 && station==5 && plane==0");
  c2->cd(14);
  _digits.Draw("channel","tracker==1 && station==5 && plane==1");
  c2->cd(15);
  _digits.Draw("channel","tracker==1 && station==5 && plane==2");
  c2->Update();
*/
  c3->cd(1);
  _spacepoints.Draw("x:y>>duplets", "type==2 && tracker==0");
  duplets->Draw("same");
  _spacepoints.Draw("x:y>>triplets", "type==3 && tracker==0", "same");
  triplets->Draw("same");
  // draw all stations...
  c3->cd(2);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==1");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==1", "same");
  triplets_copy->Draw("same");
  c3->cd(3);
  _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==2");
  duplets_copy->Draw("same");
  _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==2", "same");
  triplets_copy->Draw("same");
  c3->cd(4);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==3");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==3", "same");
    triplets_copy->Draw("same");
    c3->cd(5);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==4");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==4", "same");
    triplets_copy->Draw("same");
    c3->cd(6);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==0 && station==5");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==0 && station==5", "same");
    triplets_copy->Draw("same");

    c3->cd(7);
    _spacepointscopy.Draw("type", "tracker==0");

    c3->cd(8);
    _spacepoints.Draw("x:y>>duplets", "type==2 && tracker==1");
    duplets->Draw("same");
    _spacepoints.Draw("x:y>>triplets", "type==3 && tracker==1", "same");
    triplets->Draw("same");

    c3->cd(9);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==1");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==1", "same");
    triplets_copy->Draw("same");
    c3->cd(10);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==2");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==2", "same");
    triplets_copy->Draw("same");
    c3->cd(11);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==3");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==3", "same");
    triplets_copy->Draw("same");
    c3->cd(12);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==4");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==4", "same");
    triplets_copy->Draw("same");
    c3->cd(13);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2 && tracker==1 && station==5");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3 && tracker==1 && station==5", "same");
    triplets_copy->Draw("same");
    c3->cd(14);
    _spacepointscopy.Draw("type", "tracker==1");
    c3->Update();
/*
    c4->cd(1);
    _npe_plane0->Draw();
    c4->cd(2);
    _npe_plane1->Draw();
    c4->cd(3);
    _npe_plane2->Draw();
    c4->Update();

    c5->cd(1);
    _digits.Draw("time");
    // c5_1->SetLogy(1);
    // c5_1->SetGrid(1,1);
    // _unpacked.Draw("adc", "bank==0 || bank==2 || bank==5 ||
    // bank==7 || bank==9|| bank==10|| bank==11 ||bank==12 ||
    // bank==13|| bank==14");
    c5->Update();

    c6->cd(1);
    _adc_plane0->Draw();
    c6->cd(2);
    _adc_plane1->Draw();
    c6->cd(3);
    _adc_plane2->Draw();
    c6->cd(4);
    _dig_npe_plane0->Draw();
    c6->cd(5);
    _dig_npe_plane1->Draw();
    c6->cd(6);
    _dig_npe_plane2->Draw();
    c6->Update();

  // Display spacepoints type.
    c2->cd(1);
    _spacepoints.Draw("type", "tracker==0 && station==1 ");
    c2->Update();
    c2->cd(2);
    _spacepoints.Draw("type", "tracker==0 && station==2 ");
    c2->Update();
    c2->cd(3);
    _spacepoints.Draw("type", "tracker==0 && station==3 ");
    c2->Update();
    c2->cd(4);
    _spacepoints.Draw("type", "tracker==0 && station==4 ");
    c2->Update();
    c2->cd(5);
    _spacepoints.Draw("type", "tracker==0 && station==5 ");
    c2->Update();
    // tracker 1
    c2->cd(6);
    _spacepoints.Draw("type", "tracker==1 && station==1 ");
    c2->Update();
    c2->cd(7);
    _spacepoints.Draw("type", "tracker==1 && station==2 ");
    c2->Update();
    c2->cd(8);
    _spacepoints.Draw("type", "tracker==1 && station==3 ");
    c2->Update();
    c2->cd(9);
    _spacepoints.Draw("type", "tracker==1 && station==4 ");
    c2->Update();
    c2->cd(10);
    _spacepoints.Draw("type", "tracker==1 && station==5 ");
    c2->Update();

  // Display efficiency.

    c3->cd(1);
    _events.Draw("station_hits", "tracker==0");
    c3->cd(2);
    _events.Draw("station_hits", "tracker==1");
*/
}

