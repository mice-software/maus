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

#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"

namespace MAUS {

SciFiTrackPoint::SciFiTrackPoint() : _id(0.),
                                     _f_chi2(0.),
                                     _s_chi2(0.),
                                     _x(0.),
                                     _px(0.),
                                     _y(0.),
                                     _py(0.),
                                     _mc_x(0.),
                                     _mc_px(0.),
                                     _mc_y(0.),
                                     _mc_py(0.),
                                     _pull(0.),
                                     _residual(0.),
                                     _smoothed_residual(0.) {}

SciFiTrackPoint::~SciFiTrackPoint() {}

SciFiTrackPoint::SciFiTrackPoint(const KalmanSite *kalman_site) {
  _id = kalman_site->id();

  _f_chi2 = kalman_site->chi2(KalmanSite::Filtered);
  _s_chi2 = kalman_site->chi2(KalmanSite::Smoothed);

  TMatrixD state_vector = kalman_site->a(KalmanSite::Smoothed);
  int dimension = state_vector.GetNrows();

  if ( dimension == 4 ) {
    _x  = state_vector(0, 0);
    _px = state_vector(1, 0);
    _y  = state_vector(2, 0);
    _py = state_vector(3, 0);
  } else if ( dimension == 5 ) {
    _x  = state_vector(0, 0);
    _px = state_vector(1, 0)/state_vector(4, 0);
    _y  = state_vector(2, 0);
    _py = state_vector(3, 0)/state_vector(4, 0);
  }

  CLHEP::Hep3Vector mc_mom = kalman_site->true_momentum();
  CLHEP::Hep3Vector mc_pos = kalman_site->true_position();
  _mc_x  = mc_pos.x();
  _mc_px = mc_mom.x();
  _mc_y  = mc_pos.y();
  _mc_py = mc_mom.y();

  _pull              = kalman_site->residual(KalmanSite::Projected)(0, 0);
  _residual          = kalman_site->residual(KalmanSite::Filtered)(0, 0);
  _smoothed_residual = kalman_site->residual(KalmanSite::Smoothed)(0, 0);
}

SciFiTrackPoint::SciFiTrackPoint(const SciFiTrackPoint &point) {
  _id = point.id();

  _f_chi2 = point.f_chi2();
  _s_chi2 = point.s_chi2();

  _x  = point.x();
  _px = point.px();
  _y  = point.y();
  _py = point.py();

  _mc_x  = point.mc_x();
  _mc_px = point.mc_px();
  _mc_y  = point.mc_y();
  _mc_py = point.mc_py();

  _pull              = point.pull();
  _residual          = point.residual();
  _smoothed_residual = point.smoothed_residual();
}

SciFiTrackPoint& SciFiTrackPoint::operator=(const SciFiTrackPoint &rhs) {
  if ( this == &rhs ) {
    return *this;
  }
  _id = rhs.id();

  _f_chi2 = rhs.f_chi2();
  _s_chi2 = rhs.s_chi2();

  _x  = rhs.x();
  _px = rhs.px();
  _y  = rhs.y();
  _py = rhs.py();

  _mc_x  = rhs.mc_x();
  _mc_px = rhs.mc_px();
  _mc_y  = rhs.mc_y();
  _mc_py = rhs.mc_py();

  _pull              = rhs.pull();
  _residual          = rhs.residual();
  _smoothed_residual = rhs.smoothed_residual();

  return *this;
}

} // ~namespace MAUS
