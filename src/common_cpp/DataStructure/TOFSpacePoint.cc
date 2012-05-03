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
      _part_event_number(0), _dt(0.) {
}

TOFSpacePoint::TOFSpacePoint(const TOFSpacePoint& _tofspacepoint)
    : _phys_event_number(0), _pixel_key(""), _charge(0), _station(0),
      _slabY(0), _slabX(0), _charge_product(0), _time(0.), _detector(""),
      _part_event_number(0), _dt(0.) {
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

