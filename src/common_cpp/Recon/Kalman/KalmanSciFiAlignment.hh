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

// C++ headers
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "TMath.h"
#include "TMatrixD.h"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "Interface/Squeal.hh"
#include "Config/MiceModule.hh"

namespace MAUS {

class KalmanSciFiAlignment {
 public:
  KalmanSciFiAlignment();

  ~KalmanSciFiAlignment();

  bool load_misaligments();


  void update(KalmanSite site);

  void save();
  // MiceModule* find_plane(int tracker, int station, int plane);

  TMatrixD get_shifts(int site_id)     const { return shifts_array[site_id]; }
  TMatrixD get_rotations(int site_id)  const { return rotations_array[site_id]; }
  TMatrixD get_cov_shifts(int site_id) const { return covariance_shifts[site_id]; }
  TMatrixD get_cov_rotat(int site_id)  const { return covariance_rotations[site_id]; }

  void set_shifts(TMatrixD shifts, int site_id)        { shifts_array[site_id]        = shifts; }
  void set_rotations(TMatrixD rotations, int site_id)  { rotations_array[site_id]     = rotations; }
  void set_cov_shifts(TMatrixD cov_s, int site_id)     { covariance_shifts[site_id]   = cov_s; }
  void set_cov_rotat(TMatrixD cov_r, int site_id)      { covariance_rotations[site_id]= cov_r; }

 private:
  std::string file, fname;

  TMatrixD shifts_array[30];
  TMatrixD rotations_array[30];
  TMatrixD covariance_rotations[30];
  TMatrixD covariance_shifts[30];
  // std::vector<MiceModule*> _modules;
};

} // ~namespace MAUS

#endif
