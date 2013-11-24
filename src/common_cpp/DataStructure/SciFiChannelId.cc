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
    : _fibre(0), _station(0), _tracker(0), _plane(0), _used(false), _digit_id(0) {
}

SciFiChannelId::SciFiChannelId(const SciFiChannelId& channel)
    : _fibre(channel._fibre), _station(channel._station),
      _tracker(channel._tracker), _plane(channel._plane), _used(false),
      _digit_id(channel._digit_id) {
}

SciFiChannelId& SciFiChannelId::operator=(const SciFiChannelId& channel) {
    if (this == &channel) {
        return *this;
    }
    _fibre = channel._fibre;
    _station = channel._station;
    _tracker = channel._tracker;
    _plane = channel._plane;
	_used = channel._used;
    _digit_id = channel._digit_id;
    return *this;
}

SciFiChannelId::~SciFiChannelId() {
}

int SciFiChannelId::GetFibreNumber() const {
    return _fibre;
}

void SciFiChannelId::SetFibreNumber(int fibre) {
    _fibre = fibre;
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

bool SciFiChannelId::GetUsed() const {
    return _used;
}

void SciFiChannelId::SetUsed(bool used) {
    _used = used;
}

double SciFiChannelId::GetID() const {
    return _digit_id;
}

void SciFiChannelId::SetID(double digit_id) {
    _digit_id = digit_id;
}

} // ~namespace MAUS
