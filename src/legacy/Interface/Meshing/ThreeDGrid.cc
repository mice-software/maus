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

#include "src/legacy/Interface/Meshing/ThreeDGrid.hh"
#include "Utils/Exception.hh"

ThreeDGrid::ThreeDGrid()
    : _x(2, 0), _y(2, 0), _z(2, 0), _xSize(0), _ySize(0), _zSize(0), _maps(0),
      _constantSpacing(false) {
    SetConstantSpacing();
    _x[1] = _y[1] = _z[1] = 1.;
}

ThreeDGrid::ThreeDGrid(int xSize, const double *x,
                       int ySize, const double *y,
                       int zSize, const double *z)
    : _x(x, x+xSize), _y(y, y+ySize), _z(z, z+zSize),
      _xSize(xSize), _ySize(ySize), _zSize(zSize),
      _maps(), _constantSpacing(false) {
  SetConstantSpacing();
  if (_xSize < 2 || _ySize < 2 || _zSize < 2)
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                          "3D Grid must be at least 2x2x2 grid",
                          "ThreeDGrid::ThreeDGrid(...)")
    );
}

ThreeDGrid::ThreeDGrid(std::vector<double> x,
                       std::vector<double> y,
                       std::vector<double> z)
    : _x(x), _y(y), _z(z),
      _xSize(x.size()), _ySize(y.size()), _zSize(z.size()),
      _maps(), _constantSpacing(false) {
  SetConstantSpacing();
  if (_xSize < 2 || _ySize < 2 || _zSize < 2)
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                            "3D Grid must be at least 2x2x2 grid",
                            "ThreeDGrid::ThreeDGrid(...)")
      );
}

ThreeDGrid::ThreeDGrid(double dX, double dY, double dZ,
                       double minX, double minY, double minZ,
                       int numberOfXCoords, int numberOfYCoords,
                       int numberOfZCoords)
    :  _x(numberOfXCoords), _y(numberOfYCoords), _z(numberOfZCoords),
       _xSize(numberOfXCoords), _ySize(numberOfYCoords), _zSize(numberOfZCoords),
       _maps(), _constantSpacing(true) {
    for (int i = 0; i < numberOfXCoords; i++)
        _x[i] = minX+i*dX;
    for (int j = 0; j < numberOfYCoords; j++)
        _y[j] = minY+j*dY;
    for (int k = 0; k < numberOfZCoords; k++)
        _z[k] = minZ+k*dZ;

    SetConstantSpacing();
}

ThreeDGrid::~ThreeDGrid() {
}

// state starts at 1,1,1
Mesh::Iterator ThreeDGrid::Begin() const {
    return Mesh::Iterator(std::vector<int>(3, 1), this);
}

Mesh::Iterator ThreeDGrid::End() const {
    std::vector<int> end(3, 1);
    end[0] = _xSize+1;
    return Mesh::Iterator(end, this);
}

void ThreeDGrid::Position(const Mesh::Iterator& it, double * position) const {
    position[0] = x(it._state[0]);
    position[1] = y(it._state[1]);
    position[2] = z(it._state[2]);
}


Mesh::Iterator& ThreeDGrid::AddEquals
    (Mesh::Iterator& lhs, int difference) const {
    if (difference < 0)
        return SubEquals(lhs, -difference);

    lhs._state[0] +=  difference/(_ySize*_zSize);
    difference     = difference%(_ySize*_zSize);
    lhs._state[1] += difference/(_zSize);
    lhs._state[2] += difference%(_zSize);

    if (lhs._state[1] > _ySize) {
        lhs._state[0]++;
        lhs._state[1] -= _ySize;
    }
    if (lhs._state[2] > _zSize) {
        lhs._state[1]++;
        lhs._state[2] -= _zSize;
    }

    return lhs;
}

Mesh::Iterator& ThreeDGrid::SubEquals
                                  (Mesh::Iterator& lhs, int difference) const {
    if (difference < 0)
        return AddEquals(lhs, -difference);

    lhs._state[0] -= difference/(_ySize*_zSize);
    difference     = difference%(_ySize*_zSize);
    lhs._state[1] -= difference/(_zSize);
    lhs._state[2] -= difference%(_zSize);

    while (lhs._state[2] < 1) {
        lhs._state[1]--;
        lhs._state[2] += _zSize;
    }
    while (lhs._state[1] < 1) {
        lhs._state[0]--;
        lhs._state[1] += _ySize;
    }

    return lhs;
}


