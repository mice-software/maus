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

#include "DataStructure/EMRSpillData.hh"
#include "DataStructure/EMRPlaneHit.hh"

namespace MAUS {

EMRSpillData::EMRSpillData()
  : _emrplanehitarray() {
}

EMRSpillData::EMRSpillData(const EMRSpillData& _emrspilldata)
  : _emrplanehitarray() {
  *this = _emrspilldata;
}

EMRSpillData& EMRSpillData::operator=(const EMRSpillData& _emrspilldata) {
  if (this == &_emrspilldata) {
      return *this;
  }
  this->_emrplanehitarray = _emrspilldata._emrplanehitarray;
  for (size_t i = 0; i < this->_emrplanehitarray.size(); i++)
    this->_emrplanehitarray[i] = new EMRPlaneHit(*(this->_emrplanehitarray[i]));

  return *this;
}

EMRSpillData::~EMRSpillData() {
  int nph = _emrplanehitarray.size();
  for (int i = 0; i < nph; i++)
    delete _emrplanehitarray[i];

  _emrplanehitarray.resize(0);
}

EMRPlaneHitArray EMRSpillData::GetEMRPlaneHitArray() const {
  return _emrplanehitarray;
}

void EMRSpillData::SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehitarray) {
  int nplhits = _emrplanehitarray.size();
  for (int i = 0; i < nplhits; i++) {
    if (_emrplanehitarray[i] != NULL)
        delete _emrplanehitarray[i];
  }
  _emrplanehitarray = emrplanehitarray;
}
}

