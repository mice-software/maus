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
    : _tot(0), _deltat(0) {
}

EMRBarHit::EMRBarHit(const EMRBarHit& _emrbarhit)
    : _tot(0), _deltat(0) {
    *this = _emrbarhit;
}

EMRBarHit& EMRBarHit::operator=(const EMRBarHit& _emrbarhit) {
    if (this == &_emrbarhit) {
        return *this;
    }
    SetTot(_emrbarhit._tot);
    SetDeltaT(_emrbarhit._deltat);
    return *this;
}

EMRBarHit::~EMRBarHit() {
}

int EMRBarHit::GetTot() const {
    return _tot;
}

void EMRBarHit::SetTot(int tot) {
    _tot = tot;
}

int EMRBarHit::GetDeltaT() const {
    return _deltat;
}

void EMRBarHit::SetDeltaT(int deltat) {
    _deltat = deltat;
}
}

