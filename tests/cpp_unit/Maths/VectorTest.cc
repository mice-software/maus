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

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"
#include "CLHEP/Matrix/Vector.h"

#include "Maths/Complex.hh"
#include "Maths/Vector.hh"
#include "Utils/Exception.hh"

using MAUS::Vector;
using MAUS::operator ==;
using MAUS::operator !=;
using MAUS::operator +;
using MAUS::operator +=;
using MAUS::operator -;
using MAUS::operator -=;
using MAUS::operator *;
using MAUS::operator *=;
using MAUS::operator /;
using MAUS::real;
using MAUS::imag;

// Defined in ComplexTest.cc
bool equal(const MAUS::complex c1, const MAUS::complex c2);
bool equal(double c1, double c2);

class VectorTest : public testing::Test {
 public:
  VectorTest()
    : d_mv1(4),
      d_mv2(Vector<double>(4, 4.)),
      d_mv3(Vector<double>(&da[0], 3)),
      d_mv6(d_mv1),
      c_mv1(Vector<MAUS::complex>(4)),
      c_mv2(Vector<MAUS::complex>(4, MAUS::Complex::complex(1., 1.))),
      c_mv3(Vector<MAUS::complex>(&ca[0], 3)),
      c_mv6(c_mv1) {
  }

 protected:
  static const double da[];
  static const Vector<double> d_mv0;
  Vector<double> d_mv1;
  Vector<double> d_mv2;
  Vector<double> d_mv3;
  static const Vector<double> d_mv4;
  static const Vector<double> d_mv5;
  Vector<double> d_mv6;

  static const MAUS::complex ca[3];
  static const Vector<MAUS::complex> c_mv0;
  Vector<MAUS::complex> c_mv1;
  Vector<MAUS::complex> c_mv2;
  Vector<MAUS::complex> c_mv3;
  static const Vector<MAUS::complex> c_mv4;
  static const Vector<MAUS::complex> c_mv5;
  Vector<MAUS::complex> c_mv6;
};

// ****************************************
// VectorTest static const initializations
// ****************************************

const double VectorTest::da[3] = {1., 2., 3.};
const Vector<double> VectorTest::d_mv0 = Vector<double>();
const Vector<double> VectorTest::d_mv4
  = Vector<double>(std::vector<double>(&da[0], &da[3]));
const Vector<double> VectorTest::d_mv5 = d_mv4;

const MAUS::complex VectorTest::ca[3] = {MAUS::Complex::complex(1., -1.),
                                         MAUS::Complex::complex(2., 0.),
                                         MAUS::Complex::complex(3., 1.)};
const Vector<MAUS::complex> VectorTest::c_mv0 = Vector<MAUS::complex>();
const Vector<MAUS::complex> VectorTest::c_mv4
  = Vector<MAUS::complex>(std::vector<MAUS::complex>(&ca[0], &ca[3]));
const Vector<MAUS::complex> VectorTest::c_mv5 = c_mv4;

// ***********
// test cases
// ***********

TEST_F(VectorTest, Size) {
  ASSERT_EQ(d_mv1.size(), (size_t) 4);
  ASSERT_EQ(d_mv2.size(), (size_t) 4);
  ASSERT_EQ(d_mv3.size(), (size_t) 3);
  ASSERT_EQ(d_mv4.size(), (size_t) 3);
  ASSERT_EQ(c_mv1.size(), (size_t) 4);
  ASSERT_EQ(c_mv2.size(), (size_t) 4);
  ASSERT_EQ(c_mv3.size(), (size_t) 3);
  ASSERT_EQ(c_mv4.size(), (size_t) 3);
}

