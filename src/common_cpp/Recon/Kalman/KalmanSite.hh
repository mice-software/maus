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

#include "src/common_cpp/DataStructure/SciFiCluster.hh"
// #include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"

namespace MAUS {

class KalmanSite {
 public:
  KalmanSite();

  ~KalmanSite();

  KalmanSite(const KalmanSite &site);

  KalmanSite& operator=(const KalmanSite& site);

  /// Assigns PROJECTED state vector at the site.
  void set_projected_a(TMatrixD projected_a) { _projected_a = projected_a; }
  void set_a(TMatrixD a)                     { _a = a; }
  void set_smoothed_a(TMatrixD smoothed_a)   { _smoothed_a = smoothed_a; }

  void set_projected_covariance_matrix(TMatrixD Cp) { _projected_C = Cp; }
  void set_covariance_matrix(TMatrixD C)            { _C = C; }
  void set_smoothed_covariance_matrix(TMatrixD C)   { _smoothed_C = C; }

  void set_measurement(double alpha)                { _v(0, 0) = alpha;
                                                      _v(1, 0) = 0.0;
                                                      _alpha   = alpha; }
  void set_smoothed_alpha(double alpha_smoothed)   { _alpha_smoothed = alpha_smoothed; }
  void set_projected_alpha(double alpha)           { _alpha_projected = alpha; }

  void set_direction(CLHEP::Hep3Vector dir)        { _direction = dir; }
  void set_z(double z)                             { _z = z; }
  void set_id(int id)                              { _id = id; }
  void set_pitch(double factor)                    { _pitch = factor; }
  void set_type(int type);

  void set_residual_x(double residual_x)           { _residual_x = residual_x; }
  void set_residual_y(double residual_y)           { _residual_y = residual_y; }
  void set_chi2(double chi2)                       { _chi2 = chi2; }

  void set_shifts(TMatrixD shift)                  { _s = shift; }
  void set_rotations(TMatrixD rot)                 { _r = rot; }
  void set_R_covariance(TMatrixD cov_r)            { _Cov_r = cov_r; }
  void set_S_covariance(TMatrixD cov_s)            { _Cov_s = cov_s; }
  void set_true_momentum(CLHEP::Hep3Vector mc_mom) { _mc_mom = mc_mom; }
  void set_true_position(CLHEP::Hep3Vector mc_pos) { _mc_pos = mc_pos; }

  /// Returns PROJECTED state vector at the site.
  TMatrixD get_projected_a()                 const { return _projected_a; }
  TMatrixD get_a()                           const { return _a; }
  TMatrixD get_smoothed_a()                  const { return _smoothed_a; }
  TMatrixD get_covariance_matrix()           const { return _C; }
  TMatrixD get_projected_covariance_matrix() const { return _projected_C; }
  TMatrixD get_smoothed_covariance_matrix()  const { return _smoothed_C; }
  TMatrixD get_measurement()                 const { return _v; }

  double get_alpha()                         const { return _alpha; }
  double get_smoothed_alpha()                const { return _alpha_smoothed; }
  double get_z()                             const { return _z; }
  int get_id()                               const { return _id; }
  int get_type()                             const { return _type; }
  double get_projected_alpha()               const { return _alpha_projected; }
  double get_residual_x()                    const { return _residual_x; }
  double get_residual_y()                    const { return _residual_y; }
  double get_chi2()                          const { return _chi2; }
  double get_pitch()                         const { return _pitch; }
  CLHEP::Hep3Vector get_direction()          const { return _direction; }
  CLHEP::Hep3Vector get_true_momentum()      const { return _mc_mom; }
  CLHEP::Hep3Vector get_true_position()      const { return _mc_pos; }
  // Misalignments
  TMatrixD get_shifts()       const { return _s; }
  TMatrixD get_rotations()    const { return _r; }
  TMatrixD get_R_covariance() const { return _Cov_r; }
  TMatrixD get_S_covariance() const { return _Cov_s; }

 private:
  CLHEP::Hep3Vector _mc_pos;

  CLHEP::Hep3Vector _mc_mom;

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

  /// shifts
  TMatrixD _s;
  TMatrixD _Cov_s;

  /// rotations
  TMatrixD _r;
  TMatrixD _Cov_r;

  double _z, _alpha, _alpha_projected, _alpha_smoothed;

  double _chi2;

  int _id;

  int _type;

  CLHEP::Hep3Vector _direction;

  /// The residual at this site. (filtered-meas)
  double _residual_x, _residual_y;

  double _pitch;
};

} // ~namespace MAUS

#endif
