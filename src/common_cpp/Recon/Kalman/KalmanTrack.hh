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

#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

namespace MAUS {

class KalmanSite;

typedef struct SciFiParams {
  /// Polystyrene's atomic number.
  static const double Z()               { return 5.61291; }
  /// Width of the fibre plane.
  static const double Plane_Width()     { return 0.6523; }
  static const double Radiation_Legth() { return 43.72; }
  /// Fractional Radiation Length in cm3/g
  static const double R0()              { return Plane_Width()/Radiation_Legth(); }
  /// Density in g.cm-3
  static const double Density()         { return 1.06000; }
  /// Mean excitation energy in eV.
  static const double Mean_Excitation_Energy() { return 68.7; }
  /// Atomic number in g.mol-1 per styrene monomer
  static const double A()               { return 104.15; }
} FibreParameters;

typedef struct BetheBloch {
  /// Bethe Bloch constant in MeV g-1 cm2 (for A=1gmol-1)
  static const double K() { return 0.307075; }
} BetheBlochParameters;

class KalmanTrack {
 public:
  KalmanTrack();

  virtual ~KalmanTrack() {}

  virtual void update_propagator(KalmanSite *old_site, KalmanSite *new_site) = 0;

  void calc_filtered_state(KalmanSite *a_site);

  void update_misaligments(KalmanSite *a_site, KalmanSite *old_site);

  void update_V(KalmanSite *a_site);

  void update_covariance(KalmanSite *a_site);

  void update_H(KalmanSite *a_site);

  void update_W(KalmanSite *a_site);

  void calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site);
  void set_residual(KalmanSite *a_site);

  TMatrixD solve_measurement_equation(TMatrixD a, TMatrixD s);

  void calc_system_noise(KalmanSite *old_site, KalmanSite *new_site);
  double BetheBlochStoppingPower(double p);
  void subtract_energy_loss(KalmanSite *old_site, KalmanSite *new_site);
  void calc_covariance(KalmanSite *old_site, KalmanSite *new_site);

  void update_back_transportation_matrix(KalmanSite *optimum_site, KalmanSite *smoothing_site);
  void smooth_back(KalmanSite *optimum_site, KalmanSite *smoothing_site);
  void prepare_for_smoothing(std::vector<KalmanSite> &sites);
  void exclude_site(KalmanSite *site);

  TMatrixD get_propagator() { return _F; }
  TMatrixD get_pull(KalmanSite *a_site);
  TMatrixD get_system_noise() { return _Q; }
  TMatrixD get_kalman_gain(KalmanSite *a_site);
  double get_chi2() const { return _chi2; }
  double get_ndf() const { return _ndf; }
  double get_P_value() const { return _P_value; }
  double get_tracker() const { return _tracker; }
  double get_mass() const { return _mass; }
  double get_momentum() const { return _momentum; }

  TMatrixD get_Q() const { return _Q; }
  void set_Q(TMatrixD Q) { _Q = Q; }
  // void get_site_properties(KalmanSite *site, double &thickess, double &density);

  void set_mass(double mass) { _mass = mass; }

  void set_momentum(double momentum) { _momentum = momentum; }

  void compute_chi2(const std::vector<KalmanSite> &sites);

 protected:
  // int _inversion_sanity;

  TMatrixD _H;

  TMatrixD _S;

  TMatrixD _V;

  TMatrixD _Q;

  TMatrixD _A;

  TMatrixD _F;

  TMatrixD _K;

  TMatrixD _W;

  double _chi2, _ndf, _P_value;

  int _n_parameters;

  int _tracker;

  double _mass, _momentum;

  double _active_radius; // = 150.;
};

} // ~namespace MAUS

#endif

