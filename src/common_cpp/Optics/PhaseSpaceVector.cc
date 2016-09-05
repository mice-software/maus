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

#include <cmath>
#include <iostream>

#include "CLHEP/Units/PhysicalConstants.h"
#include "TLorentzVector.h"

#include "DataStructure/ThreeVector.hh"
#include "DataStructure/VirtualHit.hh"
#include "Utils/Exception.hh"
#include "Maths/Vector.hh"

namespace MAUS {

PhaseSpaceVector::PhaseSpaceVector()
    : Vector<double>(6)
{ }

PhaseSpaceVector::PhaseSpaceVector(const Vector<double>& original_instance)
    : Vector<double>() {
  if (original_instance.size() < 6) {
    throw(Exceptions::Exception(Exceptions::recoverable,
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

PhaseSpaceVector::PhaseSpaceVector(double const * const array)
    : Vector<double>(array, 6)
{ }

PhaseSpaceVector::PhaseSpaceVector(const double t, const double E,
                                   const double x, const double px,
                                   const double y, const double py)
    : Vector<double>() {
  const double data[6] = {
    t, E, x, px, y, py
  };
  build_vector(6, data);
}

PhaseSpaceVector::PhaseSpaceVector(const MAUS::VirtualHit & hit)
    : MAUS::Vector<double>() {
  const MAUS::ThreeVector position = hit.GetPosition();
  const double mass = hit.GetMass();
  const MAUS::ThreeVector momentum = hit.GetMomentum();
  const double energy = ::sqrt(mass*mass + ::pow(momentum.mag(), 2.));
  const double data[6] = {
    hit.GetTime(), energy,
    position.x(), momentum.x(),
    position.y(), momentum.y()
  };
  build_vector(6, data);
}

PhaseSpaceVector::PhaseSpaceVector(const double time,
                                   const double energy,
                                   const MAUS::ThreeVector position,
                                   const MAUS::ThreeVector momentum)
    : MAUS::Vector<double>() {
  const double data[6] = {
    time, energy, position.x(), momentum.x(), position.y(), momentum.y()
  };
  build_vector(6, data);
}

PhaseSpaceVector::PhaseSpaceVector(const TLorentzVector position,
                                   const TLorentzVector momentum)
    : MAUS::Vector<double>() {
  const double data[6] = {
    position.T(), momentum.E(),
    position.X(), momentum.Px(),
    position.Y(), momentum.Py()
  };
  build_vector(6, data);
}

PhaseSpaceVector::~PhaseSpaceVector() { }

// *************************
//  Assignment Operators
// *************************

PhaseSpaceVector & PhaseSpaceVector::operator=(const PhaseSpaceVector & rhs) {
  (*this).Vector<double>::operator=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator+=(const PhaseSpaceVector& rhs) {
  Vector<double>::operator+=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator-=(const PhaseSpaceVector& rhs) {
  Vector<double>::operator-=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator*=(const PhaseSpaceVector& rhs) {
  Vector<double>::operator*=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator/=(const PhaseSpaceVector& rhs) {
  Vector<double>::operator/=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator+=(const double& rhs) {
  Vector<double>::operator+=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator-=(const double& rhs) {
  Vector<double>::operator-=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator*=(const double& rhs) {
  Vector<double>::operator*=(rhs);

  return *this;
}

PhaseSpaceVector & PhaseSpaceVector::operator/=(const double& rhs) {
  Vector<double>::operator/=(rhs);

  return *this;
}

// *************************
//  Algebraic Operators
// *************************

const PhaseSpaceVector PhaseSpaceVector::operator+(const PhaseSpaceVector& rhs)
    const {
  return PhaseSpaceVector(*this) += rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator-(const PhaseSpaceVector& rhs)
    const {
  return PhaseSpaceVector(*this) -= rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator*(const PhaseSpaceVector& rhs)
    const {
  return PhaseSpaceVector(*this) *= rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator/(const PhaseSpaceVector& rhs)
    const {
  return PhaseSpaceVector(*this) /= rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator+(const double& rhs) const {
  return PhaseSpaceVector(*this) += rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator-(const double& rhs) const {
  return PhaseSpaceVector(*this) -= rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator*(const double& rhs) const {
  return PhaseSpaceVector(*this) *= rhs;
}

const PhaseSpaceVector PhaseSpaceVector::operator/(const double& rhs) const {
  return PhaseSpaceVector(*this) /= rhs;
}

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& vector) {
  out << "t: " << vector[0] << " E: "   << vector[1]
      << " x: " << vector[2] << " Px: "  << vector[3]
      << " y: " << vector[4] << " Py: "  << vector[5];

  return out;
}
}  // namespace MAUS
