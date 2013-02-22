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

#include "src/common_cpp/DataStructure/KLDigit.hh"


namespace MAUS {

KLDigit::KLDigit()
    : _pmt(0), _charge_mm(0), _charge_pm(0), _phys_event_number(0),
      _kl_key(""), _part_event_number(0), _cell(0), _position_max(0) {
}

KLDigit::KLDigit(int pmt, int charge_mm, int charge_pm, int phys_event_number,
                 std::string kl_key, int part_event_number, int cell, int position_max)
    : _pmt(pmt), _charge_mm(charge_mm), _charge_pm(charge_pm),
      _phys_event_number(phys_event_number),
      _kl_key(kl_key), _part_event_number(part_event_number),
      _cell(cell), _position_max(position_max) {
}

KLDigit::KLDigit(const KLDigit& _kldigit)
    : _pmt(0), _charge_mm(0), _charge_pm(0), _phys_event_number(0),
      _kl_key(""), _part_event_number(0), _cell(0), _position_max(0) {
    *this = _kldigit;
}

KLDigit& KLDigit::operator=(const KLDigit& _kldigit) {
    if (this == &_kldigit) {
        return *this;
    }
    SetPmt(_kldigit._pmt);
    SetChargeMm(_kldigit._charge_mm);
    SetChargePm(_kldigit._charge_pm);
    SetPhysEventNumber(_kldigit._phys_event_number);
    SetKlKey(_kldigit._kl_key);
    SetPartEventNumber(_kldigit._part_event_number);
    SetCell(_kldigit._cell);
    SetPositionMax(_kldigit._position_max);
    return *this;
}

KLDigit::~KLDigit() {
}

int KLDigit::GetPmt() const {
    return _pmt;
}

void KLDigit::SetPmt(int pmt) {
    _pmt = pmt;
}

int KLDigit::GetChargeMm() const {
    return _charge_mm;
}

void KLDigit::SetChargeMm(int charge_mm) {
    _charge_mm = charge_mm;
}

int KLDigit::GetChargePm() const {
    return _charge_pm;
}

void KLDigit::SetChargePm(int charge_pm) {
    _charge_pm = charge_pm;
}

int KLDigit::GetPhysEventNumber() const {
    return _phys_event_number;
}

void KLDigit::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

std::string KLDigit::GetKlKey() const {
    return _kl_key;
}

void KLDigit::SetKlKey(std::string kl_key) {
    _kl_key = kl_key;
}

int KLDigit::GetPartEventNumber() const {
    return _part_event_number;
}

void KLDigit::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

int KLDigit::GetCell() const {
    return _cell;
}

void KLDigit::SetCell(int cell) {
    _cell = cell;
}

int KLDigit::GetPositionMax() const {
    return _position_max;
}

void KLDigit::SetPositionMax(int position_max) {
    _position_max = position_max;
}
}

