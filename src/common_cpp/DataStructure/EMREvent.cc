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
#include "DataStructure/EMRPlaneHit.hh"

namespace MAUS {

EMREvent::EMREvent()
  : _emrplanehitarray() {
//    for (int planeid=0; planeid<48; planeid++) {
//         EMRPlaneHit emrplanehit;
//         emrplanehit.SetPlane(planeid);
//         _emrplanehitarray.push_back(emrplanehit);
//    }
}

EMREvent::EMREvent(const EMREvent& _emrevent) {
  *this = _emrevent;
}

EMREvent& EMREvent::operator=(const EMREvent& _emrevent) {
  if (this == &_emrevent) {
        return *this;
  }
  SetEMRPlaneHitArray(_emrevent._emrplanehitarray);
  return *this;
}

EMREvent::~EMREvent() {
  int nplhits = _emrplanehitarray.size();
  for (int i = 0; i < nplhits; i++)
    delete _emrplanehitarray[i];

  _emrplanehitarray.resize(0);
}

EMRPlaneHitArray EMREvent::GetEMRPlaneHitArray() const {
  return _emrplanehitarray;
}

void EMREvent::SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehitarray) {
  _emrplanehitarray = emrplanehitarray;
}
}

