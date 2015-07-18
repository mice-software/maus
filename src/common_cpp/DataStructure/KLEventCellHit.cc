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

#include "src/common_cpp/DataStructure/KLEventCellHit.hh"


namespace MAUS {

KLEventCellHit::KLEventCellHit()
    : _kl() {
}

KLEventCellHit::KLEventCellHit(const KLEventCellHit& _kleventcellhit)
    : _kl() {
    *this = _kleventcellhit;
}

KLEventCellHit& KLEventCellHit::operator=(const KLEventCellHit& _kleventcellhit) {
    if (this == &_kleventcellhit) {
        return *this;
    }
    SetKLCellHitArray(_kleventcellhit._kl);
    return *this;
}

KLEventCellHit::~KLEventCellHit() {
}

KLCellHitArray KLEventCellHit::GetKLCellHitArray() const {
    return _kl;
}

KLCellHitArray* KLEventCellHit::GetKLCellHitArrayPtr() {
    return &_kl;
}

KLCellHit KLEventCellHit::GetKLCellHitArrayElement(size_t index) const {
    return _kl[index];
}

size_t KLEventCellHit::GetKLCellHitArraySize() const {
    return _kl.size();
}

void KLEventCellHit::SetKLCellHitArray(KLCellHitArray kl) {
    _kl = kl;
}
}

