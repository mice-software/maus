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
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class KalmanSite;

typedef struct SciFiParams {
  /// Polystyrene's atomic number.
  static const double Z()               { return 5.61291; }
  /// Width of the fibre plane in mm.
  static const double Plane_Width()     { return 0.6523; }
  /// Fibre radiation lenght in mm
  static const double Radiation_Legth() { return 424.0; }
  /// Fractional Radiation Length
  static const double R0(double lenght) { return lenght/Radiation_Legth(); }
  /// Density in g.cm-3
  static const double Density()         { return 1.06000; }
  /// Mean excitation energy in eV.
  static const double Mean_Excitation_Energy() { return 68.7; }
  /// Atomic number in g.mol-1 per styrene monomer
  static const double A()               { return 104.15; }
  /// Channel width in mm
  static const double Pitch()           { return 1.4945; }
  /// Active Radius in mm
  static const double Active_Radius()   { return 150.; }
  /// RMS per channel measurement (um).
  static const double RMS()             { return 370.; }
} FibreParameters;

typedef struct BetheBloch {
  /// Bethe Bloch constant in MeV g-1 cm2 (for A=1gmol-1)
  static const double K() { return 0.307075; }
  /// The electron mass is a parameter of Bethe-Bloch's formula.
  static const double Electron_Mass() { return 0.511; }
} BetheBlochParameters;

class KalmanTrack {
 public:
  /* @brief  Constructs taking MCS and Eloss flags.
   */
  KalmanTrack(bool MCS, bool Eloss);

  /* @brief  Destructor. Cleans up heap.
   */
  virtual ~KalmanTrack() {}

  /* @brief  Initializes member matrices.
   */
  void Initialise();

  /* @brief  Manages all extrapolation steps.
   */
  void Extrapolate(std::vector<KalmanSite> &sites, int current_site);

  /* @brief  Manages all filtering steps.
   */
  void Filter(std::vector<KalmanSite> &sites, int current_site);

  /* @brief  Manages all smoothins steps.
   */
  void Smooth(std::vector<KalmanSite> &sites, int current_site);

  /* @brief  Calculates the Propagator Matrix (F) for the current extrapolation.
   */
  virtual void UpdatePropagator(const KalmanSite *old_site, const KalmanSite *new_site) = 0;

  /* @brief  Calculates the state vector prediction at the next site.
   */
  virtual void CalculatePredictedState(const KalmanSite *old_site, KalmanSite *new_site) = 0;

  /* @brief  Projects the Covariance matrix.
   */
  void CalculateCovariance(const KalmanSite *old_site, KalmanSite *new_site);

  /* @brief  Calculates the Energy loss according to Bethe Bloch formula.
   */
  double BetheBlochStoppingPower(double p);

  /* @brief  Subtracts the energy loss computed by BetheBlochStoppingPower.
   */
  void SubtractEnergyLoss(const KalmanSite *old_site, KalmanSite *new_site);

  /* @brief  Computes the contribution of MCS to the covariance matrix.
   */
  void CalculateSystemNoise(const KalmanSite *old_site, const KalmanSite *new_site);

  /* @brief  Computes the filtered state.
   */
  void CalculateFilteredState(KalmanSite *a_site);

  /* @brief  Computes the filtered covariance matrix.
   */
  void UpdateCovariance(KalmanSite *a_site);


  void SetResidual(KalmanSite *a_site, KalmanSite::State kalman_state);

  /* @brief  Solves the measurement equation.
   */
  TMatrixD SolveMeasurementEquation(const TMatrixD &a, const TMatrixD &s);

  void UpdateV(const KalmanSite *a_site);

  void UpdateH(const KalmanSite *a_site);

  void UpdateW(const KalmanSite *a_site);

  void UpdateK(const KalmanSite *a_site);

  void ComputePull(KalmanSite *a_site);

  /* @brief  Solves the measurement equation.
   */
  void PrepareForSmoothing(KalmanSite *last_site);
  void UpdateBackTransportationMatrix(const KalmanSite *optimum_site,
                                         const KalmanSite *smoothing_site);
  void SmoothBack(const KalmanSite *optimum_site, KalmanSite *smoothing_site);

  void set_momentum(double momentum) { _momentum = momentum; }

  /// Other methods
  void ExcludeSite(KalmanSite *site);
  void ComputeChi2(const std::vector<KalmanSite> &sites);
  void UpdateMisaligments(std::vector<KalmanSite> &sites,
                           std::vector<KalmanSite> &sites_copy,
                           int station_i);

  /// Getters.
  double f_chi2()         const { return _f_chi2;   }
  double s_chi2()         const { return _s_chi2;   }
  int ndf()               const { return _ndf;      }
  double P_value()        const { return _P_value;  }
  int tracker()           const { return _tracker;  }
  double momentum()       const { return _momentum; }
  TMatrixD Q()            const { return _Q;        }

 protected:
  bool _use_MCS;

  bool _use_Eloss;

  TMatrixD _H;

  TMatrixD _S;

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

