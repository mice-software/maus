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

#include "Recon/Global/ParticleOpticalVector.hh"

#include <cmath>
#include "CLHEP/Units/PhysicalConstants.h"

#include "DataStructure/ThreeVector.hh"
#include "src/common_cpp/Maths/Vector.hh"
#include "Utils/Squeak.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"

namespace MAUS {
namespace recon {
namespace global {

ParticleOpticalVector::ParticleOpticalVector()
    : PhaseSpaceVector()
{ }

ParticleOpticalVector::ParticleOpticalVector(
    const PhaseSpaceVector& vector,
    const double t0, const double E0, const double P0)
    : PhaseSpaceVector() {
  const double beta0 = P0 / E0;
  const double gamma0 = 1. / ::sqrt(1 - beta0*beta0);
  // const double k = beta0 * ::CLHEP::c_light * gamma0 / (1 + gamma0);
  const double k = beta0 * gamma0 / (1 + gamma0);

  set_l(k * (vector.t() - t0));
  set_delta((vector.E() - E0) / E0);
  set_x(vector.x());
  set_a(vector.Px() / P0);
  set_y(vector.y());
  set_b(vector.Py() / P0);
}

ParticleOpticalVector::ParticleOpticalVector(
    const ParticleOpticalVector& original_instance)
    : PhaseSpaceVector(original_instance) { }

ParticleOpticalVector::ParticleOpticalVector(double const * const array)
    : PhaseSpaceVector(array) { }

ParticleOpticalVector::ParticleOpticalVector(
    const double l, const double delta,
    const double x, const double a,
    const double y, const double b)
    : PhaseSpaceVector(l, delta, x, a, y, b) { }

ParticleOpticalVector::~ParticleOpticalVector() {
}

// *************************
//  Assignment Operators
// *************************

ParticleOpticalVector & ParticleOpticalVector::operator=(
    const ParticleOpticalVector& rhs) {
  PhaseSpaceVector::operator=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator+=(
    const ParticleOpticalVector& rhs) {
  PhaseSpaceVector::operator+=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator-=(
    const ParticleOpticalVector& rhs) {
  PhaseSpaceVector::operator-=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator*=(
    const ParticleOpticalVector& rhs) {
  PhaseSpaceVector::operator*=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator/=(
    const ParticleOpticalVector& rhs) {
  PhaseSpaceVector::operator/=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator+=(const double& rhs) {
  PhaseSpaceVector::operator+=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator-=(const double& rhs) {
  PhaseSpaceVector::operator-=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator*=(const double& rhs) {
  PhaseSpaceVector::operator*=(rhs);

  return *this;
}

ParticleOpticalVector & ParticleOpticalVector::operator/=(const double& rhs) {
  PhaseSpaceVector::operator/=(rhs);

  return *this;
}

// *************************
//  Algebraic Operators
// *************************

const ParticleOpticalVector ParticleOpticalVector::operator+(
    const ParticleOpticalVector& rhs) const {
  return ParticleOpticalVector(*this) += rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator-(
    const ParticleOpticalVector& rhs) const {
  return ParticleOpticalVector(*this) -= rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator*(
    const ParticleOpticalVector& rhs) const {
  return ParticleOpticalVector(*this) *= rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator/(
    const ParticleOpticalVector& rhs) const {
  return ParticleOpticalVector(*this) /= rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator+(
    const double& rhs) const {
  return ParticleOpticalVector(*this) += rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator-(
    const double& rhs) const {
  return ParticleOpticalVector(*this) -= rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator*(
    const double& rhs) const {
  return ParticleOpticalVector(*this) *= rhs;
}

const ParticleOpticalVector ParticleOpticalVector::operator/(
    const double& rhs) const {
  return ParticleOpticalVector(*this) /= rhs;
}

// *************************
//  Comparison Operators
// *************************

const bool ParticleOpticalVector::operator==(
    const ParticleOpticalVector& rhs) const {
  if (static_cast<PhaseSpaceVector const * const>(this)->operator!=(rhs)) {
    return false;
  }

  return true;
}

const bool ParticleOpticalVector::operator!=(
    const ParticleOpticalVector& rhs) const {
  return !operator==(rhs);
}

std::ostream& operator<<(std::ostream& out,
                         const ParticleOpticalVector& point) {
  out << "l: " << point.l() << "  delta: "  << point.delta()
      << "  x: " << point.x()    << "  a: " << point.a()
      << "  y: " << point.y()    << "  b: " << point.b();

  return out;
}

}  // namespace global
}  // namespace recon
}  // namespace MAUS
