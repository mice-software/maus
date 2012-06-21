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

#include "Interface/Squeal.hh"
#include "Maths/Vector.hh"

namespace MAUS {

const PhaseSpaceVector::PhaseSpaceType
  PhaseSpaceVector::PhaseSpaceType::kTemporal
    = PhaseSpaceVector::PhaseSpaceType(0);
const PhaseSpaceVector::PhaseSpaceType
  PhaseSpaceVector::PhaseSpaceType::kPositional
    =  PhaseSpaceVector::PhaseSpaceType(1);

PhaseSpaceVector::PhaseSpaceVector()
    : Vector<double>(6), type_(PhaseSpaceType::kTemporal)
{ }

PhaseSpaceVector::PhaseSpaceVector(const PhaseSpaceType type)
    : Vector<double>(6), type_(type)
{ }

PhaseSpaceVector::PhaseSpaceVector(const Vector<double>& original_instance,
                                   const PhaseSpaceType type)
    : Vector<double>(), type_(type) {
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
    : Vector<double>(original_instance), type_(original_instance.type_)
{ }

PhaseSpaceVector::PhaseSpaceVector(double const * const array,
                                   const PhaseSpaceType type)
    : Vector<double>(array, 6), type_(type)
{ }

PhaseSpaceVector::PhaseSpaceVector(const double x1, const double p1,
                                   const double x2, const double p2,
                                   const double x3, const double p3,
                                   const PhaseSpaceType type)
    : Vector<double>(), type_(type) {
  const double data[6] = {
    x1, p1, x2, p2, x3, p3
  };
  build_vector(6, data);
}

PhaseSpaceVector::PhaseSpaceVector(const PhaseSpaceVector& original_instance,
                                   const double mass,
                                   const PhaseSpaceType type)
    : Vector<double>(6), type_(type) {
  if (type == PhaseSpaceType::kTemporal) {
    if (original_instance.type_ == PhaseSpaceType::kTemporal) {
      // "convert" from temporal to temporal
      (*this).Vector<double>::operator=(original_instance);
    } else {
      // convert from positional to temporal
      (*this)[3] = original_instance[1];
      (*this)[5] = original_instance[3];
      const double pz = original_instance[5];

      double & energy_ = (*this)[1];
      const double px = (*this)[3];
      const double py = (*this)[5];
std::cout << "DEBUG PhaseSpaceVector(): (px, py, pz) = " << "(" << px << ", " << py << ", " << pz << ")" << std::endl;

      const double momentum = ::sqrt(px*px + py*py + pz*pz);
std::cout << "DEBUG PhaseSpaceVector(): momentum = " << momentum << " MeV/c" << std::endl;

      // fill in the energy coordinate
      energy_ = ::sqrt(mass*mass + momentum*momentum);
std::cout << "DEBUG PhaseSpaceVector(): energy = " << (*this)[1] << " MeV" << std::endl;

      (*this)[2] = original_instance[0];
      (*this)[4] = original_instance[2];
      const double z = original_instance[4];

      double & time_ = (*this)[0];
      const double x = (*this)[2];
      const double y = (*this)[4];

      const double position = ::sqrt(x*x + y*y + z*z);
      const double velocity = ::CLHEP::c_light * momentum / energy_;

      // fill in the time coordinate
      time_ = position / velocity;
      if (z < 0) {
        time_ = -time_;
      }
    }
  } else {
    if (original_instance.type_ == PhaseSpaceType::kPositional) {
      // "convert" from positional to positional
      (*this).Vector<double>::operator=(original_instance);
    } else {
      // convert from temporal to positional
      const double energy = original_instance[1];
      (*this)[1] = original_instance[3];
      (*this)[3] = original_instance[5];

      const double px = (*this)[1];
      const double py = (*this)[3];
      double & pz_ = (*this)[5];

      const double momentum = ::sqrt(energy*energy - mass*mass);

      pz_ = ::sqrt(momentum*momentum - px*px - py*py);

      const double time = original_instance[0];
      (*this)[0] = original_instance[2];
      (*this)[2] = original_instance[4];

      const double x = (*this)[0];
      const double y = (*this)[2];
      double & z_ = (*this)[4];

      double velocity = ::CLHEP::c_light * momentum / energy;
      double position = velocity * time;

      // fill in the z coordinate
      z_ = ::sqrt(position*position - x*x - y*y);
      if (time < 0) {
        z_ = -z_;
      }
    }
  }
}

PhaseSpaceVector::~PhaseSpaceVector() { }

PhaseSpaceVector & PhaseSpaceVector::operator=(const PhaseSpaceVector & rhs) {
  if (type_ != rhs.type_) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to assign a phase space vector "
      "of a different phase space type.",
      "MAUS::PhaseSpaceVector::operator=()"));
  }

  (*this).Vector<double>::operator=(rhs);

  return *this;
}

