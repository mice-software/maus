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

#include "src/common_cpp/DataStructure/KLEventDigit.hh"


namespace MAUS {

KLEventDigit::KLEventDigit()
    : _kl() {
}

KLEventDigit::KLEventDigit(const KLEventDigit& _kleventdigit)
    : _kl() {
    *this = _kleventdigit;
}

KLEventDigit& KLEventDigit::operator=(const KLEventDigit& _kleventdigit) {
    if (this == &_kleventdigit) {
        return *this;
    }
    SetKLDigitArray(_kleventdigit._kl);
    return *this;
}

KLEventDigit::~KLEventDigit() {
}

KLDigitArray KLEventDigit::GetKLDigitArray() const {
    return _kl;
}

KLDigitArray* KLEventDigit::GetKLDigitArrayPtr() {
    return &_kl;
}
KLDigit KLEventDigit::GetKLDigitArrayElement(size_t index) const {
    return _kl[index];
}

size_t KLEventDigit::GetKLDigitArraySize() const {
    return _kl.size();
}

void KLEventDigit::SetKLDigitArray(KLDigitArray kl) {
    _kl = kl;
}
}

