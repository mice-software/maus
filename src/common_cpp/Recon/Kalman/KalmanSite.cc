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
                          _z(0.), _id(0), _chi2(0.),
                          _pitch(1.4945),
                          _direction((0., 0., 0.)),
                          _mc_pos((0., 0., 0.)),
                          _mc_mom((0., 0., 0.)) {
  initialise();
}

KalmanSite::~KalmanSite() {}

KalmanSite::KalmanSite(const KalmanSite &site): _current_state(Initialized),
                                                _z(0.), _id(0), _chi2(0.),
                                                _pitch(1.4945),
                                                _direction((0., 0., 0.)),
                                                _mc_pos((0., 0., 0.)),
                                                _mc_mom((0., 0., 0.)) {
  initialise();

  _z = site.get_z();
  _id= site.get_id();
  _chi2 = site.get_chi2();
  _pitch = site.get_pitch();
  _direction = site.get_direction();
  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();

  _projected_a = site.get_a(KalmanSite::Projected);
  _a           = site.get_a(KalmanSite::Filtered);
  _smoothed_a  = site.get_a(KalmanSite::Smoothed);
  _a_excl      = site.get_a(KalmanSite::Excluded);

  _projected_C = site.get_covariance_matrix(KalmanSite::Projected);
  _C           = site.get_covariance_matrix(KalmanSite::Filtered);
  _smoothed_C  = site.get_covariance_matrix(KalmanSite::Smoothed);

  _v = site.get_measurement();

  _pull                 = site.get_residual(KalmanSite::Projected);
  _residual             = site.get_residual(KalmanSite::Filtered);
  _smoothed_residual    = site.get_residual(KalmanSite::Smoothed);
  _covariance_residuals = site.get_covariance_residuals();

  _s = site.get_shifts();
  _Cov_s = site.get_S_covariance();

  _current_state = site.get_current_state();
}

KalmanSite& KalmanSite::operator=(const KalmanSite &rhs) {
  if ( this == &rhs ) {
    return *this;
  }

  _z  = rhs.get_z();
  _id = rhs.get_id();
  _chi2  = rhs.get_chi2();
  _pitch = rhs.get_pitch();
  _direction = rhs.get_direction();
  _mc_pos = rhs.get_true_position();
  _mc_mom = rhs.get_true_momentum();

  _projected_a = rhs.get_a(KalmanSite::Projected);
  _a           = rhs.get_a(KalmanSite::Filtered);
  _smoothed_a  = rhs.get_a(KalmanSite::Smoothed);
  _a_excl      = rhs.get_a(KalmanSite::Excluded);

  _projected_C = rhs.get_covariance_matrix(KalmanSite::Projected);
  _C           = rhs.get_covariance_matrix(KalmanSite::Filtered);
  _smoothed_C  = rhs.get_covariance_matrix(KalmanSite::Smoothed);

  _v = rhs.get_measurement();

  _pull                 = rhs.get_residual(KalmanSite::Projected);
  _residual             = rhs.get_residual(KalmanSite::Filtered);
  _smoothed_residual    = rhs.get_residual(KalmanSite::Smoothed);
  _covariance_residuals = rhs.get_covariance_residuals();

  _s     = rhs.get_shifts();
  _Cov_s = rhs.get_S_covariance();

  _current_state = rhs.get_current_state();

  return *this;
}

void KalmanSite::initialise() {
  // The state vector.
  _projected_a.ResizeTo(5, 1);
  _a.          ResizeTo(5, 1);
  _smoothed_a. ResizeTo(5, 1);
  _a_excl.     ResizeTo(5, 1);

  // The covariance matrix.
  _projected_C.ResizeTo(5, 5);
  _C.          ResizeTo(5, 5);
  _smoothed_C. ResizeTo(5, 5);

  // The measurement.
  _v.          ResizeTo(2, 1);

  // The residuals.
  _pull.                ResizeTo(2, 1);
  _residual.            ResizeTo(2, 1);
  _smoothed_residual.   ResizeTo(2, 1);
  _covariance_residuals.ResizeTo(2, 2);

  // The misalignments.
  _s.    ResizeTo(3, 1);
  _Cov_s.ResizeTo(3, 3);
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
      _a_excl = a;
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
      return _a_excl;
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
      _smoothed_C = C;
      // add this.
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
      _measured_shift = residual;
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
      return _measured_shift;
  }
}

} // ~namespace MAUS
