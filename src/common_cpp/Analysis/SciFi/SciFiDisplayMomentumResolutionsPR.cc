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

// C++ headers
#include <vector>
#include <sstream>

// ROOT headers
#include "TAxis.h"
#include "TH1D.h"
#include "TF1.h"

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResolutionsPR.hh"
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

SciFiDisplayMomentumResolutionsPR::SciFiDisplayMomentumResolutionsPR() : mOf1(NULL),
                                                                         mTree(NULL),
                                                                         mT1PtResolPtMC(NULL),
                                                                         mT2PtResolPtMC(NULL),
                                                                         mT1PzResolPtMC(NULL),
                                                                         mT2PzResolPtMC(NULL),
                                                                         mT1PtResolPzMC(NULL),
                                                                         mT2PtResolPzMC(NULL),
                                                                         mT1PzResolPzMC(NULL),
                                                                         mT2PzResolPzMC(NULL),
                                                                         mNBinsPt(200),
                                                                         mNBinsPz(200),
                                                                         mNPoints(9),
                                                                         mPtFitMin(-50),
                                                                         mPtFitMax(50),
                                                                         mPzFitMin(-50),
                                                                         mPzFitMax(50),
                                                                         mLowerBoundPtMC(0.0),
                                                                         mUpperBoundPtMC(90.0),
                                                                         mUpperBoundPzMC(250.0),
                                                                         mLowerBoundPzMC(150.0),
                                                                         mCutPzRec(500.0) {
  // Do nothing
}

SciFiDisplayMomentumResolutionsPR::~SciFiDisplayMomentumResolutionsPR() {
  if (mTree) delete mTree;
  if (mSpillData) delete mSpillData;
  if (mT1PtResolPtMC) delete mT1PtResolPtMC;
  if (mT2PtResolPtMC) delete mT2PtResolPtMC;
  if (mT1PzResolPtMC) delete mT1PzResolPtMC;
  if (mT2PzResolPtMC) delete mT2PzResolPtMC;
  if (mT1PzResolPzMC) delete mT1PzResolPzMC;
  if (mT2PzResolPzMC) delete mT2PzResolPzMC;
  if (mOf1) {
    mOf1->Close();
    delete mOf1;
  }
}

bool SciFiDisplayMomentumResolutionsPR::CalcResolution(const std::string& residual, const TCut cut,
                                                       double &res, double &res_err) {
  if (mTree) {
    mOf1->cd();
    TCanvas lCanvas;
    TH1D* hResidual(NULL);
    TCut lCut = cut;

    // Add to the cut if the only 5 spacepoint tracks are to used in the analysis
    if (m5StationOnly) {
      lCut = cut + "NumberOfPoints==5";
    }

    // Create a histogram of the input residual for the MC variable interval defined by input cut
    std::string htitle = residual + " " + lCut.GetTitle();
    hResidual = new TH1D("hResidual", htitle.c_str(), mNBinsPz, mPzFitMin, mPzFitMax);
    mTree->Draw((residual + ">>hResidual").c_str(), lCut);

    if (!hResidual) {
      std::cerr << "SciFiDisplayMomentumResolutionsPR::calc_resolution: Invalid tracker #\n";
      return false;
    }

    // Fit a gaussian to the histogram
    hResidual->Fit("gaus", "", "", mPzFitMin, mPzFitMax);
    TF1 *fit1 = hResidual->GetFunction("gaus");
    if (!fit1) {
      std::cerr << "SciFiDisplayMomentumResolutionsPR::calc_resolution: Fit failed\n";
      return false;
    }

    // Extract the sigma of the gaussian fit (equivalent to the pt resolution for this interval)
    res = fit1->GetParameter(2);
    res_err = fit1->GetParError(2);

    hResidual->Write();

    return true;
  } else {
    std::cerr << "SciFiDisplayMomentumResolutionsPR::Tree pointer invalid" << std::endl;
    return false;
  }
}

