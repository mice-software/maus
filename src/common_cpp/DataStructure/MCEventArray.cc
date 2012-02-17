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

#include "src/common_cpp/DataStructure/MCEvent.hh"

#include "src/common_cpp/DataStructure/MCEventArray.hh"

#include <iostream>

namespace MAUS {

MCEventArray::MCEventArray() : _vec() {
  std::cerr << "MCEventArray ctor " << this << std::endl;

}

MCEventArray::MCEventArray(const MCEventArray& md) : _vec() {
  std::cerr << "Target " << &md << " vector size " << md._vec.size() << std::endl;
  std::cerr << "MCEventArray copy ctor " << this << std::endl;
}
       
MCEventArray& MCEventArray::operator=(const MCEventArray& md) {
    if (this == &md) {
        return *this;
    }
    _vec = md._vec;
    return *this;    
}

MCEventArray::~MCEventArray() {
}

void MCEventArray::push_back(MCEvent* ev) {
    _vec.push_back(ev);
}

size_t MCEventArray::size() const {
    return _vec.size();
}

MCEvent*& MCEventArray::operator[](size_t i) {
    return _vec[i];
}

MCEvent* MCEventArray::operator[](size_t i) const {
    return _vec[i];
}

}

