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

#include "src/common_cpp/DataStructure/TrackerDaq.hh"


namespace MAUS {

TrackerDaq::TrackerDaq()
    : _VLSB() {
}

TrackerDaq::TrackerDaq(const TrackerDaq& _trackerDaq)
    : _VLSB() {
    *this = _trackerDaq;
}

TrackerDaq& TrackerDaq::operator=(const TrackerDaq& _trackerDaq) {
    if (this == &_trackerDaq) {
        return *this;
    }
    SetVLSBArray(_trackerDaq._VLSB);
    return *this;
}

TrackerDaq::~TrackerDaq() {
}

VLSBArray TrackerDaq::GetVLSBArray() const {
    return _VLSB;
}

VLSB TrackerDaq::GetVLSBArrayElement(size_t index) const {
    return _VLSB[index];
}

size_t TrackerDaq::GetVLSBArraySize() const {
    return _VLSB.size();
}

void TrackerDaq::SetVLSBArray(VLSBArray VLSB) {
    _VLSB = VLSB;
}

VLSB_CArray TrackerDaq::GetVLSB_CArray() const {
    return _VLSB_C;
}

VLSB_C TrackerDaq::GetVLSB_CArrayElement(size_t index) const {
    return _VLSB_C[index];
}

size_t TrackerDaq::GetVLSB_CArraySize() const {
    return _VLSB_C.size();
}

void TrackerDaq::SetVLSB_CArray(VLSB_CArray vlsb_c) {
    _VLSB_C = vlsb_c;
}
}
