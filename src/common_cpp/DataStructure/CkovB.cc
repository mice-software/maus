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

#include "src/common_cpp/DataStructure/CkovB.hh"


namespace MAUS {

CkovB::CkovB()
    : _arrival_time_6(0), _arrival_time_7(0), _arrival_time_4(0),
      _arrival_time_5(0), _pulse_5(0), _pulse_4(0), _pulse_7(0), _pulse_6(0),
      _position_min_6(0), _coincidences(0), _total_charge(0),
      _part_event_number(0), _position_min_7(0), _number_of_pes(0.),
      _position_min_5(0), _position_min_4(0) {
}

CkovB::CkovB(const CkovB& _ckovb)
    : _arrival_time_6(0), _arrival_time_7(0), _arrival_time_4(0),
      _arrival_time_5(0), _pulse_5(0), _pulse_4(0), _pulse_7(0), _pulse_6(0),
      _position_min_6(0), _coincidences(0), _total_charge(0),
      _part_event_number(0), _position_min_7(0), _number_of_pes(0.),
      _position_min_5(0), _position_min_4(0) {
    *this = _ckovb;
}

CkovB& CkovB::operator=(const CkovB& _ckovb) {
    if (this == &_ckovb) {
        return *this;
    }
    SetArrivalTime6(_ckovb._arrival_time_6);
    SetArrivalTime7(_ckovb._arrival_time_7);
    SetArrivalTime4(_ckovb._arrival_time_4);
    SetArrivalTime5(_ckovb._arrival_time_5);
    SetPulse5(_ckovb._pulse_5);
    SetPulse4(_ckovb._pulse_4);
    SetPulse7(_ckovb._pulse_7);
    SetPulse6(_ckovb._pulse_6);
    SetPositionMin6(_ckovb._position_min_6);
    SetCoincidences(_ckovb._coincidences);
    SetTotalCharge(_ckovb._total_charge);
    SetPartEventNumber(_ckovb._part_event_number);
    SetPositionMin7(_ckovb._position_min_7);
    SetNumberOfPes(_ckovb._number_of_pes);
    SetPositionMin5(_ckovb._position_min_5);
    SetPositionMin4(_ckovb._position_min_4);
    return *this;
}

CkovB::~CkovB() {
}

int CkovB::GetArrivalTime6() const {
    return _arrival_time_6;
}

void CkovB::SetArrivalTime6(int arrival_time_6) {
    _arrival_time_6 = arrival_time_6;
}

int CkovB::GetArrivalTime7() const {
    return _arrival_time_7;
}

void CkovB::SetArrivalTime7(int arrival_time_7) {
    _arrival_time_7 = arrival_time_7;
}

int CkovB::GetArrivalTime4() const {
    return _arrival_time_4;
}

void CkovB::SetArrivalTime4(int arrival_time_4) {
    _arrival_time_4 = arrival_time_4;
}

int CkovB::GetArrivalTime5() const {
    return _arrival_time_5;
}

void CkovB::SetArrivalTime5(int arrival_time_5) {
    _arrival_time_5 = arrival_time_5;
}

int CkovB::GetPulse5() const {
    return _pulse_5;
}

void CkovB::SetPulse5(int pulse_5) {
    _pulse_5 = pulse_5;
}

int CkovB::GetPulse4() const {
    return _pulse_4;
}

void CkovB::SetPulse4(int pulse_4) {
    _pulse_4 = pulse_4;
}

int CkovB::GetPulse7() const {
    return _pulse_7;
}

void CkovB::SetPulse7(int pulse_7) {
    _pulse_7 = pulse_7;
}

int CkovB::GetPulse6() const {
    return _pulse_6;
}

void CkovB::SetPulse6(int pulse_6) {
    _pulse_6 = pulse_6;
}

int CkovB::GetPositionMin6() const {
    return _position_min_6;
}

void CkovB::SetPositionMin6(int position_min_6) {
    _position_min_6 = position_min_6;
}

int CkovB::GetCoincidences() const {
    return _coincidences;
}

void CkovB::SetCoincidences(int coincidences) {
    _coincidences = coincidences;
}

int CkovB::GetTotalCharge() const {
    return _total_charge;
}

void CkovB::SetTotalCharge(int total_charge) {
    _total_charge = total_charge;
}

int CkovB::GetPartEventNumber() const {
    return _part_event_number;
}

void CkovB::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int CkovB::GetPositionMin7() const {
    return _position_min_7;
}

void CkovB::SetPositionMin7(int position_min_7) {
    _position_min_7 = position_min_7;
}

double CkovB::GetNumberOfPes() const {
    return _number_of_pes;
}

void CkovB::SetNumberOfPes(double number_of_pes) {
    _number_of_pes = number_of_pes;
}

int CkovB::GetPositionMin5() const {
    return _position_min_5;
}

void CkovB::SetPositionMin5(int position_min_5) {
    _position_min_5 = position_min_5;
}

int CkovB::GetPositionMin4() const {
    return _position_min_4;
}

void CkovB::SetPositionMin4(int position_min_4) {
    _position_min_4 = position_min_4;
}
}

