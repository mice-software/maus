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

#include "src/common_cpp/DataStructure/V1290.hh"


namespace MAUS {

V1290::V1290()
    : _bunch_id(0), _ldc_id(0), _equip_type(0), _phys_event_number(0),
      _trailing_time(0), _channel_key(""), _leading_time(0),
      _trigger_time_tag(0), _time_stamp(0), _detector(""),
      _part_event_number(0), _geo(0), _channel(0) {
}

V1290::V1290(const V1290& _v1290)
    : _bunch_id(0), _ldc_id(0), _equip_type(0), _phys_event_number(0),
      _trailing_time(0), _channel_key(""), _leading_time(0),
      _trigger_time_tag(0), _time_stamp(0), _detector(""),
      _part_event_number(0), _geo(0), _channel(0) {
    *this = _v1290;
}

V1290& V1290::operator=(const V1290& _v1290) {
    if (this == &_v1290) {
        return *this;
    }
    SetBunchId(_v1290._bunch_id);
    SetLdcId(_v1290._ldc_id);
    SetEquipType(_v1290._equip_type);
    SetPhysEventNumber(_v1290._phys_event_number);
    SetTrailingTime(_v1290._trailing_time);
    SetChannelKey(_v1290._channel_key);
    SetLeadingTime(_v1290._leading_time);
    SetTriggerTimeTag(_v1290._trigger_time_tag);
    SetTimeStamp(_v1290._time_stamp);
    SetDetector(_v1290._detector);
    SetPartEventNumber(_v1290._part_event_number);
    SetGeo(_v1290._geo);
    SetChannel(_v1290._channel);
    return *this;
}

V1290::~V1290() {
}

int V1290::GetBunchId() const {
    return _bunch_id;
}

void V1290::SetBunchId(int bunch_id) {
    _bunch_id = bunch_id;
}

int V1290::GetLdcId() const {
    return _ldc_id;
}

void V1290::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

int V1290::GetEquipType() const {
    return _equip_type;
}

void V1290::SetEquipType(int equip_type) {
    _equip_type = equip_type;
}

int V1290::GetPhysEventNumber() const {
    return _phys_event_number;
}

void V1290::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int V1290::GetTrailingTime() const {
    return _trailing_time;
}

void V1290::SetTrailingTime(int trailing_time) {
    _trailing_time = trailing_time;
}

std::string V1290::GetChannelKey() const {
    return _channel_key;
}

void V1290::SetChannelKey(std::string channel_key) {
    _channel_key = channel_key;
}

int V1290::GetLeadingTime() const {
    return _leading_time;
}

void V1290::SetLeadingTime(int leading_time) {
    _leading_time = leading_time;
}

int V1290::GetTriggerTimeTag() const {
    return _trigger_time_tag;
}

void V1290::SetTriggerTimeTag(int trigger_time_tag) {
    _trigger_time_tag = trigger_time_tag;
}

int V1290::GetTimeStamp() const {
    return _time_stamp;
}

void V1290::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

std::string V1290::GetDetector() const {
    return _detector;
}

void V1290::SetDetector(std::string detector) {
    _detector = detector;
}

int V1290::GetPartEventNumber() const {
    return _part_event_number;
}

void V1290::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int V1290::GetGeo() const {
    return _geo;
}

void V1290::SetGeo(int geo) {
    _geo = geo;
}

int V1290::GetChannel() const {
    return _channel;
}

void V1290::SetChannel(int channel) {
    _channel = channel;
}
}

