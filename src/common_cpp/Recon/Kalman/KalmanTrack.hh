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
#include "src/common_cpp/Recon/Kalman/Particle.hh"
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
  KalmanTrack();

  virtual ~KalmanTrack() {}

  void extrapolate(std::vector<KalmanSite> &sites, int current_site);

  void filter(std::vector<KalmanSite> &sites, int current_site);

  void smooth(std::vector<KalmanSite> &sites, int current_site);

  virtual void update_propagator(const KalmanSite *old_site, const KalmanSite *new_site) = 0;

  /// Projection: calculation of predicted state.
  void calc_predicted_state(const KalmanSite *old_site, KalmanSite *new_site);
  /// Projection: calculation of covariance matrix.
  void calc_covariance(const KalmanSite *old_site, KalmanSite *new_site);
  /// Bethe-Bloch calculation.
  double BetheBlochStoppingPower(double p);
  /// Subtract energy loss.
  void subtract_energy_loss(const KalmanSite *old_site, KalmanSite *new_site);
  /// Error added by Multiple Coulomb Scattering.
  void calc_system_noise(const KalmanSite *old_site, const KalmanSite *new_site);

  /// Filtering: update of relevant matrices.
  void update_V(const KalmanSite *a_site);
  void update_H(const KalmanSite *a_site);
  void update_W(const KalmanSite *a_site);
  void update_K(const KalmanSite *a_site);
  void compute_pull(KalmanSite *a_site);
  /// Filtering: calculation of filtered state.
  void calc_filtered_state(KalmanSite *a_site);

  /// Filtering: update of covariance matrix.
  void update_covariance(KalmanSite *a_site);
  void set_residual(KalmanSite *a_site, KalmanSite::State kalman_state);
  TMatrixD solve_measurement_equation(const TMatrixD &a, const TMatrixD &s);
  // void update_misaligments(KalmanSite *a_site, KalmanSite *old_site);

  /// Smoothing: updates back transportation matrix.
  void update_back_transportation_matrix(const KalmanSite *optimum_site,
                                         const KalmanSite *smoothing_site);
  void smooth_back(const KalmanSite *optimum_site, KalmanSite *smoothing_site);
  void prepare_for_smoothing(KalmanSite *last_site);
  void exclude_site(KalmanSite *site);

  void compute_chi2(const std::vector<KalmanSite> &sites);
  void compute_emittance(KalmanSite site);

  void update_misaligments(std::vector<KalmanSite> &sites,
                           std::vector<KalmanSite> &sites_copy,
                           int station_i);

  /// Getters.
  TMatrixD propagator()   const { return _F;        }
  TMatrixD system_noise() const { return _Q;        }
  TMatrixD Q()            const { return _Q;        }
  double f_chi2()         const { return _f_chi2;   }
  double s_chi2()         const { return _s_chi2;        }
  int ndf()               const { return _ndf;      }
  double P_value()        const { return _P_value;  }
  int tracker()           const { return _tracker;  }
  double mass()           const { return _mass;     }
  double momentum()       const { return _momentum; }

  void set_Q(TMatrixD Q)             { _Q = Q;               }
  void set_mass(double mass)         { _mass = mass;         }
  void set_momentum(double momentum) { _momentum = momentum; }

 protected:
  bool _use_MCS, _use_Eloss;

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

  double _mass, _momentum;

  Particle _particle;

  int _particle_charge;

  struct Emittance {
    double epsilon;
    double alpha;
    double beta;
    double gamma;
  };

  Emittance emittance;
};

} // ~namespace MAUS

#endif

