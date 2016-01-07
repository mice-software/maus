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

#include "src/common_cpp/DataStructure/EMRTrackPoint.hh"

namespace MAUS {

EMRTrackPoint::EMRTrackPoint()
  : _pos(ThreeVector(0., 0., 0.)), _gpos(ThreeVector(0., 0., 0.)),
    _poserr(ThreeVector(0., 0., 0.)), _ch(-1.), _resx(0.), _resy(0.), _chi2(-1.) {
}

EMRTrackPoint::EMRTrackPoint(const EMRTrackPoint &emrtp)
  : _pos(ThreeVector(0., 0., 0.)), _gpos(ThreeVector(0., 0., 0.)),
    _poserr(ThreeVector(0., 0., 0.)), _ch(-1.), _resx(0.), _resy(0.), _chi2(-1.) {

  *this = emrtp;
}

EMRTrackPoint& EMRTrackPoint::operator=(const EMRTrackPoint &emrtp) {
  if ( this == &emrtp ) {
    return *this;
  }

  this->SetPosition(emrtp._pos);
  this->SetGlobalPosition(emrtp._gpos);
  this->SetPositionErrors(emrtp._poserr);
  this->SetChannel(emrtp._ch);
  this->SetResiduals(emrtp._resx, emrtp._resy);
  this->SetChi2(emrtp._chi2);

  return *this;
}

EMRTrackPoint::~EMRTrackPoint() {
}

void EMRTrackPoint::SetResiduals(double resx, double resy) {

  _resx = resx;
  _resy = resy;
}
} // namespace MAUS
