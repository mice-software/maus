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
 *
 * This provides a similar interface as std::complex, but wrapps the GSL complex
 * number C library instead.
 *
 * This is primarily intended for use by the MAUS Matrix and Vector classes
 * which wrap the GSL matrix and vector C libraries. As such, it is implemented
 * not as a class but rather a typedef of the gsl_complex type with some
 * associated free functions. This allows Matrix and Vector to simply hold a
 * reference to an object that has elements of type gsl_complex instead of
 * constantly converting from one complex type to another.
 *
 * Because of the name clash with the typedef name, the "constructors" for
 * complex are in a nested namespace MAUS::Complex (having a MAUS::complex
 * namespace would also clash).
 */

#ifndef COMMON_CPP_MATHS_COMPLEX_HH
#define COMMON_CPP_MATHS_COMPLEX_HH

#include <iostream>

#include "gsl/gsl_complex_math.h"

namespace MAUS {
typedef gsl_complex complex;

// ************************
//  Construction Functions
// ************************
namespace Complex {
MAUS::complex complex(const MAUS::complex& original_instance);
MAUS::complex complex(const double  real);
MAUS::complex complex(const double  real, const double  imag);
}  // namespace MAUS::Complex
const MAUS::complex polar(const double  r, const double  theta);

// **********************
//  Property Functions
// **********************

const double real(const MAUS::complex& c);
const double imag(const MAUS::complex& c);
const double abs(const MAUS::complex& c);
const double abs2(const MAUS::complex& c);
const double arg(const MAUS::complex& c);
const double norm(const MAUS::complex& c);
const double norm2(const MAUS::complex& c);
MAUS::complex conj(const MAUS::complex& c);

// **********************
//  Elementary Functions
// **********************

MAUS::complex exp(const MAUS::complex& z);
MAUS::complex log(const MAUS::complex& z);
MAUS::complex log10(const MAUS::complex& z);
MAUS::complex pow(const MAUS::complex& z, const MAUS::complex& a);
MAUS::complex pow(const MAUS::complex& z, const double  x);
MAUS::complex sqrt(const MAUS::complex& z);
MAUS::complex sqrt(const double  x);

// **********************
//  Trig Functions
// **********************

MAUS::complex sin(const MAUS::complex& c);
MAUS::complex cos(const MAUS::complex& c);
MAUS::complex tan(const MAUS::complex& c);
MAUS::complex csc(const MAUS::complex& c);
MAUS::complex sec(const MAUS::complex& c);
MAUS::complex cot(const MAUS::complex& c);

// ************************
// TODO(plane1@iit.edu): Inverse Trig Functions
// ************************

// **********************
//  Hyperbolic Functions
// **********************

MAUS::complex sinh(const MAUS::complex& c);
MAUS::complex cosh(const MAUS::complex& c);
MAUS::complex tanh(const MAUS::complex& c);
MAUS::complex csch(const MAUS::complex& c);
MAUS::complex sech(const MAUS::complex& c);
MAUS::complex coth(const MAUS::complex& c);

// ******************************
// TODO(plane1@iit.edu): Inverse Hyperbolic Functions
// ******************************

// *******************
//  Unitary Operators
// *******************

MAUS::complex operator -(const MAUS::complex& operand);

// **********************
//  Assignment Operators
// **********************

MAUS::complex& operator+=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator+=(MAUS::complex& lhs, const double         rhs);
MAUS::complex& operator-=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator-=(MAUS::complex& lhs, const double         rhs);
MAUS::complex& operator*=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator*=(MAUS::complex& lhs, const double         rhs);
MAUS::complex& operator/=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator/=(MAUS::complex& lhs, const double         rhs);

// **********************
//  Algebraic Operators
// **********************

const MAUS::complex operator+(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator+(const MAUS::complex&   lhs,
                              const double           rhs);
const MAUS::complex operator+(const double           lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator-(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator-(const MAUS::complex&   lhs,
                              const double           rhs);
const MAUS::complex operator-(const double           lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator*(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator*(const MAUS::complex&   lhs,
                              const double           rhs);
const MAUS::complex operator*(const double           lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator/(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator/(const MAUS::complex&   lhs,
                              const double           rhs);
const MAUS::complex operator/(const double           lhs,
                              const MAUS::complex&   rhs);

// **********************
//  Comparison Operators
// **********************

const bool operator==(const MAUS::complex&           lhs,
                      const MAUS::complex&           rhs);
const bool operator==(const MAUS::complex&           lhs,
                      const double                   rhs);
const bool operator==(const double                   lhs,
                      const MAUS::complex&           rhs);
const bool operator!=(const MAUS::complex&           lhs,
                      const MAUS::complex&           rhs);
const bool operator!=(const MAUS::complex&           lhs,
                      const double                   rhs);
const bool operator!=(const double                   lhs,
                      const MAUS::complex&           rhs);
// **********************
//  Stream Operators
// **********************

std::ostream& operator<<(std::ostream& out, const MAUS::complex& c);
std::istream& operator>>(std::istream& in,  MAUS::complex& c);
}  // namespace MAUS

#endif
