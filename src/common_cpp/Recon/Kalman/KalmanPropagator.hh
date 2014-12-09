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


#ifndef KALMANPROPAGATOR_HH
#define KALMANPROPAGATOR_HH

// C headers
#include <assert.h>

// C++ headers
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "TMath.h"
#include "TMatrixD.h"

#include "Utils/Exception.hh"
#include "src/common_cpp/Recon/Kalman/KalmanState.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/Constants.hh"

namespace MAUS {

class KalmanPropagator {
 public:
  /** @brief  Default constructor.
   */
  KalmanPropagator();

  /** @brief  Destructor.
   */
  virtual ~KalmanPropagator() {}

  /** @brief  Main worker for the forward propagation.
   */
  void Extrapolate(KalmanStatesPArray sites, int i);

  /** @brief  Calculates the Propagator Matrix (F) for the current extrapolation.
   */
  virtual void UpdatePropagator(const KalmanState *old_site, const KalmanState *new_site) = 0;

  /** @brief  Calculates the state vector prediction at the next site.
   */
  virtual void CalculatePredictedState(const KalmanState *old_site, KalmanState *new_site) = 0;

  virtual TMatrixD GetIntermediateState(const KalmanState *old_site,
                                        double delta_z, TMatrixD &F) = 0;

  /** @brief  Projects the Covariance matrix.
   */
  void CalculateCovariance(const KalmanState *old_site, KalmanState *new_site);

  /** @brief  Calculates the Energy loss according to Bethe Bloch formula.
   */
  double BetheBlochStoppingPower(double p, std::string material);

  /** @brief  Subtracts the energy loss computed by BetheBlochStoppingPower.
   *
   * NOTE: In the case of tracker 0, the energy subtraction corresponds to an "energy
   *       adition" in the particle tracking UP the beamline.
   *
   */
  void SubtractEnergyLoss(KalmanState *new_site);

  /** @brief  Smoothes the last fitted point, for which filtered values = smoothed ones.
   */
  void PrepareForSmoothing(KalmanState *last_site);

  /** @brief  Smoothes the track points all the way to the beggining of the track.
   */
  void Smooth(KalmanStatesPArray sites, int id);

  /** @brief  Builds the member matrix _A.
   */
  void UpdateBackTransportationMatrix(const KalmanState *optimum_site,
                                      const KalmanState *smoothing_site);

  /** @brief  Propagates backwards from one site to the previous.
   */
  void SmoothBack(const KalmanState *optimum_site, KalmanState *smoothing_site);

  /** @brief  Returns the width of the scattering angle distribution, according to the Highland formula.
   */
  double HighlandFormula(double L0, double beta, double p);

  /** @brief  Returns the track momentum at a plane.
   */
  virtual double GetTrackMomentum(const KalmanState *a_site) = 0;

  virtual TMatrixD BuildQ(TMatrixD a, double L0, double w) = 0;

  // void SetGeometryMap(SciFiGeometryMap map) { _geometry_map = map; }

  // SciFiGeometryMap GetGeometryMap()         { return _geometry_map; }

  double GetL(std::string material, double material_w);

  TMatrixD AddSystemNoise(const KalmanState *old_site, TMatrixD C_old);

  double GetA(std::string material);

  double GetZ(std::string material);

  double GetMeanExcitationEnergy(std::string material);

  double GetDensity(std::string material);

  double GetDensityCorrection(std::string material);

 protected:
  bool _use_MCS;

  bool _use_Eloss;

  TMatrixD _F;

  TMatrixD _Q;

  TMatrixD _A;

  int _n_parameters;

  MaterialParams FibreParameters;

  MaterialParams GasParameters;

  MaterialParams MylarParameters;

  // SciFiGeometryMap _geometry_map;

  double _w_scifi;

  double _w_mylar;

  double _mm_to_cm;
};

} // ~namespace MAUS

#endif
