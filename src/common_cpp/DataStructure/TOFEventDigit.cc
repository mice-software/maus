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

#include "src/common_cpp/DataStructure/TOFEventDigit.hh"


namespace MAUS {

TOFEventDigit::TOFEventDigit()
    : _tof1(), _tof0(), _tof2() {
}

TOFEventDigit::TOFEventDigit(const TOFEventDigit& _tofeventdigit)
    : _tof1(), _tof0(), _tof2() {
    *this = _tofeventdigit;
}

TOFEventDigit& TOFEventDigit::operator=(const TOFEventDigit& _tofeventdigit) {
    if (this == &_tofeventdigit) {
        return *this;
    }
    SetTOF1DigitArray(_tofeventdigit._tof1);
    SetTOF0DigitArray(_tofeventdigit._tof0);
    SetTOF2DigitArray(_tofeventdigit._tof2);
    return *this;
}

TOFEventDigit::~TOFEventDigit() {
}

TOF1DigitArray TOFEventDigit::GetTOF1DigitArray() const {
    return _tof1;
}

TOF1DigitArray* TOFEventDigit::GetTOF1DigitArrayPtr() {
    return &_tof1;
}

TOFDigit TOFEventDigit::GetTOF1DigitArrayElement(size_t index) const {
    return _tof1[index];
}

size_t TOFEventDigit::GetTOF1DigitArraySize() const {
    return _tof1.size();
}

void TOFEventDigit::SetTOF1DigitArray(TOF1DigitArray tof1) {
    _tof1 = tof1;
}

TOF0DigitArray TOFEventDigit::GetTOF0DigitArray() const {
    return _tof0;
}

TOF0DigitArray* TOFEventDigit::GetTOF0DigitArrayPtr() {
    return &_tof0;
}

TOFDigit TOFEventDigit::GetTOF0DigitArrayElement(size_t index) const {
    return _tof0[index];
}

size_t TOFEventDigit::GetTOF0DigitArraySize() const {
    return _tof0.size();
}

void TOFEventDigit::SetTOF0DigitArray(TOF0DigitArray tof0) {
    _tof0 = tof0;
}

TOF2DigitArray TOFEventDigit::GetTOF2DigitArray() const {
    return _tof2;
}

TOF2DigitArray* TOFEventDigit::GetTOF2DigitArrayPtr() {
    return &_tof2;
}

TOFDigit TOFEventDigit::GetTOF2DigitArrayElement(size_t index) const {
    return _tof2[index];
}

size_t TOFEventDigit::GetTOF2DigitArraySize() const {
    return _tof2.size();
}

void TOFEventDigit::SetTOF2DigitArray(TOF2DigitArray tof2) {
    _tof2 = tof2;
}
}

