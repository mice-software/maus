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

#include "src/common_cpp/DataStructure/TOFDigit.hh"


namespace MAUS {

TOFDigit::TOFDigit()
    : _pmt(0), _trigger_leading_time(0), _trigger_request_leading_time(0),
      _trigger_trailing_time(0), _trigger_request_trailing_time(0),
      _charge_mm(0), _trailing_time(0), _plane(0), _charge_pm(0),
      _phys_event_number(0), _station(0), _leading_time(0), _time_stamp(0),
      _tof_key(""), _trigger_time_tag(0), _part_event_number(0), _slab(0) {
}

TOFDigit::TOFDigit(const TOFDigit& _tofdigit)
    : _pmt(0), _trigger_leading_time(0), _trigger_request_leading_time(0),
      _trigger_trailing_time(0), _trigger_request_trailing_time(0),
      _charge_mm(0), _trailing_time(0), _plane(0), _charge_pm(0),
      _phys_event_number(0), _station(0), _leading_time(0), _time_stamp(0),
      _tof_key(""), _trigger_time_tag(0), _part_event_number(0), _slab(0) {
    *this = _tofdigit;
}

TOFDigit& TOFDigit::operator=(const TOFDigit& _tofdigit) {
    if (this == &_tofdigit) {
        return *this;
    }
    SetPmt(_tofdigit._pmt);
    SetTriggerLeadingTime(_tofdigit._trigger_leading_time);
    SetTriggerRequestLeadingTime(_tofdigit._trigger_request_leading_time);
    SetTriggerTrailingTime(_tofdigit._trigger_trailing_time);
    SetTriggerRequestTrailingTime(_tofdigit._trigger_request_trailing_time);
    SetChargeMm(_tofdigit._charge_mm);
    SetTrailingTime(_tofdigit._trailing_time);
    SetPlane(_tofdigit._plane);
    SetChargePm(_tofdigit._charge_pm);
    SetPhysEventNumber(_tofdigit._phys_event_number);
    SetStation(_tofdigit._station);
    SetLeadingTime(_tofdigit._leading_time);
    SetTimeStamp(_tofdigit._time_stamp);
    SetTofKey(_tofdigit._tof_key);
    SetTriggerTimeTag(_tofdigit._trigger_time_tag);
    SetPartEventNumber(_tofdigit._part_event_number);
    SetSlab(_tofdigit._slab);
    return *this;
}

TOFDigit::~TOFDigit() {
}

int TOFDigit::GetPmt() const {
    return _pmt;
}

void TOFDigit::SetPmt(int pmt) {
    _pmt = pmt;
}

int TOFDigit::GetTriggerLeadingTime() const {
    return _trigger_leading_time;
}

void TOFDigit::SetTriggerLeadingTime(int trigger_leading_time) {
    _trigger_leading_time = trigger_leading_time;
}

int TOFDigit::GetTriggerRequestLeadingTime() const {
    return _trigger_request_leading_time;
}

void TOFDigit::SetTriggerRequestLeadingTime(int trigger_request_leading_time) {
    _trigger_request_leading_time = trigger_request_leading_time;
}

int TOFDigit::GetTriggerTrailingTime() const {
    return _trigger_trailing_time;
}

void TOFDigit::SetTriggerTrailingTime(int trigger_trailing_time) {
    _trigger_trailing_time = trigger_trailing_time;
}

int TOFDigit::GetTriggerRequestTrailingTime() const {
    return _trigger_request_trailing_time;
}

void TOFDigit::SetTriggerRequestTrailingTime(int trigger_request_trailing_time) {
    _trigger_request_trailing_time = trigger_request_trailing_time;
}

int TOFDigit::GetChargeMm() const {
    return _charge_mm;
}

void TOFDigit::SetChargeMm(int charge_mm) {
    _charge_mm = charge_mm;
}

int TOFDigit::GetTrailingTime() const {
    return _trailing_time;
}

void TOFDigit::SetTrailingTime(int trailing_time) {
    _trailing_time = trailing_time;
}

int TOFDigit::GetPlane() const {
    return _plane;
}

void TOFDigit::SetPlane(int plane) {
    _plane = plane;
}

int TOFDigit::GetChargePm() const {
    return _charge_pm;
}

void TOFDigit::SetChargePm(int charge_pm) {
    _charge_pm = charge_pm;
}

int TOFDigit::GetPhysEventNumber() const {
    return _phys_event_number;
}

void TOFDigit::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int TOFDigit::GetStation() const {
    return _station;
}

void TOFDigit::SetStation(int station) {
    _station = station;
}

int TOFDigit::GetLeadingTime() const {
    return _leading_time;
}

void TOFDigit::SetLeadingTime(int leading_time) {
    _leading_time = leading_time;
}

int TOFDigit::GetTimeStamp() const {
    return _time_stamp;
}

void TOFDigit::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

std::string TOFDigit::GetTofKey() const {
    return _tof_key;
}

void TOFDigit::SetTofKey(std::string tof_key) {
    _tof_key = tof_key;
}

int TOFDigit::GetTriggerTimeTag() const {
    return _trigger_time_tag;
}

void TOFDigit::SetTriggerTimeTag(int trigger_time_tag) {
    _trigger_time_tag = trigger_time_tag;
}

int TOFDigit::GetPartEventNumber() const {
    return _part_event_number;
}

void TOFDigit::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int TOFDigit::GetSlab() const {
    return _slab;
}

void TOFDigit::SetSlab(int slab) {
    _slab = slab;
}
}

