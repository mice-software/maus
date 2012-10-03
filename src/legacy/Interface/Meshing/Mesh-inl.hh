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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include <vector>

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

Mesh::Iterator::Iterator() {
}

Mesh::Iterator::Iterator(const Mesh::Iterator& in)
    : _mesh(in._mesh), _state(in._state)   {
}

Mesh::Iterator::Iterator(std::vector<int> state, const Mesh* mesh)
    : _mesh(mesh), _state(state) {
}

Mesh::Iterator::~Iterator() {
}

int Mesh::Iterator::ToInteger() const {
    return _mesh->ToInteger(*this);
}

std::vector<int> Mesh::Iterator::State() const {
    return _state;
}

int& Mesh::Iterator::operator[](int i) {
    return _state[i];
}

const int& Mesh::Iterator::operator[](int i) const {
    return _state[i];
}

const Mesh* Mesh::Iterator::GetMesh() const {
    return _mesh;
}

const Mesh::Iterator&  Mesh::Iterator::operator= (const Mesh::Iterator& rhs) {
    _mesh  = rhs._mesh;
    _state = rhs._state;
    return *this;
}

void Mesh::Iterator::Position(double* point) const {
    _mesh->Position(*this, point);
}

std::vector<double> Mesh::Iterator::Position() const {
    std::vector<double> PointV(_mesh->PositionDimension());
    _mesh->Position(*this, &PointV[0]);
    return PointV;
}

Mesh::Iterator& operator++(Mesh::Iterator& lhs) {
    return lhs._mesh->AddOne(lhs);
}

Mesh::Iterator& operator--(Mesh::Iterator& lhs) {
    return lhs._mesh->SubOne(lhs);
}

Mesh::Iterator  operator++(Mesh::Iterator& lhs, int) {
    Mesh::Iterator copy = lhs;
    lhs._mesh->AddOne(lhs);
    return copy;
}

Mesh::Iterator  operator--(Mesh::Iterator& lhs, int) {
    Mesh::Iterator copy = lhs;
    lhs._mesh->SubOne(lhs);
    return copy;
}

Mesh::Iterator  operator-
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    Mesh::Iterator lhsCopy(lhs);
    Mesh::Iterator lhsNew(lhs._mesh->SubEquals(lhsCopy, rhs));
    return lhsNew;
}

Mesh::Iterator  operator- (const Mesh::Iterator& lhs, const int& difference) {
    Mesh::Iterator lhsCopy(lhs);
    Mesh::Iterator lhsNew(lhs._mesh->SubEquals(lhsCopy, difference));
    return lhsNew;
}

Mesh::Iterator  operator+
                      (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    Mesh::Iterator lhsCopy(lhs);
    Mesh::Iterator lhsNew(lhs._mesh->AddEquals(lhsCopy, rhs));
    return lhsNew;
}

Mesh::Iterator  operator+ (const Mesh::Iterator& lhs, const int& difference) {
    Mesh::Iterator lhsCopy(lhs);
    Mesh::Iterator lhsNew(lhs._mesh->AddEquals(lhsCopy, difference));
    return lhsNew;
}


Mesh::Iterator& operator+=(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    return lhs._mesh->AddEquals(lhs, rhs);
}

Mesh::Iterator& operator-=(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    return lhs._mesh->SubEquals(lhs, rhs);
}

Mesh::Iterator& operator+=(Mesh::Iterator& lhs, const int& rhs) {
    return lhs._mesh->AddEquals(lhs, rhs);
}

Mesh::Iterator& operator-=(Mesh::Iterator& lhs, const int& rhs) {
    return lhs._mesh->SubEquals(lhs, rhs);
}


bool operator>=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    if (lhs._mesh->IsGreater(lhs, rhs) || lhs == rhs) return true;
    return false;
}

bool operator<=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    if (lhs._mesh->IsGreater(lhs, rhs)) return false;
    return true;
}

bool operator>(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    return !(lhs <= rhs);
}

bool operator<(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    return !(lhs >= rhs);
}

bool operator==(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    if (lhs._state == rhs._state)
        return true;
    return false;
}

bool operator!=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) {
    if (lhs._mesh == rhs._mesh && lhs._state == rhs._state)
        return false;
    return true;
}
