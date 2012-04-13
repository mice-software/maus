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

  TCanvas *c1 = new TCanvas("c1", "adc Values", 200, 10, 700, 500);
  TCanvas *c2 = new TCanvas("c2", "Digits", 200, 10, 700, 500);
  TCanvas *c3 = new TCanvas("c3", "SpacePoints", 200, 10, 700, 500);

  triplets = new TH2F("triplets", "Spacepoints (x, y)", 300, -150, 150, 300, -150, 150);
  duplets  = new TH2F("duplets", "Spacepoints (x, y)", 300, -150, 150, 300, -150, 150);
  _trig_efficiency  = new TH2F("_trig_efficiency", "Trigger Eff: SE/TOF1", 30, 0, 30, 50, 0, 1.2);
  _graph = new TGraph();

  _trig_efficiency->SetMarkerStyle(20);
  _trig_efficiency->SetMarkerSize(0.6);
  _trig_efficiency->SetMarkerColor(kBlack);

  _unpacked.SetNameTitle("unpacked", "unpacked");
  _unpacked.Branch("adc", &_adc, "adc/I");
  _unpacked.Branch("bank", &_bank, "bank/I");
  _unpacked.Branch("chan", &_chan, "chan/I");
  _unpacked.Branch("activebank", &_activebank, "activebank/I");

  _digits.SetNameTitle("digits", "digits");
  _digits.Branch("plane", &_plane, "plane/I");
  _digits.Branch("channel", &_channel, "channel/D");
  _digits.Branch("npe", &_npe, "npe/D");

  _spacepoints.SetNameTitle("spacepoints", "spacepoints");
  _spacepoints.Branch("pe", &_pe, "pe/D");
  _spacepoints.Branch("x", &_x, "x/D");
  _spacepoints.Branch("y", &_y, "y/D");
  _spacepoints.Branch("z", &_z, "z/D");
  _spacepoints.Branch("type", &_type, "type/I");

  c1->Divide(5, 2);
  c1->SetFillColor(21);
  c1->GetFrame()->SetFillColor(42);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

  c2->Divide(2, 1);
  c2->SetFillColor(21);
  c2->GetFrame()->SetFillColor(42);
  c2->GetFrame()->SetBorderSize(6);
  c2->GetFrame()->SetBorderMode(-1);

  c3->Divide(3, 1);
  gStyle->SetLabelSize(0.07, "xyz");
  gStyle->SetTitleSize(0.07, "xy");
  gStyle->SetTitleOffset(0.6, "x");
  gStyle->SetTitleOffset(0.5, "y");
  // gStyle->SetMarkerStyle(34);
  // gStyle->SetMarkerSize(0.6);
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

  Squeak::activateCout(1);

  // std::cerr << "Begin Reducer Process" << std::endl;
  try {
    root = JsonWrapper::StringToJson(document);
    // std::cerr << root.type() << " obj:" << Json::Value(Json::objectValue).type()
    //           << " null:" << Json::Value().type() << " str:"
    //           << Json::Value("").type() << std::endl;
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
    if ( is_physics_daq_event(root) )
      unpacked_data_histograms(root);

    if ( root.isMember("digits") )
      digits_histograms(root);

    if ( root.isMember("space_points") )
      draw_spacepoints(root);

    if ( is_physics_daq_event(root) && root.isMember("space_points") )
      count_particle_events(root);
  } catch(Squeal squee) {
    Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

  _nSpills++;
  if (!(_nSpills%1)) {
    c1->cd(1);
    _unpacked.Draw("adc:chan", "bank == 0 ");
    c1->Update();
    c1->cd(2);
    _unpacked.Draw("adc:chan", "bank == 2 ");
    c1->Update();
    c1->cd(3);
    _unpacked.Draw("adc:chan", "bank == 5 ");
    c1->Update();
    c1->cd(4);
    _unpacked.Draw("adc:chan", "bank == 7 ");
    c1->Update();
    c1->cd(5);
    _unpacked.Draw("adc:chan", "bank == 9 ");
    c1->Update();
    c1->cd(6);
    _unpacked.Draw("adc:chan", "bank == 10 ");
    c1->Update();
    c1->cd(7);
    _unpacked.Draw("adc:chan", "bank == 11 ");
    c1->Update();
    c1->cd(8);
    _unpacked.Draw("adc:chan", "bank == 12 ");
    c1->Update();
    c1->cd(9);
    _unpacked.Draw("adc:chan", "bank == 13 ");
    c1->Update();
    c1->cd(10);
    _unpacked.Draw("adc:chan", "bank == 14 ");
    c1->Update();

    c2->cd(1);
    _unpacked.Draw("adc", "activebank==1");
    c2->cd(2);
    _digits.Draw("npe");
    // c2->SetLogy();
    // c2->cd(3);
    // _digits.Draw("npe:channel","plane==2");
    c2->Update();

    c3->cd(1);
    _spacepoints.Draw("x:y>>duplets", "type==2");
    duplets->SetMarkerStyle(20);
    duplets->SetMarkerColor(kRed);
    duplets->Draw("same");
    // gStyle->SetMarkerStyle(34);
    // gStyle->SetMarkerSize(0.6);
    _spacepoints.Draw("x:y>>triplets", "type==3", "same");
    triplets->SetMarkerStyle(20);
    triplets->SetMarkerColor(kBlue);
    triplets->Draw("same");
    c3->Update();
    c3->cd(2);
    _spacepoints.Draw("type");
    c3->Update();
    c3->cd(3);
    // _trig_efficiency->Draw();
    _graph->Draw("ac*");
    c3->Update();
  }

  // std::cerr << "End Reducer Process" << std::endl;
  return document;
}

