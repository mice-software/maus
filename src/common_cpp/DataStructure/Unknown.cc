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

#include "src/common_cpp/DataStructure/Unknown.hh"


namespace MAUS {

Unknown::Unknown()
    : _V1290() {
}

Unknown::Unknown(const Unknown& _unknown)
    : _V1290() {
    *this = _unknown;
}

Unknown& Unknown::operator=(const Unknown& _unknown) {
    if (this == &_unknown) {
        return *this;
    }
    SetV1290Array(_unknown._V1290);
    return *this;
}

Unknown::~Unknown() {
}

V1290Array Unknown::GetV1290Array() const {
    return _V1290;
}

V1290 Unknown::GetV1290ArrayElement(size_t index) const {
    return _V1290[index];
}

size_t Unknown::GetV1290ArraySize() const {
    return _V1290.size();
}

void Unknown::SetV1290Array(V1290Array V1290) {
    _V1290 = V1290;
}
}

