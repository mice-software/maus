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

#include "src/common_cpp/DataStructure/VLSB.hh"


namespace MAUS {

VLSB::VLSB()
    : _detector(""), _equip_type(0), _time_stamp(0),
      _phys_event_number(0), _bank_id(0), _adc(0),
      _part_event_number(0), _channel(0), _tdc(0),
      _discriminator(0), _ldc_id(0) {
}

VLSB::VLSB(const VLSB& vlsb)
    : _detector(""), _equip_type(0), _time_stamp(0),
      _phys_event_number(0), _bank_id(0), _adc(0),
      _part_event_number(0), _channel(0), _tdc(0),
      _discriminator(0), _ldc_id(0) {
    *this = vlsb;
}

VLSB& VLSB::operator=(const VLSB& vlsb) {
    if (this == &vlsb) {
        return *this;
    }
    SetDetector(vlsb._detector);
    SetEquipType(vlsb._equip_type);
    SetTimeStamp(vlsb._time_stamp);
    SetPhysEventNumber(vlsb._phys_event_number);
    SetBankID(vlsb._bank_id);
    SetADC(vlsb._adc);
    SetPartEventNumber(vlsb._part_event_number);
    SetChannel(vlsb._channel);
    SetTDC(vlsb._tdc);
    SetDiscriminator(vlsb._discriminator);
    SetLdcId(vlsb._ldc_id);
    return *this;
}

VLSB::~VLSB() {
}

std::string VLSB::GetDetector() const {
    return _detector;
}

void VLSB::SetDetector(std::string detector) {
    _detector = detector;
}

int VLSB::GetEquipType() const {
    return _equip_type;
}

void VLSB::SetEquipType(int equip_type) {
    _equip_type = equip_type;
}

int VLSB::GetTimeStamp() const {
    return _time_stamp;
}

void VLSB::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

int VLSB::GetPhysEventNumber() const {
    return _phys_event_number;
}

void VLSB::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int VLSB::GetBankID() const {
    return _bank_id;
}

void VLSB::SetBankID(int bank) {
    _bank_id = bank;
}

int VLSB::GetADC() const {
    return _adc;
}

void VLSB::SetADC(int adc) {
    _adc = adc;
}

int VLSB::GetPartEventNumber() const {
    return _part_event_number;
}

void VLSB::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int VLSB::GetChannel() const {
    return _channel;
}

void VLSB::SetChannel(int channel) {
    _channel = channel;
}

int VLSB::GetTDC() const {
    return _tdc;
}

void VLSB::SetTDC(int tdc) {
    _tdc = tdc;
}

int VLSB::GetDiscriminator() const {
    return _discriminator;
}

void VLSB::SetDiscriminator(int discriminator) {
    _discriminator = discriminator;
}

int VLSB::GetLdcId() const {
    return _ldc_id;
}

void VLSB::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}
}
