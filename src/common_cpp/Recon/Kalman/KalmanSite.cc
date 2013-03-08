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

#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

namespace MAUS {

KalmanSite::KalmanSite(): _current_state(Initialized),
                          _z(0.),
                          _id(-1),
                          _f_chi2(0.),
                          _s_chi2(0.),
                          _direction(CLHEP::Hep3Vector()),
                          _mc_pos(CLHEP::Hep3Vector()),
                          _mc_mom(CLHEP::Hep3Vector()) {
  initialise();
}

KalmanSite::~KalmanSite() {}

KalmanSite::KalmanSite(const KalmanSite &site): _current_state(Initialized),
                                                _z(0.),
                                                _id(-1),
                                                _f_chi2(0.),
                                                _s_chi2(0.),
                                                _direction(CLHEP::Hep3Vector()),
                                                _mc_pos(CLHEP::Hep3Vector()),
                                                _mc_mom(CLHEP::Hep3Vector()) {
  initialise();

  _z = site.get_z();
  _id= site.get_id();
  _f_chi2 = site.get_chi2(KalmanSite::Filtered);
  _s_chi2 = site.get_chi2(KalmanSite::Smoothed);
  _direction = site.get_direction();
  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();

  _projected_a = site.get_a(KalmanSite::Projected);
  _a           = site.get_a(KalmanSite::Filtered);
  _smoothed_a  = site.get_a(KalmanSite::Smoothed);
  _a_excluded  = site.get_a(KalmanSite::Excluded);

  _projected_C = site.get_covariance_matrix(KalmanSite::Projected);
  _C           = site.get_covariance_matrix(KalmanSite::Filtered);
  _smoothed_C  = site.get_covariance_matrix(KalmanSite::Smoothed);
  _C_excluded  = site.get_covariance_matrix(KalmanSite::Excluded);

  _v = site.get_measurement();

  _pull                 = site.get_residual(KalmanSite::Projected);
  _residual             = site.get_residual(KalmanSite::Filtered);
  _smoothed_residual    = site.get_residual(KalmanSite::Smoothed);
  _excluded_residual    = site.get_residual(KalmanSite::Excluded);
  _covariance_residual          = site.get_covariance_residual(KalmanSite::Filtered);
  _covariance_smoothed_residual = site.get_covariance_residual(KalmanSite::Smoothed);
  _covariance_excluded_residual = site.get_covariance_residual(KalmanSite::Excluded);

  _input_shift = site.get_input_shift();
  _input_shift_covariance = site.get_input_shift_covariance();

  _shift = site.get_shift();
  _shift_covariance = site.get_shift_covariance();

  _current_state = site.get_current_state();
}

KalmanSite& KalmanSite::operator=(const KalmanSite &rhs) {
  if ( this == &rhs ) {
    return *this;
  }

  _z  = rhs.get_z();
  _id = rhs.get_id();
  _f_chi2 = rhs.get_chi2(KalmanSite::Filtered);
  _s_chi2 = rhs.get_chi2(KalmanSite::Smoothed);
  _direction = rhs.get_direction();
  _mc_pos = rhs.get_true_position();
  _mc_mom = rhs.get_true_momentum();

  _projected_a = rhs.get_a(KalmanSite::Projected);
  _a           = rhs.get_a(KalmanSite::Filtered);
  _smoothed_a  = rhs.get_a(KalmanSite::Smoothed);
  _a_excluded  = rhs.get_a(KalmanSite::Excluded);

  _projected_C = rhs.get_covariance_matrix(KalmanSite::Projected);
  _C           = rhs.get_covariance_matrix(KalmanSite::Filtered);
  _smoothed_C  = rhs.get_covariance_matrix(KalmanSite::Smoothed);
  _C_excluded  = rhs.get_covariance_matrix(KalmanSite::Excluded);

  _v = rhs.get_measurement();

  _pull                 = rhs.get_residual(KalmanSite::Projected);
  _residual             = rhs.get_residual(KalmanSite::Filtered);
  _smoothed_residual    = rhs.get_residual(KalmanSite::Smoothed);
  _excluded_residual    = rhs.get_residual(KalmanSite::Excluded);
  _covariance_residual          = rhs.get_covariance_residual(KalmanSite::Filtered);
  _covariance_smoothed_residual = rhs.get_covariance_residual(KalmanSite::Smoothed);
  _covariance_excluded_residual = rhs.get_covariance_residual(KalmanSite::Excluded);

  _input_shift = rhs.get_input_shift();
  _input_shift_covariance = rhs.get_input_shift_covariance();

  _shift = rhs.get_shift();
  _shift_covariance = rhs.get_shift_covariance();

  _current_state = rhs.get_current_state();

  return *this;
}

void KalmanSite::initialise() {
  // The state vector.
  _projected_a.ResizeTo(5, 1);
  _a.          ResizeTo(5, 1);
  _smoothed_a. ResizeTo(5, 1);
  _a_excluded. ResizeTo(5, 1);

  // The covariance matrix.
  _projected_C.ResizeTo(5, 5);
  _C.          ResizeTo(5, 5);
  _smoothed_C. ResizeTo(5, 5);
  _C_excluded. ResizeTo(5, 5);

  // The measurement.
  _v.          ResizeTo(2, 1);

  // The residuals.
  _pull.                ResizeTo(2, 1);
  _residual.            ResizeTo(2, 1);
  _smoothed_residual.   ResizeTo(2, 1);
  _excluded_residual.   ResizeTo(2, 1);

  _covariance_residual.ResizeTo(2, 2);
  _covariance_smoothed_residual.ResizeTo(2, 2);
  _covariance_excluded_residual.ResizeTo(2, 2);

  // The misalignments.
  _input_shift.ResizeTo(3, 1);
  _input_shift_covariance.ResizeTo(3, 3);

  _shift.ResizeTo(3, 1);
  _shift_covariance.ResizeTo(3, 3);
}

void KalmanSite::set_a(TMatrixD a, State kalman_state) {
  switch ( kalman_state ) {
    case(Projected) :
      _projected_a = a;
      break;
    case(Filtered) :
      _a = a;
      break;
    case(Smoothed) :
      _smoothed_a = a;
      break;
    case(Excluded) :
      _a_excluded = a;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::set_a"));
  }
}

TMatrixD KalmanSite::get_a(State desired_state) const {
  switch ( desired_state ) {
    case(Projected) :
      return _projected_a;
      break;
    case(Filtered) :
      return _a;
      break;
    case(Smoothed) :
      return _smoothed_a;
      break;
    case(Excluded) :
      return _a_excluded;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::get_a"));
  }
}

void KalmanSite::set_covariance_matrix(TMatrixD C, State kalman_state) {
  switch ( kalman_state ) {
    case(Projected) :
      _projected_C = C;
      break;
    case(Filtered) :
      _C = C;
      break;
    case(Smoothed) :
      _smoothed_C = C;
      break;
    case(Excluded) :
      _C_excluded = C;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::set_covariance_matrix"));
  }
}

TMatrixD KalmanSite::get_covariance_matrix(State desired_state) const {
  switch ( desired_state ) {
    case(Projected) :
      return _projected_C;
      break;
    case(Filtered) :
      return _C;
      break;
    case(Smoothed) :
      return _smoothed_C;
      break;
    case(Excluded) :
      return _smoothed_C;
      // add this.
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::get_covariance_matrix"));
  }
}

void KalmanSite::set_residual(TMatrixD residual, State kalman_state) {
  switch ( kalman_state ) {
    case(Projected) :
      _pull = residual;
      break;
    case(Filtered) :
      _residual = residual;
      break;
    case(Smoothed) :
      _smoothed_residual = residual;
      break;
    case(Excluded) :
      _excluded_residual = residual;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::set_residual"));
  }
}

TMatrixD KalmanSite::get_residual(State desired_state) const {
  switch ( desired_state ) {
    case(Projected) :
      return _pull;
      break;
    case(Filtered) :
      return _residual;
      break;
    case(Smoothed) :
      return _smoothed_residual;
      break;
    case(Excluded) :
      return _excluded_residual;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::get_residual"));
  }
}

void KalmanSite::set_covariance_residual(TMatrixD C, State kalman_state) {
  switch ( kalman_state ) {
    case(Filtered) :
      _covariance_residual = C;
      break;
    case(Smoothed) :
      _covariance_smoothed_residual = C;
      break;
    case(Excluded) :
      _covariance_excluded_residual = C;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::set_covariance_residual"));
  }
}

TMatrixD KalmanSite::get_covariance_residual(State desired_state) const {
  switch ( desired_state ) {
    case(Filtered) :
      return _covariance_residual;
      break;
    case(Smoothed) :
      return _covariance_smoothed_residual;
      break;
    case(Excluded) :
      return _covariance_excluded_residual;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::get_covariance_matrix"));
  }
}

void KalmanSite::set_chi2(double chi2, State kalman_state) {
  switch ( kalman_state ) {
    case(Filtered) :
      _f_chi2 = chi2;
      break;
    case(Smoothed) :
      _s_chi2 = chi2;
      break;
    case(Excluded) :
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::set_chi2"));
  }
}

double KalmanSite::get_chi2(State desired_state) const {
  switch ( desired_state ) {
    case(Filtered) :
      return _f_chi2;
      break;
    case(Smoothed) :
      return _s_chi2;
      break;
    default :
      throw(Squeal(Squeal::recoverable,
            "Bad request.",
            "KalmanSite::get_chi2"));
  }
}

} // ~namespace MAUS
