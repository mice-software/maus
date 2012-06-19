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
#include <math.h>
#include <fstream>
#include <iostream>
#include "Interface/dataCards.hh"
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/reduce/ReduceCppMCTracker/ReduceCppMCTracker.hh"

bool ReduceCppMCTracker::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "ReduceCppMCTracker";
  _filename = "MCTracker.root";
  _nSpills = 0;
  ofstream myfile;
  myfile.open("test2.txt");
  myfile.close();

  TCanvas *c1 = new TCanvas("c1", "Efficiencies", 200, 10, 600, 500);
  c1->SetFillColor(0);
  c1->GetFrame()->SetFillColor(0);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(0);
  gStyle->SetHistFillColor(kBlack);
  c1->Divide(2, 2);

  TCanvas *c2 = new TCanvas("c2", "Energy vs Z", 200, 10, 600, 500);
  c2->SetFillColor(0);
  c2->GetFrame()->SetFillColor(0);
  c2->GetFrame()->SetBorderSize(6);
  c2->GetFrame()->SetBorderMode(0);
  gStyle->SetHistFillColor(kBlack);
  c2->Divide(2, 1);

  _mctrue.SetNameTitle("MC True" , "MC True");
  _mctrue.Branch("x", &_x, "x/D");
  _mctrue.Branch("y", &_y, "y/D");
  _mctrue.Branch("z", &_z, "z/D");
  _mctrue.Branch("px", &_px, "px/D");
  _mctrue.Branch("py", &_py, "py/D");
  _mctrue.Branch("pz", &_pz, "pz/D");
  _mctrue.Branch("energy", &_energy, "energy/D");

  _sci_fi.SetNameTitle("Sci_Fi" , "Sci_Fi");
  _sci_fi.Branch("x", &_x, "x/D");
  _sci_fi.Branch("y", &_y, "y/D");
  _sci_fi.Branch("z", &_z, "z/D");
  _sci_fi.Branch("px", &_px, "px/D");
  _sci_fi.Branch("py", &_py, "py/D");
  _sci_fi.Branch("pz", &_pz, "pz/D");
  _sci_fi.Branch("energy", &_energy, "energy/D");
  _sci_fi.Branch("de", &_de, "de/D");
  _sci_fi.Branch("tracker", &_tracker, "tracker/I");
  _sci_fi.Branch("station", &_station, "station/I");
  _sci_fi.Branch("plane", &_plane, "plane/I");

  _virt.SetNameTitle("Virt_Plane" , "Virt_Plane");
  _virt.Branch("x", &_x, "x/D");
  _virt.Branch("y", &_y, "y/D");
  _virt.Branch("z", &_z, "z/D");
  _virt.Branch("px", &_px, "px/D");
  _virt.Branch("py", &_py, "py/D");
  _virt.Branch("pz", &_pz, "pz/D");
  _virt.Branch("bx", &_bx, "bx/D");
  _virt.Branch("by", &_by, "by/D");
  _virt.Branch("bz", &_bz, "bz/D");
  _virt.Branch("energy", &_energy, "energy/D");

  _sp.SetNameTitle("Spacepoints", "Spacepoints");
  _sp.Branch("tracker", &_tracker, "tracker/I");
  _sp.Branch("station", &_station, "station/I");
  _sp.Branch("type", &_type, "type/I");
  _sp.Branch("x", &_x, "x/D");
  _sp.Branch("y", &_y, "y/D");
  _sp.Branch("z", &_z, "z/D");
  _sp.Branch("part_event", &_part_event, "part_event/I");

  _digits.SetNameTitle("Digits", "Digits");
  _digits.Branch("npe", &_npe, "npe/D");
  _digits.Branch("plane", &_plane, "plane/I");
  _digits.Branch("station", &_station, "station/I");
  _digits.Branch("tracker", &_tracker, "tracker/I");
  _digits.Branch("x", &_x, "x/D");
  _digits.Branch("y", &_y, "y/D");
  _digits.Branch("z", &_z, "z/D");

  _other.SetNameTitle("Other", "Other");
  _other.Branch("cluster_eff1", &cluster_eff1, "cluster_eff1/D");
  _other.Branch("cluster_eff2", &cluster_eff2, "cluster_eff2/D");
  _other.Branch("space_eff1", &space_eff1, "space_eff1/D");
  _other.Branch("space_eff2", &space_eff2, "space_eff2/D");
  _other.Branch("nSpills", &_nSpills, "nSpills/I");

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




