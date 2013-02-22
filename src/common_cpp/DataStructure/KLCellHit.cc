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

#include "src/common_cpp/DataStructure/KLCellHit.hh"


namespace MAUS {

KLCellHit::KLCellHit()
    : _cell(0), _phys_event_number(0), _charge(0),
      _charge_product(0), _detector(""),
      _part_event_number(0) {
}

KLCellHit::KLCellHit(int cell, int phys_event_number, int charge,  int charge_product,
                     std::string detector, int part_event_number)
    : _cell(cell), _phys_event_number(phys_event_number), _charge(charge),
      _charge_product(charge_product), _detector(detector),
      _part_event_number(part_event_number) {
}

KLCellHit::KLCellHit(const KLCellHit& _klcellhit)
    : _cell(0), _phys_event_number(0), _charge(0),
      _charge_product(0), _detector(""),
      _part_event_number(0) {
    *this = _klcellhit;
}

KLCellHit& KLCellHit::operator=(const KLCellHit& _klcellhit) {
    if (this == &_klcellhit) {
        return *this;
    }
    SetCell(_klcellhit._cell);
    SetPhysEventNumber(_klcellhit._phys_event_number);
    SetCharge(_klcellhit._charge);
    SetChargeProduct(_klcellhit._charge_product);
    SetDetector(_klcellhit._detector);
    SetPartEventNumber(_klcellhit._part_event_number);
    return *this;
}

KLCellHit::~KLCellHit() {
}

int KLCellHit::GetCell() const {
    return _cell;
}

void KLCellHit::SetCell(int cell) {
    _cell = cell;
}

int KLCellHit::GetPhysEventNumber() const {
    return _phys_event_number;
}

void KLCellHit::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int KLCellHit::GetCharge() const {
    return _charge;
}

void KLCellHit::SetCharge(int charge) {
    _charge = charge;
}

int KLCellHit::GetChargeProduct() const {
    return _charge_product;
}

void KLCellHit::SetChargeProduct(int charge_product) {
    _charge_product = charge_product;
}

std::string KLCellHit::GetDetector() const {
    return _detector;
}

void KLCellHit::SetDetector(std::string detector) {
    _detector = detector;
}

int KLCellHit::GetPartEventNumber() const {
    return _part_event_number;
}

void KLCellHit::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}
}

