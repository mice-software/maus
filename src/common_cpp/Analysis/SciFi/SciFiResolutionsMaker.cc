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
#include <vector>
#include <sstream>

// ROOT headers
#include "TCanvas.h"
#include "TH1D.h"
#include "TF1.h"

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiResolutionsMaker.hh"

namespace MAUS {

SciFiResolutionsMaker::SciFiResolutionsMaker() : mNGraphPoints(0),
                                                 mNHistoBins(0),
                                                 mGraphLowerBound(0.0),
                                                 mGraphUpperBound(0.0),
                                                 mFitMin(0.0),
                                                 mFitMax(0.0),
                                                 mCuts("") {
  // Do nothing
}

SciFiResolutionsMaker::~SciFiResolutionsMaker() {
  // Do nothing
}

bool SciFiResolutionsMaker::CalcResolution(TTree* aTree, const std::string& aResidual,
                                           const TCut& aCut, double& aRes, double &aResError) {
  if (aTree) {
    TCanvas lCanvas;
    TH1D* hResidual(NULL);
    TCut lCut = aCut;

    // Create a histogram of the input residual for the MC variable interval defined by input cut
    std::string htitle = aResidual + " " + lCut.GetTitle();
    hResidual = new TH1D("hResidual", htitle.c_str(), mNHistoBins, mFitMin, mFitMax);
    aTree->Draw((aResidual + ">>hResidual").c_str(), lCut);
    if (!hResidual) {
      std::cerr << "SciFiResolutionsMaker::calc_resolution: Invalid tracker #\n";
      return false;
    }
    // Fit a gaussian to the histogram
    hResidual->Fit("gaus", "", "", mFitMin, mFitMax);
    TF1 *fit1 = hResidual->GetFunction("gaus");
    if (!fit1) {
      std::cerr << "SciFiResolutionsMaker::calc_resolution: Fit failed\n";
      return false;
    }
    // Extract the sigma of the gaussian fit (equivalent to the pt resolution for this interval)
    aRes = fit1->GetParameter(2);
    aResError = fit1->GetParError(2);

    hResidual->Write();
    return true;
  } else {
    std::cerr << "SciFiResolutionsMaker::Tree pointer invalid" << std::endl;
    return false;
  }
}

void SciFiResolutionsMaker::Clear() {
  mNGraphPoints = 0;
  mNHistoBins = 0;
  mGraphLowerBound = 0.0;
  mGraphUpperBound = 0.0;
  mFitMin = 0.0;
  mFitMax = 0.0;
  mCuts = "";
}

TGraphErrors* SciFiResolutionsMaker::MakeGraph(TTree* aTree, const std::string& aXParam,
                                               const std::string&  aYResidual) {

  double lXRange = mGraphUpperBound - mGraphLowerBound;        // x axis range of graph
  double lResolutionError =  lXRange / ( mNGraphPoints * 2 );  // Error in x of each data point
  // The mid x value of the first data point
  double lFirstXPoint = mGraphLowerBound + lResolutionError;
  std::vector<TCut> lCuts(mNGraphPoints);      // The cuts defining the x intervals
  std::vector<double> lX(mNGraphPoints);       // The centre of the x intervals
  std::vector<double> lXErr(mNGraphPoints);    // The width of the intervals
  std::vector<double> lY_t1(mNGraphPoints);    // The resultant y resolutions
  std::vector<double> lYErr_t1(mNGraphPoints); // The errors associated with y
  std::vector<double> lY_t2(mNGraphPoints);    // The resultant y resolutions
  std::vector<double> lYErr_t2(mNGraphPoints); // The errors associated with y

  // Cuts in x
  std::string s1 = aXParam + ">=";
  std::string s2 = "&&" + aXParam + "<";
  for (int i = 0; i < mNGraphPoints; ++i) {
    std::stringstream ss1;
    double lPointLowerBound = mGraphLowerBound + (lResolutionError * 2 * i);
    double lPointUpperBound = lPointLowerBound + (lResolutionError * 2);
    ss1 << s1 << lPointLowerBound << s2 << lPointUpperBound;
    lCuts[i] = ss1.str().c_str();
    std::cerr << "lCuts[" << i << "] = " << lCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < mNGraphPoints; ++i) {
    lX[i] = lFirstXPoint + (lResolutionError * 2 * i);
    lXErr[i] = lResolutionError;
    std::cerr << "lX[" << i << "] = " << lX[i] << std::endl;
  }

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < mNGraphPoints; ++i) {
    TCut lInputCut = lCuts[i]&&mCuts;
    CalcResolution(aTree, aYResidual, lInputCut, lY_t1[i], lYErr_t1[i]);
  }
  for (int i = 0; i < mNGraphPoints; ++i) {
    TCut lInputCut = lCuts[i]&&mCuts;
    CalcResolution(aTree, aYResidual, lInputCut, lY_t2[i], lYErr_t2[i]);
  }

  // Create the resultant resolution plots
  return new TGraphErrors(mNGraphPoints, &lX[0], &lY_t1[0], &lXErr[0], &lYErr_t1[0]);
}

} // ~namespace MAUS
