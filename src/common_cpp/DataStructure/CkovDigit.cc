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

#include "src/common_cpp/DataStructure/CkovDigit.hh"


namespace MAUS {

CkovDigit::CkovDigit()
    : _A(), _B() {
}

CkovDigit::CkovDigit(const CkovDigit& _ckovdigit)
    : _A(), _B() {
    *this = _ckovdigit;
}

CkovDigit& CkovDigit::operator=(const CkovDigit& _ckovdigit) {
    if (this == &_ckovdigit) {
        return *this;
    }
    SetCkovA(_ckovdigit._A);
    SetCkovB(_ckovdigit._B);
    return *this;
}

CkovDigit::~CkovDigit() {
}

CkovA CkovDigit::GetCkovA() const {
    return _A;
}

void CkovDigit::SetCkovA(CkovA A) {
    _A = A;
}

CkovB CkovDigit::GetCkovB() const {
    return _B;
}

void CkovDigit::SetCkovB(CkovB B) {
    _B = B;
}
}