void SciFiDisplayMomentumResolutionsPR::Fill() {
  mOf1->cd();
  if (mTree) {
    // Loop over the data for each track in the spill and fill the tree with reduced data
    for ( size_t i = 0; i < GetData()->mDataPR.size(); ++i ) {
      mTrackData = GetData()->mDataPR[i];
      mTree->Fill();
      if (mTrackData.TrackerNumber == 0) {
        // TODO: Do I want anything here?
      } else if (mTrackData.TrackerNumber == 1) {
      }
    }
  } else {
    std::cerr << "SciFiDisplayMomentumResolutionsPR: Warning, local ROOT Tree not setup\n";
  }
}

TCut SciFiDisplayMomentumResolutionsPR::FormTCut(const std::string &var, const std::string &op,
                                                 double value) {
  TCut cut = "";
  std::stringstream ss1;
  TString s1;
  ss1 << var << op << value;
  ss1 >> s1;
  cut = s1;
  return cut;
}

SciFiDataMomentumPR* SciFiDisplayMomentumResolutionsPR::GetData() {
  return GetDataTemplate();
}

SciFiDataBase* SciFiDisplayMomentumResolutionsPR::MakeDataObject() {
  return MakeDataObjectTemplate();
}

void SciFiDisplayMomentumResolutionsPR::MakePtPtResolutions() {
  mResolMaker.Clear();
  mResolMaker.SetNGraphPoints(mNPoints);
  mResolMaker.SetNHistoBins(mNBinsPt);
  mResolMaker.SetGraphLowerBound(mLowerBoundPtMC);
  mResolMaker.SetGraphUpperBound(mUpperBoundPtMC);
  mResolMaker.SetFitMin(mPtFitMin);
  mResolMaker.SetFitMax(mPtFitMax);

  mResolMaker.SetCuts("TrackerNumber==0");
  mT1PtResolPtMC = mResolMaker.MakeGraph(mTree, "PtMc", "PtMc-PtRec");
  mResolMaker.SetCuts("TrackerNumber==1");
  mT2PtResolPtMC = mResolMaker.MakeGraph(mTree, "PtMc", "PtMc-PtRec");
}

void SciFiDisplayMomentumResolutionsPR::MakePzPtResolutions() {
  mResolMaker.Clear();
  mResolMaker.SetNGraphPoints(mNPoints);
  mResolMaker.SetNHistoBins(mNBinsPz);
  mResolMaker.SetGraphLowerBound(mLowerBoundPtMC);
  mResolMaker.SetGraphUpperBound(mUpperBoundPtMC);
  mResolMaker.SetFitMin(mPzFitMin);
  mResolMaker.SetFitMax(mPzFitMax);

  TCut PzRecCut = FormTCut("TMath::Abs(PzRec)", "<", mCutPzRec);
  TCut T1Cut = "TrackerNumber==0";
  TCut T2Cut = "TrackerNumber==1";
  TCut TotalCut = T1Cut + PzRecCut;

  mResolMaker.SetCuts(TotalCut);
  mT1PzResolPtMC = mResolMaker.MakeGraph(mTree, "PtMc", "PzMc+Charge*PzRec");
  TotalCut = T2Cut + PzRecCut;
  mResolMaker.SetCuts(TotalCut);
  mT2PzResolPtMC = mResolMaker.MakeGraph(mTree, "PtMc", "PzMc-Charge*PzRec");
}

void SciFiDisplayMomentumResolutionsPR::MakePtPzResolutions() {
  mResolMaker.Clear();
  mResolMaker.SetNGraphPoints(mNPoints);
  mResolMaker.SetNHistoBins(mNBinsPt);
  mResolMaker.SetGraphLowerBound(mLowerBoundPzMC);
  mResolMaker.SetGraphUpperBound(mUpperBoundPzMC);
  mResolMaker.SetFitMin(mPtFitMin);
  mResolMaker.SetFitMax(mPtFitMax);

  TCut PzRecCut = FormTCut("TMath::Abs(PzRec)", "<", mCutPzRec);
  TCut T1Cut = "TrackerNumber==0";
  TCut T2Cut = "TrackerNumber==1";
  TCut TotalCut = T1Cut + PzRecCut;

  mResolMaker.SetCuts(TotalCut);
  mT1PtResolPzMC = mResolMaker.MakeGraph(mTree, "PzMc", "PtMc-PtRec");
  TotalCut = T2Cut + PzRecCut;
  mResolMaker.SetCuts(TotalCut);
  mT2PtResolPzMC = mResolMaker.MakeGraph(mTree, "PzMc", "PtMc-PtRec");
}

