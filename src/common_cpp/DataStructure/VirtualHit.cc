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

#include "src/common_cpp/DataStructure/VirtualHit.hh"

namespace MAUS {
VirtualHit::VirtualHit() : _station_id(0), _particle_id(0), _track_id(0),
      _time(0), _mass(0), _charge(0), _proper_time(0), _path_length(0), 
      _position(0, 0, 0), _momentum(0, 0, 0), _b_field(0, 0, 0),
      _e_field(0, 0, 0) {
}

VirtualHit::VirtualHit(const VirtualHit& md) {
    *this = md;
}
 
VirtualHit& VirtualHit::operator=(const VirtualHit& md) {
    _station_id = md._station_id;
    _particle_id = md._particle_id;
    _track_id = md._track_id;
    _time = md._time;
    _mass = md._mass;
    _charge = md._charge;
    _proper_time = md._proper_time;
    _path_length = md._path_length;
    _position = md._position;
    _momentum = md._momentum;
    _b_field = md._b_field;
    _e_field = md._e_field;
    return *this;
}

int VirtualHit::GetStationId() const {
    return _station_id;
}

void VirtualHit::SetStationId(int id) {
    _station_id = id;
}

int VirtualHit::GetParticleId() const {
    return _particle_id;
}

void VirtualHit::SetParticleId(int id) {
    _particle_id = id;
}

int VirtualHit::GetTrackId() const {
    return _track_id;
}

void VirtualHit::SetTrackId(int id) {
    _track_id = id;
}

double VirtualHit::GetTime() const {
    return _time;
}

void VirtualHit::SetTime(double time) {
    _time = time;
}

double VirtualHit::GetMass() const {
    return _mass;
}

void VirtualHit::SetMass(double mass) {
    _mass = mass;
}

double VirtualHit::GetCharge() const {
    return _charge;
}

void VirtualHit::SetCharge(double charge) {
    _charge = charge;
}

double VirtualHit::GetProperTime() const {
    return _proper_time;
}

void VirtualHit::SetProperTime(double proper_time) {
    _proper_time = proper_time;
}

double VirtualHit::GetPathLength() const {
    return _path_length;
}

void VirtualHit::SetPathLength(double path_length) {
    _path_length = path_length;
}

ThreeVector VirtualHit::GetPosition() const {
    return _position;
}

void VirtualHit::SetPosition(ThreeVector position) {
    _position = position;
}

ThreeVector VirtualHit::GetMomentum() const {
    return _momentum;
}

void VirtualHit::SetMomentum(ThreeVector momentum) {
    _momentum = momentum;
}

ThreeVector VirtualHit::GetBField() const {
    return _b_field;
}

void VirtualHit::SetBField(ThreeVector b_field) {
    _b_field = b_field;
}

ThreeVector VirtualHit::GetEField() const {
    return _e_field;
}

void VirtualHit::SetEField(ThreeVector e_field) {
    _e_field = e_field;
}

}

