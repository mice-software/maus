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

#include "src/common_cpp/DataStructure/Pmt1.hh"


namespace MAUS {

Pmt1::Pmt1()
    : _raw_time(0.), _charge(0), _leading_time(0), _time(0.), _tof_key(""),
      _trigger_request_leading_time(0) {
}

Pmt1::Pmt1(const Pmt1& _pmt1)
    : _raw_time(0.), _charge(0), _leading_time(0), _time(0.), _tof_key(""),
      _trigger_request_leading_time(0) {
    *this = _pmt1;
}

Pmt1& Pmt1::operator=(const Pmt1& _pmt1) {
    if (this == &_pmt1) {
        return *this;
    }
    SetRawTime(_pmt1._raw_time);
    SetCharge(_pmt1._charge);
    SetLeadingTime(_pmt1._leading_time);
    SetTime(_pmt1._time);
    SetTofKey(_pmt1._tof_key);
    SetTriggerRequestLeadingTime(_pmt1._trigger_request_leading_time);
    return *this;
}

Pmt1::~Pmt1() {
}

double Pmt1::GetRawTime() const {
    return _raw_time;
}

void Pmt1::SetRawTime(double raw_time) {
    _raw_time = raw_time;
}

int Pmt1::GetCharge() const {
    return _charge;
}

void Pmt1::SetCharge(int charge) {
    _charge = charge;
}

int Pmt1::GetLeadingTime() const {
    return _leading_time;
}

void Pmt1::SetLeadingTime(int leading_time) {
    _leading_time = leading_time;
}

double Pmt1::GetTime() const {
    return _time;
}

void Pmt1::SetTime(double time) {
    _time = time;
}

std::string Pmt1::GetTofKey() const {
    return _tof_key;
}

void Pmt1::SetTofKey(std::string tof_key) {
    _tof_key = tof_key;
}

int Pmt1::GetTriggerRequestLeadingTime() const {
    return _trigger_request_leading_time;
}

void Pmt1::SetTriggerRequestLeadingTime(int trigger_request_leading_time) {
    _trigger_request_leading_time = trigger_request_leading_time;
}
}