std::string  ReduceCppMCTracker::process(std::string document) {
  Json::FastWriter writer;
  Json::Value root;
  Json::Value scuts;
  TCanvas *c1 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c1"));
  TCanvas *c2 = reinterpret_cast<TCanvas*> (gROOT->GetListOfCanvases()->FindObject("c2"));


  // Check if the JSON document can be parsed, else return error only
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
    _nSpills = root["spill_number"].asInt();

// Start MC Events==============================================================
    int mc_part_size = root["mc_events"].size();
    for (int a = 0; a < mc_part_size; a++) {


  // Start Sci Fi Hits==========================================================
      // if (root["mc_events"][a].isMember("sci_fi_hits")) {
        int hit_size = root["mc_events"][a]["sci_fi_hits"].size();
        for (int b = 0; b < hit_size; b++) {
          scuts = root["mc_events"][a]["sci_fi_hits"][b];
          _x = scuts["position"]["x"].asDouble();
          _y = scuts["position"]["y"].asDouble();
          _z = scuts["position"]["z"].asDouble();
          _px = scuts["momentum"]["x"].asDouble();
          _py = scuts["momentum"]["y"].asDouble();
          _pz = scuts["momentum"]["z"].asDouble();
          _energy = scuts["energy"].asDouble();
          _de = scuts["energy_deposited"].asDouble();
          _tracker = scuts["channel_id"]["tracker_number"].asDouble();
          _station = scuts["channel_id"]["station_number"].asDouble();
          _plane = scuts["channel_id"]["plane_number"].asDouble();
          _mctrue.Fill();
          _sci_fi.Fill();
          }
        // }
  // End Sci Fi Hits============================================================


  // Start Virtual Hits=========================================================
        // if (root["mc_events"][a].isMember("virtual_hits")) {
          int virtual_size = root["mc_events"][a]["virtual_hits"].size();
          for (int c = 0; c < virtual_size; c++) {
            scuts = root["mc_events"][a]["virtual_hits"][c];
            _x = scuts["position"]["x"].asDouble();
            _y = scuts["position"]["y"].asDouble();
            _z = scuts["position"]["z"].asDouble();
            _px = scuts["momentum"]["x"].asDouble();
            _py = scuts["momentum"]["y"].asDouble();
            _pz = scuts["momentum"]["z"].asDouble();
            _bx = scuts["b_field"]["x"].asDouble();
            _by = scuts["b_field"]["y"].asDouble();
            _bz = scuts["b_field"]["z"].asDouble();

            _pT = sqrt(_px * _px + _py * _py + _pz * _pz);
            _energy = sqrt(105.658 * 105.658 + _pT * _pT);

            _mctrue.Fill();
            _virt.Fill();
          }
        // }
  // End Virtual Hits===========================================================
      }
// End MC=======================================================================


// Start Recon Events===========================================================
    int recon_size = root["recon_events"].size();
    int cut_recon_size = recon_size;
    int cluster_count1 = 0;
    int cluster_count2 = 0;
    int space_count1 = 0;
    int space_count2 = 0;
    ofstream pyfile;
    pyfile.open("test2.txt" , fstream::app);
    pyfile << "Total number of particles this spill: "
      << recon_size << std::endl;
    for (int h = 0; h < recon_size; h++) {
      scuts = root["recon_events"][h]["tof_event"]["tof_slab_hits"]["tof2"];
      pyfile << "Slab hits in TOF2 = " << scuts.size() << std::endl;
      if (Trigger(scuts)) {
        pyfile << "Spill number: " << _nSpills << " Particle number: " << h
          << std::endl;


  // Start of Digits============================================================
        // if (root["recon_events"][h]["sci_fi_event"].
        //     isMember("sci_fi_digits")) {
          scuts = root["recon_events"][h]["sci_fi_event"]["sci_fi_digits"];
          int digit0_size = scuts["tracker0"].size();
          for (int i = 0; i < digit0_size; i++) {
            _npe = scuts["tracker0"][i]["npe"].asDouble();
            _tracker = scuts["tracker0"][i]["tracker"].asInt();
            _station = scuts["tracker0"][i]["station"].asInt();
            _plane = scuts["tracker0"][i]["plane"].asInt();
            _x = scuts["tracker0"][i]["true_position"]["x"].asDouble();
            _y = scuts["tracker0"][i]["true_position"]["y"].asDouble();
            _z = scuts["tracker0"][i]["true_position"]["z"].asDouble();
            _digits.Fill();
            }
          int digit1_size = scuts["tracker1"].size();
          for (int j = 0; j < digit1_size; j++) {
            _npe = scuts["tracker1"][j]["npe"].asDouble();
            _tracker = scuts["tracker1"][j]["tracker"].asInt();
            _station = scuts["tracker1"][j]["station"].asInt();
            _plane = scuts["tracker1"][j]["plane"].asInt();
            _x = scuts["tracker1"][j]["true_position"]["x"].asDouble();
            _y = scuts["tracker1"][j]["true_position"]["y"].asDouble();
            _z = scuts["tracker1"][j]["true_position"]["z"].asDouble();
            _digits.Fill();
          }
        // }
  // End of Digits==============================================================


  // Start of Clusters==========================================================
        // if (root["recon_events"][h]["sci_fi_event"].
        //   isMember("sci_fi_clusters")) {
          scuts = root["recon_events"][h]["sci_fi_event"]["sci_fi_clusters"];
          int cluster0_size = scuts["tracker0"].size();
          int cluster1_size = scuts["tracker1"].size();
          cluster_count1 += cluster0_size;
          cluster_count2 += cluster1_size;
          pyfile << "Total number of clusters: Tracker 1: " << cluster_count1
            << "   Tracker 2: " << cluster_count2 << std::endl;
        // }
  // End of Clusters============================================================


  // Start of Space Points======================================================
        // if (root["recon_events"][h]["sci_fi_event"].
        //   isMember("sci_fi_space_points")) {
          scuts = root["recon_events"][h]["sci_fi_event"]["sci_fi_space_points"];
          int space0_size = scuts["tracker0"].size();
          int space1_size = scuts["tracker1"].size();
          for (int k = 0; k < space0_size; k++) {
            _x = scuts["tracker0"][k]["position"]["x"].asDouble();
            _y = scuts["tracker0"][k]["position"]["y"].asDouble();
            _z = scuts["tracker0"][k]["position"]["z"].asDouble();
            _station = scuts["tracker0"][k]["station"].asInt();
            _tracker = scuts["tracker0"][k]["tracker"].asInt();
            std::string type = scuts["tracker0"][k]["type"].asString();
            if (type == "triplet") {_type = 3;}
            if (type == "duplet") {_type = 2;}
            _part_event = scuts["tracker0"][k]["part_event_number"].asInt();
            _sp.Fill();
          }
          for (int l = 0; l < space1_size; l++) {
            _x = scuts["tracker1"][l]["position"]["x"].asDouble();
            _y = scuts["tracker1"][l]["position"]["y"].asDouble();
            _z = scuts["tracker1"][l]["position"]["z"].asDouble();
            _station = scuts["tracker1"][l]["station"].asInt();
            _tracker = scuts["tracker1"][l]["tracker"].asInt();
            std::string type = scuts["tracker1"][l]["type"].asString();
            if (type == "triplet") {_type = 3;}
            if (type == "duplet") {_type = 2;}
            _part_event = scuts["tracker1"][l]["part_event_number"].asInt();
            _sp.Fill();
          }
          space_count1 += space0_size;
          space_count2 += space1_size;
          pyfile << "Total number of space points: Tracker 1: " << space_count1
            << "   Tracker 2: " << space_count2 << std::endl;
        // }
  // End of Space Points========================================================
      } else {cut_recon_size--;}
    }
//End Recon Events==============================================================


    pyfile << std::endl << "At the end of the spill we have " << cut_recon_size
      << " particles that made it through to TOF2s." << std::endl;
    pyfile << "  Total number of space points: Tracker 1: " << space_count1
      << "   Tracker 2: " << space_count2 << std::endl;
    pyfile << "  Total number of clusters: Tracker 1: " << cluster_count1
      << "   Tracker 2: " << cluster_count2 << std::endl;

    space_eff1 = space_count1/(cut_recon_size*5.0);
    space_eff2 = space_count2/(cut_recon_size*5.0);
    cluster_eff1 = cluster_count1/(cut_recon_size*15.0);
    cluster_eff2 = cluster_count2/(cut_recon_size*15.0);
    pyfile << std::endl << "  Total cluster efficiency: Tracker 1: " << cluster_eff1
      << "   Tracker 2: " << cluster_eff2 << std::endl;
    pyfile << "  Total space point efficiency: Tracker 1: " << space_eff1
      << "   Tracker 2: " << space_eff2 << std::endl << std::endl;
    _other.Fill();
    pyfile.close();
  } catch(Squeal squee) {
    Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

  c1->cd(1);
  _other.SetMarkerStyle(2);
  _other.SetMarkerSize(1);
  _other.Draw("cluster_eff1:nSpills");
  c1->cd(2);
  _other.Draw("cluster_eff2:nSpills");
  c1->cd(3);
  _other.Draw("space_eff1:nSpills");
  c1->cd(4);
  _other.Draw("space_eff2:nSpills");

  c1->Modified();
  c1->Update();

  c2->cd(1);
  _mctrue.Draw("energy:z");
  c2->cd(2);
  _digits.Draw("npe");

  c2->Modified();
  c2->Update();
  Save();
  return document;
}


void ReduceCppMCTracker::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _mctrue.Write();
  _sci_fi.Write();
  _virt.Write();
  _sp.Write();
  _digits.Write();
  _other.Write();

  datafile.ls();
  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}

bool ReduceCppMCTracker::Trigger(Json::Value path) {
  if (path.size() > 0) {return true;}
  return false;
}


bool ReduceCppMCTracker::death()  {
  Save();
  return true;
}

