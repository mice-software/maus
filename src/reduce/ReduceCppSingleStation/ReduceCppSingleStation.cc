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
#include <iostream>
#include <fstream>
#include <cmath>
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/reduce/ReduceCppSingleStation/ReduceCppSingleStation.hh"

bool ReduceCppSingleStation::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "ReduceCppSingleStation";
  _filename = "se.root";
  _nSpills = 0;

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
  TCanvas *c1 = new TCanvas("c1", "Efficiency (SE/TOF1)", 200, 10, 700, 500);
  TCanvas *c2 = new TCanvas("c2", "Plane Profiles", 200, 10, 700, 500);
  TCanvas *c3 = new TCanvas("c3", "SpacePoints", 200, 10, 700, 500);
  TCanvas *c4 = new TCanvas("c4", "Cluster NPE", 200, 10, 700, 500);
  TCanvas *c5 = new TCanvas("c5", "Digits Time", 200, 10, 700, 500);

  c1->Divide(1, 1);
  c2->Divide(3, 1);
  c3->Divide(2, 2);
  c4->Divide(3, 1);
  c5->Divide(1, 1);

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
  _graph   = new TGraph();
  _station = new TGraph();
  _plane0  = new TGraph();
  _plane1  = new TGraph();
  _plane2  = new TGraph();

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

  //
  // Trees
  //
  _unpacked.SetNameTitle("unpacked", "unpacked");
  _unpacked.Branch("adc", &_adc, "adc/I");
  _unpacked.Branch("tdc", &_tdc, "tdc/I");
  _unpacked.Branch("bank", &_bank, "bank/I");
  _unpacked.Branch("chan", &_chan, "chan/I");
  _unpacked.Branch("activebank", &_activebank, "activebank/I");

  _digits.SetNameTitle("digits", "digits");
  _digits.Branch("plane", &_plane_dig, "plane/I");
  _digits.Branch("channel", &_channel_dig, "channel/D");
  _digits.Branch("npe", &_npe_dig, "npe/D");
  _digits.Branch("time", &_time, "time/D");
  _digits.Branch("adc", &_adc_dig, "adc/I");

  _doublet_clusters.SetNameTitle("digits", "digits");
  _doublet_clusters.Branch("plane", &_plane, "plane/I");
  _doublet_clusters.Branch("channel", &_channel, "channel/D");
  _doublet_clusters.Branch("npe", &_npe, "npe/D");

  _spacepoints.SetNameTitle("spacepoints", "spacepoints");
  _spacepoints.Branch("pe", &_pe, "pe/D");
  _spacepoints.Branch("x", &_x, "x/D");
  _spacepoints.Branch("y", &_y, "y/D");
  _spacepoints.Branch("z", &_z, "z/D");
  _spacepoints.Branch("type", &_type, "type/I");

  _spacepointscopy.SetNameTitle("spacepoints_copy", "spacepoints_copy");
  _spacepointscopy.Branch("pe", &_pe, "pe/D");
  _spacepointscopy.Branch("x", &_x, "x/D");
  _spacepointscopy.Branch("y", &_y, "y/D");
  _spacepointscopy.Branch("z", &_z, "z/D");
  _spacepointscopy.Branch("type", &_type, "type/I");

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

  triplets->SetMarkerStyle(20);
  triplets->SetMarkerColor(kBlue);
  triplets_copy->SetMarkerStyle(20);
  triplets_copy->SetMarkerColor(kBlue);

  duplets->SetMarkerStyle(20);
  duplets->SetMarkerColor(kRed);
  duplets_copy->SetMarkerStyle(20);
  duplets_copy->SetMarkerColor(kRed);

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

bool ReduceCppSingleStation::death()  {
  Save();
  std::cout << "************ Dead of Single Station Reducer ************" << std::endl;
  return true;
}

