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


#include <limits>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"
#include "CLHEP/Random/Random.h"

#include "Maths/HermitianMatrix.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

using MAUS::Matrix;
using MAUS::HermitianMatrix;
using MAUS::SymmetricMatrix;
using MAUS::Vector;
using MAUS::real;
using MAUS::imag;
using MAUS::conj;
using MAUS::operator *;
using MAUS::operator -;

// Defined in ComplexTest.cc
bool equal(double d1, double d2);
bool equal(MAUS::complex c1, MAUS::complex c2);

class HermitianMatrixTest : public ::testing::Test {
 public:
  HermitianMatrixTest() {
    for (size_t index = 0; index < data_size_; ++index) {
      complex_data_doubled_[index] = complex_data_[index] * 2.;
      complex_data_multiple_[index] = complex_data_[index]
                                    * complex_scaling_factor_;
    }
  }

  bool elements_equal(const HermitianMatrix& matrix,
                      const MAUS::complex value) {
    MAUS::complex value_conj = conj(value);
    MAUS::complex value_real = MAUS::Complex::complex(real(value));
    bool are_equal = true;
    if (matrix.size() != size_) {
      return false;
    }
    for (size_t row = 1; row <= size_; ++row) {
      for (size_t column = 1; column <= row; ++column) {
        if (row == column) {
          are_equal &= ::equal(matrix(row, column), value_real);
        } else {
          are_equal &= ::equal(matrix(row, column), value);
          are_equal &= ::equal(matrix(column, row), value_conj);
        }
      }
    }
    return are_equal;
  }

  bool equal(const HermitianMatrix& matrix, const MAUS::complex* array) {
    bool are_equal = true;
    if (matrix.size() != size_) {
      return false;
    }
    for (size_t row = 1; row <= size_; ++row) {
      for (size_t column = 1; column <= row; ++column) {
        are_equal &= ::equal(matrix(row, column),
                             array[(row-1)*size_+(column-1)]);
        if (row != column) {
          are_equal &= ::equal(matrix(column, row),
                               conj(array[(row-1)*size_+(column-1)]));
        }
      }
    }
    return are_equal;
  }

  template <typename Tmplt>
  bool equal(const Matrix<Tmplt>& m1, const Matrix<Tmplt>& m2) {
    bool are_equal = true;
    if (   m1.number_of_rows()    != m2.number_of_rows()
        || m1.number_of_columns() != m2.number_of_columns()) {
      return false;
    }
    for (size_t i = 1; i <= m1.number_of_rows(); i++)
      for (size_t j = 1; j <= m1.number_of_columns(); j++)
        are_equal &= ::equal(m1(i, j), m2(i, j));
    return are_equal;
  }

  bool equal(const HermitianMatrix& m1, const HermitianMatrix& m2) {
    return equal((Matrix<MAUS::complex>&) m1, (Matrix<MAUS::complex>&) m2);
  }

  bool equal(const Vector<MAUS::complex>& v1, const Vector<MAUS::complex>& v2) {
    bool are_equal = true;
    if (v1.size() != v2.size()) {
      return false;
    }
    for (size_t i = 1; i <= v1.size(); ++i)
      are_equal &= ::equal(v1(i), v2(i));
    return are_equal;
  }
 protected:
  static const size_t data_size_;
  static const double real_data_[25];
  static const double imag_data_[25];
  static const MAUS::complex complex_data_[25];
  static const size_t size_;
  MAUS::complex complex_data_doubled_[25];
  static const MAUS::complex complex_scaling_factor_;
  MAUS::complex complex_data_multiple_[25];
};

template bool HermitianMatrixTest::equal(const Matrix<double>& m1,
                                const Matrix<double>& m2);
template bool HermitianMatrixTest::equal(const Matrix<MAUS::complex>& m1,
                                const Matrix<MAUS::complex>& m2);

// ************************************************
// HermitianMatrixTest static const initializations
// ************************************************

const double HermitianMatrixTest::real_data_[25] = {
  0.,    1.,     2.,     3.,    -5.,
  1.,   -13.,   -21.,    34.5,   55.7,
  2.,   -21.,   -32.5,  -57.5,  -91.2,
  3.,    34.5,  -57.5,   2.65,  -3.58,
  -5.,  -55.7,  -91.2,  -3.58,   3.38
};

