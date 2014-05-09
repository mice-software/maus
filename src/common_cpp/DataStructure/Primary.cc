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

#include "src/common_cpp/DataStructure/Primary.hh"

namespace MAUS {

Primary::Primary() : _seed(0), _particle_id(0), _energy(0.), _time(0.),
                     _position(0, 0, 0), _momentum(0, 0, 0), _spin(0, 0, 0) {
}

Primary::Primary(const Primary& md) {
    *this = md;
}

Primary& Primary::operator=(const Primary& md) {
    if (this == &md) {
        return *this;
    }
    _seed = md._seed;
    _particle_id = md._particle_id;
    _energy = md._energy;
    _time = md._time;
    _position = md._position;
    _momentum = md._momentum;
    _spin = md._spin;
    return *this;
}

Primary::~Primary() {
}

int Primary::GetParticleId() const {
    return _particle_id;
}

void Primary::SetParticleId(int id) {
    _particle_id = id;
}

double Primary::GetEnergy() const {
    return _energy;
}

void Primary::SetEnergy(double energy) {
    _energy = energy;
}

double Primary::GetTime() const {
    return _time;
}

void Primary::SetTime(double time) {
    _time = time;
}

ThreeVector Primary::GetPosition() const {
    return _position;
}

void Primary::SetPosition(ThreeVector pos) {
    _position = pos;
}

ThreeVector Primary::GetMomentum() const {
    return _momentum;
}

void Primary::SetMomentum(ThreeVector mom) {
    _momentum = mom;
}


ThreeVector Primary::GetSpin() const {
    return _spin;
}

void Primary::SetSpin(ThreeVector spin) {
    _spin = spin;
}

int Primary::GetRandomSeed() const {
    return _seed;
}

void Primary::SetRandomSeed(int seed) {
    _seed = seed;
}
}

