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

KalmanSite::KalmanSite(): _z(0.), _alpha(0.), _alpha_projected(0.), _id(0),
                          _residual_x(0.), _residual_y(0.), _direction((0., 0., 0.)) {
  _C.ResizeTo(5, 5);
  _a.ResizeTo(5, 1);
  _v.ResizeTo(2, 1);
  _projected_C.ResizeTo(5, 5);
  _smoothed_C.ResizeTo(5, 5);
  _projected_a.ResizeTo(5, 1);
  _smoothed_a.ResizeTo(5, 1);
}

KalmanSite::~KalmanSite() {}

KalmanSite::KalmanSite(const KalmanSite &site): _z(0.), _alpha(0.), _alpha_projected(0.), _id(0),
                          _residual_x(0.), _residual_y(0.), _direction((0., 0., 0.)) {
  _a.ResizeTo(5, 1);
  _C.ResizeTo(5, 5);
  _v.ResizeTo(2, 1);
  _projected_C.ResizeTo(5, 5);
  _smoothed_C.ResizeTo(5, 5);
  _projected_a.ResizeTo(5, 1);
  _smoothed_a.ResizeTo(5, 1);

  _a = site.get_a();
  _C = site.get_covariance_matrix();
  _v = site.get_measurement();
  _alpha = _v(0, 0);
  _projected_C = site.get_projected_covariance_matrix();
  _smoothed_C  = site.get_smoothed_covariance_matrix();
  _projected_a = site.get_projected_a();
  _smoothed_a  = site.get_smoothed_a();
  _z = site.get_z();
  _id= site.get_id();
  _direction = site.get_direction();
  _alpha_projected = site.get_projected_alpha();
  _alpha_smoothed  = site.get_smoothed_alpha();
  _residual_x = site.get_residual_x();
  _residual_y = site.get_residual_y();

  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();
}

KalmanSite& KalmanSite::operator=(const KalmanSite &site) {
  if ( this == &site ) {
    return *this;
  }

  _a = site.get_a();
  _C = site.get_covariance_matrix();
  _v = site.get_measurement();
  _alpha = _v(0, 0);
  _projected_C = site.get_projected_covariance_matrix();
  _smoothed_C  = site.get_smoothed_covariance_matrix();
  _projected_a = site.get_projected_a();
  _smoothed_a  = site.get_smoothed_a();
  _z = site.get_z();
  _id= site.get_id();
  _direction = site.get_direction();
  _alpha_projected = site.get_projected_alpha();
  _alpha_smoothed  = site.get_smoothed_alpha();
  _residual_x = site.get_residual_x();
  _residual_y = site.get_residual_y();

  _mc_pos = site.get_true_position();
  _mc_mom = site.get_true_momentum();

  return *this;
}

} // ~namespace MAUS
