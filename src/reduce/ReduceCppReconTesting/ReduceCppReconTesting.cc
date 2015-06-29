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
#include <iostream>
#include <fstream>
#include <bitset>

#include "TCanvas.h"
//~ #include "src/map/MapCppGlobalTrackMatching/MapCppGlobalTrackMatching.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/reduce/ReduceCppReconTesting/ReduceCppReconTesting.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/Recon/Global/MCTruthTools.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/legacy/BeamTools/BTTracker.hh"

#include "TFile.h"
#include "TH2.h"

namespace MAUS {

bool ReduceCppReconTesting::birth(std::string aJsonConfigDocument) {

  mClassname = "ReduceCppReconTesting";

  // JsonCpp setup - check file parses correctly, if not return false
  Json::Value configJSON;
  _ntuples["Tracker110xy"] = new TNtuple("Tracker110xy","Tracker110xy","x:y");
  _ntuples["Tracker111xy"] = new TNtuple("Tracker111xy","Tracker111xy","x:y");
  _ntuples["Tracker112xy"] = new TNtuple("Tracker112xy","Tracker112xy","x:y");
  _ntuples["Tracker120xy"] = new TNtuple("Tracker120xy","Tracker120xy","x:y");
  _ntuples["Tracker121xy"] = new TNtuple("Tracker121xy","Tracker121xy","x:y");
  _ntuples["Tracker122xy"] = new TNtuple("Tracker122xy","Tracker122xy","x:y");
  _ntuples["Tracker130xy"] = new TNtuple("Tracker130xy","Tracker130xy","x:y");
  _ntuples["Tracker131xy"] = new TNtuple("Tracker131xy","Tracker131xy","x:y");
  _ntuples["Tracker132xy"] = new TNtuple("Tracker132xy","Tracker132xy","x:y");
  _ntuples["Tracker140xy"] = new TNtuple("Tracker140xy","Tracker140xy","x:y");
  _ntuples["Tracker141xy"] = new TNtuple("Tracker141xy","Tracker141xy","x:y");
  _ntuples["Tracker142xy"] = new TNtuple("Tracker142xy","Tracker142xy","x:y");
  _ntuples["Tracker150xy"] = new TNtuple("Tracker150xy","Tracker150xy","x:y");
  _ntuples["Tracker151xy"] = new TNtuple("Tracker151xy","Tracker151xy","x:y");
  _ntuples["Tracker152xy"] = new TNtuple("Tracker152xy","Tracker152xy","x:y");

  _ntuples["Tracker110dxdy"] = new TNtuple("Tracker110dxdy","Tracker110dxdy","dx:dy");
  _ntuples["Tracker111dxdy"] = new TNtuple("Tracker111dxdy","Tracker111dxdy","dx:dy");
  _ntuples["Tracker112dxdy"] = new TNtuple("Tracker112dxdy","Tracker112dxdy","dx:dy");
  _ntuples["Tracker120dxdy"] = new TNtuple("Tracker120dxdy","Tracker120dxdy","dx:dy");
  _ntuples["Tracker121dxdy"] = new TNtuple("Tracker121dxdy","Tracker121dxdy","dx:dy");
  _ntuples["Tracker122dxdy"] = new TNtuple("Tracker122dxdy","Tracker122dxdy","dx:dy");
  _ntuples["Tracker130dxdy"] = new TNtuple("Tracker130dxdy","Tracker130dxdy","dx:dy");
  _ntuples["Tracker131dxdy"] = new TNtuple("Tracker131dxdy","Tracker131dxdy","dx:dy");
  _ntuples["Tracker132dxdy"] = new TNtuple("Tracker132dxdy","Tracker132dxdy","dx:dy");
  _ntuples["Tracker140dxdy"] = new TNtuple("Tracker140dxdy","Tracker140dxdy","dx:dy");
  _ntuples["Tracker141dxdy"] = new TNtuple("Tracker141dxdy","Tracker141dxdy","dx:dy");
  _ntuples["Tracker142dxdy"] = new TNtuple("Tracker142dxdy","Tracker142dxdy","dx:dy");
  _ntuples["Tracker150dxdy"] = new TNtuple("Tracker150dxdy","Tracker150dxdy","dx:dy");
  _ntuples["Tracker151dxdy"] = new TNtuple("Tracker151dxdy","Tracker151dxdy","dx:dy");
  _ntuples["Tracker152dxdy"] = new TNtuple("Tracker152dxdy","Tracker152dxdy","dx:dy");

  _ntuples["Tracker110dpxdpy"] = new TNtuple("Tracker110dpxdpy","Tracker110dpxdpy","dpx:dpy");
  _ntuples["Tracker111dpxdpy"] = new TNtuple("Tracker111dpxdpy","Tracker111dpxdpy","dpx:dpy");
  _ntuples["Tracker112dpxdpy"] = new TNtuple("Tracker112dpxdpy","Tracker112dpxdpy","dpx:dpy");
  _ntuples["Tracker120dpxdpy"] = new TNtuple("Tracker120dpxdpy","Tracker120dpxdpy","dpx:dpy");
  _ntuples["Tracker121dpxdpy"] = new TNtuple("Tracker121dpxdpy","Tracker121dpxdpy","dpx:dpy");
  _ntuples["Tracker122dpxdpy"] = new TNtuple("Tracker122dpxdpy","Tracker122dpxdpy","dpx:dpy");
  _ntuples["Tracker130dpxdpy"] = new TNtuple("Tracker130dpxdpy","Tracker130dpxdpy","dpx:dpy");
  _ntuples["Tracker131dpxdpy"] = new TNtuple("Tracker131dpxdpy","Tracker131dpxdpy","dpx:dpy");
  _ntuples["Tracker132dpxdpy"] = new TNtuple("Tracker132dpxdpy","Tracker132dpxdpy","dpx:dpy");
  _ntuples["Tracker140dpxdpy"] = new TNtuple("Tracker140dpxdpy","Tracker140dpxdpy","dpx:dpy");
  _ntuples["Tracker141dpxdpy"] = new TNtuple("Tracker141dpxdpy","Tracker141dpxdpy","dpx:dpy");
  _ntuples["Tracker142dpxdpy"] = new TNtuple("Tracker142dpxdpy","Tracker142dpxdpy","dpx:dpy");
  _ntuples["Tracker150dpxdpy"] = new TNtuple("Tracker150dpxdpy","Tracker150dpxdpy","dpx:dpy");
  _ntuples["Tracker151dpxdpy"] = new TNtuple("Tracker151dpxdpy","Tracker151dpxdpy","dpx:dpy");
  _ntuples["Tracker152dpxdpy"] = new TNtuple("Tracker152dpxdpy","Tracker152dpxdpy","dpx:dpy");

  _ntuples["TrackerPropdxdy"] = new TNtuple("TrackerPropdxdy","TrackerPropdxdy","dx:dy");
  _ntuples["TrackerPropdpxdpy"] = new TNtuple("TrackerPropdpxdpy","TrackerPropdpxdpy","dpx:dpy");
  
  _ntuples["TOF1dxdy"] = new TNtuple("TOF1dxdy","TOF1dxdy","dx:dy");
  _ntuples["TOF2dxdy"] = new TNtuple("TOF2dxdy","TOF2dxdy","dx:dy");
  _ntuples["EMRdxerrx"] = new TNtuple("EMRdxerrx","EMRdxerrx","dx:errx");
  _ntuples["EMRdyerry"] = new TNtuple("EMRdyerry","EMRdyerry","dy:erry");

  _ntuples["TOF1offMCxy"] = new TNtuple("TOF1offMCxy","TOF1offSlabsxy","x:y");
  _ntuples["TOF2offMCxy"] = new TNtuple("TOF2offMCxy","TOF2offSlabsxy","x:y");
  try {
    configJSON = JsonWrapper::StringToJson(aJsonConfigDocument);
    return true;
  } catch (Exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, mClassname);
  } catch (std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, mClassname);
  }
  return false;
}

