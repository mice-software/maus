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

#include <string>
#include <sstream>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/reduce/ReduceCppPatternRecognition/ReduceCppPatternRecognition.hh"

bool ReduceCppPatternRecognition::birth(std::string argJsonConfigDocument) {

  _classname = "ReduceCppPatternRecognition";
  _filename = "cosmics.root";                     // The output file
  _nSpills = 0;

  // Set up the ROOT canvases
  // TCanvas *c1 = new TCanvas("c1", "Spacepoint X, Y, Z Histos", 200, 10, 700, 500);
  // c1->Divide(3, 2);

  TCanvas *c2 = new TCanvas("c2", "Spacepoint RealSpace Projections", 200, 10, 700, 500);
  c2->Divide(3, 2);

  // TCanvas *c3 = new TCanvas("c3", "Spacepoint X, Y, Z Histos (1 Spill)", 200, 10, 700, 500);
  // c3->Divide(3, 2);

  TCanvas *c4 = new TCanvas("c4", "Spacepoint RealSpace Projections (1 Spill)", 200, 10, 700, 500);
  c4->Divide(3, 2);

  TCanvas *c5 = new TCanvas("c5", "Info Box", 200, 10, 300, 170);
  c5->Update();  // Just to get rid of a compiler warning for an unused variable


  // Set up TTree to hold digits
  _digits.SetNameTitle("digits", "digits");
  _digits.Branch("npe", &_npe, "npe/D");
  _digits.Branch("tracker", &_tracker_dig, "tracker/I");

  // Set up TTree to hold spacepoints
  _spacepoints.SetNameTitle("spacepoints", "spacepoints");
  _spacepoints.Branch("tracker", &_tracker, "tracker/I");
  _spacepoints.Branch("station", &_station, "station/I");
  _spacepoints.Branch("x", &_x, "x/D");
  _spacepoints.Branch("y", &_y, "y/D");
  _spacepoints.Branch("z", &_z, "z/D");
  _spacepoints.Branch("type", &_type, "type/I");

  // Second TTre to hold spacepoints for just the current spill
  _spacepoints_1spill.SetNameTitle("spacepoints_1spill", "spacepoints_1spill");
  _spacepoints_1spill.Branch("tracker", &_tracker, "tracker/I");
  _spacepoints_1spill.Branch("station", &_station, "station/I");
  _spacepoints_1spill.Branch("x", &_x, "x/D");
  _spacepoints_1spill.Branch("y", &_y, "y/D");
  _spacepoints_1spill.Branch("z", &_z, "z/D");
  _spacepoints_1spill.Branch("type", &_type, "type/I");

  // Set up TTree to hold Pattern Recognition tracks
  _tracks.SetNameTitle("tracks", "tracks");
  // _tracks.Branch("station_hits", &_station_hits, "station_hits/I");
  _tracks.Branch("tracker", &_tracker_event, "tracker/I");
  _tracks.Branch("mx", &_mx, "mx/D");
  _tracks.Branch("my", &_my, "my/D");
  _tracks.Branch("x0", &_x0, "x0/D");
  _tracks.Branch("y0", &_y0, "y0/D");
  _tracks.Branch("n_sp", &_num_points, "n_sp/I");

  // JsonCpp setup - check file parses correctly, if not return false
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

std::string  ReduceCppPatternRecognition::process(std::string document) {

  std::cout << "Starting Pattern Recognition Reducer" << std::endl;

  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;

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

  // Retrieve the ROOT canvases
  // TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));
  TCanvas *c2 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c2"));
  // TCanvas *c3 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c3"));
  TCanvas *c4 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c4"));
  TCanvas *c5 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c5"));
  TPaveText *pt = new TPaveText(.05, .1, .95, .8);
  TTree * p_spoints  = &_spacepoints;
  TTree * p_spoints_1spill  = &_spacepoints_1spill;

  update_info(c5, pt);

  // Populate the TTrees
  _spacepoints_1spill.Reset();
  try {
    if ( root.isMember("recon_events") ) {
      int n_events = root["recon_events"].size();

      // Loop over events.
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
          _spacepoints_1spill.Fill();
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
          _spacepoints_1spill.Fill();
        }

        Json::Value tracks_tracker0 = root["recon_events"][event_i]["sci_fi_event"]
        ["sci_fi_pr_tracks"]["straight"]["tracker0"];
        Json::Value tracks_tracker1 = root["recon_events"][event_i]["sci_fi_event"]
        ["sci_fi_pr_tracks"]["straight"]["tracker1"];

        for ( int trk_i = 0; trk_i < static_cast<int>(tracks_tracker0.size()); trk_i++ ) {
          _mx = tracks_tracker0[trk_i]["mx"].asDouble();
          _my = tracks_tracker0[trk_i]["my"].asDouble();
          _x0 = tracks_tracker0[trk_i]["x0"].asDouble();
          _y0 = tracks_tracker0[trk_i]["y0"].asDouble();
          _num_points = tracks_tracker0[trk_i]["num_points"].asInt();
          _tracks.Fill();
          _trks_zx_trkr0.push_back(make_track(_x0, _mx));
          _trks_zy_trkr0.push_back(make_track(_y0, _my));
          std::cout << "reducer t0 tracks: " << tracks_tracker0.size();
          std::cout << " x0 = " << _x0 << " mx = " << _mx;
          std::cout << " y0 = " << _y0 << " my = " << _my << std::endl;
        }

        for ( int trk_i = 0; trk_i < static_cast<int>(tracks_tracker1.size()); trk_i++ ) {
          _mx = tracks_tracker1[trk_i]["mx"].asDouble();
          _my = tracks_tracker1[trk_i]["my"].asDouble();
          _x0 = tracks_tracker1[trk_i]["x0"].asDouble();
          _y0 = tracks_tracker1[trk_i]["y0"].asDouble();
          _num_points = tracks_tracker1[trk_i]["num_points"].asInt();
          _tracks.Fill();
          _trks_zx_trkr1.push_back(make_track(_x0, _mx));
          _trks_zy_trkr1.push_back(make_track(_y0, _my));
          std::cout << "reducer t1 tracks: " << tracks_tracker1.size();
          std::cout << " x0 = " << _x0 << " mx = " << _mx;
          std::cout << " y0 = " << _y0 << " my = " << _my << std::endl;
        }
      } // ends loop over particle events
    } else {
      std::cout << "No recon events found" << std::endl;
    }
  } catch(Squeal squee) {
     Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

  _nSpills++;

  // Draw the graphs and histos
  if ( p_spoints->GetEntries() > 0 ) {
    // draw_histos(p_spoints, c1);
    draw_graphs(p_spoints, c2);
  }

  if ( p_spoints_1spill->GetEntries() > 0 ) {
    // draw_histos(p_spoints_1spill, c3);
    draw_graphs(p_spoints_1spill, c4);
  }

  if ( get_num_tracks() > 0 )
    draw_tracks(c4);

  clear_tracks();

  // _tracks.Scan("x0:mx:y0:my:n_sp");
  std::cout << "Finished spill " << _nSpills << std::endl;
  std::cout << "Spacepoints this spill: " << _spacepoints_1spill.GetEntries() << std::endl;
  std::cout << "Cumulative spacepoints: " << _spacepoints.GetEntries() << std::endl;
  return document;
}

