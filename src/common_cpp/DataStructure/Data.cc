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

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {

int Data::_reference_count = 0;
int Data::_max_reference_count = 0;

Data::Data() : MAUSEvent<Spill>("Spill"), _spill(NULL) {
    IncreaseRefCount();
}

Data::Data(const Data& data) : MAUSEvent<Spill>(), _spill(NULL) {
    *this = data;
    IncreaseRefCount();
}

Data::~Data() {
    DecreaseRefCount();
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
    SetEventType(data.GetEventType());
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

int Data::GetSizeOf() const {
  Data spill;
  return sizeof(spill);
}

void Data::IncreaseRefCount() {
    _reference_count++;
    if (_reference_count > _max_reference_count) {
        throw Exception(Exception::recoverable,
                        "Too many data references",
                        "Data::IncreaseRefCount");
    }
    // std::cerr << "Data " << this << " ref: " << _reference_count << std::endl;
    // std::cerr << MAUS::Exception().MakeStackTrace(1) << std::endl;
}

void Data::DecreaseRefCount() {
    _reference_count--;
    // std::cerr << "~Data " << this << " ref: " << _reference_count << std::endl;
    // std::cerr << MAUS::Exception().MakeStackTrace(1) << std::endl;
}
}

