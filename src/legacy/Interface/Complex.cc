/*
 *  Comlex.cc
 *  
 *
 *  Created by Peter Lane on 11/1/11.
 *  Copyright 2011 Muon Ionization Cooling Experiment. All rights reserved.
 *
 */
#include <iostream>
#include <sstream>
#include <string>

#include "gsl/gsl_complex_math.h"

#include "Interface/Complex.hh"

namespace MAUS
{
  namespace Complex
  {
  //************************
  // Construction Functions
  //************************
    
  MAUS::complex complex(const MAUS::complex& original_instance)
  {
    MAUS::complex c = MAUS::Complex::complex(original_instance.dat[0],
                                             original_instance.dat[1]);
    return c;
  }

  MAUS::complex complex(const double& real)
  {
    MAUS::complex c = gsl_complex_rect(real, 0.0);
    return c;
  }

  MAUS::complex complex(const double& real, const double& imag)
  {
    MAUS::complex c = gsl_complex_rect(real, imag);
    return c;
  }

  const MAUS::complex polar(const double& r, const double& theta)
  {
    return gsl_complex_polar(r, theta);
  }
  
  //**********************
  // Property Functions
  //**********************
  
  const double real(const MAUS::complex& c)
  {
    return c.dat[0];
  }
  
  const double imag(const MAUS::complex& c)
  {
    return c.dat[1];
  }
  
  const double abs(const MAUS::complex& c)
  {
    return gsl_complex_abs(c);
  }
  
  const double abs2(const MAUS::complex& c)
  {
    return gsl_complex_abs2(c);
  }
  
  const double arg(const MAUS::complex& c)
  {
    return gsl_complex_arg(c);
  }
  
  const double norm(const MAUS::complex& c)
  {
    return gsl_complex_abs(c);
  }
  
  const double norm2(const MAUS::complex& c)
  {
    return gsl_complex_abs2(c);
  }
  
  MAUS::complex conj(const MAUS::complex& c)
  {
    return gsl_complex_conjugate(c);
  }
  
  //**********************
  // Elementary Functions
  //**********************
  
  MAUS::complex exp(const MAUS::complex& z)
  {
    return gsl_complex_exp(z);
  }
  
  MAUS::complex log(const MAUS::complex& z)
  {
    return gsl_complex_log(z);
  }
  
  MAUS::complex log10(const MAUS::complex& z)
  {
    return gsl_complex_log10(z);
  }
  
  MAUS::complex pow(const MAUS::complex& z, const MAUS::complex& a)
  {
    return gsl_complex_pow(z, a);
  }
  
  MAUS::complex pow(const MAUS::complex& z, const double& x)
  {
    return gsl_complex_pow_real(z, x);
  }
  
  MAUS::complex sqrt(const MAUS::complex& z)
  {
    return gsl_complex_sqrt(z);
  }
  
  MAUS::complex sqrt(const double& x)
  {
    return gsl_complex_sqrt_real(x);
  }
  
  //**********************
  // Trig Functions
  //**********************
  
  MAUS::complex sin(const MAUS::complex& c)
  {
    return gsl_complex_sin(c);
  }
  
  MAUS::complex cos(const MAUS::complex& c)
  {
    return gsl_complex_cos(c);
  }
  
  MAUS::complex tan(const MAUS::complex& c)
  {
    return gsl_complex_tan(c);
  }
  
  MAUS::complex csc(const MAUS::complex& c)
  {
    return gsl_complex_csc(c);
  }
  
  MAUS::complex sec(const MAUS::complex& c)
  {
    return gsl_complex_sec(c);
  }
  
  MAUS::complex cot(const MAUS::complex& c)
  {
    return gsl_complex_cot(c);
  }
  
  //************************
  // TODO: Inverse Trig Functions
  //************************
  
  //**********************
  // Hyperbolic Functions
  //**********************
  
  MAUS::complex sinh(const MAUS::complex& c)
  {
    return gsl_complex_sinh(c);
  }
  
  MAUS::complex cosh(const MAUS::complex& c)
  {
    return gsl_complex_cosh(c);
  }
  
  MAUS::complex tanh(const MAUS::complex& c)
  {
    return gsl_complex_tanh(c);
  }
  
  MAUS::complex csch(const MAUS::complex& c)
  {
    return gsl_complex_csch(c);
  }
  
  MAUS::complex sech(const MAUS::complex& c)
  {
    return gsl_complex_sech(c);
  }
  
  MAUS::complex coth(const MAUS::complex& c)
  {
    return gsl_complex_coth(c);
  }
  
  //******************************
  // TODO: Inverse Hyperbolic Functions
  //******************************
  }
}

//*******************
// Unitary Operators
//*******************

MAUS::complex operator -(const MAUS::complex& operand)
{
  return gsl_complex_negative(operand);
}

//**********************
// Assignment Operators
//**********************

MAUS::complex& operator+=(MAUS::complex& lhs, const MAUS::complex& rhs)
{
  lhs.dat[0] += rhs.dat[0];
  lhs.dat[1] += rhs.dat[1];
  return lhs;
}

MAUS::complex& operator+=(MAUS::complex& lhs, const double& rhs)
{
  lhs.dat[0] += rhs;
  return lhs;
}

