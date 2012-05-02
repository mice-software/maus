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

#include "src/common_cpp/DataStructure/CkovDaq.hh"


namespace MAUS {

CkovDaq::CkovDaq()
    : _V1731() {
}

CkovDaq::CkovDaq(const CkovDaq& _ckovdaq)
    : _V1731() {
    *this = _ckovdaq;
}

CkovDaq& CkovDaq::operator=(const CkovDaq& _ckovdaq) {
    if (this == &_ckovdaq) {
        return *this;
    }
    SetV1731Array(_ckovdaq._V1731);
    return *this;
}

CkovDaq::~CkovDaq() {
}

V1731Array CkovDaq::GetV1731Array() const {
    return _V1731;
}

V1731 CkovDaq::GetV1731ArrayElement(size_t index) const{
    return _V1731[index];
}

size_t CkovDaq::GetV1731ArraySize() const {
    return _V1731.size();
}

void CkovDaq::SetV1731Array(V1731Array V1731) {
    _V1731 = V1731;
}
}

