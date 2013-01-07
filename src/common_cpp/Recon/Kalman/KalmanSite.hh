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
#include <iostream>

// C++ headers
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace MAUS {

class KalmanSite {
 public:
  KalmanSite();

  ~KalmanSite();

  KalmanSite(const KalmanSite &site);

  KalmanSite& operator=(const KalmanSite& site);

  void initialise();

  /// SETTERS
  ///
  /// The state vector at the site.
  void set_projected_a(TMatrixD projected_a) { _projected_a = projected_a; }
  void set_a(TMatrixD a)                     { _a = a; }
  void set_smoothed_a(TMatrixD smoothed_a)   { _smoothed_a = smoothed_a; }
  void set_excluded_state(TMatrixD a_excl)   { _a_excl = a_excl; }
  /// The covariance of the state vector.
  void set_projected_covariance_matrix(TMatrixD Cp) { _projected_C = Cp; }
  void set_covariance_matrix(TMatrixD C)            { _C = C; }
  void set_smoothed_covariance_matrix(TMatrixD C)   { _smoothed_C = C; }
  /// Sets the measurement vector.
  void set_measurement(double alpha)                { _v(0, 0) = alpha;
                                                      _v(1, 0) = 0.0; }
  /// The properties of the site.
  void set_direction(CLHEP::Hep3Vector dir)        { _direction = dir; }
  void set_z(double z)                             { _z = z; }
  void set_id(int id)                              { _id = id; }
  void set_pitch(double factor)                    { _pitch = factor; }
  void set_type(int type);
  /// Residuals.
  void set_pull(TMatrixD pull)                     { _pull = pull; }
  void set_residual(TMatrixD res)                  { _residual = res; }
  void set_smoothed_residual(TMatrixD res)         { _smoothed_residual = res; }
  /// The covariance of the innovation.
  void set_covariance_residuals(TMatrixD cov_res)  { _covariance_residuals = cov_res; }
  void set_chi2(double chi2)                       { _chi2 = chi2; }
  /// Misalignments matrices and vectors.
  void set_shifts(TMatrixD shift)                  { _s = shift; }
  void set_S_covariance(TMatrixD cov_s)            { _Cov_s = cov_s; }

  void set_true_momentum(CLHEP::Hep3Vector mc_mom) { _mc_mom = mc_mom; }
  void set_true_position(CLHEP::Hep3Vector mc_pos) { _mc_pos = mc_pos; }

  /// GETTERS
  /// The state vector and its covariance.
  TMatrixD get_projected_a()                 const { return _projected_a; }
  TMatrixD get_a()                           const { return _a; }
  TMatrixD get_smoothed_a()                  const { return _smoothed_a; }
  TMatrixD get_a_excl()                      const { return _a_excl; }
  TMatrixD get_covariance_matrix()           const { return _C; }
  TMatrixD get_projected_covariance_matrix() const { return _projected_C; }
  TMatrixD get_smoothed_covariance_matrix()  const { return _smoothed_C; }
  /// The measurement.
  TMatrixD get_measurement()                 const { return _v; }
  /// The site properties.
  double get_z()                             const { return _z; }
  int get_id()                               const { return _id; }
  int get_type()                             const { return _type; }
  double get_chi2()                          const { return _chi2; }
  double get_pitch()                         const { return _pitch; }
  CLHEP::Hep3Vector get_direction()          const { return _direction; }
  /// The residuals and its covariance.
  TMatrixD get_pull()                        const { return _pull; }
  TMatrixD get_residual()                    const { return _residual; }
  TMatrixD get_smoothed_residual()           const { return _smoothed_residual; }
  TMatrixD get_covariance_residuals()        const { return _covariance_residuals; }
  /// Misalignments and their covariance.
  TMatrixD get_shifts()       const { return _s; }
  TMatrixD get_S_covariance() const { return _Cov_s; }

  CLHEP::Hep3Vector get_true_momentum()      const { return _mc_mom; }
  CLHEP::Hep3Vector get_true_position()      const { return _mc_pos; }

 private:
  /// Z placement of the site (mm).
  double _z;

  /// Site id.
  int _id;

  /// The Chi2 at this site.
  double _chi2;

  /// Type of detector (SciFi, TOF, Cherenkov).
  int _type;

  /// Pitch of the detector.
  double _pitch;

  /// Orientation of the measuring plane.
  CLHEP::Hep3Vector _direction;

  CLHEP::Hep3Vector _mc_pos;
  CLHEP::Hep3Vector _mc_mom;

  /// The state vector.
  TMatrixD _projected_a;
  TMatrixD _a;
  TMatrixD _smoothed_a;
  TMatrixD _a_excl;

  /// The covariance matrix.
  TMatrixD _projected_C;
  TMatrixD _C;
  TMatrixD _smoothed_C;

  /// The measurement.
  TMatrixD _v;

  /// The residuals.
  TMatrixD _pull;
  TMatrixD _residual;
  TMatrixD _smoothed_residual;

  TMatrixD _covariance_residuals;

  /// The misalignments.
  TMatrixD _s;
  TMatrixD _Cov_s;
};

} // ~namespace MAUS

#endif