bool ReduceCppPatternRecognition::death()  {
  Save();
  return true;
}

void ReduceCppPatternRecognition::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _digits.Write();
  _spacepoints.Write();
  _spacepoints_1spill.Write();
  _tracks.Write();

  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}

TF1 ReduceCppPatternRecognition::make_track(double c, double m) {
  // Note: in the function expression, x is just the independent variable, which
  // in this case is the z coordinate in the tracker coordinate system
  TF1 trk = TF1("trk", "[0]+([1]*x)", _trk_lower_bound, _trk_upper_bound);
  trk.SetParameters(c, m);
  return trk;
}

void ReduceCppPatternRecognition::draw_tracks(TCanvas * c1) {

  for (int i = 0; i < static_cast<int>(_trks_zx_trkr0.size()); ++i) {
    c1->cd(2);
    _trks_zx_trkr0[i].Draw("same");
    c1->Update();
  }

  for (int i = 0; i < static_cast<int>(_trks_zy_trkr0.size()); ++i) {
    c1->cd(3);
    _trks_zy_trkr0[i].Draw("same");
    c1->Update();
  }

  for (int i = 0; i < static_cast<int>(_trks_zx_trkr1.size()); ++i) {
    c1->cd(5);
    _trks_zx_trkr1[i].Draw("same");
    c1->Update();
  }

  for (int i = 0; i < static_cast<int>(_trks_zy_trkr1.size()); ++i) {
    c1->cd(6);
    _trks_zy_trkr1[i].Draw("same");
    c1->Update();
  }
}

void ReduceCppPatternRecognition::clear_tracks() {
  _trks_zx_trkr0.clear();
  _trks_zy_trkr0.clear();
  _trks_zx_trkr1.clear();
  _trks_zy_trkr1.clear();
}

