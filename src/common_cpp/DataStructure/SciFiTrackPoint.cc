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

SciFiTrackPoint::SciFiTrackPoint() : _spill(-1),
                                     _event(-1),
                                     _tracker(-1),
                                     _station(-1),
                                     _plane(-1),
                                     _channel(666),
                                     _f_chi2(-1),
                                     _s_chi2(-1),
                                     _pos(ThreeVector(0, 0, 0)),
                                     _mom(ThreeVector(0, 0, 0)),
                                     _pull(-1),
                                     _residual(-1),
                                     _smoothed_residual(-1),
                                     _mc_x(0.),
                                     _mc_px(0.),
                                     _mc_y(0.),
                                     _mc_py(0.),
                                     _mc_pz(0.),
                                     _cluster(0) {
}

SciFiTrackPoint::~SciFiTrackPoint() {}

SciFiTrackPoint::SciFiTrackPoint(const KalmanState *kalman_site) {
  int id = kalman_site->id();
  if ( id < 0 ) {
    _tracker = 0;
  } else {
    _tracker = 1;
  }
  _spill = kalman_site->spill();
  _event = kalman_site->event();

  id = abs(id);
  _station = ((id-1)/3)+1;
  _plane   = (id-1)%3;
  _channel = kalman_site->measurement()(0, 0);

  _f_chi2 = kalman_site->chi2(KalmanState::Filtered);
  _s_chi2 = kalman_site->chi2(KalmanState::Smoothed);

  TMatrixD state_vector = kalman_site->a(KalmanState::Smoothed);
  int dimension = state_vector.GetNrows();

  if ( dimension == 4 ) {
    _pos.setZ(kalman_site->z());
    _mom.setZ(200.0); // MeV/c
    _pos.setX(state_vector(0, 0));
    _mom.setX(state_vector(1, 0));
    _pos.setY(state_vector(2, 0));
    _mom.setY(state_vector(3, 0));
  } else if ( dimension == 5 ) {
    _pos.setX(state_vector(0, 0));
    _mom.setX(state_vector(1, 0)/fabs(state_vector(4, 0)));
    _pos.setY(state_vector(2, 0));
    _mom.setY(state_vector(3, 0)/fabs(state_vector(4, 0)));
    _pos.setZ(kalman_site->z());
    _mom.setZ(1./fabs(state_vector(4, 0)));
  }

  ThreeVector mc_mom = kalman_site->true_momentum();
  ThreeVector mc_pos = kalman_site->true_position();
  _mc_x  = mc_pos.x();
  _mc_px = mc_mom.x();
  _mc_y  = mc_pos.y();
  _mc_py = mc_mom.y();
  _mc_pz = mc_mom.z();

  _pull              = kalman_site->residual(KalmanState::Projected)(0, 0);
  _residual          = kalman_site->residual(KalmanState::Filtered)(0, 0);
  _smoothed_residual = kalman_site->residual(KalmanState::Smoothed)(0, 0);

  TMatrixD C = kalman_site->covariance_matrix(KalmanState::Smoothed);
  int size = C.GetNrows();
  int num_elements = size*size;
  double* matrix_elements = C.GetMatrixArray();
  std::vector<double> covariance(matrix_elements, matrix_elements+num_elements);
  _covariance = covariance;
  _cluster = kalman_site->cluster();
}

SciFiTrackPoint::SciFiTrackPoint(const SciFiTrackPoint &point) {
  _spill = point.spill();
  _event = point.event();

  _tracker = point.tracker();
  _station = point.station();
  _plane   = point.plane();
  _channel = point.channel();

  _f_chi2 = point.f_chi2();
  _s_chi2 = point.s_chi2();

  _pos = point.pos();
  _mom = point.mom();

  _mc_x  = point.mc_x();
  _mc_px = point.mc_px();
  _mc_y  = point.mc_y();
  _mc_py = point.mc_py();
  _mc_pz = point.mc_pz();

  _pull              = point.pull();
  _residual          = point.residual();
  _smoothed_residual = point.smoothed_residual();

  _covariance = point.covariance();
  _cluster = point.cluster();
}

SciFiTrackPoint& SciFiTrackPoint::operator=(const SciFiTrackPoint &rhs) {
  if ( this == &rhs ) {
    return *this;
  }
  _spill   = rhs.spill();
  _event   = rhs.event();
  _tracker = rhs.tracker();
  _station = rhs.station();
  _plane   = rhs.plane();
  _channel = rhs.channel();

  _f_chi2 = rhs.f_chi2();
  _s_chi2 = rhs.s_chi2();

  _pos = rhs.pos();
  _mom = rhs.mom();

  _mc_x  = rhs.mc_x();
  _mc_px = rhs.mc_px();
  _mc_y  = rhs.mc_y();
  _mc_py = rhs.mc_py();
  _mc_pz = rhs.mc_pz();

  _pull              = rhs.pull();
  _residual          = rhs.residual();
  _smoothed_residual = rhs.smoothed_residual();

  _covariance= rhs.covariance();
  _cluster = rhs.cluster();

  return *this;
}

} // ~namespace MAUS