std::string ReduceCppReconTesting::process(std::string document) {
  JsonCppSpillConverter json2cppconverter;
  Json::Value *data_json = NULL;
  MAUS::Data *data_cpp = NULL;
  Json::Value imported_json = JsonWrapper::StringToJson(document);
  data_json = new Json::Value(imported_json);
  data_cpp = json2cppconverter(data_json);
  _spill = data_cpp->GetSpill();

  if (_spill) {
    ReconEventPArray* recon_events = _spill->GetReconEvents();
    MCEventPArray* mc_events = _spill->GetMCEvents();
    if (recon_events and mc_events) {
      MCEvent* mc_event = mc_events->at(0);
      ReconEvent* recon_event = recon_events->at(0);
      GlobalEvent* global_event = recon_event->GetGlobalEvent();
      
      // We make the assumption that there is exactly one track or spacepoint per detector
      MAUS::DataStructure::Global::Track* tracker0_track = 0;
      MAUS::DataStructure::Global::Track* tracker1_track = 0;
      MAUS::DataStructure::Global::Track* emr_track = 0;
      MAUS::DataStructure::Global::SpacePoint* tof0_sp = 0;
      MAUS::DataStructure::Global::SpacePoint* tof1_sp = 0;
      MAUS::DataStructure::Global::SpacePoint* tof2_sp = 0;
      MAUS::DataStructure::Global::SpacePoint* kl_sp = 0;

      MAUS::DataStructure::Global::TrackPArray* imported_tracks = global_event->get_tracks();
      MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
      for (imported_track_iter = imported_tracks->begin();
           imported_track_iter != imported_tracks->end();
           ++imported_track_iter) {
        if ((*imported_track_iter)->get_mapper_name() == "MapCppGlobalReconImport") {
          if ((*imported_track_iter)->HasDetector(MAUS::DataStructure::Global::kTracker0)) {
            tracker0_track = (*imported_track_iter);
          }
          if ((*imported_track_iter)->HasDetector(MAUS::DataStructure::Global::kTracker1)) {
            tracker1_track = (*imported_track_iter);
          }
          if ((*imported_track_iter)->HasDetector(MAUS::DataStructure::Global::kEMR)) {
            if ((*imported_track_iter)->get_emr_range_primary() > 0.0001) {
              emr_track = (*imported_track_iter);
            }
          }
        }
      }
      std::vector<MAUS::DataStructure::Global::SpacePoint*>* imported_spacepoints = global_event->get_space_points();
      std::vector<MAUS::DataStructure::Global::SpacePoint*>::iterator imported_sp_iter;
      for (imported_sp_iter = imported_spacepoints->begin();
           imported_sp_iter != imported_spacepoints->end();
           ++imported_sp_iter) {
        if ((*imported_sp_iter)->get_detector() == MAUS::DataStructure::Global::kTOF0) {
          tof0_sp = (*imported_sp_iter);
        }
        if ((*imported_sp_iter)->get_detector() == MAUS::DataStructure::Global::kTOF1) {
          tof1_sp = (*imported_sp_iter);
        }
        if ((*imported_sp_iter)->get_detector() == MAUS::DataStructure::Global::kTOF2) {
          tof2_sp = (*imported_sp_iter);
        }
        if ((*imported_sp_iter)->get_detector() == MAUS::DataStructure::Global::kCalorimeter) {
          kl_sp = (*imported_sp_iter);
        }
      }

      // Test RK4 on Tracker 1
      BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
      SciFiHit* tracker112_hit = MCTruthTools::GetTrackerPlaneHit(mc_event, 1, 1, 2);
      SciFiHit* tracker150_hit = MCTruthTools::GetTrackerPlaneHit(mc_event, 1, 5, 0);
      
      if (tracker112_hit and tracker150_hit) {
        std::cerr << "Station 1 " << tracker112_hit->GetPosition().X() << " " << tracker112_hit->GetPosition().Y() << " " << tracker112_hit->GetPosition().Z() << " "
                  << tracker112_hit->GetEnergy() << " " << tracker112_hit->GetMomentum().X() << " " << tracker112_hit->GetMomentum().Y() << " " << tracker112_hit->GetMomentum().Z() << "\n";
        std::cerr << "Station 5 " << tracker150_hit->GetPosition().X() << " " << tracker150_hit->GetPosition().Y() << " " << tracker150_hit->GetPosition().Z() << " "
                  << tracker150_hit->GetEnergy() << " " << tracker150_hit->GetMomentum().X() << " " << tracker150_hit->GetMomentum().Y() << " " << tracker150_hit->GetMomentum().Z() << "\n";
        double x_1[] = {0., tracker112_hit->GetPosition().X(), tracker112_hit->GetPosition().Y(), tracker112_hit->GetPosition().Z(),
                        tracker112_hit->GetEnergy(), tracker112_hit->GetMomentum().X(), tracker112_hit->GetMomentum().Y(), tracker112_hit->GetMomentum().Z()};
        BTTracker::integrate(tracker150_hit->GetPosition().Z(), x_1, field, BTTracker::z, 10.0, 1);
        std::cerr << "Station5P " << x_1[1] << " " << x_1[2] << " " << x_1[3] << " " << x_1[4] << " " << x_1[5] << " " << x_1[6] << " " << x_1[7] << "\n";
        _ntuples.at("TrackerPropdxdy")->Fill(std::fabs(x_1[1]-tracker150_hit->GetPosition().X()), std::fabs(x_1[2]-tracker150_hit->GetPosition().Y()));
        _ntuples.at("TrackerPropdpxdpy")->Fill(std::fabs(x_1[5]-tracker150_hit->GetMomentum().X()), std::fabs(x_1[6]-tracker150_hit->GetMomentum().Y()));
      }

      // Tracker 1
      if (tracker1_track) {
        std::vector<const MAUS::DataStructure::Global::TrackPoint*> tracker1_tps =
            tracker1_track->GetTrackPoints();
        for (size_t i = 0; i < tracker1_tps.size(); i++) {
          std::vector<int> tracker_planes = GlobalTools::GetTrackerPlane(tracker1_tps[i]);
          TLorentzVector position = tracker1_tps[i]->get_position();
          TLorentzVector momentum = tracker1_tps[i]->get_momentum();
          SciFiHit* tracker_hit = MCTruthTools::GetTrackerPlaneHit(mc_event, 1, tracker_planes[1], tracker_planes[2]);
          if (tracker_hit) {
            std::string station = static_cast<std::ostringstream*>(&(std::ostringstream() << tracker_planes[1]))->str();
            std::string plane = static_cast<std::ostringstream*>(&(std::ostringstream() << tracker_planes[2]))->str();
            std::string xy = "Tracker1" + station + plane + "xy";
            std::string dxdy = "Tracker1" + station + plane + "dxdy";
            std::string dpxdpy = "Tracker1" + station + plane + "dpxdpy";
            _ntuples.at(xy)->Fill(position.X(), position.Y());
            _ntuples.at(dxdy)->Fill(std::fabs(position.X() - tracker_hit->GetPosition().X()), std::fabs(position.Y() - tracker_hit->GetPosition().Y()));
            _ntuples.at(dpxdpy)->Fill(std::fabs(momentum.X() - tracker_hit->GetMomentum().X()), std::fabs(momentum.Y() - tracker_hit->GetMomentum().Y()));
          }
        }
      }
      
      // TOFs
      if (tof1_sp) {
        TLorentzVector tof1_pos = tof1_sp->get_position();
        TOFHitArray* tof1_hits = MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF1);
        TOFHit tof1_hit = MCTruthTools::GetNearestZHit(tof1_hits, tof1_pos);
        _ntuples.at("TOF1dxdy")->Fill(tof1_pos.X() - tof1_hit.GetPosition().X(), tof1_pos.Y() - tof1_hit.GetPosition().Y());
        if (std::fabs(tof1_pos.X() - tof1_hit.GetPosition().X()) > 30 or std::fabs(tof1_pos.Y() - tof1_hit.GetPosition().Y()) > 30) {
          _ntuples.at("TOF1offMCxy")->Fill(tof1_hit.GetPosition().X(), tof1_hit.GetPosition().Y());
        }
      }
      if (tof2_sp) {
        TLorentzVector tof2_pos = tof2_sp->get_position();
        TOFHitArray* tof2_hits = MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF2);
        TOFHit tof2_hit = MCTruthTools::GetNearestZHit(tof2_hits, tof2_pos);
        _ntuples.at("TOF2dxdy")->Fill(tof2_pos.X() - tof2_hit.GetPosition().X(), tof2_pos.Y() - tof2_hit.GetPosition().Y());
        if (std::fabs(tof2_pos.X() - tof2_hit.GetPosition().X()) > 30 or std::fabs(tof2_pos.Y() - tof2_hit.GetPosition().Y()) > 30) {
          _ntuples.at("TOF2offMCxy")->Fill(tof2_hit.GetPosition().X(), tof2_hit.GetPosition().Y());
        }
      }

      // EMR
      if (emr_track) {
        std::vector<const MAUS::DataStructure::Global::TrackPoint*> emr_tps =
            emr_track->GetTrackPoints();
        if (emr_tps.size() > 0) {
          const MAUS::DataStructure::Global::TrackPoint* emr_tp1 = emr_tps.at(0);
          TLorentzVector emr_pos = emr_tp1->get_position();
          TLorentzVector emr_pos_err = emr_tp1->get_position_error();
          EMRHitArray* emr_hits = mc_event->GetEMRHits();
          EMRHit emr_hit = MCTruthTools::GetNearestZHit(emr_hits, emr_pos);
          _ntuples.at("EMRdxerrx")->Fill(emr_pos.X() - emr_hit.GetPosition().X(), emr_tp1->get_position_error().X());
          _ntuples.at("EMRdyerry")->Fill(emr_pos.Y() - emr_hit.GetPosition().Y(), emr_tp1->get_position_error().Y());
          std::cerr << std::fabs(emr_pos.X() - emr_hit.GetPosition().X()) << " " << emr_tp1->get_position_error().X() << "EMRX\n";
        }
      }
    }
  }
  std::string output_document = JsonWrapper::JsonToString(*data_json);
  delete data_json;
  delete data_cpp;
  return output_document;
}