TEST_F(VectorTest, Subvector) {
  // bad access of empty vector
  bool testpass = true;
  try {
    d_mv0.subvector(0, 1);
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  size_t sub1 = 2, sub2 = 3;
  Vector<double> d_mv_sub = d_mv4.subvector(sub1, sub2);
  ASSERT_TRUE(d_mv_sub.size() == size_t(sub2-sub1));
  for (size_t i = 0; i < d_mv_sub.size(); ++i)
    ASSERT_TRUE(d_mv_sub[i] == d_mv4[i+sub1]);

  // bad access of empty vector
  testpass = true;
  try {
    c_mv0.subvector(0, 1);
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  Vector<MAUS::complex> c_mv_sub = c_mv4.subvector(sub1, sub2);
  ASSERT_TRUE(c_mv_sub.size() == size_t(sub2-sub1));
  for (size_t i = 0; i < c_mv_sub.size(); ++i)
    ASSERT_TRUE(c_mv_sub[i] == c_mv4[i+sub1]);
}

TEST_F(VectorTest, Indexing) {
  // bad access of empty vector
  bool testpass = true;
  try {
    d_mv0[0];
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    const double test_vector = d_mv0[0];
    std::cout << test_vector;  // eliminate unused variable warning
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    d_mv0(1);
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    const double test_vector = d_mv0(1);
    std::cout << test_vector;  // eliminate unused variable warning
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  for (size_t i = 0; i < d_mv2.size(); ++i) ASSERT_TRUE(d_mv2(i+1) == 4.);
  for (size_t i = 0; i < d_mv3.size(); ++i) ASSERT_TRUE(d_mv3(i+1) == da[i]);
  for (size_t i = 0; i < d_mv4.size(); ++i) ASSERT_TRUE(d_mv4(i+1) == da[i]);
  for (size_t i = 0; i < d_mv5.size(); ++i) ASSERT_TRUE(d_mv5(i+1) == da[i]);
  for (size_t i = 0; i < c_mv2.size(); ++i)
    ASSERT_TRUE(c_mv2(i+1) == MAUS::Complex::complex(1., 1.));

  d_mv2(2+1) = 2.;
  EXPECT_TRUE(d_mv2(2+1) == 2.);
  d_mv2(2+1) = 4.;

  // bad access of empty vector
  testpass = true;
  try {
    c_mv0[0];
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    const MAUS::complex test_vector = c_mv0[0];
    // eliminate unused variable warning
    std::cout << real(test_vector) << imag(test_vector);
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    c_mv0(1);
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    const MAUS::complex test_vector = c_mv0(1);
    // eliminate unused variable warning
    std::cout << real(test_vector) << imag(test_vector);
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  for (size_t i = 0; i < c_mv3.size(); ++i) ASSERT_TRUE(c_mv3(i+1) == ca[i]);
  for (size_t i = 0; i < c_mv4.size(); ++i) ASSERT_TRUE(c_mv4(i+1) == ca[i]);
  for (size_t i = 0; i < c_mv5.size(); ++i) ASSERT_TRUE(c_mv5(i+1) == ca[i]);

  for (size_t i = 1; i <= d_mv2.size(); ++i) ASSERT_TRUE(d_mv2(i) == 4.);
  for (size_t i = 1; i <= d_mv3.size(); ++i) ASSERT_TRUE(d_mv3(i) == da[i-1]);
  for (size_t i = 1; i <= c_mv2.size(); ++i)
    ASSERT_TRUE(c_mv2(i) == MAUS::Complex::complex(1., 1.));
  for (size_t i = 1; i <= c_mv3.size(); ++i) ASSERT_TRUE(c_mv3(i) == ca[i-1]);

  c_mv2(2+1) = MAUS::Complex::complex(-2., 2.);
  EXPECT_TRUE(c_mv2(2+1) == MAUS::Complex::complex(-2., 2.));
  c_mv2(2+1) = MAUS::Complex::complex(1., 1.);
}

TEST_F(VectorTest, Equals) {
  Vector<double> empty_d_vector;
  EXPECT_EQ(empty_d_vector, d_mv0);

  EXPECT_EQ(d_mv5, d_mv4);

  Vector<MAUS::complex> empty_c_vector;
  EXPECT_EQ(empty_c_vector, c_mv0);

  EXPECT_EQ(c_mv5, c_mv4);
}

TEST_F(VectorTest, NotEquals) {
  d_mv3(1) *= -1;
  EXPECT_TRUE(d_mv5 != d_mv0);
  EXPECT_TRUE(d_mv5 != d_mv1);
  EXPECT_TRUE(d_mv5 != d_mv2);
  EXPECT_TRUE(d_mv5 != d_mv3);
  d_mv3(1) *= -1;

  c_mv3(1) *= -1;
  EXPECT_TRUE(c_mv5 != c_mv0);
  EXPECT_TRUE(c_mv5 != c_mv1);
  EXPECT_TRUE(c_mv5 != c_mv2);
  EXPECT_TRUE(c_mv5 != c_mv3);
  c_mv3(1) *= -1;
}

TEST_F(VectorTest, StdVectorConstructor) {
  std::vector<double> stdvec(4, 4.);  // like d_mv2
  Vector<double> vector_d0(stdvec);
  ASSERT_TRUE(vector_d0 == d_mv2);
}

TEST_F(VectorTest, HepVectorConstructor) {
  CLHEP::HepVector hepvec(4, 1.);
  Vector<double> d_test_vec(4, 1.);
  Vector<double> vector_d0(hepvec);
  ASSERT_EQ(vector_d0, d_test_vec);

  hepvec = CLHEP::HepVector(4, 1.);
  Vector<MAUS::complex> vector_c0(hepvec);
  Vector<MAUS::complex> c_test_vec(4, MAUS::Complex::complex(1., 0.));
  ASSERT_EQ(vector_c0, c_test_vec);
}

TEST_F(VectorTest, Assignment) {
  // bad empty vector assignment
  Vector<double> empty_d_vector(4);
  bool testpass = true;
  try {
    empty_d_vector = d_mv0;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  // bad assignment of differently sized vector
  testpass = true;
  try {
    d_mv2 = d_mv4;  // d_mv2.size() is 4 while d_mv4.size() is 3
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  Vector<double> vector_d0 = d_mv3;
  vector_d0 = vector_d0;  // check for specific subtle bug when self-allocating
  EXPECT_TRUE(vector_d0 == d_mv3);  // should be exactly ==
  for (size_t i = 0; i < d_mv3.size(); ++i) vector_d0(i+1) = 2.*vector_d0(i+1);
  EXPECT_TRUE(vector_d0 != d_mv3);  // verify deepcopy

  // check special assignement to null matrix
  Vector<double> vector_d1;
  vector_d1 = vector_d0;
  EXPECT_TRUE(vector_d1 == vector_d0);

  // bad empty vector assignment
  Vector<MAUS::complex> empty_c_vector(4);
  testpass = true;
  try {
    empty_c_vector = c_mv0;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  // bad assignment of differently sized vector
  testpass = true;
  try {
    c_mv2 = c_mv4;  // c_mv2.size() is 4 while c_mv4.size() is 3
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  Vector<MAUS::complex> vector_c0 = c_mv3;
  vector_c0 = vector_c0;  // check for specific subtle bug when self-allocating
  EXPECT_TRUE(vector_c0 == c_mv3);  // should be exactly ==
  for (size_t i = 0; i < c_mv3.size(); ++i) vector_c0(i+1) = 2.*vector_c0(i+1);
  EXPECT_TRUE(vector_c0 != c_mv3);  // verify deepcopy)

  // check special assignement to null matrix
  Vector<MAUS::complex> vector_c1;
  vector_c1 = vector_c0;
  EXPECT_TRUE(vector_c1 == vector_c0);
}

TEST_F(VectorTest, Multiplication) {
  // real, vector multiplication

  // bad multiplication of differently sized vectors
  Vector<double> test_d_vector(3, 2.);
  bool testpass = true;
  try {
    test_d_vector *= d_mv2;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    test_d_vector * d_mv0;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  test_d_vector *= d_mv4;  // {2., 2., 2.} * {1., 2., 3.}
  for (size_t i = 0; i < test_d_vector.size(); ++i)
    ASSERT_TRUE(fabs(test_d_vector(i+1) - da[i]*2.) < 1e-9);

  // real, scalar multiplication

  d_mv3 *= 2.;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    ASSERT_TRUE(fabs(d_mv3(i+1) - da[i]*2.) < 1e-9);

  d_mv3 = d_mv4 * 2.;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    ASSERT_TRUE(fabs(d_mv3(i+1) - da[i]*2.) < 1e-9);

  d_mv3 = 2. * d_mv4;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    ASSERT_TRUE(fabs(d_mv3(i+1) - da[i]*2.) < 1e-9);

  d_mv3 = d_mv4;

  // complex, vector multiplication

  // bad multiplication of differently sized vectors
  Vector<MAUS::complex> test_c_vector(3, MAUS::Complex::complex(2., 1.));
  testpass = true;
  try {
    test_c_vector *= c_mv2;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    test_c_vector * c_mv0;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  // {(2.,1.), (2.,1.), (2.,1.)} * {(1.,-1.), (2.,0.), (3.,1.)}
  test_c_vector *= c_mv4;
  MAUS::complex difference;
  difference = test_c_vector(1) - MAUS::Complex::complex(3., -1.);
  ASSERT_TRUE(fabs(real(difference)) < 1e-9);
  ASSERT_TRUE(fabs(imag(difference)) < 1e-9);
  difference = test_c_vector(2) - MAUS::Complex::complex(4., 2.);
  ASSERT_TRUE(fabs(real(difference)) < 1e-9);
  ASSERT_TRUE(fabs(imag(difference)) < 1e-9);
  difference = test_c_vector(3) - MAUS::Complex::complex(5., 5.);
  ASSERT_TRUE(fabs(real(difference)) < 1e-9);
  ASSERT_TRUE(fabs(imag(difference)) < 1e-9);

  // complex, scalar multiplication

  c_mv3 *= MAUS::Complex::complex(2., -2.);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    ASSERT_TRUE(equal(c_mv3(i+1), ca[i]*MAUS::Complex::complex(2., -2.)));

  c_mv3 = c_mv4 * MAUS::Complex::complex(2., -2.);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    ASSERT_TRUE(equal(c_mv3(i+1), ca[i]*MAUS::Complex::complex(2., -2.)));

  c_mv3 = MAUS::Complex::complex(2., -2.) * c_mv4;
  for (size_t i = 0; i < c_mv3.size(); ++i)
    ASSERT_TRUE(equal(c_mv3(i+1), ca[i]*MAUS::Complex::complex(2., -2.)));

  c_mv3 = c_mv4;
}

TEST_F(VectorTest, Division) {
  // real, vector multiplication

  // bad division of differently sized vectors
  Vector<double> test_d_vector(3, 2.);
  bool testpass = true;
  try {
    test_d_vector /= d_mv2;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    test_d_vector / d_mv0;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  test_d_vector /= d_mv4;  // {2., 2., 2.} / {1., 2., 3.}
  for (size_t i = 0; i < test_d_vector.size(); ++i)
    ASSERT_TRUE(fabs(test_d_vector(i+1) - 2./da[i]) < 1e-9);

  // real, scalar division

  d_mv3 /= 2.;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]/2.) < 1e-9);
  d_mv3 = d_mv4;

  d_mv3 = d_mv3 / 2.;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]/2.) < 1e-9);
  d_mv3 = d_mv4;

  // complex, vector multiplication

  // bad multiplication of differently sized vectors
  Vector<MAUS::complex> test_c_vector(3, MAUS::Complex::complex(2., 1.));
  testpass = true;
  try {
    test_c_vector /= c_mv2;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);
  testpass = true;
  try {
    test_c_vector / c_mv0;
    testpass = false;
  } catch (MAUS::Exceptions::Exception exc) {}
  EXPECT_TRUE(testpass);

  // {(2.,1.), (2.,1.), (2.,1.)} / {(1.,-1.), (2.,0.), (3.,1.)}
  test_c_vector /= c_mv4;
  MAUS::complex difference;
  difference = test_c_vector(1) - MAUS::Complex::complex(0.5, 1.5);
  ASSERT_TRUE(fabs(real(difference)) < 1e-9);
  ASSERT_TRUE(fabs(imag(difference)) < 1e-9);
  difference = test_c_vector(2) - MAUS::Complex::complex(1., 0.5);
  ASSERT_TRUE(fabs(real(difference)) < 1e-9);
  ASSERT_TRUE(fabs(imag(difference)) < 1e-9);
  difference = test_c_vector(3) - MAUS::Complex::complex(0.7, 0.1);
  ASSERT_TRUE(fabs(real(difference)) < 1e-9);
  ASSERT_TRUE(fabs(imag(difference)) < 1e-9);

  // complex, scalar multiplication

  c_mv3 /= MAUS::Complex::complex(2., -3);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]/MAUS::Complex::complex(2., -3)));
  c_mv3 = c_mv4;

  c_mv3 = c_mv3 / MAUS::Complex::complex(2., -3);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]/MAUS::Complex::complex(2., -3)));
  c_mv3 = c_mv4;
}

