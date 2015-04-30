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
  : _plane(0), _orientation(0), _emrbararray(),
    _emrbararray_primary(), _emrbararray_secondary(),
    _charge(0.0), _charge_corrected(0.0), _pedestal_area(0.0),
    _time(0), _spill(0), _trigger(0), _run(0), _deltat(0), _samples() {
}

EMRPlaneHit::EMRPlaneHit(const EMRPlaneHit& _emrplanehit)
  : _plane(0), _orientation(0), _emrbararray(),
    _emrbararray_primary(), _emrbararray_secondary(),
    _charge(0.0), _charge_corrected(0.0), _pedestal_area(0.0),
    _time(0), _spill(0), _trigger(0), _run(0), _deltat(0), _samples() {
  *this = _emrplanehit;
}

EMRPlaneHit& EMRPlaneHit::operator=(const EMRPlaneHit& _emrplanehit) {
  if (this == &_emrplanehit) {
      return *this;
  }

  SetPlane(_emrplanehit._plane);
  SetOrientation(_emrplanehit._orientation);

  this->_emrbararray = _emrplanehit._emrbararray;
  for (size_t i = 0; i < this->_emrbararray.size(); ++i)
    this->_emrbararray[i] = new EMRBar(*this->_emrbararray[i]);

  this->_emrbararray_primary = _emrplanehit._emrbararray_primary;
  for (size_t i = 0; i < this->_emrbararray_primary.size(); ++i)
    this->_emrbararray_primary[i] = new EMRBar(*this->_emrbararray_primary[i]);

  this->_emrbararray_secondary = _emrplanehit._emrbararray_secondary;
  for (size_t i = 0; i < this->_emrbararray_secondary.size(); ++i)
    this->_emrbararray_secondary[i] = new EMRBar(*this->_emrbararray_secondary[i]);

  SetCharge(_emrplanehit._charge);
  SetChargeCorrected(_emrplanehit._charge_corrected);
  SetPedestalArea(_emrplanehit._pedestal_area);
  SetTime(_emrplanehit._time);
  SetSpill(_emrplanehit._spill);
  SetTrigger(_emrplanehit._trigger);
  SetRun(_emrplanehit._run);
  SetDeltaT(_emrplanehit._deltat);
  SetSamples(_emrplanehit._samples);

  return *this;
}

EMRPlaneHit::~EMRPlaneHit() {
  int nbars = _emrbararray.size();
  for (int i = 0; i < nbars; i++)
    delete _emrbararray[i];

  _emrbararray.resize(0);

  nbars = _emrbararray_primary.size();
  for (int i = 0; i < nbars; i++)
    delete _emrbararray_primary[i];

  _emrbararray_primary.resize(0);

  nbars = _emrbararray_secondary.size();
  for (int i = 0; i < nbars; i++)
    delete _emrbararray_secondary[i];

  _emrbararray_secondary.resize(0);
}

int EMRPlaneHit::GetPlane() const {
    return _plane;
}

void EMRPlaneHit::SetPlane(int plane) {
    _plane = plane;
}

int EMRPlaneHit::GetOrientation() const {
    return _orientation;
}

void EMRPlaneHit::SetOrientation(int orient) {
    _orientation = orient;
}

EMRBarArray EMRPlaneHit::GetEMRBarArray() const {
    return _emrbararray;
}

EMRBarArray EMRPlaneHit::GetEMRBarArrayPrimary() const {
    return _emrbararray_primary;
}

EMRBarArray EMRPlaneHit::GetEMRBarArraySecondary() const {
    return _emrbararray_secondary;
}

void EMRPlaneHit::SetEMRBarArray(EMRBarArray emrbararray) {
    _emrbararray = emrbararray;
}

void EMRPlaneHit::SetEMRBarArrayPrimary(EMRBarArray emrbararray_primary) {
    _emrbararray_primary = emrbararray_primary;
}

void EMRPlaneHit::SetEMRBarArraySecondary(EMRBarArray emrbararray_secondary) {
    _emrbararray_secondary = emrbararray_secondary;
}

double EMRPlaneHit::GetCharge() const {
    return _charge;
}

void EMRPlaneHit::SetCharge(double charge) {
    _charge = charge;
}

double EMRPlaneHit::GetChargeCorrected() const {
    return _charge_corrected;
}

void EMRPlaneHit::SetChargeCorrected(double charge_corrected) {
    _charge_corrected = charge_corrected;
}

double EMRPlaneHit::GetPedestalArea() const {
    return _pedestal_area;
}

void EMRPlaneHit::SetPedestalArea(double pedestal_area) {
    _pedestal_area = pedestal_area;
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

int EMRPlaneHit::GetRun() const {
    return _run;
}

void EMRPlaneHit::SetRun(int run) {
    _run = run;
}

int EMRPlaneHit::GetDeltaT() const {
    return _deltat;
}

void EMRPlaneHit::SetDeltaT(int deltat) {
    _deltat = deltat;
}

std::vector<int> EMRPlaneHit::GetSamples() const {
    return _samples;
}

void EMRPlaneHit::SetSamples(std::vector<int> samples) {
    _samples = samples;
}
}

