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

#include "Interface/Squeal.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace MAUS {

class KalmanSite {
 public:
  KalmanSite();

  ~KalmanSite();

  KalmanSite(const KalmanSite &site);

  KalmanSite& operator=(const KalmanSite& site);

  enum State { Initialized = -1,
               Projected,
               Filtered,
               Smoothed,
               Excluded };

  /// Initialises member matrices.
  void initialise();

  /// SETTERS
  ///
  /// The state vector at the site.
  void set_a(TMatrixD a, State current_state);
  void set_covariance_matrix(TMatrixD C, State current_state);
  void set_residual(TMatrixD residual, State current_state);


  TMatrixD get_a(State desired_state) const;
  TMatrixD get_covariance_matrix(State desired_state) const;
  TMatrixD get_residual(State desired_state) const;

  void set_covariance_residual(TMatrixD C, State kalman_state);
  TMatrixD get_covariance_residual(State desired_state) const;
  /// The residuals and its covariance.

  /// Shifts.
  void set_input_shift(TMatrixD input_shift) { _input_shift = input_shift; }
  TMatrixD get_input_shift() const { return _input_shift; }
  void set_input_shift_covariance(TMatrixD input_covariance) {
                _input_shift_covariance = input_covariance; }

  TMatrixD get_input_shift_covariance() const { return _input_shift_covariance; }

  void set_shift(TMatrixD shift) { _shift = shift; }
  TMatrixD get_shift() const { return _shift; }
  TMatrixD get_shift_covariance() const { return _shift_covariance; }
  void set_shift_covariance(TMatrixD shift_covariance) {
                _shift_covariance = shift_covariance; }

  /// Sets the measurement vector.
  void set_measurement(double alpha)                { _v(0, 0) = alpha;
                                                      _v(1, 0) = 0.0; }
  /// The properties of the site.
  void set_direction(CLHEP::Hep3Vector dir)        { _direction = dir; }
  void set_z(double z)                             { _z = z; }
  void set_id(int id)                              { _id = id; }
  // void set_pitch(double factor)                    { _pitch = factor; }

  /// The covariance of the innovation.
  void set_chi2(double chi2, State kalman_state);
  double get_chi2(State desired_state) const;

  /// The measurement.
  TMatrixD get_measurement()                 const { return _v; }
  /// The site properties.
  double get_z()                             const { return _z; }
  int get_id()                               const { return _id; }


  // double get_pitch()                         const { return _pitch; }
  CLHEP::Hep3Vector get_direction()          const { return _direction; }

  State get_current_state() const { return _current_state; }
  void set_current_state(State kalman_state) { _current_state = kalman_state; }

  CLHEP::Hep3Vector get_true_momentum()      const { return _mc_mom; }
  CLHEP::Hep3Vector get_true_position()      const { return _mc_pos; }
  void set_true_momentum(CLHEP::Hep3Vector mc_mom) { _mc_mom = mc_mom; }
  void set_true_position(CLHEP::Hep3Vector mc_pos) { _mc_pos = mc_pos; }

 private:
  /// State of the site.
  State _current_state;
  /// Z placement of the site (mm).
  double _z;
  /// Site id.
  int _id;
  /// The Chi2 at this site.
  double _f_chi2;
  double _s_chi2;
  /// Pitch of the detector.
  // double _pitch;
  /// Orientation of the measuring plane.
  CLHEP::Hep3Vector _direction;

  /// The state vector.
  TMatrixD _projected_a;
  TMatrixD _a;
  TMatrixD _smoothed_a;
  TMatrixD _a_excluded;

  /// The covariance matrix.
  TMatrixD _projected_C;
  TMatrixD _C;
  TMatrixD _smoothed_C;
  TMatrixD _C_excluded;

  /// The measurement.
  TMatrixD _v;

  /// The residuals.
  TMatrixD _pull;
  TMatrixD _residual;
  TMatrixD _smoothed_residual;
  TMatrixD _excluded_residual;

  TMatrixD _covariance_residual;
  TMatrixD _covariance_smoothed_residual;
  TMatrixD _covariance_excluded_residual;

  /// Alignment.
  TMatrixD _input_shift;
  TMatrixD _shift;

  TMatrixD _input_shift_covariance;
  TMatrixD _shift_covariance;

  CLHEP::Hep3Vector _mc_pos;
  CLHEP::Hep3Vector _mc_mom;
};

} // ~namespace MAUS

#endif
