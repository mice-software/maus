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
  double x_range = mUpperBoundPtMC - mLowerBoundPtMC;  // Range of the pz resolution graph
  double resolution_error =  x_range / ( mNPoints * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = mLowerBoundPtMC + resolution_error;

  std::vector<TCut> vCuts(mNPoints);          // The cuts defining the pt_mc intervals
  std::vector<double> vPtMc(mNPoints);        // The centre of the pt_mc intervals
  std::vector<double> vPtMcErr(mNPoints);     // The width of the intervals
  std::vector<double> vPtRes_t1(mNPoints);    // The resultant pt resolutions
  std::vector<double> vPtResErr_t1(mNPoints); // The errors associated with the pt res
  std::vector<double> vPtRes_t2(mNPoints);    // The resultant pt resolutions
  std::vector<double> vPtResErr_t2(mNPoints); // The errors associated with the pt res

  // Cuts in pt_mc
  std::string s1 = "PtMc>=";
  std::string s2 = "&&PtMc<";
  for (int i = 0; i < mNPoints; ++i) {
    std::stringstream ss1;
    double point_lower_bound = mLowerBoundPtMC + (resolution_error * 2 * i);
    double point_upper_bound = point_lower_bound + (resolution_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < mNPoints; ++i) {
    vPtMc[i] = first_resolution_point + (resolution_error * 2 * i);
    vPtMcErr[i] = resolution_error;
    std::cerr << "vPtMc[" << i << "] = " << vPtMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pt TCut
  // TCut tcut_ptrec = form_tcut("TMath::Abs(pt_rec)", "<", _cutPtRec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT1&&tcut_ptrec;
    TCut input_cut = vCuts[i]&&cutT1;
    CalcResolution(residual, input_cut, vPtRes_t1[i], vPtResErr_t1[i]);
  }
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT2&&tcut_ptrec;
    TCut input_cut = vCuts[i]&&cutT2;
    CalcResolution(residual, input_cut, vPtRes_t2[i], vPtResErr_t2[i]);
  }

  // Create the resultant resolution plots
  mT1PtResolPtMC = new TGraphErrors(mNPoints, &vPtMc[0], &vPtRes_t1[0],
                                        &vPtMcErr[0], &vPtResErr_t1[0]);
  mT2PtResolPtMC = new TGraphErrors(mNPoints, &vPtMc[0], &vPtRes_t2[0],
                                        &vPtMcErr[0], &vPtResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::make_pzpt_resolutions() {

  double pz_range = mUpperBoundPtMC - mLowerBoundPtMC;  // Range of the pz resolution graph
  double resolution_error =  pz_range / ( mNPoints * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = mLowerBoundPtMC + resolution_error;
  std::vector<TCut> vCuts(mNPoints);          // The cuts defining the pt_mc intervals
  std::vector<double> vPtMc(mNPoints);        // The centre of the pt_mc intervals
  std::vector<double> vPtMcErr(mNPoints);     // The width of the intervals
  std::vector<double> vPzRes_t1(mNPoints);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t1(mNPoints); // The errors associated with the pz res
  std::vector<double> vPzRes_t2(mNPoints);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t2(mNPoints); // The errors associated with the pz res

  // Cuts in pt_mc
  std::string s1 = "PtMc>=";
  std::string s2 = "&&PtMc<";
  for (int i = 0; i < mNPoints; ++i) {
    std::stringstream ss1;
    double point_lower_bound = mLowerBoundPtMC + (resolution_error * 2 * i);
    double point_upper_bound = point_lower_bound + (resolution_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < mNPoints; ++i) {
    vPtMc[i] = first_resolution_point + (resolution_error * 2 * i);
    vPtMcErr[i] = resolution_error;
    std::cerr << "vPtMc[" << i << "] = " << vPtMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pz TCut
  TCut tcut_pzrec = FormTCut("TMath::Abs(PzRec)", "<", mCutPzRec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PzMc+Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    CalcResolution(residual, input_cut, vPzRes_t1[i], vPzResErr_t1[i]);
  }
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PzMc-Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    CalcResolution(residual, input_cut, vPzRes_t2[i], vPzResErr_t2[i]);
  }

  // Create the resultant resolution plots
  mT1PzResolPtMC = new TGraphErrors(mNPoints, &vPtMc[0], &vPzRes_t1[0],
                                    &vPtMcErr[0], &vPzResErr_t1[0]);
  mT2PzResolPtMC = new TGraphErrors(mNPoints, &vPtMc[0], &vPzRes_t2[0],
                                    &vPtMcErr[0], &vPzResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::MakePtPzResolutions() {

  double mUpperBoundPzMC = 250.0;
  double mLowerBoundPzMC = 150.0;
  double x_range = mUpperBoundPzMC - mLowerBoundPzMC;  // Range of the pz resolution graph
  double x_error =  x_range / ( mNPoints * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = mLowerBoundPzMC + x_error;
  std::vector<TCut> vCuts(mNPoints);          // The cuts defining the pt_mc intervals
  std::vector<double> vPzMc(mNPoints);        // The centre of the pt_mc intervals
  std::vector<double> vPzMcErr(mNPoints);     // The width of the intervals
  std::vector<double> vPtRes_t1(mNPoints);    // The resultant pz resolutions
  std::vector<double> vPtResErr_t1(mNPoints); // The errors associated with the pz res
  std::vector<double> vPtRes_t2(mNPoints);    // The resultant pz resolutions
  std::vector<double> vPtResErr_t2(mNPoints); // The errors associated with the pz res

  // Cuts in pt_mc
  std::string s1 = "PzMc>=";
  std::string s2 = "&&PzMc<";
  for (int i = 0; i < mNPoints; ++i) {
    std::stringstream ss1;
    double point_lower_bound = mLowerBoundPzMC + (x_error * 2 * i);
    double point_upper_bound = point_lower_bound + (x_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < mNPoints; ++i) {
    vPzMc[i] = first_resolution_point + (x_error * 2 * i);
    vPzMcErr[i] = x_error;
    std::cerr << "vPzMc[" << i << "] = " << vPzMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pz TCut
  // TCut tcut_pzrec = form_tcut("TMath::Abs(PzRec)", "<", _cut_pz_rec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    TCut input_cut = vCuts[i]&&cutT1;
    CalcResolution(residual, input_cut, vPtRes_t1[i], vPtResErr_t1[i]);
  }
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    TCut input_cut = vCuts[i]&&cutT2;
    CalcResolution(residual, input_cut, vPtRes_t2[i], vPtResErr_t2[i]);
  }

  // Create the resultant resolution plots
  mT1PtResolPzMC = new TGraphErrors(mNPoints, &vPzMc[0], &vPtRes_t1[0],
                                        &vPzMcErr[0], &vPtResErr_t1[0]);
  mT2PtResolPzMC = new TGraphErrors(mNPoints, &vPzMc[0], &vPtRes_t2[0],
                                        &vPzMcErr[0], &vPtResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::MakePzPzResolutions() {

  double x_range = mUpperBoundPzMC - mLowerBoundPzMC;  // Range of the pz resolution graph
  double x_error =  x_range / ( mNPoints * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = mLowerBoundPzMC + x_error;
  std::vector<TCut> vCuts(mNPoints);          // The cuts defining the pt_mc intervals
  std::vector<double> vPzMc(mNPoints);        // The centre of the pt_mc intervals
  std::vector<double> vPzMcErr(mNPoints);     // The width of the intervals
  std::vector<double> vPzRes_t1(mNPoints);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t1(mNPoints); // The errors associated with the pz res
  std::vector<double> vPzRes_t2(mNPoints);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t2(mNPoints); // The errors associated with the pz res

  // Cuts in pt_mc
  std::string s1 = "PzMc>=";
  std::string s2 = "&&PzMc<";
  for (int i = 0; i < mNPoints; ++i) {
    std::stringstream ss1;
    double point_lower_bound = mLowerBoundPzMC + (x_error * 2 * i);
    double point_upper_bound = point_lower_bound + (x_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < mNPoints; ++i) {
    vPzMc[i] = first_resolution_point + (x_error * 2 * i);
    vPzMcErr[i] = x_error;
    std::cerr << "vPzMc[" << i << "] = " << vPzMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pz TCut
  TCut tcut_pzrec = FormTCut("TMath::Abs(PzRec)", "<", mCutPzRec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PzMc+Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    CalcResolution(residual, input_cut, vPzRes_t1[i], vPzResErr_t1[i]);
  }
  for (int i = 0; i < mNPoints; ++i) {
    std::string residual("PzMc-Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    CalcResolution(residual, input_cut, vPzRes_t2[i], vPzResErr_t2[i]);
  }

  // Create the resultant resolution plots
  mT1PzResolPzMC = new TGraphErrors(mNPoints, &vPzMc[0], &vPzRes_t1[0],
                                        &vPzMcErr[0], &vPzResErr_t1[0]);
  mT2PzResolPzMC = new TGraphErrors(mNPoints, &vPzMc[0], &vPzRes_t2[0],
                                        &vPzMcErr[0], &vPzResErr_t2[0]);
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
  make_pzpt_resolutions();
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