TEST_F(VectorTest, Addition) {
  d_mv3 += (d_mv3/2.);
  for (size_t i = 0; i < d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]*(1.+1./2.)) < 1e-9);
  d_mv3  = d_mv4;

  c_mv3 += (c_mv3/MAUS::Complex::complex(3, -2));
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]*(1.+1./MAUS::Complex::complex(3, -2))));
  c_mv3  = c_mv4;

  d_mv3  = d_mv3 + (d_mv3/2.);
  for (size_t i = 0; i < d_mv3.size(); ++i) {
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]*(1.+1./2.) ) < 1e-9);
    }
  d_mv3  = d_mv4;

  c_mv3  = c_mv3 + (c_mv3/MAUS::Complex::complex(3, -2));
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]*(1.+1./MAUS::Complex::complex(3, -2))));
  c_mv3  = c_mv4;
}

TEST_F(VectorTest, Inversion) {
  for (size_t i = 0; i < d_mv3.size(); ++i)
    EXPECT_TRUE(fabs((-d_mv3)(i+1)
                     + d_mv3(i+1)) < 1e-9);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal((-c_mv3)(i+1), -(c_mv3(i+1))));
}

TEST_F(VectorTest, Subtraction) {
  d_mv3 -= d_mv4;
  c_mv3 -= c_mv4;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3(i+1)) < 1e-9);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal((-c_mv3)(i+1), MAUS::Complex::complex(0, 0)));
  d_mv3 = d_mv4;
  c_mv3 = c_mv4;
  d_mv3 = d_mv3 - d_mv4;
  c_mv3 = c_mv3 - c_mv4;
  for (size_t i = 0; i < d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3(i+1) ) < 1e-9);
  for (size_t i = 0; i < c_mv3.size(); ++i)
    EXPECT_TRUE(equal((-c_mv3)(i+1), MAUS::Complex::complex(0, 0)));
}


TEST_F(VectorTest, ComplexDecomposition) {
  Vector<double> real_part = real(c_mv4);
  EXPECT_EQ(real_part, d_mv4);

  Vector<double> imag_part = imag(c_mv4);
  for (int index = 0; index < 3; ++index) {
    EXPECT_EQ(imag_part[index], static_cast<double>(index-1));
  }
}

TEST_F(VectorTest, Streaming) {
  std::stringstream ss;
  ss  << d_mv4 << "\n";
  Vector<double>    d_mvio;
  Vector<MAUS::complex> c_mvio;
  ss >> d_mvio;
  ss << c_mv4 << std::endl;
  ss >> c_mvio;
  for (size_t i = 1; i <= d_mvio.size(); ++i)
    ASSERT_TRUE(equal(d_mvio(i), d_mv4(i)));
  for (size_t i = 1; i <= c_mvio.size(); ++i)
    ASSERT_TRUE(equal(c_mvio(i), c_mv4(i)));
}
