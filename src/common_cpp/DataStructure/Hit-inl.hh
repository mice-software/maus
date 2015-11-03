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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_HIT_INL_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_HIT_INL_HH_

namespace MAUS {

template <class ChannelId>
Hit<ChannelId>::Hit() : _track_id(0), _particle_id(0), _energy(0), _mass(0), _charge(0), _time(0),
             _energy_deposited(0), _path_length(0), _position(0, 0, 0), _momentum(0, 0, 0),
             _channel_id(NULL) {
}

template <class ChannelId>
Hit<ChannelId>::Hit(const Hit<ChannelId>& md)
           : _track_id(0), _particle_id(0), _energy(0), _mass(0), _charge(0), _time(0),
             _energy_deposited(0), _path_length(0), _position(0, 0, 0), _momentum(0, 0, 0),
             _channel_id(NULL) {
    *this = md;
}

template <class ChannelId>
Hit<ChannelId>& Hit<ChannelId>::operator=(const Hit<ChannelId>& md) {
    if (this == &md) {
        return *this;
    }
    _track_id = md._track_id;
    _particle_id = md._particle_id;
    _energy = md._energy;
    _charge = md._charge;
    _mass = md._mass;
    _time = md._time;
    _energy_deposited = md._energy_deposited;
    _path_length = md._path_length;
    _position = md._position;
    _momentum = md._momentum;
    if (_channel_id != NULL) {
        delete _channel_id;
    }
    if (md._channel_id == NULL) {
        _channel_id = NULL;
    } else {
        _channel_id = new ChannelId(*(md._channel_id));
    }
    return *this;
}

template <class ChannelId>
Hit<ChannelId>::~Hit() {
    if (_channel_id != NULL) {
        delete _channel_id;
        _channel_id = NULL;
    }
}

template <class ChannelId>
int Hit<ChannelId>::GetTrackId() const {
    return _track_id;
}

template <class ChannelId>
void Hit<ChannelId>::SetTrackId(int id) {
    _track_id = id;
}

template <class ChannelId>
int Hit<ChannelId>::GetParticleId() const {
    return _particle_id;
}

template <class ChannelId>
void Hit<ChannelId>::SetParticleId(int pid) {
    _particle_id = pid;
}

template <class ChannelId>
double Hit<ChannelId>::GetEnergy() const {
    return _energy;
}

template <class ChannelId>
void Hit<ChannelId>::SetEnergy(double energy) {
    _energy = energy;
}

template <class ChannelId>
double Hit<ChannelId>::GetCharge() const {
    return _charge;
}

template <class ChannelId>
void Hit<ChannelId>::SetCharge(double charge) {
    _charge = charge;
}

template <class ChannelId>
double Hit<ChannelId>::GetMass() const {
    return _mass;
}

template <class ChannelId>
void Hit<ChannelId>::SetMass(double mass) {
    _mass = mass;
}

template <class ChannelId>
double Hit<ChannelId>::GetTime() const {
    return _time;
}

template <class ChannelId>
void Hit<ChannelId>::SetTime(double time) {
    _time = time;
}

template <class ChannelId>
double Hit<ChannelId>::GetEnergyDeposited() const {
    return _energy_deposited;
}

template <class ChannelId>
void Hit<ChannelId>::SetEnergyDeposited(double edep) {
    _energy_deposited = edep;
}

template <class ChannelId>
void Hit<ChannelId>::AddEnergyDeposited(double edep) {
    _energy_deposited += edep;
}

template <class ChannelId>
ThreeVector Hit<ChannelId>::GetPosition() const {
    return _position;
}

template <class ChannelId>
void Hit<ChannelId>::SetPosition(ThreeVector pos) {
    _position = pos;
}

template <class ChannelId>
ThreeVector Hit<ChannelId>::GetMomentum() const {
    return _momentum;
}

template <class ChannelId>
void Hit<ChannelId>::SetMomentum(ThreeVector mom) {
    _momentum = mom;
}

template <class ChannelId>
ChannelId* Hit<ChannelId>::GetChannelId() const {
    return _channel_id;
}

template <class ChannelId>
void Hit<ChannelId>::SetChannelId(ChannelId* id) {
    if (_channel_id != NULL) {
        delete _channel_id;
    }
    _channel_id = id;
}

template <class ChannelId>
double Hit<ChannelId>::GetPathLength() const {
    return _path_length;
}

template <class ChannelId>
void Hit<ChannelId>::SetPathLength(double path) {
    _path_length = path;
}

template <class ChannelId>
void Hit<ChannelId>::AddPathLength(double path) {
    _path_length += path;
}
}

#endif