Mesh::Iterator& ThreeDGrid::AddEquals
                      (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const {
    return AddEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& ThreeDGrid::SubEquals
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const {
    return SubEquals(lhs, rhs.ToInteger());
}

Mesh::Iterator& ThreeDGrid::AddOne(Mesh::Iterator& lhs) const {
    if (lhs._state[1] == _ySize && lhs._state[2] == _zSize) {
        ++lhs._state[0];
        lhs._state[1] = lhs._state[2] = 1;
    } else if (lhs._state[2] == _zSize) {
        ++lhs._state[1];
        lhs._state[2] = 1;
    } else {
        ++lhs._state[2];
    }
    return lhs;
}

Mesh::Iterator& ThreeDGrid::SubOne(Mesh::Iterator& lhs) const {
    if (lhs._state[1] == 1 && lhs._state[2] == 1) {
        --lhs._state[0];
        lhs._state[1] = _ySize;
        lhs._state[2] = _zSize;
    } else if (lhs._state[2] == 1) {
        --lhs._state[1];
        lhs._state[2] = _zSize;
    } else {
        --lhs._state[2];
    }
    return lhs;
}

// Check relative position
bool ThreeDGrid::IsGreater
                (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const {
    if (lhs._state[0] > rhs._state[0])
        return true;
    else if (lhs._state[0] == rhs._state[0] && lhs._state[1] > rhs._state[1])
        return true;
    else if (lhs._state[0] == rhs._state[0] &&
             lhs._state[1] == rhs._state[1] &&
             lhs._state[2] > rhs._state[2])
        return true;
    return false;
}

// remove *map if it exists; delete this if there are no more VectorMaps
void ThreeDGrid::Remove(VectorMap* map) {
    std::vector<VectorMap*>::iterator it =
                                    std::find(_maps.begin(), _maps.end(), map);
    if (it < _maps.end()) {
        _maps.erase(it);
    }
    if (_maps.begin() == _maps.end()) {
        delete this;
    }
}

// add *map if it has not already been added
void ThreeDGrid::Add(VectorMap* map) {
    std::vector<VectorMap*>::iterator it =
                                     std::find(_maps.begin(), _maps.end(), map);
    if (it == _maps.end()) {
        _maps.push_back(map);
    }
}

void ThreeDGrid::SetConstantSpacing() {
    _constantSpacing = true;
    for (unsigned int i = 0; i < _x.size()-1; i++)
        if (fabs(1-(_x[i+1]-_x[i])/(_x[1]-_x[0])) > 1e-9) {
            _constantSpacing = false;
            return;
        }
    for (unsigned int i = 0; i < _y.size()-1; i++)
        if (fabs(1-(_y[i+1]-_y[i])/(_y[1]-_y[0])) > 1e-9) {
            _constantSpacing = false;
            return;
        }
    for (unsigned int i = 0; i < _z.size()-1; i++)
        if (fabs(1-(_z[i+1]-_z[i])/(_z[1]-_z[0])) > 1e-9) {
            _constantSpacing = false;
            return;
        }
}

Mesh::Iterator ThreeDGrid::Nearest(const double* position) const {
    std::vector<int> index(3);
    LowerBound(position[0], index[0],
               position[1], index[1],
               position[2], index[2]);
    if (index[0] < _xSize-1 && index[0] >= 0)
        index[0] += (2*(position[0] - _x[index[0]])  >
                     _x[index[0]+1]-_x[index[0]] ? 2 : 1);
    else
        index[0]++;
    if (index[1] < _ySize-1 && index[1] >= 0)
        index[1] += (2*(position[1] - _y[index[1]])  >
                    _y[index[1]+1]-_y[index[1]] ? 2 : 1);
    else
        index[1]++;
    if (index[2] < _zSize-1 && index[2] >= 0)
        index[2] += (2*(position[2] - _z[index[2]])  >
                    _z[index[2]+1]-_z[index[2]] ? 2 : 1);
    else
        index[2]++;
    if (index[0] < 1)
        index[0] = 1;
    if (index[1] < 1)
        index[1] = 1;
    if (index[2] < 1)
        index[2] = 1;
    if (index[0] > _xSize)
        index[0] = _xSize;
    if (index[1] > _ySize)
        index[1] = _ySize;
    if (index[2] > _zSize)
        index[2] = _zSize;
    return Mesh::Iterator(index, this);
}

