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

#include "DataStructure/EMREvent.hh"

namespace MAUS {

EMREvent::EMREvent()
  : _emreventtracks(), _vertex(ThreeVector(0., 0., 0.)), _evertex(ThreeVector(0., 0., 0.)),
    _deltat(-1.), _distance(-1.), _polar(0.), _azimuth(0.) {
}

EMREvent::EMREvent(const EMREvent& emre)
  : _emreventtracks(), _vertex(ThreeVector(0., 0., 0.)), _evertex(ThreeVector(0., 0., 0.)),
    _deltat(-1.), _distance(-1.), _polar(0.), _azimuth(0.) {

  *this = emre;
}

EMREvent& EMREvent::operator=(const EMREvent& emre) {
  if (this == &emre)
        return *this;

  this->_emreventtracks = emre._emreventtracks;
  for (size_t i = 0; i < this->_emreventtracks.size(); i++)
    this->_emreventtracks[i] = new EMREventTrack(*(this->_emreventtracks[i]));

  this->SetVertex(emre._vertex);
  this->SetVertexErrors(emre._evertex);
  this->SetDeltaT(emre._deltat);
  this->SetDistance(emre._distance);
  this->SetPolar(emre._polar);
  this->SetAzimuth(emre._azimuth);

  return *this;
}

EMREvent::~EMREvent() {

  for (size_t i = 0; i < _emreventtracks.size(); i++)
    delete _emreventtracks[i];

  _emreventtracks.resize(0);
}

void EMREvent::SetEMREventTrackArray(EMREventTrackArray emreventtracks) {

  for (size_t i = 0; i < _emreventtracks.size(); i++) {
    if (_emreventtracks[i] != NULL)
        delete _emreventtracks[i];
  }
  _emreventtracks = emreventtracks;
}

EMREventTrack* EMREvent::GetMotherPtr() {

  for (size_t i = 0; i < _emreventtracks.size(); i++) {
    if ( _emreventtracks[i]->GetType() == "mother" )
        return _emreventtracks[i];
  }

  return NULL;
}

EMREventTrack* EMREvent::GetDaughterPtr() {

  for (size_t i = 0; i < _emreventtracks.size(); i++) {
    if ( _emreventtracks[i]->GetType() == "daughter" )
        return _emreventtracks[i];
  }

  return NULL;
}
} // namespace MAUS
