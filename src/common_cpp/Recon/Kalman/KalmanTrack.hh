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
#include <CLHEP/Vector/ThreeVector.h>

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
// #include "src/common_cpp/Recon/Kalman/Particle.hh"

//#include "src/common_cpp/Utils/Globals.hh"
//#include "src/common_cpp/Globals/GlobalsManager.hh"

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
} BetheBlochParameters;

class KalmanTrack {
 public:
  /* @brief	Constructs taking MCS and Eloss flags.
   */
  KalmanTrack(bool MCS, bool Eloss);

  /* @brief	Destructor. Cleans up heap.
   */
  virtual ~KalmanTrack() {}

  /* @brief	Initializes member matrices.
   */
  void init();

  /* @brief	Manages all extrapolation steps.
   */
  void extrapolate(std::vector<KalmanSite> &sites, int current_site);

  /* @brief	Manages all filtering steps.
   */
  void filter(std::vector<KalmanSite> &sites, int current_site);

  /* @brief	Manages all smoothins steps.
   */
  void smooth(std::vector<KalmanSite> &sites, int current_site);

  /* @brief	Calculates the Propagator Matrix (F) for the current extrapolation.
   */
  virtual void update_propagator(const KalmanSite *old_site, const KalmanSite *new_site) = 0;

  /// Projection methods
  virtual void calc_predicted_state(const KalmanSite *old_site, KalmanSite *new_site) = 0;
  void calc_covariance(const KalmanSite *old_site, KalmanSite *new_site);
  double BetheBlochStoppingPower(double p);
  void subtract_energy_loss(const KalmanSite *old_site, KalmanSite *new_site);
  /// Error added by Multiple Coulomb Scattering.
  void calc_system_noise(const KalmanSite *old_site, const KalmanSite *new_site);

  /// Filtering methods
  void calc_filtered_state(KalmanSite *a_site);
  void update_covariance(KalmanSite *a_site);
  void set_residual(KalmanSite *a_site, KalmanSite::State kalman_state);
  TMatrixD solve_measurement_equation(const TMatrixD &a, const TMatrixD &s);
  /// Update of relevant matrices.
  void update_V(const KalmanSite *a_site);
  void update_H(const KalmanSite *a_site);
  void update_W(const KalmanSite *a_site);
  void update_K(const KalmanSite *a_site);
  void compute_pull(KalmanSite *a_site);

  /// Smoothing methods
  void prepare_for_smoothing(KalmanSite *last_site);
  void update_back_transportation_matrix(const KalmanSite *optimum_site,
                                         const KalmanSite *smoothing_site);
  void smooth_back(const KalmanSite *optimum_site, KalmanSite *smoothing_site);


  /// Other methods
  void exclude_site(KalmanSite *site);
  void compute_chi2(const std::vector<KalmanSite> &sites);
  void update_misaligments(std::vector<KalmanSite> &sites,
                           std::vector<KalmanSite> &sites_copy,
                           int station_i);

  /// Getters.
  double f_chi2()         const { return _f_chi2;   }
  double s_chi2()         const { return _s_chi2;        }
  int ndf()               const { return _ndf;      }
  double P_value()        const { return _P_value;  }
  int tracker()           const { return _tracker;  }
  double momentum()       const { return _momentum; }

  void set_momentum(double momentum) { _momentum = momentum; }

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

