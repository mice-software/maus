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

#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"


namespace MAUS {

TOFEventSpacePoint::TOFEventSpacePoint()
    : _tof0(), _tof1(), _tof2() {
}

TOFEventSpacePoint::TOFEventSpacePoint(const TOFEventSpacePoint& _tofeventspacepoint)
    : _tof0(), _tof1(), _tof2() {
    *this = _tofeventspacepoint;
}

TOFEventSpacePoint& TOFEventSpacePoint::operator=(const TOFEventSpacePoint& _tofeventspacepoint) {
    if (this == &_tofeventspacepoint) {
        return *this;
    }
    SetTOF0SpacePointArray(_tofeventspacepoint._tof0);
    SetTOF1SpacePointArray(_tofeventspacepoint._tof1);
    SetTOF2SpacePointArray(_tofeventspacepoint._tof2);
    return *this;
}

TOFEventSpacePoint::~TOFEventSpacePoint() {
}

TOF0SpacePointArray TOFEventSpacePoint::GetTOF0SpacePointArray() const {
    return _tof0;
}

TOF0SpacePointArray* TOFEventSpacePoint::GetTOF0SpacePointArrayPtr() {
    return &_tof0;
}
TOFSpacePoint TOFEventSpacePoint::GetTOF0SpacePointArrayElement(size_t index) const {
    return _tof0[index];
}

size_t TOFEventSpacePoint::GetTOF0SpacePointArraySize() const {
    return _tof0.size();
}

void TOFEventSpacePoint::SetTOF0SpacePointArray(TOF0SpacePointArray tof0) {
    _tof0 = tof0;
}


TOF1SpacePointArray TOFEventSpacePoint::GetTOF1SpacePointArray() const {
    return _tof1;
}

TOF1SpacePointArray* TOFEventSpacePoint::GetTOF1SpacePointArrayPtr() {
    return &_tof1;
}
TOFSpacePoint TOFEventSpacePoint::GetTOF1SpacePointArrayElement(size_t index) const {
    return _tof1[index];
}

size_t TOFEventSpacePoint::GetTOF1SpacePointArraySize() const {
    return _tof1.size();
}

void TOFEventSpacePoint::SetTOF1SpacePointArray(TOF1SpacePointArray tof1) {
    _tof1 = tof1;
}

TOF2SpacePointArray TOFEventSpacePoint::GetTOF2SpacePointArray() const {
    return _tof2;
}

TOF2SpacePointArray* TOFEventSpacePoint::GetTOF2SpacePointArrayPtr() {
    return &_tof2;
}
TOFSpacePoint TOFEventSpacePoint::GetTOF2SpacePointArrayElement(size_t index) const {
    return _tof2[index];
}

size_t TOFEventSpacePoint::GetTOF2SpacePointArraySize() const {
    return _tof2.size();
}

void TOFEventSpacePoint::SetTOF2SpacePointArray(TOF2SpacePointArray tof2) {
    _tof2 = tof2;
}
}

