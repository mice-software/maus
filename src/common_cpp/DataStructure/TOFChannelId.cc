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

#include "src/common_cpp/DataStructure/TOFChannelId.hh"

namespace MAUS {

TOFChannelId::TOFChannelId()
    : _slab(0), _plane(0), _station(0) {
}

TOFChannelId::TOFChannelId(const TOFChannelId& channel)
    : _slab(channel._slab),  _plane(channel._plane),
      _station(channel._station) {
}

TOFChannelId& TOFChannelId::operator=(const TOFChannelId& channel) {
    if (this == &channel) {
        return *this;
    }
    _slab = channel._slab;
    _station = channel._station;
    _plane = channel._plane;
    return *this;
}

TOFChannelId::~TOFChannelId() {
}

int TOFChannelId::GetSlab() const {
    return _slab;
}

void TOFChannelId::SetSlab(int slab) {
    _slab = slab;
}

int TOFChannelId::GetStation() const {
    return _station;
}

void TOFChannelId::SetStation(int station) {
    _station = station;
}

int TOFChannelId::GetPlane() const {
    return _plane;
}

void TOFChannelId::SetPlane(int plane) {
    _plane = plane;
}

}

