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
                                     _chi2(-1),
                                     _pos(ThreeVector(0, 0, 0)),
                                     _mom(ThreeVector(0, 0, 0)),
                                     _covariance(0),
                                     _errors(0),
                                     _pull(-1),
                                     _residual(-1),
                                     _smoothed_residual(-1) {
  _cluster = new TRef();
}

SciFiTrackPoint::~SciFiTrackPoint() {
  delete _cluster;
}

//SciFiTrackPoint::SciFiTrackPoint(const KalmanState *kalman_site) {
//  int id = kalman_site->id();
//  if ( id < 0 ) {
//    _tracker = 0;
//  } else {
//    _tracker = 1;
//  }
//  _spill = kalman_site->spill();
//  _event = kalman_site->event();
//
//  id = abs(id);
//  _station = ((id-1)/3)+1;
//  _plane   = (id-1)%3;
//  _channel = kalman_site->measurement()(0, 0);
//
//  _chi2 = kalman_site->chi2();
//
//  TMatrixD state_vector = kalman_site->a(KalmanState::Smoothed);
//  int dimension = state_vector.GetNrows();
//
//  if ( dimension == 4 ) {
//    _pos.setZ(kalman_site->z());
//    _mom.setZ(200.0); // MeV/c
//    _pos.setX(state_vector(0, 0));
//    _mom.setX(state_vector(1, 0));
//    _pos.setY(state_vector(2, 0));
//    _mom.setY(state_vector(3, 0));
//  } else if ( dimension == 5 ) {
//    _pos.setX(state_vector(0, 0));
//    _mom.setX(state_vector(1, 0));
//    _pos.setY(state_vector(2, 0));
//    _mom.setY(state_vector(3, 0));
//    _pos.setZ(kalman_site->z());
//    _mom.setZ(1./fabs(state_vector(4, 0)));
//  }
//
//  _pull              = kalman_site->residual(KalmanState::Projected)(0, 0);
//  _residual          = kalman_site->residual(KalmanState::Filtered)(0, 0);
//  _smoothed_residual = kalman_site->residual(KalmanState::Smoothed)(0, 0);
//
//  TMatrixD C = kalman_site->covariance_matrix(KalmanState::Smoothed);
//  int size = C.GetNrows();
//  int num_elements = size*size;
//  double* matrix_elements = C.GetMatrixArray();
//  std::vector<double> covariance(num_elements);
//  for ( int i = 0; i < num_elements; ++i ) {
//    covariance[i] = matrix_elements[i];
//  }
//  _covariance = covariance;
//  std::vector<double> errors(size);
//  for ( int i = 0; i < size; ++i ) {
//    errors[i] = std::sqrt(fabs(C(i, i)));
//  }
//  _errors = errors;
//
//  _cluster = new TRef(kalman_site->cluster());
//}


//SciFiTrackPoint::SciFiTrackPoint(const Kalman::State& state, const SciFiGeometryHelper* geom) {
//  int id = state.GetId();
//  if ( id < 0 ) {
//    _tracker = 0;
//  } else {
//    _tracker = 1;
//  }
//  // TODO
////  _spill = state->spill();
////  _event = kalman_site->event();
//
//  TMatrixD state_vector = state.GetVector();
//  int dimension = state_vector.GetNrows();
//
//  const SciFiGeometryMap& geom_map = geom->GeometryMap();
//
//  SciFiGeometryMap::const_iterator it = geom_map.find(id);
//
//  ThreeVector reference_pos = geom->GetReferenceFramePosition(_tracker);
//  HepRotation reference_rot = geom->GetReferenceFrameRotation(_tracker);
//  
//  id = abs(id);
//  _station = ((id-1)/3)+1;
//  _plane   = (id-1)%3;
//
//  if ( dimension == 4 ) {
//    _pos.setZ(state.GetPosition());
//    _mom.setZ(200.0); // MeV/c
//    _pos.setX(state_vector(0, 0));
//    _mom.setX(state_vector(1, 0));
//    _pos.setY(state_vector(2, 0));
//    _mom.setY(state_vector(3, 0));
//  } else if ( dimension == 5 ) {
//    _pos.setX(state_vector(0, 0));
//    _mom.setX(state_vector(1, 0));
//    _pos.setY(state_vector(2, 0));
//    _mom.setY(state_vector(3, 0));
//    _pos.setZ(state.GetPosition());
//    _mom.setZ(1./fabs(state_vector(4, 0)));
//  }
//  _pos *= reference_rot;
//  _pos += reference_pos;
//
//  _mom *= reference_rot;
//
//  // TODO
////  _pull              = kalman_site->residual(KalmanState::Projected)(0, 0);
////  _residual          = kalman_site->residual(KalmanState::Filtered)(0, 0);
////  _smoothed_residual = kalman_site->residual(KalmanState::Smoothed)(0, 0);
//  _pull = 0.0;
//  _residual = 0.0;
//  _smoothed_residual = 0.0;
//
//  TMatrixD C = state.GetCovariance();
//  int size = C.GetNrows();
//  int num_elements = size*size;
//  double* matrix_elements = C.GetMatrixArray();
//  std::vector<double> covariance(num_elements);
//  for ( int i = 0; i < num_elements; ++i ) {
//    covariance[i] = matrix_elements[i];
//  }
//  _covariance = covariance;
//  std::vector<double> errors(size);
//  for ( int i = 0; i < size; ++i ) {
//    errors[i] = std::sqrt(fabs(C(i, i)));
//  }
//  _errors = errors;
//
//  //TODO
////  _cluster = new TRef(kalman_site->cluster());
//  _cluster = new TRef();
//}

SciFiTrackPoint::SciFiTrackPoint(const SciFiTrackPoint &point) {
  _spill = point.spill();
  _event = point.event();

  _tracker = point.tracker();
  _station = point.station();
  _plane   = point.plane();
  _channel = point.channel();

  _chi2 = point.chi2();

  _pos = point.pos();
  _mom = point.mom();

  _covariance = point._covariance;
  _errors = point._errors;

  _pull              = point.pull();
  _residual          = point.residual();
  _smoothed_residual = point.smoothed_residual();

  _covariance = point.covariance();
  _cluster = new TRef(*point.get_cluster());
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

  _chi2 = rhs.chi2();

  _pos = rhs.pos();
  _mom = rhs.mom();

  _covariance = rhs._covariance;
  _errors = rhs._errors;

  _pull              = rhs.pull();
  _residual          = rhs.residual();
  _smoothed_residual = rhs.smoothed_residual();

  _covariance= rhs.covariance();

  if (_cluster) delete _cluster;
  _cluster = new TRef(*rhs.get_cluster());

  return *this;
}

} // ~namespace MAUS
