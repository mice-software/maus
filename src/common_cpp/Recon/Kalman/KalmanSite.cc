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

KalmanSite::KalmanSite(): _z(0.), _id(0), _chi2(0.),
                          _type(-1), _pitch(0.),
                          _direction((0., 0., 0.)),
                          _mc_pos((0., 0., 0.)),
                          _mc_mom((0., 0., 0.)) {
  initialise();
}

KalmanSite::~KalmanSite() {}

void KalmanSite::set_type(int type) {
  _type = type;
  // set the detector pitch (mm)
  if        ( type == 0 ) { // TOF0
    _pitch = 40.;
  } else if ( type == 1 ) { // TOF1
    _pitch = 60.;
  } else if ( type == 2 ) { // SciFi
    _pitch = (7.*0.427)/2.;
  }
}

KalmanSite::KalmanSite(const KalmanSite &site): _z(0.), _id(0), _chi2(0.),
                                                _type(-1), _pitch(0.),
                                                _direction((0., 0., 0.)),
                                                _mc_pos((0., 0., 0.)),
                                                _mc_mom((0., 0., 0.)) {
  initialise();

  _z = site.get_z();
  _id= site.get_id();
  _chi2 = site.get_chi2();
  _type = site.get_type();
  _pitch = site.get_pitch();
  _direction = site.get_direction();
  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();

  _a = site.get_a();
  _projected_a = site.get_projected_a();
  _smoothed_a  = site.get_smoothed_a();

  _projected_C = site.get_projected_covariance_matrix();
  _C = site.get_covariance_matrix();
  _smoothed_C  = site.get_smoothed_covariance_matrix();

  _v = site.get_measurement();

  _pull = site.get_pull();
  _residual = site.get_residual();
  _smoothed_residual = site.get_smoothed_residual();
  _covariance_residuals = site.get_covariance_residuals();

  _s = site.get_shifts();
  _Cov_s = site.get_S_covariance();
}

KalmanSite& KalmanSite::operator=(const KalmanSite &rhs) {
  if ( this == &rhs ) {
    return *this;
  }

  std::cerr << "operator = is called.\n";

  _z  = rhs.get_z();
  _id = rhs.get_id();
  _chi2  = rhs.get_chi2();
  _type  = rhs.get_type();
  _pitch = rhs.get_pitch();
  _direction = rhs.get_direction();
  _mc_pos = rhs.get_true_position();
  _mc_mom = rhs.get_true_momentum();

  _a = rhs.get_a();
  _projected_a = rhs.get_projected_a();
  _smoothed_a  = rhs.get_smoothed_a();

  _projected_C = rhs.get_projected_covariance_matrix();
  _C = rhs.get_covariance_matrix();
  _smoothed_C  = rhs.get_smoothed_covariance_matrix();

  _v = rhs.get_measurement();

  _pull     = rhs.get_pull();
  _residual = rhs.get_residual();
  _smoothed_residual    = rhs.get_smoothed_residual();
  _covariance_residuals = rhs.get_covariance_residuals();

  _s     = rhs.get_shifts();
  _Cov_s = rhs.get_S_covariance();

  return *this;
}

void KalmanSite::initialise() {
  // The state vector.
  _projected_a.ResizeTo(5, 1);
  _a.          ResizeTo(5, 1);
  _smoothed_a. ResizeTo(5, 1);

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

} // ~namespace MAUS
