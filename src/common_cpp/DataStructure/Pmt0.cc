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

#include "src/common_cpp/DataStructure/Pmt0.hh"


namespace MAUS {

Pmt0::Pmt0()
    : _raw_time(0.), _charge(0), _leading_time(0), _time(0.), _tof_key(""),
      _trigger_request_leading_time(0) {
}

Pmt0::Pmt0(const Pmt0& _pmt0)
    : _raw_time(0.), _charge(0), _leading_time(0), _time(0.), _tof_key(""),
      _trigger_request_leading_time(0) {
    *this = _pmt0;
}

Pmt0& Pmt0::operator=(const Pmt0& _pmt0) {
    if (this == &_pmt0) {
        return *this;
    }
    SetRawTime(_pmt0._raw_time);
    SetCharge(_pmt0._charge);
    SetLeadingTime(_pmt0._leading_time);
    SetTime(_pmt0._time);
    SetTofKey(_pmt0._tof_key);
    SetTriggerRequestLeadingTime(_pmt0._trigger_request_leading_time);
    return *this;
}

Pmt0::~Pmt0() {
}

double Pmt0::GetRawTime() const {
    return _raw_time;
}

void Pmt0::SetRawTime(double raw_time) {
    _raw_time = raw_time;
}

int Pmt0::GetCharge() const {
    return _charge;
}

void Pmt0::SetCharge(int charge) {
    _charge = charge;
}

int Pmt0::GetLeadingTime() const {
    return _leading_time;
}

void Pmt0::SetLeadingTime(int leading_time) {
    _leading_time = leading_time;
}

double Pmt0::GetTime() const {
    return _time;
}

void Pmt0::SetTime(double time) {
    _time = time;
}

std::string Pmt0::GetTofKey() const {
    return _tof_key;
}

void Pmt0::SetTofKey(std::string tof_key) {
    _tof_key = tof_key;
}

int Pmt0::GetTriggerRequestLeadingTime() const {
    return _trigger_request_leading_time;
}

void Pmt0::SetTriggerRequestLeadingTime(int trigger_request_leading_time) {
    _trigger_request_leading_time = trigger_request_leading_time;
}
}

