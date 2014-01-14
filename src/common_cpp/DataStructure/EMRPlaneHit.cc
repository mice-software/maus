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
#include "DataStructure/EMRBar.hh"

namespace MAUS {

EMRPlaneHit::EMRPlaneHit()
    : _plane(0), _emrbararray(), _charge(0), _time(0),
      _spill(0), _trigger(0), _deltat(0), _orientation(0) {
//     for (int barid=0; barid<59; barid++) {
//         EMRBar emrbar;
//         emrbar.SetBar(barid);
//         _emrbararray.push_back(emrbar);
//     }
}

EMRPlaneHit::EMRPlaneHit(const EMRPlaneHit& _emrplanehit)
    : _plane(0), _emrbararray(), _charge(0), _time(0),
      _spill(0), _trigger(0), _deltat(0), _orientation(0) {
    *this = _emrplanehit;
}

EMRPlaneHit& EMRPlaneHit::operator=(const EMRPlaneHit& _emrplanehit) {
    if (this == &_emrplanehit) {
        return *this;
    }
    SetPlane(_emrplanehit._plane);
    SetEMRBarArray(_emrplanehit._emrbararray);
    SetCharge(_emrplanehit._charge);
    SetTime(_emrplanehit._time);
    SetSpill(_emrplanehit._spill);
    SetTrigger(_emrplanehit._trigger);
    SetDeltaT(_emrplanehit._deltat);
    SetOrientation(_emrplanehit._orientation);
    return *this;
}

EMRPlaneHit::~EMRPlaneHit() {
  int nbars = _emrbararray.size();
  for (int i = 0; i < nbars; i++)
    delete _emrbararray[i];

  _emrbararray.resize(0);

//   delete[] _emrbararray;
}

int EMRPlaneHit::GetPlane() const {
    return _plane;
}

void EMRPlaneHit::SetPlane(int plane) {
    _plane = plane;
}

EMRBarArray EMRPlaneHit::GetEMRBarArray() const {
    return _emrbararray;
}

void EMRPlaneHit::SetEMRBarArray(EMRBarArray emrbararray) {
    _emrbararray = emrbararray;
}

int EMRPlaneHit::GetCharge() const {
    return _charge;
}

void EMRPlaneHit::SetCharge(int charge) {
    _charge = charge;
}

int EMRPlaneHit::GetTime() const {
    return _time;
}

void EMRPlaneHit::SetTime(int time) {
    _time = time;
}

int EMRPlaneHit::GetSpill() const {
    return _spill;
}

void EMRPlaneHit::SetSpill(int spill) {
    _spill = spill;
}

int EMRPlaneHit::GetTrigger() const {
    return _trigger;
}

void EMRPlaneHit::SetTrigger(int trigger) {
    _trigger = trigger;
}

int EMRPlaneHit::GetDeltaT() const {
    return _deltat;
}

void EMRPlaneHit::SetDeltaT(int deltat) {
    _deltat = deltat;
}

int EMRPlaneHit::GetOrientation() const {
    return _orientation;
}

void EMRPlaneHit::SetOrientation(int orient) {
    _orientation = orient;
}
}

