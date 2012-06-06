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

#include "src/common_cpp/DataStructure/CkovA.hh"


namespace MAUS {

CkovA::CkovA()
    : _position_min_0(0), _arrival_time_2(0), _arrival_time_3(0),
      _arrival_time_0(0), _arrival_time_1(0), _pulse_1(0), _pulse_0(0),
      _pulse_3(0), _pulse_2(0), _coincidences(0), _position_min_2(0),
      _position_min_3(0), _total_charge(0), _position_min_1(0),
      _part_event_number(0), _number_of_pes(0.) {
}

CkovA::CkovA(const CkovA& _ckova)
    : _position_min_0(0), _arrival_time_2(0), _arrival_time_3(0),
      _arrival_time_0(0), _arrival_time_1(0), _pulse_1(0), _pulse_0(0),
      _pulse_3(0), _pulse_2(0), _coincidences(0), _position_min_2(0),
      _position_min_3(0), _total_charge(0), _position_min_1(0),
      _part_event_number(0), _number_of_pes(0.) {
    *this = _ckova;
}

CkovA& CkovA::operator=(const CkovA& _ckova) {
    if (this == &_ckova) {
        return *this;
    }
    SetPositionMin0(_ckova._position_min_0);
    SetArrivalTime2(_ckova._arrival_time_2);
    SetArrivalTime3(_ckova._arrival_time_3);
    SetArrivalTime0(_ckova._arrival_time_0);
    SetArrivalTime1(_ckova._arrival_time_1);
    SetPulse1(_ckova._pulse_1);
    SetPulse0(_ckova._pulse_0);
    SetPulse3(_ckova._pulse_3);
    SetPulse2(_ckova._pulse_2);
    SetCoincidences(_ckova._coincidences);
    SetPositionMin2(_ckova._position_min_2);
    SetPositionMin3(_ckova._position_min_3);
    SetTotalCharge(_ckova._total_charge);
    SetPositionMin1(_ckova._position_min_1);
    SetPartEventNumber(_ckova._part_event_number);
    SetNumberOfPes(_ckova._number_of_pes);
    return *this;
}

CkovA::~CkovA() {
}

int CkovA::GetPositionMin0() const {
    return _position_min_0;
}

void CkovA::SetPositionMin0(int position_min_0) {
    _position_min_0 = position_min_0;
}

int CkovA::GetArrivalTime2() const {
    return _arrival_time_2;
}

void CkovA::SetArrivalTime2(int arrival_time_2) {
    _arrival_time_2 = arrival_time_2;
}

int CkovA::GetArrivalTime3() const {
    return _arrival_time_3;
}

void CkovA::SetArrivalTime3(int arrival_time_3) {
    _arrival_time_3 = arrival_time_3;
}

int CkovA::GetArrivalTime0() const {
    return _arrival_time_0;
}

void CkovA::SetArrivalTime0(int arrival_time_0) {
    _arrival_time_0 = arrival_time_0;
}

int CkovA::GetArrivalTime1() const {
    return _arrival_time_1;
}

void CkovA::SetArrivalTime1(int arrival_time_1) {
    _arrival_time_1 = arrival_time_1;
}

int CkovA::GetPulse1() const {
    return _pulse_1;
}

void CkovA::SetPulse1(int pulse_1) {
    _pulse_1 = pulse_1;
}

int CkovA::GetPulse0() const {
    return _pulse_0;
}

void CkovA::SetPulse0(int pulse_0) {
    _pulse_0 = pulse_0;
}

int CkovA::GetPulse3() const {
    return _pulse_3;
}

void CkovA::SetPulse3(int pulse_3) {
    _pulse_3 = pulse_3;
}

int CkovA::GetPulse2() const {
    return _pulse_2;
}

void CkovA::SetPulse2(int pulse_2) {
    _pulse_2 = pulse_2;
}

int CkovA::GetCoincidences() const {
    return _coincidences;
}

void CkovA::SetCoincidences(int coincidences) {
    _coincidences = coincidences;
}

int CkovA::GetPositionMin2() const {
    return _position_min_2;
}

void CkovA::SetPositionMin2(int position_min_2) {
    _position_min_2 = position_min_2;
}

int CkovA::GetPositionMin3() const {
    return _position_min_3;
}

void CkovA::SetPositionMin3(int position_min_3) {
    _position_min_3 = position_min_3;
}

int CkovA::GetTotalCharge() const {
    return _total_charge;
}

void CkovA::SetTotalCharge(int total_charge) {
    _total_charge = total_charge;
}

int CkovA::GetPositionMin1() const {
    return _position_min_1;
}

void CkovA::SetPositionMin1(int position_min_1) {
    _position_min_1 = position_min_1;
}

int CkovA::GetPartEventNumber() const {
    return _part_event_number;
}

void CkovA::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

double CkovA::GetNumberOfPes() const {
    return _number_of_pes;
}

void CkovA::SetNumberOfPes(double number_of_pes) {
    _number_of_pes = number_of_pes;
}
}