std::string  ReduceCppSingleStation::process(std::string document) {
  // JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  // Check if the JSON document can be parsed, else return error only

  TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));
  TCanvas *c2 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c2"));
  TCanvas *c3 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c3"));
  TCanvas *c4 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c4"));
  TCanvas *c5 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c5"));

  std::ofstream file1;
  std::ofstream file2;
  std::ofstream file3;
  std::ofstream file4;

  Squeak::activateCout(1);

  try {
    root = JsonWrapper::StringToJson(document);
  }
  catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    Squeak::mout(Squeak::error) << "Failed to parse input document!" << std::endl;
    return writer.write(root);
  }
  try {
    if ( is_physics_daq_event(root) ) {
      unpacked_data_histograms(root);
      digits_histograms(root);
      doublet_clusters_histograms(root);
      _spacepoints.Reset();
      draw_spacepoints(root);
      count_particle_events(root);
      compute_station_efficiencies(root);
    }
  } catch(Squeal squee) {
    Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

  _nSpills++;

  if (!(_nSpills%5)) {
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
    file4.open ("efficiency_station.txt");
    file4 << 0 << " " << _plane_0_hits << " " << _plane_0_counter << "\n";
    file4 << 1 << " " << _plane_1_hits << " " << _plane_1_counter << "\n";
    file4 << 2 << " " << _plane_2_hits << " " << _plane_2_counter << "\n";
    file4.close();
  }
    // c7->cd(1);
    // _station = new TGraph(3,_plane_array,_station_eff);
    // _station->Draw("AC*");
    // c7->cd(2);
    // _plane0 = new TGraph(214,_channel_array,_plane0_eff);
    // _plane0->Draw("AC*");
    /*  c7->cd(3);
    _plane0 = new TGraph(214,_channel_array,_plane1_eff);
    _plane0->Draw("AC*");
    c7->cd(4);
    _plane0 = new TGraph(214,_channel_array,_plane2_eff);
    _plane0->Draw("AC*");*/
    // c7->Update();


  if (!(_nSpills%1)) {
    c1->cd(1);
    _graph->Draw("ac*");
    c1->Update();

    c2->cd(1);
    _hist_plane0->Draw();
    c2->cd(2);
    _hist_plane1->Draw();
    c2->cd(3);
    _hist_plane2->Draw();
    c2->Update();

    c3->cd(1);
    _spacepoints.Draw("x:y>>duplets", "type==2");
    duplets->Draw("same");
    _spacepoints.Draw("x:y>>triplets", "type==3", "same");
    triplets->Draw("same");
    c3->cd(2);
    _spacepoints.Draw("type");
    c3->Update();

    c3->cd(3);
    _spacepointscopy.Draw("x:y>>duplets_copy", "type==2");
    duplets_copy->Draw("same");
    _spacepointscopy.Draw("x:y>>triplets_copy", "type==3", "same");
    triplets_copy->Draw("same");
    c3->cd(4);
    _spacepointscopy.Draw("type");
    c3->Update();

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
/*
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
*/
  }

  return document;
}

void ReduceCppSingleStation::compute_station_efficiencies(Json::Value root) {
  int n_events = root["recon_events"].size();

  // root["recon_events"][5]["sci_fi_event"]["sci_fi_space_points"]["single_station"][0];
  // std::cerr << n_events << std::endl;
  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["single_station"].isNull() )
      continue;
    Json::Value spacepoints = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"],
          "single_station" ,
          JsonWrapper::arrayValue);
    // std::cerr << spacepoints.size() << std::endl;
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
      // std::cerr << "Numb clusters: " << numb_clusters << std::endl;
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
      // std::cerr << chan_0 << " " << chan_1 << " " << chan_2 << std::endl;
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

void ReduceCppSingleStation::count_particle_events(Json::Value root) {
  _spill_counter += 1.;
  int n_events = root["recon_events"].size();

  int numb_triggers = root["recon_events"].size();
  int numb_spacepoints = 0;

  for ( int event_i = 0; event_i < numb_triggers; event_i++ ) {
    if ( !root["recon_events"][event_i]["sci_fi_event"]
          ["sci_fi_space_points"]["single_station"].isNull() ) {
      numb_spacepoints += 1;
    }
  }

  float effic = static_cast<float>(numb_spacepoints)/numb_triggers;
  float _spill_counter_copy = static_cast<float>(_spill_counter);
  _graph->SetPoint(_spill_counter, _spill_counter_copy, effic);

  if ( effic > 1.0 ) {
    std::cerr << " WARNING: Efficiency = " << effic << "!" << std::endl;
    std::cerr << "TOF1 triggers: " << numb_triggers << std::endl;
    std::cerr << "SE daq triggers: " << root["daq_data"]["single_station"].size()
              << std::endl;
  }
}


