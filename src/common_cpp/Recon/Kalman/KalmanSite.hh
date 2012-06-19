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


#ifndef KALMANSITE_HH
#define KALMANSITE_HH

// C headers
#include <assert.h>

// C++ headers
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"

class KalmanSite {
 public:
  KalmanSite();

  ~KalmanSite();

  KalmanSite(const KalmanSite &site);

  KalmanSite& operator=(const KalmanSite& site);

  /// Assigns PROJECTED state vector at the site.
  void set_projected_a(TMatrixD projected_a) { _projected_a = projected_a; }

  /// Returns PROJECTED state vector at the site.
  TMatrixD get_projected_a() const { return _projected_a; }

  /// Assigns state vector at the site.
  void set_a(TMatrixD a) { _a = a; }

  /// Returns state vector at the site.
  TMatrixD get_a() const { return _a; }

  /// Assigns PROJECTED state vector at the site.
  void set_smoothed_a(TMatrixD smoothed_a) { _smoothed_a = smoothed_a; }

  /// Returns PROJECTED state vector at the site.
  TMatrixD get_smoothed_a() const { return _smoothed_a; }

  /// Sets the covariance matrix.
  void set_covariance_matrix(TMatrixD C) { _C = C; }

  /// Returns the covariance matrix.
  TMatrixD get_covariance_matrix() const { return _C; }

  void set_projected_covariance_matrix(TMatrixD Cp) { _projected_C = Cp; }

  TMatrixD get_projected_covariance_matrix() const { return _projected_C; }

  void set_smoothed_covariance_matrix(TMatrixD C) { _smoothed_C = C; }

  TMatrixD get_smoothed_covariance_matrix() const { return _smoothed_C; }

  void set_measurement(double alpha) { _v(0, 0) = alpha;
                                       _v(1, 0) = 0.0;
                                       _alpha   = alpha; }

  TMatrixD get_measurement() const { return _v; }

  double get_alpha() const { return _alpha; }

  void set_direction(Hep3Vector dir) { _direction = dir; }

  Hep3Vector get_direction() const { return _direction; }

  void set_z(double z) { _z = z; }

  double get_z() const { return _z; }

  void set_id(int id) { _id = id; }

  int get_id() const { return _id; }

  void set_projected_alpha(double alpha) { _alpha_projected = alpha; }

  double get_projected_alpha() const { return _alpha_projected; }

  void set_residual_x(double residual_x) { _residual_x = residual_x; }

  double get_residual_x() const { return _residual_x; }

  void set_residual_y(double residual_y) { _residual_y = residual_y; }

  double get_residual_y() const { return _residual_y; }

  /////////////////////////////////////
  void set_true_momentum(Hep3Vector mc_mom) { _mc_mom = mc_mom; }

  Hep3Vector get_true_momentum() const { return _mc_mom; }

  void set_true_position(Hep3Vector mc_pos) { _mc_pos = mc_pos; }

  Hep3Vector get_true_position() const { return _mc_pos; }
  /////////////////////////////////////

 private:
  Hep3Vector _mc_pos;

  Hep3Vector _mc_mom;

  /// The state vector.
  TMatrixD _a;

  /// The projected state.
  TMatrixD _projected_a;

  /// The smoothed state.
  TMatrixD _smoothed_a;

  /// The covariance matrix.
  TMatrixD _C;

  /// The projected cov matrix.
  TMatrixD _projected_C;

  /// The projected cov matrix.
  TMatrixD _smoothed_C;

  /// The measurement.
  TMatrixD _v;

  double _z, _alpha, _alpha_projected;

  int _id;

  Hep3Vector _direction;

  /// The residual at this site. (filtered-meas)
  double _residual_x, _residual_y;

  // enum state { INVALID = 0, PROJECTED, FILTERED, SMOOTHED };

  // state _site_state;
};

#endif
