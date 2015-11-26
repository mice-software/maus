/* This file is part of MAUS: http://   micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include <stdio.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"

#include "Maths/Complex.hh"

using MAUS::imag;
using MAUS::real;
using MAUS::conj;
using MAUS::operator ==;
using MAUS::operator !=;
using MAUS::operator *;
using MAUS::operator *=;
using MAUS::operator /;
using MAUS::operator /=;
using MAUS::operator +;
using MAUS::operator +=;
using MAUS::operator -;
using MAUS::operator -=;
using MAUS::operator <<;
using MAUS::operator >>;

bool equal(const MAUS::complex c1, const MAUS::complex c2) {
  return     fabs(real(c1) - real(c2)) < 1e-9
          && fabs(imag(c1) - imag(c2) ) < 1e-9;
}

bool equal(double c1, double c2) { return fabs(c1-c2) < 1e-9; }

class ComplexTest : public testing::Test {
 public:
  ComplexTest() : c1(MAUS::Complex::complex(3,  -2)),
                  c2(MAUS::Complex::complex(3,  -2)),
                  c3(MAUS::Complex::complex(3,   2)),
                  c4(MAUS::Complex::complex(3,  -2)),
                  cs(MAUS::Complex::complex(6.)),
                  cc(MAUS::Complex::complex(3,  -2))
  { }
 protected:
  static constexpr double Pi = 3.141592653589793238462643383279502884197169399;
  MAUS::complex c1;
  MAUS::complex c2;
  MAUS::complex c3;
  MAUS::complex c4;
  MAUS::complex cs;
  MAUS::complex ct;
  const MAUS::complex cc;
};

TEST_F(ComplexTest, PseudoConstructor) {
  // real part explicitly zero, default imaginary part
  const MAUS::complex c1 = MAUS::Complex::complex(0.0);
  EXPECT_TRUE(equal(real(c1), 0.0));
  EXPECT_TRUE(equal(imag(c1), 0.0));

  // real and imaginary part explicitly zero
  const MAUS::complex c2 = MAUS::Complex::complex(0.0, 0.0);
  EXPECT_TRUE(equal(real(c2), 0.0));
  EXPECT_TRUE(equal(imag(c2), 0.0));

  // arbitrary real and imaginary part
  const MAUS::complex c3 = MAUS::Complex::complex(3.14, -15.92);
  EXPECT_TRUE(equal(real(c3), 3.14));
  EXPECT_TRUE(equal(imag(c3), -15.92));
}

TEST_F(ComplexTest, Equals) {
  // operator ==
  ASSERT_TRUE(c1 == c2);
  ASSERT_FALSE(c1 == c3);
  ASSERT_TRUE(c1 == c4);

  double real_number = 3.141;
  ct =  MAUS::Complex::complex(real_number, 0.0);
  ASSERT_TRUE(ct == real_number);
  ASSERT_TRUE(real_number == ct);
}

TEST_F(ComplexTest, NotEquals) {
  // operator !=
  ASSERT_FALSE(c1 != c2);
  ASSERT_TRUE(c1 != c3);
  ASSERT_FALSE(c1 != c4);

  double real_number = 3.141;
  ct =  MAUS::Complex::complex(0.707, 0.0);
  ASSERT_TRUE(ct != real_number);
  ASSERT_TRUE(real_number != ct);
}

TEST_F(ComplexTest, Components) {
  // real and imag functions
  EXPECT_TRUE(fabs((real(c1) - 3) ) < 1e-9);
  EXPECT_TRUE(fabs((imag(c1) + 2) ) < 1e-9);
  EXPECT_TRUE(fabs((real(cc) - 3) ) < 1e-9);
  EXPECT_TRUE(fabs((imag(cc) + 2) ) < 1e-9);
}

TEST_F(ComplexTest, Double) {
  ASSERT_TRUE(real(cs) == 6.);
  ASSERT_TRUE(fabs(imag(cs)) < 1e-9);
}

TEST_F(ComplexTest, Conjugate) {
  // complext conjugate
  EXPECT_TRUE(conj(MAUS::Complex::complex(3, -2))
              == MAUS::Complex::complex(3, 2));
  EXPECT_TRUE(conj(MAUS::Complex::complex(3))
              == MAUS::Complex::complex(3));
}

TEST_F(ComplexTest, Assignment) {
  // operator =
  ct = c1;
  EXPECT_TRUE(ct == c1);
}

TEST_F(ComplexTest, Multiplication) {
  // multiplication
  ct = c1*2.;
  EXPECT_TRUE(real(ct) == real(c1)*2. && imag(ct) == imag(c1)*2.);
  ct *= 2.;
  EXPECT_TRUE(real(ct) == real(c1)*4. && imag(ct) == imag(c1)*4.);

  ct = 2.*ct;
  EXPECT_TRUE(real(ct) == real(c1)*8. && imag(ct) == imag(c1)*8.);

  ct  = c1;
  ct *= c3;
  EXPECT_TRUE(real(ct) == (real(c1)*real(c3) - imag(c1)*imag(c3)));
  EXPECT_TRUE(imag(ct) == (real(c1)*imag(c3) + imag(c1)*real(c3)));
  ct  = c1*c3;
  EXPECT_TRUE(real(ct) == (real(c1)*real(c3) - imag(c1)*imag(c3)));
  EXPECT_TRUE(imag(ct) == (real(c1)*imag(c3) + imag(c1)*real(c3)));
}

TEST_F(ComplexTest, Division) {
  ct = c1;
  ct = ct/2.;
  ct = ct*2.;
  EXPECT_TRUE(equal(c1, ct));
  ct /= 2.;
  ct *= 2.;
  EXPECT_TRUE(equal(ct, c1));
  ct  = 2./c1;
  ct *= c1;
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(2, 0)));
  ct  = c1;
  ct /= c3;
  ct *= c3;
  EXPECT_TRUE(equal(ct, c1));
  ct  = c1/c3;
  ct *= c3;
  EXPECT_TRUE(equal(ct, c1));
}

TEST_F(ComplexTest, Addition) {
  ct =  MAUS::Complex::complex(3, -2);
  ct += 2.;
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(5, -2)));
  ct =  MAUS::Complex::complex(3, -2) + 2.;
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(5, -2)));
  ct =  2. + MAUS::Complex::complex(3, -2);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(5, -2)));
  ct =  MAUS::Complex::complex(3, -2) + MAUS::Complex::complex(5, -1);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(8, -3)));
  ct  = MAUS::Complex::complex(3, -2);
  ct += MAUS::Complex::complex(5, -1);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(8, -3)));
}

TEST_F(ComplexTest, Negation) {
  ct =  -MAUS::Complex::complex(3, -2);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(-3, 2)));
}

TEST_F(ComplexTest, Subtraction) {
  ct =  MAUS::Complex::complex(3, -2);
  ct -= 2.;
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(1, -2)));
  ct =  MAUS::Complex::complex(3, -2) - 2.;
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(1, -2)));
  ct =  2. - MAUS::Complex::complex(3, -2);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(-1, 2)));
  ct =  MAUS::Complex::complex(3, -2) - MAUS::Complex::complex(5, -1);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(-2, -1)));
  ct  = MAUS::Complex::complex(3, -2);
  ct -= MAUS::Complex::complex(5, -1);
  EXPECT_TRUE(equal(ct, MAUS::Complex::complex(-2, -1)));
}


TEST_F(ComplexTest, Polar) {
  const double radius = 2.19;
  const double angle = 26 * Pi / 180;
  const double x = radius * cos(angle);
  const double y = radius * sin(angle);
  const MAUS::complex rectangular_value = MAUS::Complex::complex(x, y);
  const MAUS::complex polar_value = MAUS::polar(radius, angle);
  EXPECT_TRUE(equal(polar_value, rectangular_value));
}

TEST_F(ComplexTest, Magnitude) {
  double magnitude_squared = MAUS::abs2(cc);
  double norm_squared = MAUS::norm2(cc);
  double test_magnitude_squared  = real(cc)*real(cc) + imag(cc)*imag(cc);
  EXPECT_TRUE(equal(magnitude_squared, test_magnitude_squared));
  EXPECT_TRUE(equal(norm_squared, test_magnitude_squared));

  double magnitude = MAUS::abs(cc);
  double norm = MAUS::norm(cc);
  double test_magnitude  = sqrt(test_magnitude_squared);
  EXPECT_TRUE(equal(magnitude, test_magnitude));
  EXPECT_TRUE(equal(norm, test_magnitude));
}

TEST_F(ComplexTest, Arg) {
  double argument = MAUS::arg(cc);
  EXPECT_TRUE(equal(argument, atan2(imag(cc), real(cc))));
}

TEST_F(ComplexTest, Exponential) {
  // e^z = e^x (cos y + i sin y)
  MAUS::complex exponential = MAUS::exp(cc);
  ct = MAUS::Complex::complex(cos(imag(cc)), sin(imag(cc)));
  ct *= exp(real(cc));
  EXPECT_TRUE(equal(exponential, ct));
}

TEST_F(ComplexTest, Logarithms) {
  MAUS::complex natural_log = MAUS::log(cc);
  double x = log(MAUS::abs(cc));
  double y = MAUS::arg(cc);
  ct = MAUS::Complex::complex(x, y);
  EXPECT_TRUE(equal(natural_log, ct));

  MAUS::complex log_base_10 = MAUS::log10(cc);
  ct = natural_log / log(10);
  EXPECT_TRUE(equal(log_base_10, ct));
}

TEST_F(ComplexTest, Pow) {
  // z^p = r^p e^{i p theta}
  const double radius = 2.19;
  const double theta = 26.46 * Pi / 180;
  const double real_power = 4.387;
  const MAUS::complex cp = MAUS::polar(radius, theta);
  MAUS::complex cc_pow_double = MAUS::pow(cp, real_power);
  ct = MAUS::Complex::complex(cos(real_power * theta), sin(real_power * theta));
  ct *= pow(radius, real_power);
  EXPECT_TRUE(equal(cc_pow_double, ct));

  // z^w = (r e^{i theta})^(x+iy) = e^{(x lnr-y theta)+i(y lnr + x theta)}
  const MAUS::complex complex_power = MAUS::Complex::complex(5.69, 2.81);
  MAUS::complex cc_pow_complex = MAUS::pow(cp, complex_power);
  MAUS::complex e_power = MAUS::Complex::complex(
    real(complex_power) * log(radius) - imag(complex_power) * theta,
    imag(complex_power) * log(radius) + real(complex_power) * theta);
  ct = MAUS::exp(e_power);
  EXPECT_TRUE(equal(cc_pow_complex, ct));
}

TEST_F(ComplexTest, SquareRoot) {
  MAUS::complex square_root = MAUS::sqrt(64.0);
  ct = MAUS::Complex::complex(sqrt(64.0), 0.0);
  EXPECT_TRUE(equal(square_root, ct));

  square_root = MAUS::sqrt(cc);
  ct = MAUS::polar(sqrt(MAUS::norm(cc)), MAUS::arg(cc)/2);
  EXPECT_TRUE(equal(square_root, ct));
}

TEST_F(ComplexTest, TrigonometricFunctions) {
  const MAUS::complex sine = MAUS::sin(cc);
  const MAUS::complex i_cc = MAUS::Complex::complex(0.0, 1.0) * cc;
  ct = (MAUS::exp(i_cc) - MAUS::exp(-i_cc)) / MAUS::Complex::complex(0.0, 2.0);
  EXPECT_TRUE(equal(sine, ct));

  const MAUS::complex cosine = MAUS::cos(cc);
  ct = (MAUS::exp(i_cc) + MAUS::exp(-i_cc)) / MAUS::Complex::complex(2.0, 0.0);
  EXPECT_TRUE(equal(cosine, ct));

  const MAUS::complex tangent = MAUS::tan(cc);
  ct = sine / cosine;
  EXPECT_TRUE(equal(tangent, ct));

  const MAUS::complex cosecant = MAUS::csc(cc);
  ct = 1.0 / sine;
  EXPECT_TRUE(equal(cosecant, ct));

  const MAUS::complex secant = MAUS::sec(cc);
  ct = 1.0 / cosine;
  EXPECT_TRUE(equal(secant, ct));

  const MAUS::complex cotangent = MAUS::cot(cc);
  ct = 1.0 / tangent;
  EXPECT_TRUE(equal(cotangent, ct));
}

TEST_F(ComplexTest, HyperbolicTrigonometricFunctions) {
  const MAUS::complex sineh = MAUS::sinh(cc);
  ct = (MAUS::exp(cc) - MAUS::exp(-cc)) / MAUS::Complex::complex(2.0, 0.0);
  EXPECT_TRUE(equal(sineh, ct));

  const MAUS::complex cosineh = MAUS::cosh(cc);
  ct = (MAUS::exp(cc) + MAUS::exp(-cc)) / MAUS::Complex::complex(2.0, 0.0);
  EXPECT_TRUE(equal(cosineh, ct));

  const MAUS::complex tangenth = MAUS::tanh(cc);
  ct = sineh / cosineh;
  EXPECT_TRUE(equal(tangenth, ct));

  const MAUS::complex cosecanth = MAUS::csch(cc);
  ct = 1.0 / sineh;
  EXPECT_TRUE(equal(cosecanth, ct));

  const MAUS::complex secanth = MAUS::sech(cc);
  ct = 1.0 / cosineh;
  EXPECT_TRUE(equal(secanth, ct));

  const MAUS::complex cotangenth = MAUS::coth(cc);
  ct = 1.0 / tangenth;
  EXPECT_TRUE(equal(cotangenth, ct));
}

TEST_F(ComplexTest, Streaming) {
  std::stringstream test_stream;
  test_stream << c1;
  test_stream >> ct;
  EXPECT_TRUE(ct == c1);
}
