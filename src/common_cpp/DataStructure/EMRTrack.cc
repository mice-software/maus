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

#include "src/common_cpp/DataStructure/EMRTrack.hh"

namespace MAUS {

EMRTrack::EMRTrack()
  : _trackpoints(), _parx(), _pary(), _eparx(), _epary(),
    _origin(ThreeVector(0., 0., 0.)), _eorigin(ThreeVector(0., 0., 0.)),
    _polar(0.), _epolar(0.), _azimuth(0.), _eazimuth(0.),
    _chi2(-1.), _range(-1.), _erange(0.), _mom(0.), _emom(0.) {
}

EMRTrack::EMRTrack(const EMRTrack &emrt)
  : _trackpoints(), _parx(), _pary(), _eparx(), _epary(),
    _origin(ThreeVector(0., 0., 0.)), _eorigin(ThreeVector(0., 0., 0.)),
    _polar(0.), _epolar(0.), _azimuth(0.), _eazimuth(0.),
    _chi2(-1.), _range(-1.), _erange(0.), _mom(0.), _emom(0.) {

  *this = emrt;
}

EMRTrack& EMRTrack::operator=(const EMRTrack &emrt) {
  if (this == &emrt) {
    return *this;
  }

  this->SetEMRTrackPointArray(emrt._trackpoints);
  this->SetParametersX(emrt._parx);
  this->SetParametersY(emrt._pary);
  this->SetParametersErrorsX(emrt._eparx);
  this->SetParametersErrorsY(emrt._epary);
  this->SetOrigin(emrt._origin);
  this->SetOriginErrors(emrt._eorigin);
  this->SetPolar(emrt._polar);
  this->SetPolarError(emrt._epolar);
  this->SetAzimuth(emrt._azimuth);
  this->SetAzimuthError(emrt._eazimuth);
  this->SetChi2(emrt._chi2);
  this->SetRange(emrt._range);
  this->SetRangeError(emrt._erange);
  this->SetMomentum(emrt._mom);
  this->SetMomentumError(emrt._emom);

  return *this;
}

EMRTrack::~EMRTrack() {
}
} // namespace MAUS
