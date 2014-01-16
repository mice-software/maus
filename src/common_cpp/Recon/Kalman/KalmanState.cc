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

#include "src/common_cpp/Recon/Kalman/KalmanState.hh"

namespace MAUS {

KalmanState::KalmanState(): _current_state(Initialized),
                          _z(0.),
                          _id(-1),
                          _f_chi2(0.),
                          _s_chi2(0.),
                          _direction(ThreeVector()),
                          _mc_pos(ThreeVector()),
                          _mc_mom(ThreeVector()) {}

KalmanState::~KalmanState() {}

KalmanState::KalmanState(const KalmanState &site): _current_state(Initialized),
                                                _z(0.),
                                                _id(-1),
                                                _f_chi2(0.),
                                                _s_chi2(0.),
                                                _direction(ThreeVector()),
                                                _mc_pos(ThreeVector()),
                                                _mc_mom(ThreeVector()) {
  int dim = site.a(KalmanState::Projected).GetNrows();
  Initialise(dim);

  _z = site.z();
  _id= site.id();
  _f_chi2 = site.chi2(KalmanState::Filtered);
  _s_chi2 = site.chi2(KalmanState::Smoothed);
  _direction = site.direction();
  _mc_pos = site.true_position();
  _mc_mom = site.true_momentum();

  _projected_a = site.a(KalmanState::Projected);
  _a           = site.a(KalmanState::Filtered);
  _smoothed_a  = site.a(KalmanState::Smoothed);
  _a_excluded  = site.a(KalmanState::Excluded);

  _projected_C = site.covariance_matrix(KalmanState::Projected);
  _C           = site.covariance_matrix(KalmanState::Filtered);
  _smoothed_C  = site.covariance_matrix(KalmanState::Smoothed);
  _C_excluded  = site.covariance_matrix(KalmanState::Excluded);

  _v = site.measurement();

  _pull                 = site.residual(KalmanState::Projected);
  _residual             = site.residual(KalmanState::Filtered);
  _smoothed_residual    = site.residual(KalmanState::Smoothed);
  _excluded_residual    = site.residual(KalmanState::Excluded);
  _covariance_residual          = site.covariance_residual(KalmanState::Filtered);
  _covariance_smoothed_residual = site.covariance_residual(KalmanState::Smoothed);
  _covariance_excluded_residual = site.covariance_residual(KalmanState::Excluded);

  _input_shift = site.input_shift();
  _input_shift_covariance = site.input_shift_covariance();

  _shift = site.shift();
  _shift_covariance = site.shift_covariance();

  _current_state = site.current_state();
}

KalmanState& KalmanState::operator=(const KalmanState &rhs) {
  if ( this == &rhs ) {
    return *this;
  }
  int dim = rhs.a(KalmanState::Projected).GetNrows();
  this->Initialise(dim);

  _z  = rhs.z();
  _id = rhs.id();
  _f_chi2 = rhs.chi2(KalmanState::Filtered);
  _s_chi2 = rhs.chi2(KalmanState::Smoothed);
  _direction = rhs.direction();
  _mc_pos = rhs.true_position();
  _mc_mom = rhs.true_momentum();

  _projected_a = rhs.a(KalmanState::Projected);
  _a           = rhs.a(KalmanState::Filtered);
  _smoothed_a  = rhs.a(KalmanState::Smoothed);
  _a_excluded  = rhs.a(KalmanState::Excluded);

  _projected_C = rhs.covariance_matrix(KalmanState::Projected);
  _C           = rhs.covariance_matrix(KalmanState::Filtered);
  _smoothed_C  = rhs.covariance_matrix(KalmanState::Smoothed);
  _C_excluded  = rhs.covariance_matrix(KalmanState::Excluded);

  _v = rhs.measurement();

  _pull                 = rhs.residual(KalmanState::Projected);
  _residual             = rhs.residual(KalmanState::Filtered);
  _smoothed_residual    = rhs.residual(KalmanState::Smoothed);
  _excluded_residual    = rhs.residual(KalmanState::Excluded);
  _covariance_residual          = rhs.covariance_residual(KalmanState::Filtered);
  _covariance_smoothed_residual = rhs.covariance_residual(KalmanState::Smoothed);
  _covariance_excluded_residual = rhs.covariance_residual(KalmanState::Excluded);

  _input_shift = rhs.input_shift();
  _input_shift_covariance = rhs.input_shift_covariance();

  _shift = rhs.shift();
  _shift_covariance = rhs.shift_covariance();

  _current_state = rhs.current_state();

  return *this;
}

void KalmanState::Initialise(int dim) {
  // The state vector.
  _projected_a.ResizeTo(dim, 1);

  _a.          ResizeTo(dim, 1);
  _smoothed_a. ResizeTo(dim, 1);
  _a_excluded. ResizeTo(dim, 1);

  // The covariance matrix.
  _projected_C.ResizeTo(dim, dim);
  _C.          ResizeTo(dim, dim);
  _smoothed_C. ResizeTo(dim, dim);
  _C_excluded. ResizeTo(dim, dim);

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

void KalmanState::MoveToGlobalFrame(ThreeVector ref_pos) {
  ThreeVector pos(_smoothed_a(0, 0), _smoothed_a(2, 0), _z);
  ThreeVector mom(_smoothed_a(1, 0), _smoothed_a(3, 0), 1.);

  pos += ref_pos;

  int sign = 1;
  if ( _id < 0 ) sign = -1;
  _smoothed_a(0, 0) = sign*pos.x();
  _smoothed_a(1, 0) = sign*mom.x();
  _smoothed_a(2, 0) = pos.y();
  _z = pos.z();
}

void KalmanState::set_a(TMatrixD a, State kalman_state) {
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
            "KalmanState::set_a"));
  }
}

TMatrixD KalmanState::a(State desired_state) const {
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
            "KalmanState::get_a"));
  }
}

void KalmanState::set_covariance_matrix(TMatrixD C, State kalman_state) {
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
            "KalmanState::set_covariance_matrix"));
  }
}

TMatrixD KalmanState::covariance_matrix(State desired_state) const {
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
            "KalmanState::get_covariance_matrix"));
  }
}

void KalmanState::set_residual(TMatrixD residual, State kalman_state) {
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
            "KalmanState::set_residual"));
  }
}

TMatrixD KalmanState::residual(State desired_state) const {
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
            "KalmanState::get_residual"));
  }
}

void KalmanState::set_covariance_residual(TMatrixD C, State kalman_state) {
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
            "KalmanState::set_covariance_residual"));
  }
}

TMatrixD KalmanState::covariance_residual(State st) const {
  switch ( st ) {
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
            "KalmanState::get_covariance_matrix"));
  }
}

void KalmanState::set_chi2(double chi2, State kalman_state) {
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
            "KalmanState::set_chi2"));
  }
}

double KalmanState::chi2(State desired_state) const {
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
            "KalmanState::get_chi2"));
  }
}

} // ~namespace MAUS
