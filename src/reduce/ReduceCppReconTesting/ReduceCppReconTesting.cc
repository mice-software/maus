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

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/reduce/ReduceCppReconTesting/ReduceCppReconTesting.hh"
#include "TFile.h"
#include "TH2.h"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Recon/Global/MCTruthTools.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"
#include "src/common_cpp/Utils/Globals.hh"

namespace MAUS {

void ReduceCppReconTesting::_birth(const std::string& json_config) {
  Json::Value config = JsonWrapper::StringToJson(json_config);
  _pairs["PropTOF0"] = new std::vector<std::pair<double,double> >;
  _pairs["PropTOF0Epz"] = new std::vector<std::pair<double,double> >;
  _pairs["PropTOF1"] = new std::vector<std::pair<double,double> >;
  _pairs["PropTOF2"] = new std::vector<std::pair<double,double> >;
  _pairs["PropKL"] = new std::vector<std::pair<double,double> >;
  _pairs["PropEMR"] = new std::vector<std::pair<double,double> >;
}

void ReduceCppReconTesting::_process(Data* data) {
  _spill = data->GetSpill();
  if (_spill) {
    BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
    TLorentzVector zerovector(0.0, 0.0, 0.0, 0.0);
    MCEventPArray* mc_events = _spill->GetMCEvents();
    if (mc_events) {
      MCEvent* mc_event = mc_events->at(0);

      SciFiHit* tracker052_hit = MCTruthTools::GetTrackerPlaneHit(mc_event, 0, 5, 2);
      SciFiHit* tracker152_hit = MCTruthTools::GetTrackerPlaneHit(mc_event, 1, 5, 2);

      if (tracker052_hit) {
        double x_tof0[] = {0., tracker052_hit->GetPosition().X(), tracker052_hit->GetPosition().Y(), tracker052_hit->GetPosition().Z(),
                           tracker052_hit->GetEnergy(), tracker052_hit->GetMomentum().X(), tracker052_hit->GetMomentum().Y(), tracker052_hit->GetMomentum().Z()};
        double x_tof0_copy[] = {x_tof0[0], x_tof0[1], x_tof0[2], x_tof0[3], x_tof0[4], x_tof0[5], x_tof0[6], x_tof0[7]};
        double x_tof1[] = {x_tof0[0], x_tof0[1], x_tof0[2], x_tof0[3], x_tof0[4], x_tof0[5], x_tof0[6], x_tof0[7]};

        TOFHitArray* tof0_hits = MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF0);
        if (tof0_hits->size() > 0) {
          TOFHit tof0_hit = MCTruthTools::GetNearestZHit(tof0_hits, zerovector);
          //~ std::cerr << "BEFORE: " << x_tof0[1] << " " << x_tof0[2] << " " << x_tof0[3] << " # " << x_tof0[4] << " " << x_tof0[5] << " " << x_tof0[6] << " " << x_tof0[7] << "\n"; 
          GlobalTools::propagate(x_tof0, tof0_hit.GetPosition().Z(), field, 10.0, MAUS::DataStructure::Global::kMuPlus);
          //~ GlobalTools::propagate(x_tof0_copy, tof0_hit.GetPosition().Z(), field, 1.0, MAUS::DataStructure::Global::kMuPlus, false);
          //~ std::cerr << "AFTER: " << x_tof0[1] << " " << x_tof0[2] << " " << x_tof0[3] << " # " << x_tof0[4] << " " << x_tof0[5] << " " << x_tof0[6] << " " << x_tof0[7] << "\n";
          //~ std::cerr << "NodEdx: " << x_tof0_copy[1] << " " << x_tof0_copy[2] << " " << x_tof0_copy[3] << " # " << x_tof0_copy[4] << " " << x_tof0_copy[5] << " " << x_tof0_copy[6] << " " << x_tof0_copy[7] << "\n";
          //~ std::cerr << "MC: " << tof0_hit.GetPosition().X() << " " << tof0_hit.GetPosition().Y() << " " << tof0_hit.GetPosition().Z() << " # "
                    //~ << tof0_hit.GetEnergy() << " " << tof0_hit.GetMomentum().X() << " " << tof0_hit.GetMomentum().Y() << " " << tof0_hit.GetMomentum().Z() << "\n";
                    if (std::fabs(x_tof0[2] - tof0_hit.GetPosition().Y()) > 5) {
std::cerr << x_tof0[1] << " " << x_tof0[2] << " " << x_tof0[3] << " # " << x_tof0[4] << " " << x_tof0[5] << " " << x_tof0[6] << " " << x_tof0[7] << "\n";
          std::cerr << "MC: " << tof0_hit.GetPosition().X() << " " << tof0_hit.GetPosition().Y() << " " << tof0_hit.GetPosition().Z() << " # "
                    << tof0_hit.GetEnergy() << " " << tof0_hit.GetMomentum().X() << " " << tof0_hit.GetMomentum().Y() << " " << tof0_hit.GetMomentum().Z() << "\n";
                    }
          _pairs.at("PropTOF0")->push_back(std::make_pair(std::fabs(x_tof0[1] - tof0_hit.GetPosition().X()), std::fabs(x_tof0[2] - tof0_hit.GetPosition().Y())));
          _pairs.at("PropTOF0Epz")->push_back(std::make_pair(std::fabs(x_tof0[4] - tof0_hit.GetEnergy()), std::fabs(x_tof0[7] - tof0_hit.GetMomentum().Z())));

          //~ std::cerr << "STEP BY STEP: \n";
          //~ for (size_t i = 0; i < 840; i++) {
            //~ GlobalTools::propagate(x_tof0_copy, x_tof0_copy[3] - 10.0, field, 1.0, MAUS::DataStructure::Global::kMuPlus);
            //~ std::cerr << x_tof0_copy[3] << " " << x_tof0_copy[1] << " " << x_tof0_copy[2] << "\n";
          //~ }

          //~ VirtualHitArray* virtual_hits = mc_event->GetVirtualHits();
          //~ std::cerr << "VIRTUAL HITS (" << virtual_hits->size() << ")\n";
          //~ for (size_t i = 0; i < virtual_hits->size(); i++) {
            //~ std::cerr << virtual_hits->at(i).GetPosition().Z() << " " << virtual_hits->at(i).GetPosition().X() << " " << virtual_hits->at(i).GetPosition().Y() << "\n";
          //~ }
        }
        
        TOFHitArray* tof1_hits = MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF1);
        if (tof1_hits->size() > 0) {
          TOFHit tof1_hit = MCTruthTools::GetNearestZHit(tof1_hits, zerovector);
          GlobalTools::propagate(x_tof1, tof1_hit.GetPosition().Z(), field, 10.0, MAUS::DataStructure::Global::kMuPlus);
          _pairs.at("PropTOF1")->push_back(std::make_pair(std::fabs(x_tof1[1] - tof1_hit.GetPosition().X()), std::fabs(x_tof1[2] - tof1_hit.GetPosition().Y())));
        }
      }


      if (tracker152_hit) {
        double x_tof2[] = {0., tracker152_hit->GetPosition().X(), tracker152_hit->GetPosition().Y(), tracker152_hit->GetPosition().Z(),
                           tracker152_hit->GetEnergy(), tracker152_hit->GetMomentum().X(), tracker152_hit->GetMomentum().Y(), tracker152_hit->GetMomentum().Z()};
        double x_kl[] = {x_tof2[0], x_tof2[1], x_tof2[2], x_tof2[3], x_tof2[4], x_tof2[5], x_tof2[6], x_tof2[7]};
        double x_emr[] = {x_tof2[0], x_tof2[1], x_tof2[2], x_tof2[3], x_tof2[4], x_tof2[5], x_tof2[6], x_tof2[7]};

        TOFHitArray* tof2_hits = MCTruthTools::GetTOFHits(mc_event, MAUS::DataStructure::Global::kTOF2);
        if (tof2_hits->size() > 0) {
          TOFHit tof2_hit = MCTruthTools::GetNearestZHit(tof2_hits, zerovector);
          GlobalTools::propagate(x_tof2, tof2_hit.GetPosition().Z(), field, 10.0, MAUS::DataStructure::Global::kMuPlus);
          _pairs.at("PropTOF2")->push_back(std::make_pair(std::fabs(x_tof2[1] - tof2_hit.GetPosition().X()), std::fabs(x_tof2[2] - tof2_hit.GetPosition().Y())));
        }
        
        KLHitArray* kl_hits = mc_event->GetKLHits();
        if (kl_hits->size() > 0) {
          KLHit kl_hit = MCTruthTools::GetNearestZHit(kl_hits, zerovector);
          GlobalTools::propagate(x_kl, kl_hit.GetPosition().Z(), field, 10.0, MAUS::DataStructure::Global::kMuPlus);
          _pairs.at("PropKL")->push_back(std::make_pair(std::fabs(x_kl[1] - kl_hit.GetPosition().X()), std::fabs(x_kl[2] - kl_hit.GetPosition().Y())));
        }
        
        EMRHitArray* emr_hits = mc_event->GetEMRHits();
        if (emr_hits->size() > 0) {
          EMRHit emr_hit = MCTruthTools::GetNearestZHit(emr_hits, zerovector);
          GlobalTools::propagate(x_emr, emr_hit.GetPosition().Z(), field, 10.0, MAUS::DataStructure::Global::kMuPlus);
          _pairs.at("PropEMR")->push_back(std::make_pair(std::fabs(x_emr[1] - emr_hit.GetPosition().X()), std::fabs(x_emr[2] - emr_hit.GetPosition().Y())));
        }
      }
    }
  }
}

