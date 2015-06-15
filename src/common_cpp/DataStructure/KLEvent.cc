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

#include "src/common_cpp/DataStructure/KLEvent.hh"


namespace MAUS {

KLEvent::KLEvent()
    : _kl_digits(), _kl_cell_hits() {
}

KLEvent::KLEvent(const KLEvent& _klevent)
    : _kl_digits(), _kl_cell_hits() {
    *this = _klevent;
}

KLEvent& KLEvent::operator=(const KLEvent& _klevent) {
    if (this == &_klevent) {
        return *this;
    }
    SetKLEventDigit(_klevent._kl_digits);
    SetKLEventCellHit(_klevent._kl_cell_hits);
    return *this;
}

KLEvent::~KLEvent() {
}

KLEventDigit KLEvent::GetKLEventDigit() const {
    return _kl_digits;
}

KLEventDigit* KLEvent::GetKLEventDigitPtr() {
    return &_kl_digits;
}
void KLEvent::SetKLEventDigit(KLEventDigit kl_digits) {
    _kl_digits = kl_digits;
}


KLEventCellHit KLEvent::GetKLEventCellHit() const {
    return _kl_cell_hits;
}

KLEventCellHit* KLEvent::GetKLEventCellHitPtr() {
    return &_kl_cell_hits;
}

void KLEvent::SetKLEventCellHit(KLEventCellHit kl_cell_hits) {
    _kl_cell_hits = kl_cell_hits;
}
}

