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

#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"


namespace MAUS {

TOFSpacePoint::TOFSpacePoint()
    : _phys_event_number(0), _pixel_key(""), _charge(0), _station(0),
      _slabY(0), _slabX(0), _charge_product(0), _time(0.), _detector(""),
      _part_event_number(0), _dt(0.), _global_x(0), _global_y(0), _global_z(0),
      _global_x_err(0), _global_y_err(0), _global_z_err(0) {
}

TOFSpacePoint::TOFSpacePoint(const TOFSpacePoint& _tofspacepoint)
    : _phys_event_number(0), _pixel_key(""), _charge(0), _station(0),
      _slabY(0), _slabX(0), _charge_product(0), _time(0.), _detector(""),
      _part_event_number(0), _dt(0.), _global_x(0), _global_y(0), _global_z(0),
      _global_x_err(0), _global_y_err(0), _global_z_err(0) {
    *this = _tofspacepoint;
}

TOFSpacePoint& TOFSpacePoint::operator=(const TOFSpacePoint& _tofspacepoint) {
    if (this == &_tofspacepoint) {
        return *this;
    }
    SetPhysEventNumber(_tofspacepoint._phys_event_number);
    SetPixelKey(_tofspacepoint._pixel_key);
    SetCharge(_tofspacepoint._charge);
    SetStation(_tofspacepoint._station);
    SetSlaby(_tofspacepoint._slabY);
    SetSlabx(_tofspacepoint._slabX);
    SetChargeProduct(_tofspacepoint._charge_product);
    SetTime(_tofspacepoint._time);
    SetDetector(_tofspacepoint._detector);
    SetPartEventNumber(_tofspacepoint._part_event_number);
    SetDt(_tofspacepoint._dt);
    SetGlobalPosX(_tofspacepoint._global_x);
    SetGlobalPosY(_tofspacepoint._global_y);
    SetGlobalPosZ(_tofspacepoint._global_z);
    SetGlobalPosXErr(_tofspacepoint._global_x_err);
    SetGlobalPosYErr(_tofspacepoint._global_y_err);
    SetGlobalPosZErr(_tofspacepoint._global_z_err);
    return *this;
}

TOFSpacePoint::~TOFSpacePoint() {
}

int TOFSpacePoint::GetPhysEventNumber() const {
    return _phys_event_number;
}

void TOFSpacePoint::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

std::string TOFSpacePoint::GetPixelKey() const {
    return _pixel_key;
}

void TOFSpacePoint::SetPixelKey(std::string pixel_key) {
    _pixel_key = pixel_key;
}

int TOFSpacePoint::GetCharge() const {
    return _charge;
}

void TOFSpacePoint::SetCharge(int charge) {
    _charge = charge;
}

int TOFSpacePoint::GetStation() const {
    return _station;
}

void TOFSpacePoint::SetStation(int station) {
    _station = station;
}

int TOFSpacePoint::GetSlaby() const {
    return _slabY;
}

void TOFSpacePoint::SetSlaby(int slabY) {
    _slabY = slabY;
}

int TOFSpacePoint::GetSlabx() const {
    return _slabX;
}

void TOFSpacePoint::SetSlabx(int slabX) {
    _slabX = slabX;
}

int TOFSpacePoint::GetVertSlab() const {
    return _slabY;
}

void TOFSpacePoint::SetVertSlab(int slabY) {
    _slabY = slabY;
}

int TOFSpacePoint::GetHorizSlab() const {
    return _slabX;
}

void TOFSpacePoint::SetHorizSlab(int slabX) {
    _slabX = slabX;
}

double TOFSpacePoint::GetGlobalPosX() const {
    return _global_x;
}

double TOFSpacePoint::GetGlobalPosY() const {
    return _global_y;
}

double TOFSpacePoint::GetGlobalPosZ() const {
    return _global_z;
}

double TOFSpacePoint::GetGlobalPosXErr() const {
    return _global_x_err;
}

double TOFSpacePoint::GetGlobalPosYErr() const {
    return _global_y_err;
}

double TOFSpacePoint::GetGlobalPosZErr() const {
    return _global_z_err;
}

void TOFSpacePoint::SetGlobalPosX(double global_x) {
    _global_x = global_x;
}

void TOFSpacePoint::SetGlobalPosY(double global_y) {
    _global_y = global_y;
}

void TOFSpacePoint::SetGlobalPosZ(double global_z) {
    _global_z = global_z;
}

void TOFSpacePoint::SetGlobalPosXErr(double global_x_err) {
    _global_x_err = global_x_err;
}

void TOFSpacePoint::SetGlobalPosYErr(double global_y_err) {
    _global_y_err = global_y_err;
}

void TOFSpacePoint::SetGlobalPosZErr(double global_z_err) {
    _global_z_err = global_z_err;
}

int TOFSpacePoint::GetChargeProduct() const {
    return _charge_product;
}

void TOFSpacePoint::SetChargeProduct(int charge_product) {
    _charge_product = charge_product;
}

double TOFSpacePoint::GetTime() const {
    return _time;
}

void TOFSpacePoint::SetTime(double time) {
    _time = time;
}

std::string TOFSpacePoint::GetDetector() const {
    return _detector;
}

void TOFSpacePoint::SetDetector(std::string detector) {
    _detector = detector;
}

int TOFSpacePoint::GetPartEventNumber() const {
    return _part_event_number;
}

void TOFSpacePoint::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

double TOFSpacePoint::GetDt() const {
    return _dt;
}

void TOFSpacePoint::SetDt(double dt) {
    _dt = dt;
}
}

