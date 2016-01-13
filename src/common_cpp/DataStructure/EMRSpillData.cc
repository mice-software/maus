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

namespace MAUS {

EMRSpillData::EMRSpillData()
  : _emrbarhits(), _emreventtracks() {
}

EMRSpillData::EMRSpillData(const EMRSpillData& emrsd)
  : _emrbarhits(), _emreventtracks() {
  *this = emrsd;
}

EMRSpillData& EMRSpillData::operator=(const EMRSpillData& emrsd) {
  if (this == &emrsd)
      return *this;

  this->SetEMRBarHitArray(emrsd._emrbarhits);

  this->_emreventtracks = emrsd._emreventtracks;
  for (size_t i = 0; i < this->_emreventtracks.size(); i++)
    this->_emreventtracks[i] = new EMREventTrack(*(this->_emreventtracks[i]));

  return *this;
}

EMRSpillData::~EMRSpillData() {

  for (size_t i = 0; i < _emreventtracks.size(); i++)
    delete _emreventtracks[i];

  _emreventtracks.resize(0);
}

void EMRSpillData::SetEMREventTrackArray(EMREventTrackArray emreventtracks) {

  for (size_t i = 0; i < _emreventtracks.size(); i++) {
    if (_emreventtracks[i] != NULL)
        delete _emreventtracks[i];
  }
  _emreventtracks = emreventtracks;
}
} // namespace MAUS
