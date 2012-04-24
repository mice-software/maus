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

  TCanvas *c1 = new TCanvas("c1", "ADC Monitor", 200, 10, 700, 500);

  _unpacked.SetNameTitle("unpacked", "unpacked");
  _unpacked.Branch("adc", &_adc, "adc/I");
  _unpacked.Branch("bank", &_bank, "bank/I");
  _unpacked.Branch("chan", &_chan, "chan/I");

  c1->Divide(4, 2);
  c1->SetFillColor(21);
  c1->GetFrame()->SetFillColor(42);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

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

std::string  ReduceCppSingleStation::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  // Check if the JSON document can be parsed, else return error only

  TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));

  try {
    root = JsonWrapper::StringToJson(document);}
  catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  try {
    if ( root.isMember("daq_data") ) {
      int n_events = root["daq_data"]["single_station"].size();
      // std::cout << "Number of events: " << n_events << std::endl;
      // Loop over events.
      for (int PartEvent = 0; PartEvent < n_events; PartEvent++) {
        if ( root["daq_data"]["single_station"][PartEvent].isNull() ) continue;
        Json::Value i_PartEvent = GetPartEvent(root,
                                               "daq_data",
                                               "single_station",
                                               PartEvent);

        int number_channels_within = i_PartEvent["VLSB"].size();
        // std::cout << "Size of VLSB: " << number_channels_within << std::endl;
        for ( int i = 0; i < number_channels_within; i++ ) {
          int adc = i_PartEvent["VLSB"][i]["adc"].asInt();
          int bank = i_PartEvent["VLSB"][i]["bank_id"].asInt();
          int chan = i_PartEvent["VLSB"][i]["channel"].asInt();
          _adc = adc;
          _bank= bank;
          _chan= chan;
          _unpacked.Fill();
        }
      } // ends loop over particle events
    }
  } catch(Squeal squee) {
     Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

  _nSpills++;
  // Display spacepoints type.
  // display_spacepoints();
  if (!(_nSpills%2)) {
    c1->cd(1);
    _unpacked.Draw("adc:chan", "bank == 0 ");
    c1->Update();
    c1->cd(2);
    _unpacked.Draw("adc:chan", "bank == 1 ");
    c1->Update();
    c1->cd(3);
    _unpacked.Draw("adc:chan", "bank == 2 ");
    c1->Update();
    c1->cd(4);
    _unpacked.Draw("adc:chan", "bank == 3 ");
    c1->Update();
    c1->cd(5);
    _unpacked.Draw("adc:chan", "bank == 4 ");
    c1->Update();
    c1->cd(6);
    _unpacked.Draw("adc:chan", "bank == 5 ");
    c1->Update();
    c1->cd(7);
    _unpacked.Draw("adc:chan", "bank == 6 ");
    c1->Update();
    c1->cd(8);
    _unpacked.Draw("adc:chan", "bank == 7 ");
    c1->Update();
  }

  return document;
}

bool ReduceCppSingleStation::death()  {
  Save();
  return true;
}

void ReduceCppSingleStation::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _unpacked.Write();

  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}

Json::Value ReduceCppSingleStation::GetPartEvent(Json::Value root, std::string entry_type,
                         std::string detector, int part_event) {
  Json::Value xPartEvent;
  Json::Value xHitDoc = JsonWrapper::GetProperty(root,
                                                 entry_type ,
                                                 JsonWrapper::objectValue);

  xPartEvent = xHitDoc[detector][part_event];
  return xPartEvent;
}
