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

#include "src/common_cpp/DataStructure/TOFEventSlabHit.hh"


namespace MAUS {

TOFEventSlabHit::TOFEventSlabHit()
    : _tof1(), _tof0(), _tof2() {
}

TOFEventSlabHit::TOFEventSlabHit(const TOFEventSlabHit& _tofeventslabhit)
    : _tof1(), _tof0(), _tof2() {
    *this = _tofeventslabhit;
}

TOFEventSlabHit& TOFEventSlabHit::operator=(const TOFEventSlabHit& _tofeventslabhit) {
    if (this == &_tofeventslabhit) {
        return *this;
    }
    SetTOF1SlabHitArray(_tofeventslabhit._tof1);
    SetTOF0SlabHitArray(_tofeventslabhit._tof0);
    SetTOF2SlabHitArray(_tofeventslabhit._tof2);
    return *this;
}

TOFEventSlabHit::~TOFEventSlabHit() {
}

TOF1SlabHitArray TOFEventSlabHit::GetTOF1SlabHitArray() const {
    return _tof1;
}

TOF1SlabHitArray* TOFEventSlabHit::GetTOF1SlabHitArrayPtr() {
    return &_tof1;
}

TOFSlabHit TOFEventSlabHit::GetTOF1SlabHitArrayElement(size_t index) const {
    return _tof1[index];
}

size_t TOFEventSlabHit::GetTOF1SlabHitArraySize() const {
    return _tof1.size();
}

void TOFEventSlabHit::SetTOF1SlabHitArray(TOF1SlabHitArray tof1) {
    _tof1 = tof1;
}

TOF0SlabHitArray TOFEventSlabHit::GetTOF0SlabHitArray() const {
    return _tof0;
}

TOF0SlabHitArray* TOFEventSlabHit::GetTOF0SlabHitArrayPtr() {
    return &_tof0;
}

TOFSlabHit TOFEventSlabHit::GetTOF0SlabHitArrayElement(size_t index) const {
    return _tof0[index];
}

size_t TOFEventSlabHit::GetTOF0SlabHitArraySize() const {
    return _tof0.size();
}

void TOFEventSlabHit::SetTOF0SlabHitArray(TOF0SlabHitArray tof0) {
    _tof0 = tof0;
}

TOF2SlabHitArray TOFEventSlabHit::GetTOF2SlabHitArray() const {
    return _tof2;
}

TOF2SlabHitArray* TOFEventSlabHit::GetTOF2SlabHitArrayPtr() {
    return &_tof2;
}

TOFSlabHit TOFEventSlabHit::GetTOF2SlabHitArrayElement(size_t index) const {
    return _tof2[index];
}

size_t TOFEventSlabHit::GetTOF2SlabHitArraySize() const {
    return _tof2.size();
}

void TOFEventSlabHit::SetTOF2SlabHitArray(TOF2SlabHitArray tof2) {
    _tof2 = tof2;
}
}

