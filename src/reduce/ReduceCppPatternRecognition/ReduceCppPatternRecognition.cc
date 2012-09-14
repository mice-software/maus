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

#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

#include "src/reduce/ReduceCppPatternRecognition/ReduceCppPatternRecognition.hh"

namespace MAUS {

bool ReduceCppPatternRecognition::birth(std::string argJsonConfigDocument) {

  _classname = "ReduceCppPatternRecognition";
  _filename = "reduce_pattern_recognition.root"; // The output file
  _nSpills = 0;

  // Set up the ROOT canvases
  TCanvas *sp_xy = new TCanvas("sp_xy", "Spacepoint x-y Projections", 200, 10, 700, 500);
  sp_xy->Divide(3, 2);
  TCanvas *sp_xy_1 = new TCanvas("sp_xy_1",
                                   "Spacepoint x-y Projections (1 Spill)", 200, 10, 700, 500);
  sp_xy_1->Divide(3, 2);
  TCanvas *c_info = new TCanvas("c_info", "Info Box", 300, 335);
  c_info->Update();  // Just to get rid of a compiler warning for an unused variable

  // Set up TTree to hold spacepoints
  _spoints.SetNameTitle("spacepoints", "spacepoints");
  _spoints.Branch("tracker", &_tracker, "tracker/I");
  _spoints.Branch("station", &_station, "station/I");
  _spoints.Branch("x", &_x, "x/D");
  _spoints.Branch("y", &_y, "y/D");
  _spoints.Branch("z", &_z, "z/D");
  _spoints.Branch("type", &_type, "type/I");

  // Second TTre to hold spacepoints for just the current spill
  _spoints_1spill.SetNameTitle("spacepoints_1spill", "spacepoints_1spill");
  _spoints_1spill.Branch("tracker", &_tracker, "tracker/I");
  _spoints_1spill.Branch("station", &_station, "station/I");
  _spoints_1spill.Branch("x", &_x, "x/D");
  _spoints_1spill.Branch("y", &_y, "y/D");
  _spoints_1spill.Branch("z", &_z, "z/D");
  _spoints_1spill.Branch("type", &_type, "type/I");

  // Set up TTree to hold Pattern Recognition straight tracks
  _tracks.SetNameTitle("tracks", "tracks");
  _tracks.Branch("tracker", &_tracker, "tracker/I");
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

std::string ReduceCppPatternRecognition::process(std::string document) {
  std::cout << "Starting Pattern Recognition Reducer" << std::endl;

  // Read in the JSON data to a Spill object
  bool read_success = read_in_json(document);

  if (read_success) {

    // Retrieve the ROOT canvases
    TCanvas *sp_xy = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("sp_xy"));
    TCanvas *sp_xy_1 = reinterpret_cast<TCanvas*>
                       (gROOT->GetListOfCanvases()->FindObject("sp_xy_1"));
    TCanvas *c_info = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c_info"));

    TPaveText *pt = new TPaveText(.05, .1, .95, .95);
    pt->SetTextAlign(12);
    pt->SetFillColor(0);
    TTree * p_spoints  = &_spoints;
    TTree * p_spoints_1spill = &_spoints_1spill;

    update_info(c_info, pt);

    // Populate the TTrees
    _spoints_1spill.Reset();  // Reset so that this tree only holds data for the current spill
    try {
      if ( _spill.GetReconEvents() ) {
        // Loop over events
        for ( unsigned int evt_i = 0; evt_i < _spill.GetReconEvents()->size(); evt_i++ ) {
          SciFiEvent *event = _spill.GetReconEvents()->at(evt_i)->GetSciFiEvent();
          // Loop over spacepoints
          for ( unsigned int sp_i = 0; sp_i < event->spacepoints().size(); sp_i++ ) {
            SciFiSpacePoint *spoint = event->spacepoints()[sp_i];
            // Fill station number
            _station = spoint->get_station();
            // Fill type
            if ( spoint->get_type() == "triplet" )
              _type = 3;
            if ( spoint->get_type() == "duplet" )
              _type = 2;
            // Fill tracker number
            _tracker = spoint->get_tracker();
            // Fill position
            _x = spoint->get_position().x();
            _y = spoint->get_position().y();
            _z = spoint->get_position().z();
            // Fill trees
            _spoints.Fill();
            _spoints_1spill.Fill();
          } // ~Loop over spacepoints
          // Loop over straight tracks
          for ( unsigned int trk_i = 0; trk_i < event->straightprtracks().size(); trk_i++ ) {
            SciFiStraightPRTrack strk = event->straightprtracks()[trk_i];
            _x0 = strk.get_x0();
            _mx = strk.get_mx();
            _y0 = strk.get_y0();
            _my = strk.get_my();
            _num_points = strk.get_num_points();
            _tracker = strk.get_tracker();
            _tracks.Fill();
            if ( _tracker == 0 ) {
              _trks_zx_trkr0.push_back(make_track(_x0, _mx));
              _trks_zy_trkr0.push_back(make_track(_y0, _my));
            } else if ( _tracker == 1 ) {
              _trks_zx_trkr1.push_back(make_track(_x0, _mx));
              _trks_zy_trkr1.push_back(make_track(_y0, _my));
            }
            // std::cout << " x0 = " << _x0 << " mx = " << _mx;
            // std::cout << " y0 = " << _y0 << " my = " << _my << std::endl;
          } // ~Loop over straight tracks
        } // ~Loop over events
      } else {
        // std::cout << "No recon events found" << std::endl;
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
      draw_graphs(p_spoints, sp_xy);
    }

    if ( p_spoints_1spill->GetEntries() > 0 ) {
      draw_graphs(p_spoints_1spill, sp_xy_1);
    }

    if ( get_num_tracks() > 0 ) {
      draw_tracks(sp_xy_1);
      sp_xy_1->Update();
    }

    clear_tracks();

    std::cout << "Finished spill " << _nSpills << std::endl;
    std::cout << "Spacepoints this spill: " << _spoints_1spill.GetEntries() << std::endl;
    std::cout << "Cumulative spacepoints: " << _spoints.GetEntries() << std::endl;
  } else {
    std::cerr << "Failed to import json to spill\n";
  }

  return JsonWrapper::JsonToString(root);
}

bool ReduceCppPatternRecognition::death()  {
  Save();
  return true;
}

bool ReduceCppPatternRecognition::read_in_json(std::string json_data) {

  Json::FastWriter writer;
  Json::Reader reader;

  try {
    root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    _spill = *spill_proc.JsonToCpp(root);
    return true;
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed when importing JSON to Spill";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    writer.write(root);
    return false;
  }
}

void ReduceCppPatternRecognition::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _spoints.Write();
  _spoints_1spill.Write();
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

  pt->AddLine(.0, .875, 1., .875);

  int event_size = 0;
  int digit_size = 0;
  int cluster_size = 0;
  int spoint_size = 0;
  int track_size = 0;

  if ( _spill.GetReconEvents() ) {
    event_size = _spill.GetReconEvents()->size();
    for ( unsigned int evt_i = 0; evt_i < _spill.GetReconEvents()->size(); evt_i++ ) {
      SciFiEvent *event = _spill.GetReconEvents()->at(evt_i)->GetSciFiEvent();
      digit_size += event->digits().size();
      cluster_size += event->clusters().size();
      spoint_size += event->spacepoints().size();
      track_size += event->straightprtracks().size();
    }
  }

  ss1 << "Events: " << event_size;
  s1 = ss1.str();
  c1->cd(1);
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Digits: " << digit_size;
  s1 = ss1.str();
  c1->cd(1);
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Clusters: " << cluster_size;
  s1 = ss1.str();
  c1->cd(1);
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Spacepoints: " << spoint_size;
  s1 = ss1.str();
  c1->cd(1);
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Tracks: " << track_size;
  s1 = ss1.str();
  c1->cd(1);
  pt->AddText(s1.c_str());
  ss1.str("");

  pt->AddLine(.0, .25, 1., .25);

  ss1 << "Cumulative spoints: " << get_num_spoints() + spoint_size;
  s1 = ss1.str();
  pt->AddText(s1.c_str());
  ss1.str("");

  ss1 << "Cumulative tracks: " << get_num_tracks() + track_size;
  s1 = ss1.str();
  pt->AddText(s1.c_str());
  ss1.str("");

  pt->Draw();
  c1->Update();
}

} // ~namespace MAUS
