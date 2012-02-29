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

#include "src/common_cpp/DataStructure/Hit.hh"

namespace MAUS {

Hit::Hit() : _track_id(0), _particle_id(0), _energy(0), _charge(0), _time(0),
             _energy_deposited(0), _position(0, 0, 0), _momentum(0, 0, 0),
             _channel_id(NULL) {
}

Hit::Hit(const Hit& md)
           : _track_id(0), _particle_id(0), _energy(0), _charge(0), _time(0),
             _energy_deposited(0), _position(0, 0, 0), _momentum(0, 0, 0),
             _channel_id(NULL) {
    *this = md;
}
         
Hit& Hit::operator=(const Hit& md) {
    if (this == &md) {
        return *this;
    }
    _track_id = md._track_id;
    _particle_id = md._particle_id;
    _energy = md._energy;
    _charge = md._charge;
    _time = md._time;
    _energy_deposited = md._energy_deposited;
    _position = md._position;
    _momentum = md._momentum;
    if (_channel_id != NULL) {
        delete _channel_id;
    }
    if (md._channel_id == NULL) {
        _channel_id = NULL;
    } else {
        _channel_id = md._channel_id->Clone();
    }
    return *this;
}
 
Hit::~Hit() {
    if (_channel_id != NULL) {
        delete _channel_id;
        _channel_id = NULL;
    }
}


int Hit::GetTrackId() const {
    return _track_id;    
}

void Hit::SetTrackId(int id) {
    _track_id = id;
}

int Hit::GetParticleId() const {
    return _particle_id;
}

void Hit::SetParticleId(int pid) {
    _particle_id = pid;
}

double Hit::GetEnergy() const {
    return _energy;
}

void Hit::SetEnergy(double energy) {
    _energy = energy;
}

double Hit::GetCharge() const {
    return _charge;
}

void Hit::SetCharge(double charge) {
    _charge = charge;
}

double Hit::GetTime() const {
    return _time;
}

void Hit::SetTime(double time) {
    _time = time;
}

double Hit::GetEnergyDeposited() const {
    return _energy_deposited;
}

void Hit::SetEnergyDeposited(double edep) {
    _energy_deposited = edep;
}

ThreeVector Hit::GetPosition() const {
    return _position;
}

void Hit::SetPosition(ThreeVector pos) {
    _position = pos;
}

ThreeVector Hit::GetMomentum() const {
    return _momentum;
}

void Hit::SetMomentum(ThreeVector mom) {
    _momentum = mom;
}

ChannelId* Hit::GetChannelId() const {
    return _channel_id;
}

void Hit::SetChannelId(ChannelId* id) {
    if (_channel_id != NULL) {
        delete _channel_id;
    }
    _channel_id = id;
}

}
