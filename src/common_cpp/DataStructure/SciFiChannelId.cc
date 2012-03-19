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

#include "src/common_cpp/DataStructure/SciFiChannelId.hh"

namespace MAUS {

SciFiChannelId::SciFiChannelId()
    : _fiber(0), _station(0), _tracker(0), _plane(0) {
}

SciFiChannelId::SciFiChannelId(const SciFiChannelId& channel)
    : _fiber(channel._fiber), _station(channel._station),
      _tracker(channel._tracker), _plane(channel._plane) {
}

SciFiChannelId& SciFiChannelId::operator=(const SciFiChannelId& channel) {
    if (this == &channel) {
        return *this;
    }
    _fiber = channel._fiber;
    _station = channel._station;
    _tracker = channel._tracker;
    _plane = channel._plane;
    return *this;
}

SciFiChannelId::~SciFiChannelId() {
}

int SciFiChannelId::GetFiberNumber() const {
    return _fiber;
}

void SciFiChannelId::SetFiberNumber(int fiber) {
    _fiber = fiber;
}

int SciFiChannelId::GetStationNumber() const {
    return _station;
}

void SciFiChannelId::SetStationNumber(int station) {
    _station = station;
}

int SciFiChannelId::GetTrackerNumber() const {
    return _tracker;
}

void SciFiChannelId::SetTrackerNumber(int tracker) {
    _tracker = tracker;
}

int SciFiChannelId::GetPlaneNumber() const {
    return _plane;
}

void SciFiChannelId::SetPlaneNumber(int plane) {
    _plane = plane;
}
}