const double HermitianMatrixTest::imag_data_[25] = {
  0.,      3.58,   91.2,   55.7,     5.,
  -3.58,   0.,     57.5,  -34.5,     4.98,
  -91.2,  -57.5,   0.,     21.,     -2.,
  -55.7,   34.5,  -21.,    0.,       112.7,
  -5.,    -4.98,   2.,    -112.7,    0.
};

const MAUS::complex HermitianMatrixTest::complex_data_[25] = {
  MAUS::Complex::complex(0., 0.),      MAUS::Complex::complex(1., 3.58),
  MAUS::Complex::complex(2., 91.2),    MAUS::Complex::complex(3., 55.7),
  MAUS::Complex::complex(-5., 5.),

  MAUS::Complex::complex(1., -3.58),  MAUS::Complex::complex(-13., 0.),
  MAUS::Complex::complex(-21., 57.5),    MAUS::Complex::complex(34.5, -34.5),
  MAUS::Complex::complex(55.7, 4.98),

  MAUS::Complex::complex(2., -91.2),  MAUS::Complex::complex(-21., -57.5),
  MAUS::Complex::complex(-32.5, 0.),    MAUS::Complex::complex(-57.5, 21.),
  MAUS::Complex::complex(-91.2, -2.),

  MAUS::Complex::complex(3., -55.7),  MAUS::Complex::complex(34.5, 34.5),
  MAUS::Complex::complex(-57.5, -21.),  MAUS::Complex::complex(2.65, 0.),
  MAUS::Complex::complex(-3.58, 112.7),

  MAUS::Complex::complex(-5., -5.),    MAUS::Complex::complex(-55.7, -4.98),
  MAUS::Complex::complex(-91.2, 2.),    MAUS::Complex::complex(-3.58, -112.7),
  MAUS::Complex::complex(3.38, 0.)
};

const size_t HermitianMatrixTest::data_size_ = 25;
const size_t HermitianMatrixTest::size_      = 5;
const MAUS::complex HermitianMatrixTest::complex_scaling_factor_
  = MAUS::Complex::complex(2, -5);

// ***********
// test cases
// ***********

TEST_F(HermitianMatrixTest, DefaultConstructor) {
  HermitianMatrix matrix_h0;
  ASSERT_EQ(matrix_h0.size(), (size_t) 0);
}

TEST_F(HermitianMatrixTest, CopyConstructor) {
  // should work if matrix is const
  const HermitianMatrix matrix_h0(size_, complex_data_);
  const HermitianMatrix matrix_h1(matrix_h0);
  EXPECT_TRUE(equal(matrix_h0, matrix_h1));

  // check that handles 0 length okay
  HermitianMatrix matrix_h2;
  HermitianMatrix matrix_h3(matrix_h2);
  EXPECT_TRUE(equal(matrix_h2, matrix_h3));
}

TEST_F(HermitianMatrixTest, ConstructorSizeOnly) {
  HermitianMatrix matrix_h0(size_);
  EXPECT_TRUE(elements_equal(matrix_h0, MAUS::Complex::complex(0.)));
}

TEST_F(HermitianMatrixTest, ConstructorFill) {
  HermitianMatrix matrix_h0(size_, complex_data_[4]);
  EXPECT_TRUE(elements_equal(matrix_h0, complex_data_[4]));
}

TEST_F(HermitianMatrixTest, ConstructorFromArray) {
  HermitianMatrix matrix_h0(size_, complex_data_);
  EXPECT_TRUE(equal(matrix_h0, complex_data_));
}

TEST_F(HermitianMatrixTest, Set) {
  const HermitianMatrix matrix_h0(size_, complex_data_);
  HermitianMatrix matrix_h1(size_);
  for (size_t row = 1; row <= size_; ++row) {
    for (size_t column = 1; column <= row; ++column) {
      matrix_h1.set(row, column, complex_data_[(row-1)*size_+(column-1)]);
    }
  }
  EXPECT_TRUE(equal(matrix_h0, matrix_h1));
}

TEST_F(HermitianMatrixTest, Assignment) {
  // plain vanilla assignment
  const HermitianMatrix matrix_h0(size_, complex_data_);
  HermitianMatrix matrix_h1;
  matrix_h1 = matrix_h0;
  EXPECT_TRUE(equal(matrix_h1, matrix_h0));
}

