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

#include "DataStructure/EMRBarHit.hh"

namespace MAUS {

EMRBarHit::EMRBarHit()
  : _tot(0), _deltat(0), _hittime(0),
    _x(0.0), _ex(0.0), _y(0.0),
    _ey(0.0), _z(0.0), _ez(0.0),
    _charge_corrected(0.0), _total_charge_corrected(0.0) {
}

EMRBarHit::EMRBarHit(const EMRBarHit& _emrbarhit)
  : _tot(0), _deltat(0), _hittime(0),
    _x(0.0), _ex(0.0), _y(0.0),
    _ey(0.0), _z(0.0), _ez(0.0),
    _charge_corrected(0.0), _total_charge_corrected(0.0) {
  *this = _emrbarhit;
}

EMRBarHit& EMRBarHit::operator=(const EMRBarHit& _emrbarhit) {
    if (this == &_emrbarhit) {
        return *this;
    }

    SetTot(_emrbarhit._tot);
    SetDeltaT(_emrbarhit._deltat);
    SetHitTime(_emrbarhit._hittime);
    SetX(_emrbarhit._x);
    SetErrorX(_emrbarhit._ex);
    SetY(_emrbarhit._y);
    SetErrorY(_emrbarhit._ey);
    SetZ(_emrbarhit._z);
    SetErrorZ(_emrbarhit._ez);
    SetChargeCorrected(_emrbarhit._charge_corrected);
    SetTotalChargeCorrected(_emrbarhit._total_charge_corrected);
    return *this;
}

EMRBarHit::~EMRBarHit() {
}

int EMRBarHit::GetTot() const {
    return _tot;
}

void EMRBarHit::SetTot(int tot) {
    _tot = tot;
}

int EMRBarHit::GetDeltaT() const {
    return _deltat;
}

void EMRBarHit::SetDeltaT(int deltat) {
    _deltat = deltat;
}

int EMRBarHit::GetHitTime() const {
    return _hittime;
}

void EMRBarHit::SetHitTime(int hittime) {
    _hittime = hittime;
}

double EMRBarHit::GetX() const {
    return _x;
}

void EMRBarHit::SetX(double x) {
    _x = x;
}

double EMRBarHit::GetErrorX() const {
    return _ex;
}

void EMRBarHit::SetErrorX(double ex) {
    _ex = ex;
}

double EMRBarHit::GetY() const {
    return _y;
}

void EMRBarHit::SetY(double y) {
    _y = y;
}

double EMRBarHit::GetErrorY() const {
    return _ey;
}

void EMRBarHit::SetErrorY(double ey) {
    _ey = ey;
}

double EMRBarHit::GetZ() const {
    return _z;
}

void EMRBarHit::SetZ(double z) {
    _z = z;
}

double EMRBarHit::GetErrorZ() const {
    return _ez;
}

void EMRBarHit::SetErrorZ(double ez) {
    _ez = ez;
}

double EMRBarHit::GetChargeCorrected() const {
    return _charge_corrected;
}

void EMRBarHit::SetChargeCorrected(double charge_corrected) {
    _charge_corrected = charge_corrected;
}

double EMRBarHit::GetTotalChargeCorrected() const {
    return _total_charge_corrected;
}

void EMRBarHit::SetTotalChargeCorrected(double total_charge_corrected) {
    _total_charge_corrected = total_charge_corrected;
}
}

