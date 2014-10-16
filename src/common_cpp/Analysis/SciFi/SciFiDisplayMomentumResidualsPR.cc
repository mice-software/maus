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


// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResidualsPR.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

SciFiDisplayMomentumResidualsPR::SciFiDisplayMomentumResidualsPR() : mTree(NULL),
                                                                     mSpillData(NULL),
                                                                     mResidualPtT1(NULL),
                                                                     mResidualPzT1(NULL),
                                                                     mResidualPzT1Log(NULL),
                                                                     mResidualPtT2(NULL),
                                                                     mResidualPzT2(NULL),
                                                                     mResidualPzT2Log(NULL) {
  // Do nothing
}

SciFiDisplayMomentumResidualsPR::~SciFiDisplayMomentumResidualsPR() {
  if (mTree) delete mTree;
  if (mTrackData) delete mTrackData;
  if (mResidualPtT1) delete mResidualPtT1;
  if (mResidualPzT1) delete mResidualPzT1;
  if (mResidualPzT1Log) delete mResidualPzT1Log;
  if (mResidualPtT2) delete mResidualPtT2;
  if (mResidualPzT2) delete mResidualPzT2;
  if (mResidualPzT2Log) delete mResidualPzT2Log;
}

void SciFiDisplayMomentumResidualsPR::Plot(TCanvas* aCanvas) {
  // If canvas if passed in use it, otherwise initialise the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    // std::cout << "Using input canvas at " << aCanvas << std::endl;
    lCanvas = aCanvas;
  } else {
    if (mCanvas) {
      // std::cout << "Using current local canvas at " << _Canvas << std::endl;
      mCanvas->Clear();
    } else {
      mCanvas = new TCanvas("cResiduals", "Momentum Residuals");
      // std::cout << "Creating new canvas at " << _Canvas << std::endl;
    }
    lCanvas = mCanvas;
  }

  lCanvas->cd();
  mTree->Scan("TrackerNumber");
  mTree->Draw("PtRec-PtMc");
  // mResidualPtT1->Draw();
}

SciFiDataBase* SciFiDisplayMomentumResidualsPR::SetUp() {
  SciFiDataBase* data = SetUpSciFiData();
  SetUpRoot();
  return data;
}

SciFiDataBase* SciFiDisplayMomentumResidualsPR::SetUpSciFiData() {
  mSpillData = new SciFiDataMomentumPR();
  return mSpillData;
}

bool SciFiDisplayMomentumResidualsPR::SetUpRoot() {
  if (mTrackData) {
    // Set up the TTree
    mTree = new TTree("mTree", "SciFi Momentum Data");
    mTree->Branch("TrackerNumber", &mTrackData.TrackerNumber, "TrackerNumber/I");
    mTree->Branch("NumberOfPoints", &mTrackData.NumberOfPoints, "NumberOfPoints/I");
    mTree->Branch("Charge", &mTrackData.Charge, "Charge/I");
    mTree->Branch("PtMc", &mTrackData.PtMc, "PtMc/D");
    mTree->Branch("PzMc", &mTrackData.PzMc, "PtMz/D");
    mTree->Branch("PtRec", &mTrackData.PtRec, "PtRec/D");
    mTree->Branch("PzRec", &mTrackData.PzRec, "PtRec/D");
    // Set up the residual histograms
    int res_n_bins = 100;
    mResidualPtT1 = new TH1D("ResidualPtT1", "T1 p_{t} Residual", res_n_bins, -5, 5);
    mResidualPtT1->GetXaxis()->SetTitle("p_{T}^{MC} - p_{T} (MeV/c)");
    mResidualPzT1 = new TH1D("ResidualPzT1", "T1 p_{z} Residual", res_n_bins, -30, 30);
    mResidualPzT1->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
    mResidualPzT1Log = new TH1D("ResidualPzT1Log", "T1 p_{z} Residual", res_n_bins, -500, 500);
    mResidualPzT1Log->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
    mResidualPtT2 = new TH1D("ResidualPtT2", "T2 p_{t} Residual", res_n_bins, -5, 5);
    mResidualPtT2->GetXaxis()->SetTitle("p_{T}^{MC} - p_{T} (MeV/c)");
    mResidualPzT2 = new TH1D("ResidualPzT2", "T2 p_{z} Residual", res_n_bins, -30, 30);
    mResidualPzT2->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
    mResidualPzT2Log = new TH1D("ResidualPzT2Log", "T2 p_{z} Residual", res_n_bins, -500, 500);
    mResidualPzT2Log->GetXaxis()->SetTitle("p_{z}^{MC} - p_{z} (MeV/c)");
    return true;
  } else {
    std::cerr << "SciFiDisplayMomentumResidualsPR: mSpillData not set, please set and try again\n";
    return false;
  }
}

void SciFiDisplayMomentumResidualsPR::Update() {
  std::cerr << "SciFiDisplayMomentumResidualsPR: Updating display\n";
  if (mTree) {
    // Loop over the data for each track in the spill and fill the tree with reduced data
    for ( int i = 0; i < mSpillData->mData.size(); ++i ) {
      mTrackData = *(mSpillData->mData[i]);
      mTree->Fill();
    }
  } else {
    std::cerr << "SciFiDisplayMomentumResidualsPR: Warning, local ROOT Tree not setup\n";
  }
}
} // ~namespace MAUS