bool ReduceCppSingleStation::is_physics_daq_event(Json::Value root) {
  if (root.isMember("daq_data") &&
      !root["daq_data"].isNull() &&
      root["daq_data"].isMember("single_station") &&
      root["daq_event_type"].asString() == "physics_event") {
    return true;
  } else {
    return false;
  }
}

void ReduceCppSingleStation::draw_spacepoints(Json::Value root) {
  int n_events = root["recon_events"].size();

  // root["recon_events"][5]["sci_fi_event"]["sci_fi_space_points"]["single_station"][0];
  // std::cerr << n_events << std::endl;
  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"]["single_station"].isNull() ) continue;
    Json::Value spacepoints = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_space_points"],
          "single_station" ,
          JsonWrapper::arrayValue);
    // std::cerr << spacepoints.size() << std::endl;
    int numb_spacepoints = spacepoints.size();
    for ( int sp_j = 0; sp_j < numb_spacepoints; sp_j++ ) {
        _x =  spacepoints[sp_j]["position"]["x"].asDouble();
        _y =  spacepoints[sp_j]["position"]["y"].asDouble();
        _z =  spacepoints[sp_j]["position"]["z"].asDouble();
        _pe = spacepoints[sp_j]["npe"].asDouble();
        std::string type = spacepoints[sp_j]["type"].asString();
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

void ReduceCppSingleStation::doublet_clusters_histograms(Json::Value root) {
  int n_events = root["recon_events"].size();

  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"]["single_station"].isNull() ) continue;
    Json::Value clusters = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_clusters"],
          "single_station" ,
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

void ReduceCppSingleStation::digits_histograms(Json::Value root) {
  int n_events = root["recon_events"].size();

  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["single_station"].isNull() ) continue;
    Json::Value digits = JsonWrapper::GetProperty(
          root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"],
          "single_station" ,
          JsonWrapper::arrayValue);

    int numb_digits = digits.size();

    double chan_sum = 0;
    for ( int digit_j = 0; digit_j < numb_digits; digit_j++ ) {
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

void ReduceCppSingleStation::unpacked_data_histograms(Json::Value root) {
  Json::Value daq_data = JsonWrapper::GetProperty(root,
                                                  "daq_data" ,
                                                  JsonWrapper::objectValue);
  int n_events = daq_data["single_station"].size();
  // Loop over events.
  for (int event_i = 0; event_i < n_events; event_i++) {
    if ( daq_data["single_station"][event_i].isNull() ) continue;
    Json::Value i_PartEvent = daq_data["single_station"][event_i];
    int number_channels_within = i_PartEvent["VLSB"].size();
    for ( int i = 0; i < number_channels_within; i++ ) {
      _tdc  = i_PartEvent["VLSB"][i]["tdc"].asInt();
      _adc  = i_PartEvent["VLSB"][i]["adc"].asInt();
      _bank = i_PartEvent["VLSB"][i]["bank_id"].asInt();
      _chan = i_PartEvent["VLSB"][i]["channel"].asInt();
      int dicrim = _chan = i_PartEvent["VLSB"][i]["discriminator"].asInt();
      if ( dicrim != 0 ) {
        std::cerr << "*************** DISCRIMINATOR != 0 ***************" << std::endl;
      }
      if ( _bank == 0 || _bank == 2 || _bank == 5 ||
           _bank == 7 || _bank == 9 || _bank == 10 ||
           _bank == 11 || _bank == 12 || _bank == 13 ||
           _bank == 14) {
        _activebank = 1;
      } else {
       _activebank = 0;
      }
      _unpacked.Fill();
    }
  }
}

void ReduceCppSingleStation::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _unpacked.Write();
  _digits.Write();
  _spacepointscopy.Write();

  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}
