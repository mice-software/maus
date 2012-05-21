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

#include "src/common_cpp/DataStructure/V1724.hh"


namespace MAUS {

V1724::V1724()
    : _ldc_id(0), _charge_mm(0), _equip_type(0), _detector(""),
      _phys_event_number(0), _charge_pm(0), _channel_key(""),
      _trigger_time_tag(0), _time_stamp(0), _pedestal(0),
      _part_event_number(0), _geo(0), _position_max(0), _channel(0) {
}

V1724::V1724(const V1724& _v1724)
    : _ldc_id(0), _charge_mm(0), _equip_type(0), _detector(""),
      _phys_event_number(0), _charge_pm(0), _channel_key(""),
      _trigger_time_tag(0), _time_stamp(0), _pedestal(0),
      _part_event_number(0), _geo(0), _position_max(0), _channel(0) {
    *this = _v1724;
}

V1724& V1724::operator=(const V1724& _v1724) {
    if (this == &_v1724) {
        return *this;
    }
    SetLdcId(_v1724._ldc_id);
    SetChargeMm(_v1724._charge_mm);
    SetEquipType(_v1724._equip_type);
    SetDetector(_v1724._detector);
    SetPhysEventNumber(_v1724._phys_event_number);
    SetChargePm(_v1724._charge_pm);
    SetChannelKey(_v1724._channel_key);
    SetTriggerTimeTag(_v1724._trigger_time_tag);
    SetTimeStamp(_v1724._time_stamp);
    SetPedestal(_v1724._pedestal);
    SetPartEventNumber(_v1724._part_event_number);
    SetGeo(_v1724._geo);
    SetPositionMax(_v1724._position_max);
    SetChannel(_v1724._channel);
    return *this;
}

V1724::~V1724() {
}

int V1724::GetLdcId() const {
    return _ldc_id;
}

void V1724::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

int V1724::GetChargeMm() const {
    return _charge_mm;
}

void V1724::SetChargeMm(int charge_mm) {
    _charge_mm = charge_mm;
}

int V1724::GetEquipType() const {
    return _equip_type;
}

void V1724::SetEquipType(int equip_type) {
    _equip_type = equip_type;
}

std::string V1724::GetDetector() const {
    return _detector;
}

void V1724::SetDetector(std::string detector) {
    _detector = detector;
}

int V1724::GetPhysEventNumber() const {
    return _phys_event_number;
}

void V1724::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int V1724::GetChargePm() const {
    return _charge_pm;
}

void V1724::SetChargePm(int charge_pm) {
    _charge_pm = charge_pm;
}

std::string V1724::GetChannelKey() const {
    return _channel_key;
}

void V1724::SetChannelKey(std::string channel_key) {
    _channel_key = channel_key;
}

double V1724::GetTriggerTimeTag() const {
    return _trigger_time_tag;
}

void V1724::SetTriggerTimeTag(double trigger_time_tag) {
    _trigger_time_tag = trigger_time_tag;
}

int V1724::GetTimeStamp() const {
    return _time_stamp;
}

void V1724::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

int V1724::GetPedestal() const {
    return _pedestal;
}

void V1724::SetPedestal(int pedestal) {
    _pedestal = pedestal;
}

int V1724::GetPartEventNumber() const {
    return _part_event_number;
}

void V1724::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int V1724::GetGeo() const {
    return _geo;
}

void V1724::SetGeo(int geo) {
    _geo = geo;
}

int V1724::GetPositionMax() const {
    return _position_max;
}

void V1724::SetPositionMax(int position_max) {
    _position_max = position_max;
}

int V1724::GetChannel() const {
    return _channel;
}

void V1724::SetChannel(int channel) {
    _channel = channel;
}
}

