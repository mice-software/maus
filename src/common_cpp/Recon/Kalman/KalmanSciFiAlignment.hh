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

#ifndef KALMANSCIFIALIGNMENT_HH
#define KALMANSCIFIALIGNMENT_HH

// C headers
#include <assert.h>
#include <math.h>
#include <sstream>

// C++ headers
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>

#include "TMath.h"
#include "TMatrixD.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCollection.h"
#include "TList.h"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "Interface/Squeal.hh"

namespace MAUS {

class KalmanSciFiAlignment {
 public:
  KalmanSciFiAlignment();

  ~KalmanSciFiAlignment();

  /** @brief
   *
   *  Loads misalignment info (x, y and their covariances)
   *  which are stored in a txt file. They are loaded into
   *  member TMatrixD's _shifts_array and _covariance_shifts.
   *
   */
  // bool LoadMisaligments();

  /** @brief
   *
   *  Updates info stored for a given site.
   *
   */
  // void Update(const KalmanSite &site);

  /** @brief
   *
   *  Saves to txt and calls SaveToRootFile.
   *
   */
  // void Save();

  /** @brief
   *
   *  Saves updates to TGraphs in the root output.
   *
   */
  // void SaveToRootFile();

  /** @brief
   *
   *  Initialises the root output file. If a root output file
   *  already exists, it loads it and appends to it.
   *
   */
  // void SetUpRootOutput();

  /** @brief
   *
   *  Saves and cloes the root output.
   *
   */
  // void CloseRootFile();
/*
  TMatrixD get_shifts(int site_id);

  TMatrixD get_cov_shifts(int site_id);

  void set_shifts(TMatrixD shifts, int site_id)        { _shifts_map[site_id]     = shifts; }

  void set_cov_shifts(TMatrixD cov_s, int site_id)     { _covariance_map[site_id] = cov_s; }

 private:
  std::string _file, _fname;

  std::map<int, TMatrixD> _shifts_map;
  std::map<int, TMatrixD> _covariance_map;

  TMultiGraph *_graphs_tracker0;
  TMultiGraph *_graphs_tracker1;

  TFile  *_rootfile;
  TTree *_tree;
*/
};

} // ~namespace MAUS

#endif
