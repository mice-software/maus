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

#include "src/common_cpp/DataStructure/VLSB_C.hh"


namespace MAUS {

VLSB_C::VLSB_C()
    : _detector(""), _equip_type(0), _time_stamp(0),
      _phys_event_number(0), _bank_id(0), _adc(0),
      _part_event_number(0), _channel(0), _tdc(0),
      _discriminator(0), _ldc_id(0), _geo(0) {
}

VLSB_C::VLSB_C(const VLSB_C& vlsb_c)
    : _detector(""), _equip_type(0), _time_stamp(0),
      _phys_event_number(0), _bank_id(0), _adc(0),
      _part_event_number(0), _channel(0), _tdc(0),
      _discriminator(0), _ldc_id(0), _geo(0) {
    *this = vlsb_c;
}

VLSB_C& VLSB_C::operator=(const VLSB_C& vlsb_c) {
    if (this == &vlsb_c) {
        return *this;
    }
    SetDetector(vlsb_c._detector);
    SetEquipType(vlsb_c._equip_type);
    SetTimeStamp(vlsb_c._time_stamp);
    SetPhysEventNumber(vlsb_c._phys_event_number);
    SetBankID(vlsb_c._bank_id);
    SetADC(vlsb_c._adc);
    SetPartEventNumber(vlsb_c._part_event_number);
    SetChannel(vlsb_c._channel);
    SetTDC(vlsb_c._tdc);
    SetDiscriminator(vlsb_c._discriminator);
    SetLdcId(vlsb_c._ldc_id);
    SetGeo(vlsb_c._geo);
    return *this;
}

VLSB_C::~VLSB_C() {
}

std::string VLSB_C::GetDetector() const {
    return _detector;
}

void VLSB_C::SetDetector(std::string detector) {
    _detector = detector;
}

int VLSB_C::GetEquipType() const {
    return _equip_type;
}

void VLSB_C::SetEquipType(int equip_type) {
    _equip_type = equip_type;
}

int VLSB_C::GetTimeStamp() const {
    return _time_stamp;
}

void VLSB_C::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

int VLSB_C::GetPhysEventNumber() const {
    return _phys_event_number;
}

void VLSB_C::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int VLSB_C::GetBankID() const {
    return _bank_id;
}

void VLSB_C::SetBankID(int bank) {
    _bank_id = bank;
}

int VLSB_C::GetADC() const {
    return _adc;
}

void VLSB_C::SetADC(int adc) {
    _adc = adc;
}

int VLSB_C::GetPartEventNumber() const {
    return _part_event_number;
}

void VLSB_C::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int VLSB_C::GetChannel() const {
    return _channel;
}

void VLSB_C::SetChannel(int channel) {
    _channel = channel;
}

int VLSB_C::GetTDC() const {
    return _tdc;
}

void VLSB_C::SetTDC(int tdc) {
    _tdc = tdc;
}

int VLSB_C::GetDiscriminator() const {
    return _discriminator;
}

void VLSB_C::SetDiscriminator(int discriminator) {
    _discriminator = discriminator;
}

int VLSB_C::GetLdcId() const {
    return _ldc_id;
}

void VLSB_C::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

int VLSB_C::GetGeo() const {
    return _geo;
}

void VLSB_C::SetGeo(int geo) {
    _geo = geo;
}
}