void ReduceCppReconTesting::_death()  {

  TDirectory *where = gDirectory;
  Json::Value *json = Globals::GetConfigurationCards();
  const char* ntuples_filename  = (*json)["Ntuples_filename"].asString().c_str();
  TFile *rootFile = new TFile(ntuples_filename,"RECREATE");
  
  TNtuple* prop_tof0 = new TNtuple("TOF0", "TOF0", "dx:dy");
  for (size_t i = 0; i < _pairs.at("PropTOF0")->size(); i++) {
    prop_tof0->Fill(_pairs.at("PropTOF0")->at(i).first, _pairs.at("PropTOF0")->at(i).second);
  }
  prop_tof0->Write("PropTOF0");

  TNtuple* prop_tof0Epz = new TNtuple("TOF0Epz", "TOF0Epz", "dE:dpz");
  for (size_t i = 0; i < _pairs.at("PropTOF0Epz")->size(); i++) {
    prop_tof0Epz->Fill(_pairs.at("PropTOF0Epz")->at(i).first, _pairs.at("PropTOF0Epz")->at(i).second);
  }
  prop_tof0Epz->Write("PropTOF0Epz");
  
  TNtuple* prop_tof1 = new TNtuple("TOF1", "TOF1", "dx:dy");
  for (size_t i = 0; i < _pairs.at("PropTOF1")->size(); i++) {
    prop_tof1->Fill(_pairs.at("PropTOF1")->at(i).first, _pairs.at("PropTOF1")->at(i).second);
  }
  prop_tof1->Write("PropTOF1");
  
  TNtuple* prop_tof2 = new TNtuple("TOF2", "TOF2", "dx:dy");
  for (size_t i = 0; i < _pairs.at("PropTOF2")->size(); i++) {
    prop_tof2->Fill(_pairs.at("PropTOF2")->at(i).first, _pairs.at("PropTOF2")->at(i).second);
  }
  prop_tof2->Write("PropTOF2");
  
  TNtuple* prop_kl = new TNtuple("KL", "KL", "dx:dy");
  for (size_t i = 0; i < _pairs.at("PropKL")->size(); i++) {
    prop_kl->Fill(_pairs.at("PropKL")->at(i).first, _pairs.at("PropKL")->at(i).second);
  }
  prop_kl->Write("PropKL");
  
  TNtuple* prop_emr = new TNtuple("EMR", "EMR", "dx:dy");
  for (size_t i = 0; i < _pairs.at("PropEMR")->size(); i++) {
    prop_emr->Fill(_pairs.at("PropEMR")->at(i).first, _pairs.at("PropEMR")->at(i).second);
  }
  prop_emr->Write("PropEMR");

}

//~ bool ReduceCppReconTesting::checkDetector(const
    //~ MAUS::DataStructure::Global::TrackPoint* track_point, int min, int max) {
  //~ int enum_key = static_cast<int>(track_point->get_detector());
  //~ if ((enum_key >= min) and (enum_key <= max)) {
    //~ return true;
  //~ } else {
    //~ return false;
  //~ }
//~ }

//~ bool ReduceCppReconTesting::checkDetector(const
    //~ MAUS::DataStructure::Global::SpacePoint* space_point, int min, int max) {
  //~ int enum_key = static_cast<int>(space_point->get_detector());
  //~ if ((enum_key >= min) and (enum_key <= max)) {
    //~ return true;
  //~ } else {
    //~ return false;
  //~ }
//~ }

PyMODINIT_FUNC init_ReduceCppReconTesting(void) {
  PyWrapReduceBase<ReduceCppReconTesting>::PyWrapReduceBaseModInit
                              ("ReduceCppReconTesting",
                               "","","","");
}


} // ~namespace MAUS
