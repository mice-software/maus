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

KalmanSite::KalmanSite(): _z(0.), _id(0), _type(-1), _pitch(0.),
                          _alpha(0.), _alpha_projected(0.), _alpha_smoothed(0.),
                          _residual_x(0.), _residual_y(0.),
                          _direction((0., 0., 0.)), _chi2(0.) {
  _projected_C.ResizeTo(5, 5);
  _C.ResizeTo(5, 5);
  _smoothed_C.ResizeTo(5, 5);

  _projected_a.ResizeTo(5, 1);
  _a.ResizeTo(5, 1);
  _smoothed_a.ResizeTo(5, 1);

  _v.ResizeTo(2, 1);
  _s.ResizeTo(3, 1);
  _r.ResizeTo(3, 1);

  _Cov_s.ResizeTo(3, 3);
  _Cov_r.ResizeTo(3, 3);

  _mc_pos = (0., 0., 0.);
  _mc_mom = (0., 0., 0.);
}

KalmanSite::~KalmanSite() {}

void KalmanSite::set_type(int type) {
  _type = type;
  // set the detector pitch (mm)
  if ( type == 0 ) { // TOF0
    _pitch = 40.;
  } else if ( type == 1 ) { // TOF1
    _pitch = 60.;
  } else if ( type == 2 ) { // SS
    _pitch = (7.*0.427)/2.;
  }
}

KalmanSite::KalmanSite(const KalmanSite &site): _z(0.), _alpha(0.), _alpha_projected(0.), _id(0),
                                               _type(-1), _pitch(0.),
                          _residual_x(0.), _residual_y(0.), _direction((0., 0., 0.)),
                          _chi2(0.), _alpha_smoothed(0.) {
  _a.ResizeTo(5, 1);
  _C.ResizeTo(5, 5);
  _v.ResizeTo(2, 1);
  _s.ResizeTo(3, 1);
  _r.ResizeTo(3, 1);
  _Cov_s.ResizeTo(3, 3);
  _Cov_r.ResizeTo(3, 3);
  _projected_C.ResizeTo(5, 5);
  _smoothed_C.ResizeTo(5, 5);
  _projected_a.ResizeTo(5, 1);
  _smoothed_a.ResizeTo(5, 1);

  _a = site.get_a();
  _C = site.get_covariance_matrix();
  _v = site.get_measurement();
  _s = site.get_shifts();
  _r = site.get_rotations();
  _alpha = _v(0, 0);
  _projected_C = site.get_projected_covariance_matrix();
  _smoothed_C  = site.get_smoothed_covariance_matrix();
  _Cov_s = site.get_S_covariance();
  _Cov_r = site.get_R_covariance();
  _projected_a = site.get_projected_a();
  _smoothed_a  = site.get_smoothed_a();
  _z = site.get_z();
  _id= site.get_id();
  _direction = site.get_direction();
  _alpha_projected = site.get_projected_alpha();
  _alpha_smoothed  = site.get_smoothed_alpha();
  _residual_x = site.get_residual_x();
  _residual_y = site.get_residual_y();
  _chi2 = site.get_chi2();

  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();

  _pitch = site.get_pitch();
  _type = site.get_type();
}

KalmanSite& KalmanSite::operator=(const KalmanSite &site) {
  if ( this == &site ) {
    return *this;
  }

  _a = site.get_a();
  _C = site.get_covariance_matrix();
  _v = site.get_measurement();
  _s = site.get_shifts();
  _r = site.get_rotations();
  _alpha = _v(0, 0);
  _projected_C = site.get_projected_covariance_matrix();
  _smoothed_C  = site.get_smoothed_covariance_matrix();
  _Cov_s = site.get_S_covariance();
  _Cov_r = site.get_R_covariance();
  _projected_a = site.get_projected_a();
  _smoothed_a  = site.get_smoothed_a();
  _z = site.get_z();
  _id= site.get_id();
  _direction = site.get_direction();
  _alpha_projected = site.get_projected_alpha();
  _alpha_smoothed  = site.get_smoothed_alpha();
  _residual_x = site.get_residual_x();
  _residual_y = site.get_residual_y();
  _chi2 = site.get_chi2();

  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();

  _pitch = site.get_pitch();
  _type = site.get_type();

  return *this;
}

/*
void KalmanSite::set_misalignments(KalmanSciFiAlignment &kalman_align) {
  int id = this->get_id();

  _s = kalman_align.get_shifts(id);
  _r = kalman_align.get_rotations(id);
  _Cov_s = kalman_align.get_cov_shifts(id);
  _Cov_r = kalman_align.get_cov_rotat(id);
}
*/
} // ~namespace MAUS
