/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "DataStructure/EMRSpacePoint.hh"

namespace MAUS {

EMRSpacePoint::EMRSpacePoint()
  : _pos(ThreeVector(0., 0., 0.)), _gpos(ThreeVector(0., 0., 0.)),
    _poserr(ThreeVector(0., 0., 0.)), _ch(-1), _time(-1), _deltat(-1),
    _chargema(-1), _chargesa(-1) {
}

EMRSpacePoint::EMRSpacePoint(const EMRSpacePoint& emrsp)
  : _pos(ThreeVector(0., 0., 0.)), _gpos(ThreeVector(0., 0., 0.)),
    _poserr(ThreeVector(0., 0., 0.)), _ch(-1), _time(-1), _deltat(-1),
    _chargema(-1), _chargesa(-1) {

  *this = emrsp;
}

EMRSpacePoint& EMRSpacePoint::operator=(const EMRSpacePoint& emrsp) {
  if (this == &emrsp)
      return *this;

  this->SetPosition(emrsp._pos);
  this->SetGlobalPosition(emrsp._gpos);
  this->SetPositionErrors(emrsp._poserr);
  this->SetChannel(emrsp._ch);
  this->SetTime(emrsp._time);
  this->SetDeltaT(emrsp._deltat);
  this->SetChargeMA(emrsp._chargema);
  this->SetChargeSA(emrsp._chargesa);

  return *this;
}

EMRSpacePoint::~EMRSpacePoint() {
}
} // namespace MAUS