bool ReduceCppReconTesting::death()  {
  TDirectory *where = gDirectory;
  //~ TCanvas *c1 = new TCanvas("c1", "c1", 1000, 600);
  //~ c1->Divide(2,1);
  Json::Value *json = Globals::GetConfigurationCards();
  const char* ntuples_filename  = (*json)["Ntuples_filename"].asString().c_str();
  TFile *rootFile = new TFile(ntuples_filename,"RECREATE");
  _ntuples.at("EMRdxerrx")->Write("EMRdxerrx");
  _ntuples.at("EMRdyerry")->Write("EMRdyerry");
  _ntuples.at("TOF1dxdy")->Write("TOF1dxdy");
  _ntuples.at("TOF2dxdy")->Write("TOF2dxdy");
  _ntuples.at("Tracker110xy")->Write("Tracker110xy");
  _ntuples.at("Tracker111xy")->Write("Tracker111xy");
  _ntuples.at("Tracker112xy")->Write("Tracker112xy");
  _ntuples.at("Tracker120xy")->Write("Tracker120xy");
  _ntuples.at("Tracker121xy")->Write("Tracker121xy");
  _ntuples.at("Tracker122xy")->Write("Tracker122xy");
  _ntuples.at("Tracker130xy")->Write("Tracker130xy");
  _ntuples.at("Tracker131xy")->Write("Tracker131xy");
  _ntuples.at("Tracker132xy")->Write("Tracker132xy");
  _ntuples.at("Tracker140xy")->Write("Tracker140xy");
  _ntuples.at("Tracker141xy")->Write("Tracker141xy");
  _ntuples.at("Tracker142xy")->Write("Tracker142xy");
  _ntuples.at("Tracker150xy")->Write("Tracker150xy");
  _ntuples.at("Tracker151xy")->Write("Tracker151xy");
  _ntuples.at("Tracker152xy")->Write("Tracker152xy");
  _ntuples.at("Tracker110dxdy")->Write("Tracker110dxdy");
  _ntuples.at("Tracker111dxdy")->Write("Tracker111dxdy");
  _ntuples.at("Tracker112dxdy")->Write("Tracker112dxdy");
  _ntuples.at("Tracker120dxdy")->Write("Tracker120dxdy");
  _ntuples.at("Tracker121dxdy")->Write("Tracker121dxdy");
  _ntuples.at("Tracker122dxdy")->Write("Tracker122dxdy");
  _ntuples.at("Tracker130dxdy")->Write("Tracker130dxdy");
  _ntuples.at("Tracker131dxdy")->Write("Tracker131dxdy");
  _ntuples.at("Tracker132dxdy")->Write("Tracker132dxdy");
  _ntuples.at("Tracker140dxdy")->Write("Tracker140dxdy");
  _ntuples.at("Tracker141dxdy")->Write("Tracker141dxdy");
  _ntuples.at("Tracker142dxdy")->Write("Tracker142dxdy");
  _ntuples.at("Tracker150dxdy")->Write("Tracker150dxdy");
  _ntuples.at("Tracker151dxdy")->Write("Tracker151dxdy");
  _ntuples.at("Tracker152dxdy")->Write("Tracker152dxdy");
  _ntuples.at("Tracker110dpxdpy")->Write("Tracker110dpxdpy");
  _ntuples.at("Tracker111dpxdpy")->Write("Tracker111dpxdpy");
  _ntuples.at("Tracker112dpxdpy")->Write("Tracker112dpxdpy");
  _ntuples.at("Tracker120dpxdpy")->Write("Tracker120dpxdpy");
  _ntuples.at("Tracker121dpxdpy")->Write("Tracker121dpxdpy");
  _ntuples.at("Tracker122dpxdpy")->Write("Tracker122dpxdpy");
  _ntuples.at("Tracker130dpxdpy")->Write("Tracker130dpxdpy");
  _ntuples.at("Tracker131dpxdpy")->Write("Tracker131dpxdpy");
  _ntuples.at("Tracker132dpxdpy")->Write("Tracker132dpxdpy");
  _ntuples.at("Tracker140dpxdpy")->Write("Tracker140dpxdpy");
  _ntuples.at("Tracker141dpxdpy")->Write("Tracker141dpxdpy");
  _ntuples.at("Tracker142dpxdpy")->Write("Tracker142dpxdpy");
  _ntuples.at("Tracker150dpxdpy")->Write("Tracker150dpxdpy");
  _ntuples.at("Tracker151dpxdpy")->Write("Tracker151dpxdpy");
  _ntuples.at("Tracker152dpxdpy")->Write("Tracker152dpxdpy");

  _ntuples.at("TrackerPropdxdy")->Write("TrackerPropdxdy");
  _ntuples.at("TrackerPropdpxdpy")->Write("TrackerPropdpxdpy");

  _ntuples.at("TOF1offMCxy")->Write("TOF1offMCxy");
  _ntuples.at("TOF2offMCxy")->Write("TOF2offMCxy");
  return true;
}

bool ReduceCppReconTesting::checkDetector(const
    MAUS::DataStructure::Global::TrackPoint* track_point, int min, int max) {
  int enum_key = static_cast<int>(track_point->get_detector());
  if ((enum_key >= min) and (enum_key <= max)) {
    return true;
  } else {
    return false;
  }
}

bool ReduceCppReconTesting::checkDetector(const
    MAUS::DataStructure::Global::SpacePoint* space_point, int min, int max) {
  int enum_key = static_cast<int>(space_point->get_detector());
  if ((enum_key >= min) and (enum_key <= max)) {
    return true;
  } else {
    return false;
  }
}



} // ~namespace MAUS
