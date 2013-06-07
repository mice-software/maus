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


#ifndef KALMANTTRACK_HH
#define KALMANTTRACK_HH

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

#include "Interface/Squeal.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class KalmanSite;

struct SciFiParams {
  /// Polystyrene's atomic number.
  double Z;
  /// Width of the fibre plane in mm.
  double Plane_Width;
  /// Fibre radiation lenght in mm
  double Radiation_Legth;
  /// Fractional Radiation Length
  double R0(double lenght) { return lenght/Radiation_Legth; }
  /// Density in g.cm-3
  double Density;
  /// Mean excitation energy in eV.
  double Mean_Excitation_Energy;
  /// Atomic number in g.mol-1 per styrene monomer
  double A;
  /// Channel width in mm
  double Pitch;
  /// Active Radius in mm
  double Active_Radius;
  /// RMS per channel measurement (um).
  double RMS;
};

typedef struct BetheBloch {
  /// Bethe Bloch constant in MeV g-1 cm2 (for A=1gmol-1)
  static const double K() { return 0.307075; }
  /// The electron mass is a parameter of Bethe-Bloch's formula.
  static const double Electron_Mass() { return 0.511; }
} BetheBlochParameters;

class KalmanTrack {
 public:
  /** @brief  Constructor.
   */
  KalmanTrack();

  /** @brief  Destructor. Cleans up heap.
   */
  virtual ~KalmanTrack() {}

  /** @brief  Calculates the Propagator Matrix (F) for the current extrapolation.
   */
  virtual void UpdatePropagator(const KalmanSite *old_site, const KalmanSite *new_site) = 0;

  /** @brief  Calculates the state vector prediction at the next site.
   */
  virtual void CalculatePredictedState(const KalmanSite *old_site, KalmanSite *new_site) = 0;

  /** @brief  Initializes member matrices.
   */
  void Initialise();

  /** @brief  Projects the Covariance matrix.
   */
  void CalculateCovariance(const KalmanSite *old_site, KalmanSite *new_site);

  /** @brief  Calculates the Energy loss according to Bethe Bloch formula.
   */
  double BetheBlochStoppingPower(double p);

  /** @brief  Subtracts the energy loss computed by BetheBlochStoppingPower.
   */
  void SubtractEnergyLoss(const KalmanSite *old_site, KalmanSite *new_site);

  /** @brief  Computes the contribution of MCS to the covariance matrix.
   */
  void CalculateSystemNoise(const KalmanSite *old_site, const KalmanSite *new_site);

  /** @brief  Computes the filtered state.
   */
  void CalculateFilteredState(KalmanSite *a_site);

  /** @brief  Computes the filtered covariance matrix.
   */
  void UpdateCovariance(KalmanSite *a_site);

  void SetResidual(KalmanSite *a_site, KalmanSite::State kalman_state);

  /** @brief  Solves the measurement equation.
   */
  // TMatrixD SolveMeasurementEquation(const TMatrixD &a, const TMatrixD &s);

  void UpdateV(const KalmanSite *a_site);

  void UpdateH(const KalmanSite *a_site);

  void UpdateW(const KalmanSite *a_site);

  void UpdateK(const KalmanSite *a_site);

  void ComputePull(KalmanSite *a_site);

  void UpdateBackTransportationMatrix(const KalmanSite *optimum_site,
                                      const KalmanSite *smoothing_site);

  void SmoothBack(const KalmanSite *optimum_site, KalmanSite *smoothing_site);

  void set_momentum(double momentum) { _momentum = momentum; }

  /// Other methods
  void ExcludeSite(KalmanSite *site);
  void ComputeChi2(const std::vector<KalmanSite> &sites);

  /// Getters.
  double f_chi2()         const { return _f_chi2;   }
  double s_chi2()         const { return _s_chi2;   }
  double P_value()        const { return _P_value;  }
  double momentum()       const { return _momentum; }
  int ndf()               const { return _ndf;      }
  int tracker()           const { return _tracker;  }
  TMatrixD Q()            const { return _Q;        }
  TMatrixD H()            const { return _H;        }

  enum AlgorithmUsed { kalman_straight, kalman_helical };

  AlgorithmUsed GetAlgorithmUsed() { return _algorithm_used; }

 protected:
  SciFiParams FibreParameters;

  AlgorithmUsed _algorithm_used;

  TMatrixD _H;

  // TMatrixD _S;

  TMatrixD _V;

  TMatrixD _Q;

  TMatrixD _A;

  TMatrixD _F;

  TMatrixD _K;

  TMatrixD _W;

  double _f_chi2;

  double _s_chi2;

  int _ndf;

  double _P_value;

  int _n_parameters;

  size_t _n_sites;

  int _tracker;

  double _mass;

  double _momentum;

  int _particle_charge;
};

} // ~namespace MAUS

#endif

