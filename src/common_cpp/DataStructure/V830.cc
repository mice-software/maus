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

#include "src/common_cpp/DataStructure/V830.hh"


namespace MAUS {

V830::V830()
    : _ldc_id(0), _equip_type(0), _channels(), _phys_event_number(0),
      _time_stamp(0), _geo(0) {
}

V830::V830(const V830& _v830)
    : _ldc_id(0), _equip_type(0), _channels(), _phys_event_number(0),
      _time_stamp(0), _geo(0) {
    *this = _v830;
}

V830& V830::operator=(const V830& _v830) {
    if (this == &_v830) {
        return *this;
    }
    SetLdcId(_v830._ldc_id);
    SetEquipType(_v830._equip_type);
    SetChannels(_v830._channels);
    SetPhysEventNumber(_v830._phys_event_number);
    SetTimeStamp(_v830._time_stamp);
    SetGeo(_v830._geo);
    return *this;
}

V830::~V830() {
}

int V830::GetLdcId() const {
    return _ldc_id;
}

void V830::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

int V830::GetEquipType() const {
    return _equip_type;
}

void V830::SetEquipType(int equip_type) {
    _equip_type = equip_type;
}

Channels V830::GetChannels() const {
    return _channels;
}

void V830::SetChannels(Channels channels) {
    _channels = channels;
}

int V830::GetPhysEventNumber() const {
    return _phys_event_number;
}

void V830::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int V830::GetTimeStamp() const {
    return _time_stamp;
}

void V830::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

int V830::GetGeo() const {
    return _geo;
}

void V830::SetGeo(int geo) {
    _geo = geo;
}
}

