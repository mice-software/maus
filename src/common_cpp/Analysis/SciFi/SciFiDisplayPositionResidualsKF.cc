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
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayPositionResidualsKF.hh"

namespace MAUS {

SciFiDisplayPositionResidualsKF::SciFiDisplayPositionResidualsKF() : mNBins(100),
		                                                                 mLowerLimit(-2),
		                                                                 mUpperLimit(2),
		                                                                 mOf1(NULL),
                                                                     mTree(NULL),
                                                                     mResidualXT1(NULL),
																	                                   mResidualYT1(NULL),
                                                                     mResidualZT1(NULL),
                                                                     mResidualXT2(NULL),
                                                                     mResidualYT2(NULL),
																                                  	 mResidualZT2(NULL) {
  // Do nothing
}

SciFiDisplayPositionResidualsKF::~SciFiDisplayPositionResidualsKF() {
  if (mTree) delete mTree;
  if (mResidualXT1) delete mResidualXT1;
  if (mResidualYT1) delete mResidualYT1;
  if (mResidualZT1) delete mResidualZT1;
  if (mResidualXT2) delete mResidualXT2;
  if (mResidualYT2) delete mResidualYT2;
  if (mResidualZT2) delete mResidualZT2;
  if (mOf1) {
    mOf1->Close();
    delete mOf1;
  }
}

void SciFiDisplayPositionResidualsKF::Fill() {
  if (mOf1) {
	mOf1->cd();
  } else {
    std::cerr << "SciFiDisplayPositionResidualsKF: Warning, local ROOT TFile not setup\n";
  }
  if (mTree) {
    // Loop over the data for each track in the spill and fill the tree with reduced data
    for ( size_t i = 0; i < GetData()->mDataKF.size(); ++i ) {
      mTrackData = GetData()->mDataKF[i];
      mTree->Fill();
      // int q = mTrackData.Charge;
      if (mTrackData.TrackerNumber == 0) {
        mResidualXT1->Fill(mTrackData.xRec - mTrackData.xMc);
        mResidualYT1->Fill(mTrackData.yRec - mTrackData.yMc);
        mResidualZT1->Fill(mTrackData.zRec - mTrackData.zMc);
      } else if (mTrackData.TrackerNumber == 1) {
        mResidualXT2->Fill(mTrackData.xRec - mTrackData.xMc);
        mResidualYT2->Fill(mTrackData.yRec - mTrackData.yMc);
        mResidualZT2->Fill(mTrackData.zRec - mTrackData.zMc);
      }
    }
  } else {
    std::cerr << "SciFiDisplayPositionResidualsKF: Warning, local ROOT Tree not setup\n";
  }
}

SciFiDataKF* SciFiDisplayPositionResidualsKF::GetData() {
  return GetDataTemplate();
}

SciFiDataBase* SciFiDisplayPositionResidualsKF::MakeDataObject() {
  return MakeDataObjectTemplate();
}

void SciFiDisplayPositionResidualsKF::Plot(TCanvas* aCanvas) {
  if (mOf1) {
	mOf1->cd();
  } else {
	std::cerr << "SciFiDisplayPositionResidualsKF: Warning, local ROOT TFile not setup\n";
  }
  // If canvas if passed in use it, otherwise initialise the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    lCanvas = aCanvas;
  } else {
    if (mCanvas) {
      mCanvas->Clear();
    } else {
      mCanvas = new TCanvas("cPosResidualsKF", "Position Residuals KF");
    }
    lCanvas = mCanvas;
  }
  // Prepare the TCanvas
  lCanvas->cd();
  lCanvas->Divide(3, 2);
  // Draw the histograms
  lCanvas->cd(1);
  mResidualXT1->Draw();
  lCanvas->cd(2);
  mResidualYT1->Draw();
  lCanvas->cd(3);
  mResidualZT1->Draw();
  lCanvas->cd(4);
  mResidualXT2->Draw();
  lCanvas->cd(5);
  mResidualYT2->Draw();
  lCanvas->cd(6);
  mResidualZT2->Draw();
}

void SciFiDisplayPositionResidualsKF::Save() {
  if (mOf1) {
    mOf1->cd();
    if (mTree) mTree->Write();
    if (mResidualXT1) mResidualXT1->Write();
    if (mResidualYT1) mResidualYT1->Write();
    if (mResidualZT1) mResidualZT1->Write();
    if (mResidualXT2) mResidualXT2->Write();
    if (mResidualYT2) mResidualYT2->Write();
    if (mResidualZT2) mResidualZT2->Write();
    if (mCanvas) mCanvas->Write();
    mOf1->Close();
  } else {
	std::cerr << "SciFiDisplayPositionResidualsKF: Warning, local ROOT TFile not setup\n";
  }
}

SciFiDataBase* SciFiDisplayPositionResidualsKF::SetData(SciFiDataBase* data) {
  return SetDataTemplate(data);
}

SciFiDataBase* SciFiDisplayPositionResidualsKF::SetUp() {
  SetUpData();
  SetUpRoot();
  return mSpillData;
}

bool SciFiDisplayPositionResidualsKF::SetUpRoot() {
  // Setup the output TFile
  mOf1 = new TFile("SciFiDisplayPositionResidualsKFOutput.root", "recreate");
  // Set up the TTree
  mTree = new TTree("mTree", "SciFi Kalman Position Data");
  mTree->Branch("TrackerNumber", &mTrackData.TrackerNumber, "TrackerNumber/I");
  mTree->Branch("NDF", &mTrackData.NDF, "NDF/I");
  mTree->Branch("Charge", &mTrackData.Charge, "Charge/I");
  mTree->Branch("xMc", &mTrackData.xMc, "xMc/D");
  mTree->Branch("yMc", &mTrackData.yMc, "yMc/D");
  mTree->Branch("zMc", &mTrackData.zMc, "zMc/D");
  mTree->Branch("xRec", &mTrackData.xRec, "xRec/D");
  mTree->Branch("yRec", &mTrackData.yRec, "yRec/D");
  mTree->Branch("zRec", &mTrackData.zRec, "zRec/D");

  // Set up the residual histograms
  mResidualXT1 = new TH1D("ResidualXT1", "T1 X Residual", mNBins, -5, 5);
  mResidualXT1->GetXaxis()->SetTitle("X^{Rec} - X^{MC} (mm)");
  mResidualYT1 = new TH1D("ResidualYT1", "T1 Y Residual", mNBins, -5, 5);
  mResidualYT1->GetXaxis()->SetTitle("Y^{Rec}- Y^{MC} (mm)");
  mResidualZT1 = new TH1D("ResidualZT1", "T1 Z Residual", mNBins, -5, 5);
  mResidualZT1->GetXaxis()->SetTitle("Z^{Rec} - Z^{MC} (mm)");
  mResidualXT2 = new TH1D("ResidualXT2", "T2 X Residual", mNBins, -5, 5);
  mResidualXT2->GetXaxis()->SetTitle("X^{Rec} - X^{MC} (mm)");
  mResidualYT2 = new TH1D("ResidualYT2", "T2 Y Residual", mNBins, -5, 5);
  mResidualYT2->GetXaxis()->SetTitle("Y^{Rec}- Y^{MC} (mm)");
  mResidualZT2 = new TH1D("ResidualZT2", "T2 Z Residual", mNBins, -5, 5);
  mResidualZT2->GetXaxis()->SetTitle("Z^{Rec} - Z^{MC} (mm)");

  return true;
}
} // ~namespace MAUS
