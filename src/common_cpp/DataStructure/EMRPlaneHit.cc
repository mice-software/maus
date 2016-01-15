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

#include "DataStructure/EMRPlaneHit.hh"

namespace MAUS {

EMRPlaneHit::EMRPlaneHit()
  : _emrbarhits(), _plane(-1), _orientation(-1), _time(-1), _deltat(-1),
    _charge(-1.), _pedestal_area(-1.), _samples() {
}

EMRPlaneHit::EMRPlaneHit(const EMRPlaneHit& emrph)
  : _emrbarhits(), _plane(-1), _orientation(-1), _time(-1), _deltat(-1),
    _charge(-1.), _pedestal_area(-1.), _samples() {

  *this = emrph;
}

EMRPlaneHit& EMRPlaneHit::operator=(const EMRPlaneHit& emrph) {
  if (this == &emrph)
      return *this;

  this->SetEMRBarHitArray(emrph._emrbarhits);
  this->SetPlane(emrph._plane);
  this->SetOrientation(emrph._orientation);
  this->SetTime(emrph._time);
  this->SetDeltaT(emrph._deltat);
  this->SetCharge(emrph._charge);
  this->SetPedestalArea(emrph._pedestal_area);
  this->SetSampleArray(emrph._samples);

  return *this;
}

EMRPlaneHit::~EMRPlaneHit() {
}
} // namespace MAUS
