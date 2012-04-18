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


#ifndef STRAIGHTTRACK_HH
#define STRAIGHTTRACK_HH

// C headers
#include <assert.h>

// C++ headers
#include <string>
#include <vector>

#include "TMath.h"
#include "TMatrixD.h"


#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

class StraightTrack {
 public:
  StraightTrack();

  ~StraightTrack();

  // void calc_expect_measurement(KalmanSite &site);
  // h1(a_1^0)

  // void calc_pull(KalmanSite &site);
  // measurement-h1(a_1^0);

  // void calc_covariance_matrix_of_residual(KalmanSite &site);
  // C_k = [ (H_k)^T   G_k   H_k ]^-1

  // void calc_kalman_gain(KalmanSite &site);
  // K_k = C_k  (H_k)^T  G_k
  void update_propagator(KalmanSite *old_site, KalmanSite *new_site);

  void calc_filtered_state(KalmanSite *a_site);

  void update_G(KalmanSite *a_site);

  void update_covariance(KalmanSite *a_site);

  void update_H(KalmanSite *a_site);

  void calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site);

  // void calc_system_noise(KalmanSite &site);

  void calc_covariance(KalmanSite *old_site, KalmanSite *new_site);

  void update_back_transportation_matrix(KalmanSite *optimum_site, KalmanSite *smoothing_site);

  void smooth_back(KalmanSite *optimum_site, KalmanSite *smoothing_site);

 private:
  TMatrixD _H;

  TMatrixD _G;

  TMatrixD _Q;

  TMatrixD _A;

  TMatrixD _F;

  static const double sigma_x = 0.64; // x measurement resolution

  static const double A = 2./(7.*0.427); // mm to channel convertion factor.
};

#endif
