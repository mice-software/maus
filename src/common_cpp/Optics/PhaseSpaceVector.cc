/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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

/* Author: Peter Lane
 */

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"

#include <iostream>

#include "Interface/Squeak.hh"
#include "Maths/Vector.hh"

namespace MAUS {

PhaseSpaceVector::PhaseSpaceVector()
    : Vector<double>(6)
{ }

PhaseSpaceVector::PhaseSpaceVector(const Vector<double>& original_instance)
    : Vector<double>() {
  if (original_instance.size() < 6) {
    throw(Squeal(Squeal::recoverable,
                 "Attempted to construct with a Vector<double> containing "
                 "fewer than six elements",
                 "PhaseSpaceVector::PhaseSpaceVector(Vector<double>)"));
  }
  build_vector(6);
  for (size_t index = 1; index <= 6; ++index) {
    (*this)(index) = original_instance(index);
  }
}

PhaseSpaceVector::PhaseSpaceVector(const PhaseSpaceVector& original_instance)
    : Vector<double>(original_instance)
{ }

PhaseSpaceVector::PhaseSpaceVector(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py)
    : Vector<double>() {
  const double data[6] = {
    t, E, x, Px, y, Py
  };
  build_vector(6, data);
}

PhaseSpaceVector::PhaseSpaceVector(double const * const array)
    :Vector<double>(array, 6)
{ }

PhaseSpaceVector::~PhaseSpaceVector() { }

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& vector) {
  out << "t: " << vector[0] << " E: "   << vector[1]
      << " x: " << vector[2] << " Px: "  << vector[3]
      << " y: " << vector[4] << " Py: "  << vector[5];
  return out;
}
}  // namespace MAUS
