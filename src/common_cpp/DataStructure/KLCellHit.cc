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
      _charge_product(0), _detector(""), _part_event_number(0),
      _global_pos_x(0), _global_pos_y(0), _global_pos_z(0),
      _local_pos_x(0), _local_pos_y(0), _local_pos_z(0),
      _error_x(0), _error_y(0), _error_z(0),
      _flag(0) {
}

KLCellHit::KLCellHit(int cell, int phys_event_number, int charge,  int charge_product,
                     std::string detector, int part_event_number,
		     double global_pos_x, double global_pos_y, double global_pos_z,
		     double local_pos_x,  double local_pos_y, double local_pos_z,
		     double error_x,  double error_y, double error_z,
		     bool flag)
    : _cell(cell), _phys_event_number(phys_event_number), _charge(charge),
      _charge_product(charge_product), _detector(detector),
      _part_event_number(part_event_number),
      _global_pos_x(global_pos_x), _global_pos_y(global_pos_y), _global_pos_z(global_pos_z),
      _local_pos_x(local_pos_x), _local_pos_y(local_pos_y), _local_pos_z(local_pos_z),
      _error_x(error_x), _error_y(error_y), _error_z(error_z), _flag(flag) {
}

KLCellHit::KLCellHit(const KLCellHit& _klcellhit)
    : _cell(0), _phys_event_number(0), _charge(0),
      _charge_product(0), _detector(""), _part_event_number(0),
      _global_pos_x(0), _global_pos_y(0), _global_pos_z(0),
      _local_pos_x(0), _local_pos_y(0), _local_pos_z(0),
      _error_x(0), _error_y(0), _error_z(0), _flag(0) {
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
    SetGlobalPosX(_klcellhit._global_pos_x);
    SetGlobalPosY(_klcellhit._global_pos_y);
    SetGlobalPosZ(_klcellhit._global_pos_z);
    SetLocalPosX(_klcellhit._local_pos_x);
    SetLocalPosY(_klcellhit._local_pos_y);
    SetLocalPosZ(_klcellhit._local_pos_z);
    SetErrorX(_klcellhit._error_x);
    SetErrorY(_klcellhit._error_y);
    SetErrorZ(_klcellhit._error_z);
    SetFlag(_klcellhit._flag);
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

double KLCellHit::GetGlobalPosX() const {
    return _global_pos_x;
}

double KLCellHit::GetGlobalPosY() const {
    return _global_pos_y;
}

double KLCellHit::GetGlobalPosZ() const {
    return _global_pos_z;
}

void KLCellHit::SetGlobalPosX(double global_pos_x) {
    _global_pos_x = global_pos_x;
}

void KLCellHit::SetGlobalPosY(double global_pos_y) {
    _global_pos_y = global_pos_y;
}

void KLCellHit::SetGlobalPosZ(double global_pos_z) {
    _global_pos_z = global_pos_z;
}

double KLCellHit::GetLocalPosX() const {
    return _local_pos_x;
}

double KLCellHit::GetLocalPosY() const {
    return _local_pos_y;
}

double KLCellHit::GetLocalPosZ() const {
    return _local_pos_z;
}

void KLCellHit::SetLocalPosX(double local_pos_x) {
    _local_pos_x = local_pos_x;
}

void KLCellHit::SetLocalPosY(double local_pos_y) {
    _local_pos_y = local_pos_y;
}

void KLCellHit::SetLocalPosZ(double local_pos_z) {
    _local_pos_z = local_pos_z;
}

double KLCellHit::GetErrorX() const {
    return _error_x;
}

double KLCellHit::GetErrorY() const {
    return _error_y;
}

double KLCellHit::GetErrorZ() const {
    return _error_z;
}

void KLCellHit::SetErrorX(double error_x) {
    _error_x = error_x;
}

void KLCellHit::SetErrorY(double error_y) {
    _error_y = error_y;
}

void KLCellHit::SetErrorZ(double error_z) {
    _error_z = error_z;
}

bool KLCellHit::GetFlag() const {
    return _flag;
}

void KLCellHit::SetFlag(bool flag) {
    _flag = flag;
}
}

