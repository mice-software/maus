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

// ROOT headers
#include "TVirtualPad.h"

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

SciFiDisplayMomentumResidualsPR::SciFiDisplayMomentumResidualsPR() : mOf1(NULL),
                                                                     mTree(NULL),
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
  if (mResidualPtT1) delete mResidualPtT1;
  if (mResidualPzT1) delete mResidualPzT1;
  if (mResidualPzT1Log) delete mResidualPzT1Log;
  if (mResidualPtT2) delete mResidualPtT2;
  if (mResidualPzT2) delete mResidualPzT2;
  if (mResidualPzT2Log) delete mResidualPzT2Log;
  if (mOf1) {
    mOf1->Close();
    delete mOf1;
  }
}

void SciFiDisplayMomentumResidualsPR::Fill() {
  mOf1->cd();
  if (mTree) {
    // Loop over the data for each track in the spill and fill the tree with reduced data
    for ( size_t i = 0; i < GetData()->mDataPR.size(); ++i ) {
      mTrackData = GetData()->mDataPR[i];
      mTree->Fill();
      int q = mTrackData.Charge;
      if (mTrackData.TrackerNumber == 0) {
        mResidualPtT1->Fill(mTrackData.PtRec - mTrackData.PtMc);
        mResidualPzT1->Fill(q*mTrackData.PzRec + mTrackData.PzMc);
        mResidualPzT1Log->Fill(q*mTrackData.PzRec + mTrackData.PzMc);
      } else if (mTrackData.TrackerNumber == 1) {
        mResidualPtT2->Fill(mTrackData.PtRec - mTrackData.PtMc);
        mResidualPzT2->Fill(mTrackData.PzRec - mTrackData.PzMc);
        mResidualPzT2Log->Fill(mTrackData.PzRec - mTrackData.PzMc);
      }
    }
  } else {
    std::cerr << "SciFiDisplayMomentumResidualsPR: Warning, local ROOT Tree not setup\n";
  }
}

SciFiDataMomentumPR* SciFiDisplayMomentumResidualsPR::GetData() {
  return GetDataTemplate();
}

SciFiDataBase* SciFiDisplayMomentumResidualsPR::MakeDataObject() {
  return MakeDataObjectTemplate();
}

void SciFiDisplayMomentumResidualsPR::Plot(TCanvas* aCanvas) {
  // If canvas if passed in use it, otherwise initialise the member canvas
  mOf1->cd();
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    lCanvas = aCanvas;
  } else {
    if (mCanvas) {
      mCanvas->Clear();
    } else {
      mCanvas = new TCanvas("cMomResidualsPR", "Momentum Residuals PR");
    }
    lCanvas = mCanvas;
  }
  // Prepare the TCanvas
  lCanvas->cd();
  lCanvas->Divide(3, 2);
  // Draw the histograms
  lCanvas->cd(1);
  mResidualPtT1->Draw();
  lCanvas->cd(2);
  mResidualPzT1->Draw();
  TVirtualPad* pad = lCanvas->cd(3);
  pad->SetLogy();
  mResidualPzT1Log->Draw();
  lCanvas->cd(4);
  mResidualPtT2->Draw();
  lCanvas->cd(5);
  mResidualPzT2->Draw();
  pad = lCanvas->cd(6);
  pad->SetLogy();
  mResidualPzT2Log->Draw();
}

void SciFiDisplayMomentumResidualsPR::Save() {
  if (mOf1) {
    mOf1->cd();
    if (mTree) mTree->Write();
    if (mResidualPtT1) mResidualPtT1->Write();
    if (mResidualPzT1) mResidualPzT1->Write();
    if (mResidualPzT1Log) mResidualPzT1Log->Write();
    if (mResidualPtT2) mResidualPtT2->Write();
    if (mResidualPzT2) mResidualPzT2->Write();
    if (mResidualPzT2Log) mResidualPzT2Log->Write();
    if (mCanvas) mCanvas->Write();
    mOf1->Close();
  }
}

SciFiDataBase* SciFiDisplayMomentumResidualsPR::SetData(SciFiDataBase* data) {
  return SetDataTemplate(data);
}

SciFiDataBase* SciFiDisplayMomentumResidualsPR::SetUp() {
  SetUpData();
  SetUpRoot();
  return mSpillData;
}

bool SciFiDisplayMomentumResidualsPR::SetUpRoot() {
  // Setup the output TFile
  mOf1 = new TFile("SciFiDisplayMomentumResidualsPROutput.root", "recreate");
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
  mResidualPtT1->GetXaxis()->SetTitle("p_{T} - p_{T}^{MC} (MeV/c)");
  mResidualPzT1 = new TH1D("ResidualPzT1", "T1 p_{z} Residual", res_n_bins, -30, 30);
  mResidualPzT1->GetXaxis()->SetTitle("p_{z} - p_{z}^{MC} (MeV/c)");
  mResidualPzT1Log = new TH1D("ResidualPzT1Log", "T1 p_{z} Residual", res_n_bins, -500, 500);
  mResidualPzT1Log->GetXaxis()->SetTitle("p_{z} p_{z}^{MC} (MeV/c)");
  mResidualPtT2 = new TH1D("ResidualPtT2", "T2 p_{t} Residual", res_n_bins, -5, 5);
  mResidualPtT2->GetXaxis()->SetTitle("p_{T} - p_{T}^{MC}(MeV/c)");
  mResidualPzT2 = new TH1D("ResidualPzT2", "T2 p_{z} Residual", res_n_bins, -30, 30);
  mResidualPzT2->GetXaxis()->SetTitle("p_{z} - p_{z}^{MC} (MeV/c)");
  mResidualPzT2Log = new TH1D("ResidualPzT2Log", "T2 p_{z} Residual", res_n_bins, -500, 500);
  mResidualPzT2Log->GetXaxis()->SetTitle("p_{z} - p_{z}^{MC} (MeV/c)");
  return true;
}
} // ~namespace MAUS