void ReduceCppSingleStation::count_particle_events(Json::Value root) {
  _spill_counter += 1.;
  int numb_triggers = root["daq_data"]["tof1"].size();
  int numb_spacepoints = 0;

  int n_events = root["spacepoints"]["single_station"].size();
  for ( int event_i = 0; event_i < n_events; event_i++ ) {
    if ( !root["spacepoints"]["single_station"][event_i].isNull() ) {
      numb_spacepoints += 1;
    }
  }

  float effic = static_cast<float>(numb_spacepoints)/numb_triggers;
  _graph->SetPoint(_spill_counter, _spill_counter, effic);
/*
  TAxis *axis = _trig_efficiency->GetXaxis();
  std::cout << numb_spacepoints << " " << axis->GetXmax() << std::endl;
  if ( _spill_counter > axis->GetXmax() ) {
    std::cout << "REBINING" << std::endl;
    double new_X_max = _spill_counter+30;
    _trig_efficiency->RebinAxis(new_X_max, axis);
  }
  TAxis *axis_y = _trig_efficiency->GetYaxis();
  if ( effic > axis_y->GetXmax() ) {
    double new_Y_max = effic+0.3;
    _trig_efficiency->RebinAxis(new_Y_max, axis_y);
  }
*/
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
  Json::Value spacepoints = JsonWrapper::GetProperty(root,
                                                     "space_points" ,
                                                     JsonWrapper::objectValue);
  int n_events = spacepoints["single_station"].size();
  for ( int event_i = 0; event_i < n_events; event_i++ ) {
      if ( spacepoints["single_station"][event_i].isNull() ) continue;
      Json::Value i_PartEvent = JsonWrapper::GetItem(spacepoints["single_station"],
                                        event_i,
                                        JsonWrapper::arrayValue);
      int numb_spacepoints = i_PartEvent.size();
      for ( int sp_j = 0; sp_j < numb_spacepoints; sp_j++ ) {
        _x =  i_PartEvent[sp_j]["position"]["x"].asDouble();
        _y =  i_PartEvent[sp_j]["position"]["y"].asDouble();
        _z =  i_PartEvent[sp_j]["position"]["z"].asDouble();
        _pe = i_PartEvent[sp_j]["npe"].asDouble();
        std::string type = i_PartEvent[sp_j]["type"].asString();
        if ( type == "triplet" ) {
          _type = 3;
        }
        if ( type == "duplet" ) {
          _type = 2;
        }
        _spacepoints.Fill();
      }
    }
  // } else {
  //  throw(Squeal(Squeal::recoverable,
  //        std::string("SpacePoints branch is corrupted!"),
  //        "ReduceCppSingleStation::draw_spacepoints"));
  // }
}

void ReduceCppSingleStation::digits_histograms(Json::Value root) {
  Json::Value digits = JsonWrapper::GetProperty(root,
                                                "digits" ,
                                                JsonWrapper::objectValue);
  int n_events = digits["single_station"].size();
  for ( int event_i = 0; event_i < n_events; event_i++ ) {
      if ( digits["single_station"][event_i].isNull() ) continue;
      Json::Value i_PartEvent = JsonWrapper::GetItem(digits["single_station"],
                                        event_i,
                                        JsonWrapper::arrayValue);
    int numb_digits = i_PartEvent.size();
    for ( int digit_j = 0; digit_j < numb_digits; digit_j++ ) {
      _plane      = i_PartEvent[digit_j]["plane"].asInt();
      _channel = i_PartEvent[digit_j]["channel"].asDouble();
      _npe     = i_PartEvent[digit_j]["npe"].asDouble();
      _digits.Fill();
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
    // Json::Value i_PartEvent = JsonWrapper::GetProperty(daq_data["single_station"][event_i],
    //                                    "VLSB_bank",
    //                                    JsonWrapper::objectValue);
    Json::Value i_PartEvent = daq_data["single_station"][event_i];
    int number_channels_within = i_PartEvent["VLSB_bank"].size();
    for ( int i = 0; i < number_channels_within; i++ ) {
      _adc  = i_PartEvent["VLSB_bank"][i]["adc"].asInt();
      _bank = i_PartEvent["VLSB_bank"][i]["bank_id"].asInt();
      _chan = i_PartEvent["VLSB_bank"][i]["channel"].asInt();
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
  _spacepoints.Write();

  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}