MAUS::complex& operator-=(MAUS::complex& lhs, const MAUS::complex& rhs)
{
  lhs.dat[0] -= rhs.dat[0];
  lhs.dat[1] -= rhs.dat[1];
  return lhs;
}

MAUS::complex& operator-=(MAUS::complex& lhs, const double& rhs)
{
  lhs.dat[0] -= rhs;
  return lhs;
}

MAUS::complex& operator*=(MAUS::complex& lhs, const MAUS::complex& rhs)
{
  MAUS::complex tmp = lhs;
  lhs.dat[0] = lhs.dat[0] * rhs.dat[0] - lhs.dat[1] * rhs.dat[1];
  lhs.dat[1] = tmp.dat[0] * rhs.dat[1] + tmp.dat[1] * rhs.dat[0];
  return lhs;
}

MAUS::complex& operator*=(MAUS::complex& lhs, const double& rhs)
{
  lhs.dat[0] *= rhs;
  lhs.dat[1] *= rhs;
  return lhs;
}

MAUS::complex& operator/=(MAUS::complex& lhs, const MAUS::complex& rhs)
{
  double denominator = rhs.dat[0] * rhs.dat[0] + rhs.dat[1] * rhs.dat[1];
  MAUS::complex tmp = lhs;
  lhs.dat[0] = (lhs.dat[0] * rhs.dat[0] + lhs.dat[1] * rhs.dat[1])
             / denominator;
  lhs.dat[1] = (tmp.dat[1] * rhs.dat[0] - tmp.dat[0] * rhs.dat[1])
             / denominator;
  return lhs;
}

MAUS::complex& operator/=(MAUS::complex& lhs, const double& rhs)
{
  lhs.dat[0] /= rhs;
  lhs.dat[1] /= rhs;
  return lhs;
}

//**********************
// Algebraic Operators
//**********************

const MAUS::complex operator+(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result += rhs;
}

const MAUS::complex operator+(const MAUS::complex&   lhs,
                              const double&          rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result += rhs;
}

const MAUS::complex operator+(const double&          lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result += rhs;
}

const MAUS::complex operator-(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result -= rhs;
}

const MAUS::complex operator-(const MAUS::complex&   lhs,
                              const double&          rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result -= rhs;
}

const MAUS::complex operator-(const double&          lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result -= rhs;
}

const MAUS::complex operator*(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result *= rhs;
}

const MAUS::complex operator*(const MAUS::complex&   lhs,
                              const double&          rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result *= rhs;
}

const MAUS::complex operator*(const double&          lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result *= rhs;
}

const MAUS::complex operator/(const MAUS::complex&   lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result /= rhs;
}

const MAUS::complex operator/(const MAUS::complex&   lhs,
                              const double&          rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result /= rhs;
}

const MAUS::complex operator/(const double&          lhs,
                              const MAUS::complex&   rhs)
{
  MAUS::complex result = MAUS::Complex::complex(lhs);
  return result /= rhs;
}

//**********************
// Comparison Operators
//**********************

const bool operator==(const MAUS::complex&           lhs,
                      const MAUS::complex&           rhs)
{
  return lhs.dat[0] == rhs.dat[0] && lhs.dat[1] == rhs.dat[1];
}

const bool operator==(const MAUS::complex&           lhs,
                      const double&                  rhs)
{
  return lhs.dat[1] == 0.0 && lhs.dat[0] == rhs;
}

const bool operator==(const double&                  lhs,
                      const MAUS::complex&           rhs)
{
  return rhs.dat[1] == 0.0 && rhs.dat[0] == lhs;
}

const bool operator!=(const MAUS::complex&           lhs,
                      const MAUS::complex&           rhs)
{
  return !(lhs==rhs);
}

const bool operator!=(const MAUS::complex&           lhs,
                      const double&                  rhs)
{
  return !(lhs==rhs);
}

const bool operator!=(const double&                  lhs,
                      const MAUS::complex&           rhs)
{
  return !(lhs==rhs);
}

//**********************
// Stream Operators
//**********************

std::ostream& operator<<(std::ostream& out, const MAUS::complex& c)
{
  out << "(" << c.dat[0] << "," << c.dat[1] << ")";
  return out;
}

std::istream& operator>>(std::istream& in,  MAUS::complex& c)
{
  std::string dummy;
  in >> dummy >> c.dat[0] >> dummy >> c.dat[1] >> dummy;
/*
  std::string string_value;
  in >> string_value;

  std::string::size_type begin_real = string_value.find("(") + 1;
  std::string::size_type end_real = string_value.find(",");
  std::string real = string_value.substr(begin_real, end_real);
  std::istringstream real_stream(string_value.substr(begin_real, end_real));
  if (!(real_stream >> c.dat[0]))
  {
    c.dat[0] = 0.0;
  }
  
  std::string::size_type begin_imag = end_real + 1;
  std::string::size_type end_imag = string_value.find(")");
  std::string imag = string_value.substr(begin_imag, end_imag);
  std::istringstream imag_stream(string_value.substr(begin_real, end_real));
  if (!(imag_stream >> c.dat[1]))
  {
    c.dat[1] = 0.0;
  }
*/  
  return in;
}