void SciFiDisplayMomentumResolutionsPR::MakePzPzResolutions() {
  mResolMaker.Clear();
  mResolMaker.SetNGraphPoints(mNPoints);
  mResolMaker.SetNHistoBins(mNBinsPz);
  mResolMaker.SetGraphLowerBound(mLowerBoundPzMC);
  mResolMaker.SetGraphUpperBound(mUpperBoundPzMC);
  mResolMaker.SetFitMin(mPzFitMin);
  mResolMaker.SetFitMax(mPzFitMax);

  TCut PzRecCut = FormTCut("TMath::Abs(PzRec)", "<", mCutPzRec);
  TCut T1Cut = "TrackerNumber==0";
  TCut T2Cut = "TrackerNumber==1";
  TCut TotalCut = T1Cut + PzRecCut;

  mResolMaker.SetCuts(TotalCut);
  mT1PzResolPzMC = mResolMaker.MakeGraph(mTree, "PzMc", "PzMc+Charge*PzRec");
  TotalCut = T2Cut + PzRecCut;
  mResolMaker.SetCuts(TotalCut);
  mT2PzResolPzMC = mResolMaker.MakeGraph(mTree, "PzMc", "PzMc-Charge*PzRec");
}

void SciFiDisplayMomentumResolutionsPR::Plot(TCanvas* aCanvas) {
  mOf1->cd();
  // If canvas if passed in use it, otherwise initialise the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    lCanvas = aCanvas;
  } else {
    if (mCanvas) {
      mCanvas->Clear();
    } else {
      mCanvas = new TCanvas("cMomResolutionsPR", "Momentum Resolutions PR");
    }
    lCanvas = mCanvas;
  }
  // Prepare the TCanvas
  lCanvas->cd();
  lCanvas->Divide(4, 2);

  // Create the graphs
  MakePtPtResolutions();
  MakePzPtResolutions();
  MakePtPzResolutions();
  MakePzPzResolutions();

  // Draw the graphs
  if (mT1PtResolPtMC) {
    std::cout << "Drawing pt resolution as function of pt_mc in T1\n";
    lCanvas->cd(1);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT1PtResolPtMC->SetName("t1_ptpt_resol");
    mT1PtResolPtMC->SetTitle("T1 p_{t} Resolution");
    mT1PtResolPtMC->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    mT1PtResolPtMC->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    mT1PtResolPtMC->Draw("AP");
  }
  if (mT2PtResolPtMC) {
    std::cout << "Drawing pt resolution as function of pt_mc in T2\n";
    lCanvas->cd(5);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT2PtResolPtMC->SetName("t2_ptpt_resol");
    mT2PtResolPtMC->SetTitle("T2 p_{t} Resolution");
    mT2PtResolPtMC->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    mT2PtResolPtMC->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    mT2PtResolPtMC->Draw("AP");
  }
  if (mT1PzResolPtMC) {
    std::cout << "Drawing pz resolution as function of pt_mc in T1\n";
    lCanvas->cd(2);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT1PzResolPtMC->SetName("t1_pzpt_resol");
    mT1PzResolPtMC->SetTitle("T1 p_{z} Resolution");
    mT1PzResolPtMC->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    mT1PzResolPtMC->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    mT1PzResolPtMC->Draw("AP");
  }
  if (mT2PzResolPtMC) {
    std::cout << "Drawing pz resolution as function of pt_mc in T2\n";
    lCanvas->cd(6);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT2PzResolPtMC->SetName("t2_pzpt_resol");
    mT2PzResolPtMC->SetTitle("T2 p_{z} Resolution");
    mT2PzResolPtMC->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    mT2PzResolPtMC->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    mT2PzResolPtMC->Draw("AP");
  }
  if (mT1PtResolPzMC) {
    std::cout << "Drawing pt resolution as function of pz_mc in T1\n";
    lCanvas->cd(3);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT1PtResolPzMC->SetName("t1_ptpz_resol");
    mT1PtResolPzMC->SetTitle("T1 p_{t} Resolution");
    mT1PtResolPzMC->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    mT1PtResolPzMC->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    mT1PtResolPzMC->Draw("AP");
  }
  if (mT2PtResolPzMC) {
    std::cout << "Drawing pt resolution as function of pz_mc in T2\n";
    lCanvas->cd(7);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT2PtResolPzMC->SetName("t2_ptpz_resol");
    mT2PtResolPzMC->SetTitle("T2 p_{t} Resolution");
    mT2PtResolPzMC->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    mT2PtResolPzMC->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    mT2PtResolPzMC->Draw("AP");
  }
  if (mT1PzResolPzMC) {
    std::cout << "Drawing pz resolution as function of pz_mc in T1\n";
    lCanvas->cd(4);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT1PzResolPzMC->SetName("t1_pzpz_resol");
    mT1PzResolPzMC->SetTitle("T1 p_{z} Resolution");
    mT1PzResolPzMC->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    mT1PzResolPzMC->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    mT1PzResolPzMC->Draw("AP");
  }
  if (mT2PzResolPzMC) {
    std::cout << "Drawing pz resolution as function of pz_mc in T2\n";
    lCanvas->cd(8);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    mT2PzResolPzMC->SetName("t2_pzpz_resol");
    mT2PzResolPzMC->SetTitle("T2 p_{z} Resolution");
    mT2PzResolPzMC->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    mT2PzResolPzMC->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    mT2PzResolPzMC->Draw("AP");
  }
}

