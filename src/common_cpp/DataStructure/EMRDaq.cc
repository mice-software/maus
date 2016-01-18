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


#include "DataStructure/EMRDaq.hh"

namespace MAUS {

EMRDaq::EMRDaq()
: _V1731(), _dbb() {
}

EMRDaq::EMRDaq(const EMRDaq& _emrdaq)
: _V1731(), _dbb() {
  *this = _emrdaq;
}

EMRDaq& EMRDaq::operator=(const EMRDaq& _emrdaq) {

  if (this == &_emrdaq) {
    return *this;
  }

  SetV1731PartEventArray(_emrdaq._V1731);
  SetDBBArray(_emrdaq._dbb);

  return *this;
}

EMRDaq::~EMRDaq() {
}

V1731PartEventArray EMRDaq::GetV1731PartEventArray() const {
  return _V1731;
}

size_t EMRDaq::GetV1731NumPartEvents() const {
  return _V1731.size();
}

V1731HitArray EMRDaq::GetV1731PartEvent(size_t index) const {
  return _V1731[index];
}

size_t EMRDaq::GetV1731PartEventArraySize(size_t index) const {
  if (_V1731.size() <= index)
    return 0;

  return _V1731[index].size();
}

void EMRDaq::SetV1731PartEventArray(V1731PartEventArray V1731) {
  _V1731 = V1731;
}

DBBArray EMRDaq::GetDBBArray() const {
  return _dbb;
}

DBBSpillData EMRDaq::GetDBBArrayElement(size_t index) const {
  return _dbb[index];
}

size_t EMRDaq::GetDBBArraySize() const {
  return _dbb.size();
}

void EMRDaq::SetDBBArray(DBBArray s) {
  _dbb = s;
}
} // namespace MAUS
