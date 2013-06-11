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


#ifndef KALMANSMOOTHER_HH
#define KALMANSMOOTHER_HH

// C headers
#include <assert.h>

// C++ headers
#include <string>
#include <vector>
#include <cmath>

#include "TMath.h"
#include "TMatrixD.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class KalmanFilter {
 public:
  /** @brief  Default constructor.
   */
  KalmanFilter();

  /** @brief  Constructor accepting dimension of member matrices.
   */
  explicit KalmanFilter(int dim);

  /** @brief  Destructor.
   */
  ~KalmanFilter();

  /** @brief  Runs the filter stage by calling the other member functions.
   */
  void Process(KalmanSitesPArray sites, int current_site);

  /** @brief  Computes the filtered state.
   */
  void CalculateFilteredState(KalmanSite *a_site);

  /** @brief  Computes the filtered covariance matrix.
   */
  void UpdateCovariance(KalmanSite *a_site);

  /** @brief  Computes the difference between the estimation and measurement for different stages.
   */
  void SetResidual(KalmanSite *a_site, KalmanSite::State kalman_state);

  void UpdateV(const KalmanSite *a_site);

  void UpdateH(const KalmanSite *a_site);

  void UpdateW(const KalmanSite *a_site);

  void UpdateK(const KalmanSite *a_site);

  void ComputePull(KalmanSite *a_site);

  TMatrixD H() const { return _H; }

 private:
  int _n_parameters;

  TMatrixD _V;

  TMatrixD _H;

  TMatrixD _W;

  TMatrixD _K;

  SciFiParams FibreParameters;
};

} // ~namespace MAUS

#endif

