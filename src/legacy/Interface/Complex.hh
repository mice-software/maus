/*
 *  Comlex.hh
 *  
 *
 *  Created by Peter Lane on 11/1/11.
 *  Copyright 2011 Muon Ionization Cooling Experiment. All rights reserved.
 *
 */
#ifndef Complex_hh
#define Complex_hh

#include "gsl/gsl_complex_math.h"

namespace MAUS
{
  typedef gsl_complex complex;

  //************************
  // Construction Functions
  //************************
  namespace Complex
  {
  MAUS::complex complex(const MAUS::complex& original_instance);
  MAUS::complex complex(const double& real);
  MAUS::complex complex(const double& real, const double& imag);
  const MAUS::complex polar(const double& r, const double& theta);
  
  //**********************
  // Property Functions
  //**********************
  
  const double real(const MAUS::complex& c);
  const double imag(const MAUS::complex& c);
  const double abs(const MAUS::complex& c);
  const double abs2(const MAUS::complex& c);
  const double arg(const MAUS::complex& c);
  const double norm(const MAUS::complex& c);
  const double norm2(const MAUS::complex& c);
  MAUS::complex conj(const MAUS::complex& c);
  
  //**********************
  // Elementary Functions
  //**********************
  
  MAUS::complex exp(const MAUS::complex& z);
  MAUS::complex log(const MAUS::complex& z);
  MAUS::complex log10(const MAUS::complex& z);
  MAUS::complex pow(const MAUS::complex& z, const MAUS::complex& a);
  MAUS::complex pow(const MAUS::complex& z, const double& x);
  MAUS::complex sqrt(const MAUS::complex& z);
  MAUS::complex sqrt(const double& x);
  
  //**********************
  // Trig Functions
  //**********************
  
  MAUS::complex sin(const MAUS::complex& c);
  MAUS::complex cos(const MAUS::complex& c);
  MAUS::complex tan(const MAUS::complex& c);
  MAUS::complex csc(const MAUS::complex& c);
  MAUS::complex sec(const MAUS::complex& c);
  MAUS::complex cot(const MAUS::complex& c);

  //************************
  // TODO: Inverse Trig Functions
  //************************
  
  //**********************
  // Hyperbolic Functions
  //**********************
  
  MAUS::complex sinh(const MAUS::complex& c);
  MAUS::complex cosh(const MAUS::complex& c);
  MAUS::complex tanh(const MAUS::complex& c);
  MAUS::complex csch(const MAUS::complex& c);
  MAUS::complex sech(const MAUS::complex& c);
  MAUS::complex coth(const MAUS::complex& c);

  //******************************
  // TODO: Inverse Hyperbolic Functions
  //******************************
  }
}

//*******************
// Unitary Operators
//*******************

MAUS::complex operator -(const MAUS::complex& operand);

//**********************
// Assignment Operators
//**********************

MAUS::complex& operator+=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator+=(MAUS::complex& lhs, const double&        rhs);
MAUS::complex& operator-=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator-=(MAUS::complex& lhs, const double&        rhs);
MAUS::complex& operator*=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator*=(MAUS::complex& lhs, const double&        rhs);
MAUS::complex& operator/=(MAUS::complex& lhs, const MAUS::complex& rhs);
MAUS::complex& operator/=(MAUS::complex& lhs, const double&        rhs);

//**********************
// Algebraic Operators
//**********************

const MAUS::complex operator+(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator+(const MAUS::complex&   lhs,
                              const double&          rhs);
const MAUS::complex operator+(const double&          lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator-(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator-(const MAUS::complex&   lhs,
                              const double&          rhs);
const MAUS::complex operator-(const double&          lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator*(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator*(const MAUS::complex&   lhs,
                              const double&          rhs);
const MAUS::complex operator*(const double&          lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator/(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs);
const MAUS::complex operator/(const MAUS::complex&   lhs,
                              const double&          rhs);
const MAUS::complex operator/(const double&          lhs,
                              const MAUS::complex&   rhs);

//**********************
// Comparison Operators
//**********************

const bool operator==(const MAUS::complex&           lhs,
                      const MAUS::complex&           rhs);
const bool operator==(const MAUS::complex&           lhs,
                      const double&                  rhs);
const bool operator==(const double&                  lhs,
                      const MAUS::complex&           rhs);
const bool operator!=(const MAUS::complex&           lhs,
                      const MAUS::complex&           rhs);
const bool operator!=(const MAUS::complex&           lhs,
                      const double&                  rhs);
const bool operator!=(const double&                  lhs,
                      const MAUS::complex&           rhs);
//**********************
// Stream Operators
//**********************

std::ostream& operator<<(std::ostream& out, const MAUS::complex& c);
std::istream& operator>>(std::istream& in,  MAUS::complex& c);

#endif