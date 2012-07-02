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
 *
 */

#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {

Data::Data() : _spill(NULL) {}

Data::Data(const Data& data) : _spill(NULL) {
    *this = data;
}

Data::~Data() {
    if (_spill != NULL) {
        delete _spill;
        _spill = NULL;
    }
}

Data& Data::operator=(const Data& data) {
    if (this == & data) {
        return *this;
    }
    if (data._spill == NULL) {
        SetSpill(NULL);
    } else {
        SetSpill(new Spill(*data._spill));
    }
    return *this;
}

void Data::SetSpill(Spill* spill) {
    if (_spill != NULL) {
        delete _spill;
        _spill = NULL;
    }
    _spill = spill;
}

Spill* Data::GetSpill() const {
    return _spill;
}

int Data::my_sizeof() {
  Data spill;
  return sizeof(spill);
}
}