void SciFiDisplayMomentumResolutionsPR::Save() {
  if (mOf1) {
    mOf1->cd();
    if (mTree) mTree->Write();
    if (mCanvas) mCanvas->Write();
    if (mT1PtResolPtMC) mT1PtResolPtMC->Write();
    if (mT2PtResolPtMC) mT2PtResolPtMC->Write();
    if (mT1PzResolPtMC) mT1PzResolPtMC->Write();
    if (mT2PzResolPtMC) mT2PzResolPtMC->Write();
    if (mT1PtResolPzMC) mT1PtResolPzMC->Write();
    if (mT2PtResolPzMC) mT2PtResolPzMC->Write();
    if (mT1PzResolPzMC) mT1PzResolPzMC->Write();
    if (mT2PzResolPzMC) mT2PzResolPzMC->Write();
    mOf1->Close();
  }
}

SciFiDataBase* SciFiDisplayMomentumResolutionsPR::SetData(SciFiDataBase* data) {
  return SetDataTemplate(data);
}

SciFiDataBase* SciFiDisplayMomentumResolutionsPR::SetUp() {
  SetUpData();
  SetUpRoot();
  return mSpillData;
}

bool SciFiDisplayMomentumResolutionsPR::SetUpRoot() {
  // Setup the output TFile
  mOf1 = new TFile("SciFiDisplayMomentumResolutionsPROutput.root", "recreate");
  // Set up the TTree
  mTree = new TTree("mTree", "SciFi Momentum Data");
  mTree->Branch("TrackerNumber", &mTrackData.TrackerNumber, "TrackerNumber/I");
  mTree->Branch("NumberOfPoints", &mTrackData.NumberOfPoints, "NumberOfPoints/I");
  mTree->Branch("Charge", &mTrackData.Charge, "Charge/I");
  mTree->Branch("PtMc", &mTrackData.PtMc, "PtMc/D");
  mTree->Branch("PzMc", &mTrackData.PzMc, "PtMz/D");
  mTree->Branch("PtRec", &mTrackData.PtRec, "PtRec/D");
  mTree->Branch("PzRec", &mTrackData.PzRec, "PtRec/D");
  // Set up the resolution graphs

  return true;
}
} // ~namespace MAUS
