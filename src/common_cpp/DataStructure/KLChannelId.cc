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

#include "src/common_cpp/DataStructure/KLChannelId.hh"

namespace MAUS {

KLChannelId::KLChannelId()
    : _cell(0) {
}

KLChannelId::KLChannelId(const KLChannelId& channel)
    : _cell(channel._cell) {
}

KLChannelId& KLChannelId::operator=(const KLChannelId& channel) {
    if (this == &channel) {
        return *this;
    }
    _cell = channel._cell;
    return *this;
}

KLChannelId::~KLChannelId() {
}

int KLChannelId::GetCell() const {
    return _cell;
}

void KLChannelId::SetCell(int cell) {
    _cell = cell;
}
}

