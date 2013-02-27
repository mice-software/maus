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


#ifndef KALMANMONITOR_HH
#define KALMANMONITOR_HH

// C headers
#include <assert.h>

// C++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TFile.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"

#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

namespace MAUS {

class KalmanMonitor {
 public:
  KalmanMonitor();

  ~KalmanMonitor();

  void fill(std::vector<KalmanSite> const &sites);

  void print_info(std::vector<KalmanSite> const &sites);

  void save();

 private:
  TFile *file;

  int _counter;

  // std::vector<double> _site;

  TH1F *chi2_tracker0;
  TH1F *chi2_tracker1;
  TH1F *pvalue_tracker0;
  TH1F *pvalue_tracker1;

  TH1F *pull_site_3;
  TH1F *residual_site_3;
  TH1F *smoothed_residual_site_3;
  TH1F *pull_site_4;
  TH1F *residual_site_4;
  TH1F *smoothed_residual_site_4;
  TH1F *pull_site_5;
  TH1F *residual_site_5;
  TH1F *smoothed_residual_site_5;
  TH1F *pull_site_9;
  TH1F *residual_site_9;
  TH1F *smoothed_residual_site_9;

  TGraph *station1_x;
  TGraph *station2_x;
  TGraph *station3_x;
  TGraph *station4_x;
  TGraph *station5_x;
  TGraph *station6_x;
  TGraph *station7_x;
  TGraph *station8_x;
  TGraph *station9_x;
  TGraph *station10_x;
  TGraph *station1_y;
  TGraph *station2_y;
  TGraph *station3_y;
  TGraph *station4_y;
  TGraph *station5_y;
  TGraph *station6_y;
  TGraph *station7_y;
  TGraph *station8_y;
  TGraph *station9_y;
  TGraph *station10_y;
  TH1F *excl_plane11;

  TH2F *pull_hist;
  TH2F *residual_hist;
  TH2F *smooth_residual_hist;

  TH2F *x_proj_h;
  TH2F *y_proj_h;
  TH2F *px_proj_h;
  TH2F *py_proj_h;
  TH2F *pz_proj_h;

  TH2F *x_filt_h;
  TH2F *y_filt_h;
  TH2F *px_filt_h;
  TH2F *py_filt_h;
  TH2F *pz_filt_h;

  TH2F *x_smooth_h;
  TH2F *y_smooth_h;
  TH2F *px_smooth_h;
  TH2F *py_smooth_h;
  TH2F *pz_smooth_h;
  ///////////////
  TH1F *x_proj_h1;
  TH1F *y_proj_h1;
  TH1F *px_proj_h1;
  TH1F *py_proj_h1;
  TH1F *pz_proj_h1;

  TH1F *x_filt_h1;
  TH1F *y_filt_h1;
  TH1F *px_filt_h1;
  TH1F *py_filt_h1;
  TH1F *pz_filt_h1;

  TH1F *x_smooth_h1;
  TH1F *y_smooth_h1;
  TH1F *px_smooth_h1;
  TH1F *py_smooth_h1;
  TH1F *pz_smooth_h1;
};

} // ~namespace MAUS

#endif