double PhaseSpaceVector::time() const
{
  if (type_ == PhaseSpaceType::kPositional) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to get the time from a phase space vector with "
      "positional type coordinates (x, Px, y, Py, z, Pz). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kPositional) to convert.",
      "MAUS::PhaseSpaceVector::time()"));
  }
  
  return (*this)[0];
}

double PhaseSpaceVector::energy() const {
  if (type_ == PhaseSpaceType::kPositional) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to get the energy from a phase space vector with "
      "positional type coordinates (x, Px, y, Py, z, Pz). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kTemporal) to convert.",
      "MAUS::PhaseSpaceVector::energy()"));
  }

  return (*this)[1];
}

double PhaseSpaceVector::x() const {
  if (type_ == PhaseSpaceType::kPositional) {
    return (*this)[0];
  }
    
  return (*this)[2];
}

double PhaseSpaceVector::x_momentum() const {
  if (type_ == PhaseSpaceType::kPositional) {
    return (*this)[1];
  }
    
  return (*this)[3];
}

double PhaseSpaceVector::y() const {
  if (type_ == PhaseSpaceType::kPositional) {
    return (*this)[2];
  }
    
  return (*this)[4];
}
double PhaseSpaceVector::y_momentum() const {
  if (type_ == PhaseSpaceType::kPositional) {
    return (*this)[3];
  }
    
  return (*this)[5];
}

double PhaseSpaceVector::z() const {
  if (type_ == PhaseSpaceType::kTemporal) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to get the z coordinate from a phase space vector with "
      "temporal type coordinates (t, E, x, Px, y, Py). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kPositional) to convert.",
      "MAUS::PhaseSpaceVector::z()"));
  }

  return (*this)[4];
}

double PhaseSpaceVector::z_momentum() const {
  if (type_ == PhaseSpaceType::kTemporal) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to get the z momentum from a phase space vector with "
      "temporal type coordinates (t, E, x, Px, y, Py). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kPositional) to convert.",
      "MAUS::PhaseSpaceVector::z_momentum()"));
  }

  return (*this)[5];
}

void PhaseSpaceVector::set_time(double time) {
  if (type_ == PhaseSpaceType::kPositional) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to set the time of a phase space vector with "
      "positional type coordinates (x, Px, y, Py, z, Pz). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kPositional) to convert.",
      "MAUS::PhaseSpaceVector::set_time()"));
  }
  
  (*this)[0] = time;
}
void PhaseSpaceVector::set_energy(double energy) {
  if (type_ == PhaseSpaceType::kPositional) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to set the energy of a phase space vector with "
      "positional type coordinates (x, Px, y, Py, z, Pz). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kTemporal) to convert.",
      "MAUS::PhaseSpaceVector::set_energy()"));
  }

  (*this)[1] = energy;
}

void PhaseSpaceVector::set_x(double x) {
  if (type_ == PhaseSpaceType::kPositional) {
    (*this)[0] = x;
  }
    
  (*this)[2] = x;
}

void PhaseSpaceVector::set_x_momentum(double x_momentum) {
  if (type_ == PhaseSpaceType::kPositional) {
    (*this)[1] = x_momentum;
  }
    
  (*this)[3] = x_momentum;
}

void PhaseSpaceVector::set_y(double y) {
  if (type_ == PhaseSpaceType::kPositional) {
    (*this)[2] = y;
  }
    
  (*this)[4] = y;
}

void PhaseSpaceVector::set_y_momentum(double y_momentum) {
  if (type_ == PhaseSpaceType::kPositional) {
    (*this)[3] = y_momentum;
  }
    
  (*this)[5] = y_momentum;
}

void PhaseSpaceVector::set_z(double z) {
  if (type_ == PhaseSpaceType::kTemporal) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to set the z coordinate of a phase space vector with "
      "temporal type coordinates (t, E, x, Px, y, Py). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kPositional) to convert.",
      "MAUS::PhaseSpaceVector::set_z()"));
  }

  (*this)[4] = z;
}
void PhaseSpaceVector::set_z_momentum(double z_momentum) {
  if (type_ == PhaseSpaceType::kTemporal) {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to set the z momentum of a phase space vector with "
      "temporal type coordinates (t, E, x, Px, y, Py). Use "
      "PhaseSpaceVector(vector, mass, PhaseSpaceType::kPositional) to convert.",
      "MAUS::PhaseSpaceVector::set_z_momentum()"));
  }

  (*this)[5] = z_momentum;
}

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& vector) {
  if (vector.type() == PhaseSpaceVector::PhaseSpaceType::kPositional) {
    out << "x: " << vector[0] << "Px: "  << vector[1]
        << "y: " << vector[2] << "Py: "  << vector[3]
        << "z: " << vector[4] << "Pz: "  << vector[5];
  } else {
    out << "t: " << vector[0] << "E: "   << vector[1]
        << "x: " << vector[2] << "Px: "  << vector[3]
        << "y: " << vector[4] << "Py: "  << vector[5];
  }
  return out;
}
}  // namespace MAUS
