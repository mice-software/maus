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

#include "DataStructure/V1731.hh"

namespace MAUS {

V1731::V1731()
    : _ldc_id(0), _samples(), _charge_mm(0), _equip_type(0),
      _phys_event_number(0), _charge_pm(0), _arrival_time(0), _channel_key(""),
      _position_min(0), _pulse_area(0), _max_pos(0), _trigger_time_tag(0),
      _time_stamp(0), _detector(""), _part_event_number(0), _geo(0),
      _pedestal(0), _channel(0) {
}

V1731::V1731(const V1731& _v1731)
    : _ldc_id(0), _samples(), _charge_mm(0), _equip_type(0),
      _phys_event_number(0), _charge_pm(0), _arrival_time(0), _channel_key(""),
      _position_min(0), _pulse_area(0), _max_pos(0), _trigger_time_tag(0),
      _time_stamp(0), _detector(""), _part_event_number(0), _geo(0),
      _pedestal(0), _channel(0) {
    *this = _v1731;
}

V1731& V1731::operator=(const V1731& _v1731) {
    if (this == &_v1731) {
        return *this;
    }
    SetLdcId(_v1731._ldc_id);
    SetSampleArray(_v1731._samples);
    SetChargeMm(_v1731._charge_mm);
    SetEquipType(_v1731._equip_type);
    SetPhysEventNumber(_v1731._phys_event_number);
    SetChargePm(_v1731._charge_pm);
    SetArrivalTime(_v1731._arrival_time);
    SetChannelKey(_v1731._channel_key);
    SetPositionMin(_v1731._position_min);
    SetPulseArea(_v1731._pulse_area);
    SetMaxPos(_v1731._max_pos);
    SetTriggerTimeTag(_v1731._trigger_time_tag);
    SetTimeStamp(_v1731._time_stamp);
    SetDetector(_v1731._detector);
    SetPartEventNumber(_v1731._part_event_number);
    SetGeo(_v1731._geo);
    SetPedestal(_v1731._pedestal);
    SetChannel(_v1731._channel);
    return *this;
}

V1731::~V1731() {
}

int V1731::GetLdcId() const {
    return _ldc_id;
}

void V1731::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

SampleArray V1731::GetSampleArray() const {
    return _samples;
}

int V1731::GetSampleArrayElement(size_t index) const {
    return _samples[index];
}

size_t V1731::GetSampleArraySize() const {
    return _samples.size();
}

void V1731::SetSampleArray(SampleArray samples) {
    _samples = samples;
}

int V1731::GetChargeMm() const {
    return _charge_mm;
}

void V1731::SetChargeMm(int charge_mm) {
    _charge_mm = charge_mm;
}

int V1731::GetEquipType() const {
    return _equip_type;
}

void V1731::SetEquipType(int equip_type) {
    _equip_type = equip_type;
}

int V1731::GetPhysEventNumber() const {
    return _phys_event_number;
}

void V1731::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int V1731::GetChargePm() const {
    return _charge_pm;
}

void V1731::SetChargePm(int charge_pm) {
    _charge_pm = charge_pm;
}

int V1731::GetArrivalTime() const {
    return _arrival_time;
}

void V1731::SetArrivalTime(int arrival_time) {
    _arrival_time = arrival_time;
}

std::string V1731::GetChannelKey() const {
    return _channel_key;
}

void V1731::SetChannelKey(std::string channel_key) {
    _channel_key = channel_key;
}

int V1731::GetPositionMin() const {
    return _position_min;
}

void V1731::SetPositionMin(int position_min) {
    _position_min = position_min;
}

int V1731::GetPulseArea() const {
    return _pulse_area;
}

void V1731::SetPulseArea(int pulse_area) {
    _pulse_area = pulse_area;
}

int V1731::GetMaxPos() const {
    return _max_pos;
}

void V1731::SetMaxPos(int max_pos) {
    _max_pos = max_pos;
}

double V1731::GetTriggerTimeTag() const {
    return _trigger_time_tag;
}

void V1731::SetTriggerTimeTag(double trigger_time_tag) {
    _trigger_time_tag = trigger_time_tag;
}

int V1731::GetTimeStamp() const {
    return _time_stamp;
}

void V1731::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

std::string V1731::GetDetector() const {
    return _detector;
}

void V1731::SetDetector(std::string detector) {
    _detector = detector;
}

int V1731::GetPartEventNumber() const {
    return _part_event_number;
}

void V1731::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int V1731::GetGeo() const {
    return _geo;
}

void V1731::SetGeo(int geo) {
    _geo = geo;
}

int V1731::GetPedestal() const {
    return _pedestal;
}

void V1731::SetPedestal(int pedestal) {
    _pedestal = pedestal;
}

int V1731::GetChannel() const {
    return _channel;
}

void V1731::SetChannel(int channel) {
    _channel = channel;
}
}

