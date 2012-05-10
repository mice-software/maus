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

#include "src/common_cpp/DataStructure/TOFDaq.hh"


namespace MAUS {

TOFDaq::TOFDaq()
    : _V1724(), _V1290() {
}

TOFDaq::TOFDaq(const TOFDaq& _tofdaq)
    : _V1724(), _V1290() {
    *this = _tofdaq;
}

TOFDaq& TOFDaq::operator=(const TOFDaq& _tofdaq) {
    if (this == &_tofdaq) {
        return *this;
    }
    SetV1724Array(_tofdaq._V1724);
    SetV1290Array(_tofdaq._V1290);
    return *this;
}

TOFDaq::~TOFDaq() {
}

V1724Array TOFDaq::GetV1724Array() const {
    return _V1724;
}

V1724 TOFDaq::GetV1724ArrayElement(size_t index) const {
    return _V1724[index];
}

size_t TOFDaq::GetV1724ArraySize() const {
    return _V1724.size();
}

void TOFDaq::SetV1724Array(V1724Array V1724) {
    _V1724 = V1724;
}

V1290Array TOFDaq::GetV1290Array() const {
    return _V1290;
}

V1290 TOFDaq::GetV1290ArrayElement(size_t index) const {
    return _V1290[index];
}

size_t TOFDaq::GetV1290ArraySize() const {
    return _V1290.size();
}

void TOFDaq::SetV1290Array(V1290Array V1290) {
    _V1290 = V1290;
}
}

