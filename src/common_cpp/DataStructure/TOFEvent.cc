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

#include "src/common_cpp/DataStructure/TOFEvent.hh"


namespace MAUS {

TOFEvent::TOFEvent()
    : _tof_slab_hits(), _tof_space_points(), _tof_digits() {
}

TOFEvent::TOFEvent(const TOFEvent& _tofevent)
    : _tof_slab_hits(), _tof_space_points(), _tof_digits() {
    *this = _tofevent;
}

TOFEvent& TOFEvent::operator=(const TOFEvent& _tofevent) {
    if (this == &_tofevent) {
        return *this;
    }
    SetTOFEventSlabHit(_tofevent._tof_slab_hits);
    SetTOFEventSpacePoint(_tofevent._tof_space_points);
    SetTOFEventDigit(_tofevent._tof_digits);
    return *this;
}

TOFEvent::~TOFEvent() {
}

TOFEventSlabHit TOFEvent::GetTOFEventSlabHit() const {
    return _tof_slab_hits;
}

TOFEventSlabHit* TOFEvent::GetTOFEventSlabHitPtr() {
    return &_tof_slab_hits;
}

void TOFEvent::SetTOFEventSlabHit(TOFEventSlabHit tof_slab_hits) {
    _tof_slab_hits = tof_slab_hits;
}

TOFEventSpacePoint TOFEvent::GetTOFEventSpacePoint() const {
    return _tof_space_points;
}

TOFEventSpacePoint* TOFEvent::GetTOFEventSpacePointPtr() {
    return &_tof_space_points;
}

void TOFEvent::SetTOFEventSpacePoint(TOFEventSpacePoint tof_space_points) {
    _tof_space_points = tof_space_points;
}

TOFEventDigit TOFEvent::GetTOFEventDigit() const {
    return _tof_digits;
}

TOFEventDigit* TOFEvent::GetTOFEventDigitPtr() {
    return &_tof_digits;
}

void TOFEvent::SetTOFEventDigit(TOFEventDigit tof_digits) {
    _tof_digits = tof_digits;
}
}

