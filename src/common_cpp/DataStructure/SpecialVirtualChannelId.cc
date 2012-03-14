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

#include "src/common_cpp/DataStructure/SpecialVirtualChannelId.hh"

namespace MAUS {

SpecialVirtualChannelId::SpecialVirtualChannelId()
    : _station(0) {
}

SpecialVirtualChannelId::SpecialVirtualChannelId
         (const SpecialVirtualChannelId& channel) : _station(channel._station) {
}

SpecialVirtualChannelId& SpecialVirtualChannelId::operator=
                                      (const SpecialVirtualChannelId& channel) {
    if (this == &channel) {
        return *this;
    }
    _station = channel._station;
    return *this;
}

SpecialVirtualChannelId::~SpecialVirtualChannelId() {
}

int SpecialVirtualChannelId::GetStation() const {
    return _station;
}

void SpecialVirtualChannelId::SetStation(int station) {
    _station = station;
}

}