void ReduceCppPatternRecognition::draw_histos(TTree * t1, TCanvas * c1) {

  c1->cd(1);
  t1->Draw("x", "tracker==1");
  c1->Update();

  c1->cd(2);
  t1->Draw("y", "tracker==1");
  c1->Update();

  c1->cd(3);
  t1->Draw("z", "tracker==1");
  c1->Update();

  c1->cd(4);
  t1->Draw("x", "tracker==2");
  c1->Update();

  c1->cd(5);
  t1->Draw("y", "tracker==2");
  c1->Update();

  c1->cd(6);
  t1->Draw("z", "tracker==2");
  c1->Update();
}

void ReduceCppPatternRecognition::draw_graphs(TTree * t1, TCanvas * c1) {

  c1->cd(1);
  t1->Draw("x:y", "tracker==0", "goff");
  TGraph * gr_xy1 = new TGraph(t1->GetSelectedRows(), t1->GetV1(), t1->GetV2());
  gr_xy1->SetTitle("Tracker 1 X-Y Projection");
  gr_xy1->GetXaxis()->SetTitle("x(mm)");
  gr_xy1->GetYaxis()->SetTitle("y(mm)");
  gr_xy1->SetMarkerStyle(20);
  gr_xy1->SetMarkerColor(4);
  gr_xy1->Draw("AP");
  c1->Update();

  c1->cd(2);
  t1->Draw("z:x", "tracker==0", "goff");
  TGraph * gr_zx1 = new TGraph(t1->GetSelectedRows(), t1->GetV1(), t1->GetV2());
  gr_zx1->SetTitle("Tracker 1 Z-X Projection");
  gr_zx1->GetXaxis()->SetTitle("z(mm)");
  gr_zx1->GetYaxis()->SetTitle("x(mm)");
  gr_zx1->SetMarkerStyle(20);
  gr_zx1->SetMarkerColor(4);
  gr_zx1->Draw("AP");
  c1->Update();

  c1->cd(3);
  t1->Draw("z:y", "tracker==0", "goff");
  TGraph * gr_zy1 = new TGraph(t1->GetSelectedRows(), t1->GetV1(), t1->GetV2());
  gr_zy1->SetTitle("Tracker 1 Z-Y Projection");
  gr_zy1->GetXaxis()->SetTitle("z(mm)");
  gr_zy1->GetYaxis()->SetTitle("y(mm)");
  gr_zy1->SetMarkerStyle(20);
  gr_zy1->SetMarkerColor(4);
  gr_zy1->Draw("AP");
  c1->Update();

  c1->cd(4);
  t1->Draw("x:y", "tracker==1", "goff");
  TGraph * gr_xy2 = new TGraph(t1->GetSelectedRows(), t1->GetV1(), t1->GetV2());
  gr_xy2->SetTitle("Tracker 2 X-Y Projection");
  gr_xy2->GetXaxis()->SetTitle("x(mm)");
  gr_xy2->GetYaxis()->SetTitle("y(mm)");
  gr_xy2->SetMarkerStyle(20);
  gr_xy2->SetMarkerColor(4);
  gr_xy2->Draw("AP");
  c1->Update();

  c1->cd(5);
  t1->Draw("z:x", "tracker==1", "goff");
  TGraph * gr_zx2 = new TGraph(t1->GetSelectedRows(), t1->GetV1(), t1->GetV2());
  gr_zx2->SetTitle("Tracker 2 Z-X Projection");
  gr_zx2->GetXaxis()->SetTitle("z(mm)");
  gr_zx2->GetYaxis()->SetTitle("x(mm)");
  gr_zx2->SetMarkerStyle(20);
  gr_zx2->SetMarkerColor(4);
  gr_zx2->Draw("AP");
  c1->Update();

  c1->cd(6);
  t1->Draw("z:y", "tracker==1", "goff");
  TGraph * gr_zy2 = new TGraph(t1->GetSelectedRows(), t1->GetV1(), t1->GetV2());
  gr_zy2->SetTitle("Tracker 2 Z-X Projection");
  gr_zy2->GetXaxis()->SetTitle("z(mm)");
  gr_zy2->GetYaxis()->SetTitle("y(mm)");
  gr_zy2->SetMarkerStyle(20);
  gr_zy2->SetMarkerColor(4);
  gr_zy2->Draw("AP");
  c1->Update();
}

void ReduceCppPatternRecognition::update_info(TCanvas * c1, TPaveText *pt) {

  c1->Clear();
  pt->Clear();

  std::stringstream ss1;
  std::string s1;
  ss1 << "Spill #: " << _nSpills;
  s1 = ss1.str();
  c1->cd(1);
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Cumulative spoints: " << get_num_spoints();
  s1 = ss1.str();
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Cumulative tracks: " << get_num_tracks();
  s1 = ss1.str();
  pt->AddText(s1.c_str());
  ss1.str("");

  pt->Draw();
  c1->Update();
}

