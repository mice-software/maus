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

#include "DataStructure/EMRBarHit.hh"

namespace MAUS {

EMRBarHit::EMRBarHit()
  : _ch(-1), _tot(-1), _time(-1), _deltat(-1) {
}

EMRBarHit::EMRBarHit(const EMRBarHit& emrbh)
  : _ch(-1), _tot(-1), _time(-1), _deltat(-1) {
  *this = emrbh;
}

EMRBarHit& EMRBarHit::operator=(const EMRBarHit& emrbh) {
  if (this == &emrbh)
      return *this;

  this->SetChannel(emrbh._ch);
  this->SetTot(emrbh._tot);
  this->SetTime(emrbh._time);
  this->SetDeltaT(emrbh._deltat);

  return *this;
}

EMRBarHit::~EMRBarHit() {
}
} // namespace MAUS