TEST_F(HermitianMatrixTest, Addition) {
  // add and assign
  HermitianMatrix matrix_h0(size_, complex_data_);
  HermitianMatrix matrix_h1(size_);
  matrix_h1 += matrix_h0;
  ASSERT_TRUE(equal(matrix_h0, matrix_h1));

  // add
  HermitianMatrix matrix_h2 = matrix_h0 + matrix_h1;
  EXPECT_TRUE(equal(matrix_h2, complex_data_doubled_));
}

TEST_F(HermitianMatrixTest, Subtraction) {
  // subtract and assign
  HermitianMatrix matrix_h0(size_, complex_data_);
  HermitianMatrix matrix_h1(size_, complex_data_doubled_);
  HermitianMatrix matrix_h2(matrix_h1);
  matrix_h2 -= matrix_h0;
  ASSERT_TRUE(equal(matrix_h2, matrix_h0));

  // subtract
  HermitianMatrix matrix_h3 = matrix_h1 - matrix_h0;
  EXPECT_TRUE(equal(matrix_h3, complex_data_));
}

TEST_F(HermitianMatrixTest, Inverse) {
  HermitianMatrix matrix_h1(size_, complex_data_);
  HermitianMatrix matrix_h2 = inverse(matrix_h1);
  // M^-1 * M = Identity
  Matrix<MAUS::complex> matrix_c0
    = (Matrix<MAUS::complex>) matrix_h1 * matrix_h2;
  MAUS::complex one = MAUS::Complex::complex(1.);
  MAUS::complex zero = MAUS::Complex::complex(0.);
  for (size_t row = 1; row <= size_; ++row) {
    for (size_t column = 1; column <= size_; ++column) {
      EXPECT_TRUE(::equal(row == column ? one : zero, matrix_c0(row, column)));
    }
  }
}

TEST_F(HermitianMatrixTest, ComplexDecomposition) {
  const HermitianMatrix matrix_h0(size_, complex_data_);
  const SymmetricMatrix matrix_s0(size_, real_data_);

  // the real part of complex_data_ is just real_data_
  SymmetricMatrix matrix_real = real(matrix_h0);
  EXPECT_TRUE(equal(matrix_real, matrix_s0));
}

TEST_F(HermitianMatrixTest, Eigen) {
  // verify the eigensystem equation M*E = e*E
  HermitianMatrix matrix_h0(size_, complex_data_);
  std::pair< Vector<double>, Matrix<MAUS::complex> > eigensys
    = eigensystem(matrix_h0);
  Vector<double> eigenvals = eigensys.first;
  Matrix<MAUS::complex> eigenvectors = eigensys.second;
  ASSERT_EQ(eigenvals.size(), size_);
  ASSERT_EQ(eigenvectors.number_of_columns(), size_);
  for (size_t i = 1; i < eigenvals.size(); ++i) {
    double eigenvalue = eigenvals(i);
    Vector<MAUS::complex> eigenvector = eigenvectors.column(i);
    EXPECT_TRUE(equal(matrix_h0 * eigenvector,
                      MAUS::Complex::complex(eigenvalue) * eigenvector));
  }

  // verify that just getting the eigenvalues returns the same set from
  // eigensystem() (but perhaps in a different order)
  Vector<double> eigenvals2 = eigenvalues(matrix_h0);
  ASSERT_EQ(eigenvals2.size(), size_);
  for (size_t i = 1; i <= size_; ++i) {
    bool found_match = false;
    for (size_t j = 1; j <= size_; j++) {
      if (::equal(eigenvals2(i), eigenvals(j))) {
        found_match = true;
      }
    }
    EXPECT_TRUE(found_match);
  }
}

TEST_F(HermitianMatrixTest, Negation) {
  HermitianMatrix matrix_h0(size_, complex_data_);
  HermitianMatrix matrix_h1(-matrix_h0);
  for (size_t row = 1; row <= size_; ++row) {
    for (size_t column = 1; column <= row; ++column) {
      EXPECT_TRUE(::equal(-matrix_h1(row, column), matrix_h0(row, column)));
      EXPECT_TRUE(::equal(-matrix_h1(column, row), matrix_h0(column, row)));
    }
  }
}
