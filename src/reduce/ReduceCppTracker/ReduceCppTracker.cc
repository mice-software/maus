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

  TCanvas *c1 = new TCanvas("c1", "Light Yield", 200, 10, 700, 500);
  TCanvas *c2 = new TCanvas("c2", "Spacepoints Type", 200, 10, 700, 500);
  TCanvas *c3 = new TCanvas("c3", "Efficiency", 200, 10, 700, 500);

  _digits.SetNameTitle("digits", "digits");
  _digits.Branch("npe", &_npe, "npe/D");
  _digits.Branch("tracker", &_tracker_dig, "tracker/I");

  _spacepoints.SetNameTitle("spacepoints", "spacepoints");
  _spacepoints.Branch("tracker", &_tracker, "tracker/I");
  _spacepoints.Branch("station", &_station, "station/I");
  _spacepoints.Branch("type", &_type, "type/I");

  _events.SetNameTitle("events", "events");
  _events.Branch("station_hits", &_station_hits, "station_hits/I");
  _events.Branch("tracker", &_tracker_event, "tracker/I");

  c1->SetFillColor(21);
  c1->GetFrame()->SetFillColor(42);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  c1->Divide(2, 1);

  c2->Divide(5, 2);
  c3->Divide(2, 1);
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
  // Check if the JSON document can be parsed, else return error only

  TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));
  TCanvas *c2 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c2"));
  TCanvas *c3 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c3"));

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
    /*xEventType = JsonWrapper::GetProperty(root,
                                          "daq_event_type",
                                          JsonWrapper::stringValue);


    if (xEventType == "physics_event" && root.isMember("space_points")) {*/
    if ( root.isMember("space_points") ) {
      // Light Yield plots.
      light_yield(root);

      int n_events = root["space_points"]["tracker1"].size();

      // Loop over events.
      for (int PartEvent = 0; PartEvent < n_events; PartEvent++) {
        Json::Value xPartEventTracker1_SP = GetPartEvent(root,
                                                         "space_points",
                                                         "tracker1",
                                                         PartEvent);
        Json::Value xPartEventTracker2_SP = GetPartEvent(root,
                                                         "space_points",
                                                         "tracker2",
                                                         PartEvent);

        int n_sp_tracker1 = xPartEventTracker1_SP.size();
        int n_sp_tracker2 = xPartEventTracker2_SP.size();

        bool station_hit[2][6] = { {false, false, false, false, false, false},
                                   {false, false, false, false, false, false}};

        for ( int i = 0; i < n_sp_tracker1; i++ ) {
          // Fill station number.
          int station = xPartEventTracker1_SP[i]["station"].asInt();
          _station = station;
          station_hit[0][station]=true;
          // Fill type.
          std::string type = xPartEventTracker1_SP[i]["type"].asString();
          if ( type == "triplet" ) {
            _type = 3;
          }
          if ( type == "duplet" ) {
            _type = 2;
          }
          _tracker = 1;
          _spacepoints.Fill();
        }

        for ( int i = 0; i < n_sp_tracker2; i++ ) {
          // Fill station number.
          int station = xPartEventTracker2_SP[i]["station"].asInt();
          _station = station;
          station_hit[1][station]=true;
          // Fill type.
          std::string type = xPartEventTracker2_SP[i]["type"].asString();
          if ( type == "triplet" ) {
            _type = 3;
          }
          if ( type == "duplet" ) {
            _type = 2;
          }
          _tracker = 2;
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
            _station_hits = hit_counter;
            _tracker_event = tr+1;
            _events.Fill();
          }
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
  // Display light yield.
  if (!(_nSpills%25)) {
    c1->cd(1);
    _digits.Draw("npe", "tracker==1");
    c1->cd(2);
    _digits.Draw("npe", "tracker==2");
    c1->Modified();
    c1->Update();
    Save();
  }
  // Display spacepoints type.
  // display_spacepoints();
  if (!(_nSpills%25)) {
    c2->cd(1);
    _spacepoints.Draw("type", "tracker==1 && station==1 ");
    c2->Update();
    c2->cd(2);
    _spacepoints.Draw("type", "tracker==1 && station==2 ");
    c2->Update();
    c2->cd(3);
    _spacepoints.Draw("type", "tracker==1 && station==3 ");
    c2->Update();
    c2->cd(4);
    _spacepoints.Draw("type", "tracker==1 && station==4 ");
    c2->Update();
    c2->cd(5);
    _spacepoints.Draw("type", "tracker==1 && station==5 ");
    c2->Update();
    // tracker 1
    c2->cd(6);
    _spacepoints.Draw("type", "tracker==2 && station==1 ");
    c2->Update();
    c2->cd(7);
    _spacepoints.Draw("type", "tracker==2 && station==2 ");
    c2->Update();
    c2->cd(8);
    _spacepoints.Draw("type", "tracker==2 && station==3 ");
    c2->Update();
    c2->cd(9);
    _spacepoints.Draw("type", "tracker==2 && station==4 ");
    c2->Update();
    c2->cd(10);
    _spacepoints.Draw("type", "tracker==2 && station==5 ");
    c2->Update();
  }

  // Display efficiency.
  // display_efficiency();
  if (!(_nSpills%25)) {
    c3->cd(1);
    _events.Draw("station_hits", "tracker==1");
    c3->cd(2);
    _events.Draw("station_hits", "tracker==2");
  }
  return document;
}

bool ReduceCppTracker::death()  {
  Save();
  return true;
}

void ReduceCppTracker::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _digits.Write();
  _spacepoints.Write();
  _events.Write();

  datafile.ls();
  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}

Json::Value ReduceCppTracker::GetPartEvent(Json::Value root, std::string entry_type,
                         std::string detector, int part_event) {
  Json::Value xPartEvent;
  Json::Value xHitDoc = JsonWrapper::GetProperty(root,
                                                 entry_type ,
                                                 JsonWrapper::objectValue);

  xPartEvent = xHitDoc[detector][part_event];
  return xPartEvent;
}

void ReduceCppTracker::light_yield(Json::Value &root) {
  int n_events = root["space_points"]["tracker1"].size();

  // Loop over events.
  for (int PartEvent = 0; PartEvent < n_events; PartEvent++) {
    Json::Value xPartEventTracker1_SP = GetPartEvent(root,
                                                     "space_points",
                                                     "tracker1",
                                                     PartEvent);
    Json::Value xPartEventTracker2_SP = GetPartEvent(root,
                                                     "space_points",
                                                     "tracker2",
                                                     PartEvent);

    int n_sp_tracker1 = xPartEventTracker1_SP.size();
    int n_sp_tracker2 = xPartEventTracker2_SP.size();

    for ( int event_i = 0; event_i < n_sp_tracker1; event_i++ ) {
      int n_digits = xPartEventTracker1_SP[event_i]["channels"].size();
      for ( int digit_j = 0; digit_j < n_digits; digit_j++ ) {
        _npe = xPartEventTracker1_SP[event_i]["channels"][digit_j]["npe"].asDouble();
        _tracker_dig = 1;
        _digits.Fill();
      }
    }

    for ( int event_i = 0; event_i < n_sp_tracker2; event_i++ ) {
      int n_digits = xPartEventTracker2_SP[event_i]["channels"].size();
      for ( int digit_j = 0; digit_j < n_digits; digit_j++ ) {
        _npe = xPartEventTracker2_SP[event_i]["channels"][digit_j]["npe"].asDouble();
        _tracker_dig = 2;
        _digits.Fill();
      }
    }
  }
}
