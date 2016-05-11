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
                                     _gradient(ThreeVector(0, 0, 0)),
                                     _errors_pos(ThreeVector(0, 0, 0)),
                                     _errors_mom(ThreeVector(0, 0, 0)),
                                     _errors_gradient(ThreeVector(0, 0, 0)),
                                     _covariance(0),
                                     _errors(0),
                                     _pull(-1),
                                     _residual(-1),
                                     _smoothed_residual(-1),
                                     _has_data(false) {
}

SciFiTrackPoint::~SciFiTrackPoint() {
}


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
  _gradient = point.gradient();

  _errors_pos = point.pos_error();
  _errors_mom = point.mom_error();
  _errors_gradient = point.gradient_error();

  _covariance = point._covariance;
  _errors = point._errors;

  _pull              = point.pull();
  _residual          = point.residual();
  _smoothed_residual = point.smoothed_residual();

  _covariance = point.covariance();

  _has_data = point._has_data;
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
  _gradient = rhs.gradient();

  _errors_pos = rhs.pos_error();
  _errors_mom = rhs.mom_error();
  _errors_gradient = rhs.gradient_error();

  _covariance = rhs._covariance;
  _errors = rhs._errors;

  _pull              = rhs.pull();
  _residual          = rhs.residual();
  _smoothed_residual = rhs.smoothed_residual();

  _covariance = rhs.covariance();

  _has_data = rhs._has_data;

  return *this;
}
} // ~namespace MAUS

