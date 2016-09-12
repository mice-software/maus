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
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Random/Random.h"

#include "Maths/Complex.hh"
#include "Maths/Matrix.hh"
#include "Maths/Vector.hh"

using MAUS::complex;
using MAUS::Matrix;
using MAUS::Vector;
using MAUS::operator *;
using MAUS::operator +=;
using MAUS::operator -;
using MAUS::operator ==;
using MAUS::operator <<;
using MAUS::real;
using MAUS::imag;

// Defined in ComplexTest.cc
bool equal(const complex c1, const complex c2);
bool equal(double c1, double c2);

class MatrixTest : public ::testing::Test {
 public:
  MatrixTest() {
    for (size_t index = 0; index < kDataSize; ++index) {
      kDoubleDatadoubled_[index] = kDoubleData[index] * 2;
      kComplexDatadoubled_[index] = kComplexData[index] * 2.;
      kComplexDatamultiple_[index] = kComplexData[index]
                                    * kComplexScalingFactor;
    }
  }

  template <typename Tmplt>
  bool elements_equal(const Matrix<Tmplt>& matrix, const Tmplt value) {
    bool are_equal = true;
    if (   matrix.number_of_rows()    != kRows
        || matrix.number_of_columns() != kColumns) {
      return false;
    }
    for (size_t row = 1; row  <= kRows; ++row) {
      for (size_t column = 1; column <= kColumns; ++column) {
        are_equal &= ::equal(matrix(row, column), value);
      }
    }
    return are_equal;
  }

  template <typename Tmplt>
  bool equal(const Matrix<Tmplt>& matrix, const Tmplt* array) {
    bool are_equal = true;
    if (   matrix.number_of_rows()    != kRows
        || matrix.number_of_columns() != kColumns) {
      return false;
    }
    for (size_t row = 1; row <= kRows; ++row) {
      for (size_t column = 1; column <= kColumns; ++column) {
        are_equal &= ::equal(matrix(row, column),
                             array[(row-1)*kColumns+(column-1)]);
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

  template <typename Tmplt>
  bool equal(const Vector<Tmplt>& v1, const Vector<Tmplt>& v2) {
    bool are_equal = true;
    if (v1.size() != v2.size()) {
      return false;
    }
    for (size_t i = 1; i <= v1.size(); ++i)
      are_equal &= ::equal(v1(i), v2(i));
    return are_equal;
  }
 protected:
  static const size_t kDataSize;
  static const double kDoubleData[24];
  static const complex kComplexData[24];
  static const size_t kSize;  // square matrix
  static const size_t kRows;
  static const size_t kColumns;
  static const size_t kSubmatrixStartRow;
  static const size_t kSubmatrixRows;
  static const size_t kSubmatrixStartColumn;
  static const size_t kSubmatrixColumns;
  double kDoubleDatadoubled_[24];
  complex kComplexDatadoubled_[24];
  static const complex kComplexScalingFactor;
  complex kComplexDatamultiple_[24];
};

template bool MatrixTest::elements_equal(const Matrix<double>& matrix,
                                         const double value);
template bool MatrixTest::elements_equal(const Matrix<complex>& matrix,
                                         const complex value);
template bool MatrixTest::equal(const Matrix<double>& matrix,
                                const double* array);
template bool MatrixTest::equal(const Matrix<complex>& matrix,
                                const complex* array);
template bool MatrixTest::equal(const Matrix<double>& m1,
                                const Matrix<double>& m2);
template bool MatrixTest::equal(const Matrix<complex>& m1,
                                const Matrix<complex>& m2);
template bool MatrixTest::equal(const Vector<double>& v1,
                                const Vector<double>& v2);
template bool MatrixTest::equal(const Vector<complex>& v1,
                                const Vector<complex>& v2);

// ****************************************
// MatrixTest static const initializations
// ****************************************

const double MatrixTest::kDoubleData[24] = {
  0.,   1.,     2.,   3.,     -5.,    -8.,    -13.,   -21.,
  34.5, 55.7,   89.3, 144.2,  -32.5,  -57.5,  -91.2,  -146.3,
  3.14, -1.59,  2.65, -3.58,  9.79,   -3.23,  8.46,   -2.64
};

const complex MatrixTest::kComplexData[24] = {
  MAUS::Complex::complex(0., -2.64),    MAUS::Complex::complex(1., 8.46),
  MAUS::Complex::complex(2., -3.23),    MAUS::Complex::complex(3., 9.79),
  MAUS::Complex::complex(-5., -3.58),   MAUS::Complex::complex(-8., 2.65),
  MAUS::Complex::complex(-13., -1.59),  MAUS::Complex::complex(-21, 3.14),

  MAUS::Complex::complex(34.5, -146.3), MAUS::Complex::complex(55.7, -91.2),
  MAUS::Complex::complex(89.3, -57.5),  MAUS::Complex::complex(144.2, -32.5),
  MAUS::Complex::complex(-32.5, 144.2), MAUS::Complex::complex(-57.5, 89.3),
  MAUS::Complex::complex(-91.2, 55.7),  MAUS::Complex::complex(-146.3, 34.5),

  MAUS::Complex::complex(3.14, -21.),   MAUS::Complex::complex(-1.59, -13.),
  MAUS::Complex::complex(2.65, -8.),    MAUS::Complex::complex(-3.58, -5.),
  MAUS::Complex::complex(9.79, 3.),     MAUS::Complex::complex(-3.23, 2.),
  MAUS::Complex::complex(8.46, 1.),     MAUS::Complex::complex(-2.64, 0.)
};

const size_t MatrixTest::kDataSize = 24;
const size_t MatrixTest::kSize      = 4;
const size_t MatrixTest::kRows      = 3;
const size_t MatrixTest::kColumns   = 8;
const size_t MatrixTest::kSubmatrixRows         = 2;
const size_t MatrixTest::kSubmatrixColumns      = 4;
const size_t MatrixTest::kSubmatrixStartRow    = 2;
const size_t MatrixTest::kSubmatrixStartColumn = 3;
const MAUS::complex MatrixTest::kComplexScalingFactor
  = MAUS::Complex::complex(2, -5);

// ***********
// test cases
// ***********

TEST_F(MatrixTest, DefaultConstructor) {
  Matrix<double> matrix_d0;
  ASSERT_EQ(matrix_d0.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_d0.number_of_columns(),  (size_t) 0);

  Matrix<complex> matrix_c0;
  ASSERT_EQ(matrix_c0.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_c0.number_of_columns(),  (size_t) 0);
}

TEST_F(MatrixTest, SingleElementValueConstructor) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData[9]);
  EXPECT_TRUE(elements_equal(matrix_d0, kDoubleData[9]));

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData[17]);
  EXPECT_TRUE(elements_equal(matrix_c0, kComplexData[17]));
}

TEST_F(MatrixTest, ConstructorFromArray) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  EXPECT_TRUE(equal(matrix_d0, kDoubleData));

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  EXPECT_TRUE(equal(matrix_c0, kComplexData));
}

TEST_F(MatrixTest, IndexingElements) {
  Matrix<double> matrix_d1(kRows, kColumns, kDoubleData);
  const Matrix<double> matrix_d2(kRows, kColumns, kDoubleData);
  for (size_t i = 0; i < kRows; ++i) {
    for (size_t j = 0; j < kColumns; ++j) {
      // check that it returns correct value
      EXPECT_EQ(matrix_d1(i+1, j+1), kDoubleData[i*kColumns + j]);
      // check that it works with const matrix
      EXPECT_EQ(matrix_d2(i+1, j+1), kDoubleData[i*kColumns + j]);

      // check that it can set the correct value
      matrix_d1(i+1, j+1) = 4.;
      EXPECT_EQ(matrix_d1(i+1, j+1), 4.);
    }
  }

  // empty matrix
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3;
    matrix_d3(20, 20) = 5.0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<double> matrix_d3;
    const double element_d = matrix_d3(20, 20);
    std::cout << element_d;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // row out of bounds
  caught_exception = false;
  try {
    matrix_d1(-1, 1) = 5.0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    const double element_d = matrix_d1(-1, 1);
    std::cout << element_d;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // column out of bonds
  caught_exception = false;
  try {
    matrix_d1(1, -1) = 5.0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    const double element_d = matrix_d1(1, -1);
    std::cout << element_d;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  Matrix<complex> matrix_c1(kRows, kColumns, kComplexData);
  // check that it works for const as well
  const Matrix<complex> matrix_c2(kRows, kColumns, kComplexData);
  complex new_value1 = MAUS::Complex::complex(4., -2.);
  complex new_value2 = MAUS::Complex::complex(-6., 15.);
  for (size_t i = 0; i < kRows; ++i) {
    for (size_t j = 0; j < kColumns; ++j) {
      // check that it returns correct value
      EXPECT_TRUE(::equal(matrix_c1(i+1, j+1), kComplexData[i*kColumns + j]));
      // check that it works with const matrix
      EXPECT_TRUE(::equal(matrix_c2(i+1, j+1), kComplexData[i*kColumns + j]));

      // check that it can set the correct value
      matrix_c1(i+1, j+1) = new_value1;
      EXPECT_TRUE(::equal(matrix_c1(i+1, j+1), new_value1));
    }
  }

  // empty matrix
  MAUS::complex some_value = MAUS::Complex::complex(5.0, 6.0);
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3;
    matrix_c3(20, 20) = some_value;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3;
    const MAUS::complex element_c = matrix_c3(20, 20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // row out of bounds
  caught_exception = false;
  try {
    matrix_c1(-1, 1) = some_value;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    const MAUS::complex element_c = matrix_c1(-1, 1);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // column out of bounds
  caught_exception = false;
  try {
    matrix_c1(1, -1) = some_value;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    const MAUS::complex element_c = matrix_c1(1, -1);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, CopyConstructor) {
  // should work if matrix is const
  const Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  const Matrix<double> matrix_d1(matrix_d0);
  EXPECT_TRUE(equal(matrix_d0, matrix_d1));

  // check that handles 0 length okay
  Matrix<double> matrix_d2;
  Matrix<double> matrix_d3(matrix_d2);
  EXPECT_TRUE(equal(matrix_d2, matrix_d3));

  // should work if matrix is const
  const Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  const Matrix<complex> matrix_c1(matrix_c0);
  EXPECT_TRUE(equal(matrix_c0, matrix_c1));

  // check that handles 0 length okay
  Matrix<complex> matrix_c2;
  Matrix<complex> matrix_c3(matrix_c2);
  EXPECT_TRUE(equal(matrix_c2, matrix_c3));
}

TEST_F(MatrixTest, HepMatrixConstructor) {
  ::CLHEP::HepRandom hep_random;
  const ::CLHEP::HepMatrix matrix_hep0(kRows, kColumns, hep_random);
  const Matrix<double> matrix_d0(matrix_hep0);
  ASSERT_EQ(matrix_d0.number_of_rows(),     (size_t) kRows);
  ASSERT_EQ(matrix_d0.number_of_columns(),  (size_t) kColumns);
  for (size_t i = 1; i <= kRows; ++i)
    for (size_t j = 1; j <= kColumns; ++j)
      EXPECT_EQ(matrix_d0(i, j), matrix_hep0(i, j));

  // check that handles 0 length okay
  ::CLHEP::HepMatrix matrix_hep1;
  Matrix<double> matrix_d1(matrix_hep1);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) 0);

  const Matrix<complex> matrix_c0(matrix_hep0);
  ASSERT_EQ(matrix_c0.number_of_rows(),     (size_t) kRows);
  ASSERT_EQ(matrix_c0.number_of_columns(),  (size_t) kColumns);
  for (size_t i = 1; i <= kRows; ++i)
    for (size_t j = 1; j <= kColumns; ++j)
      EXPECT_EQ(matrix_c0(i, j), MAUS::Complex::complex(matrix_hep0(i, j)));

  // check that handles 0 length okay
  Matrix<complex> matrix_c1(matrix_hep1);
  ASSERT_EQ(matrix_c1.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_c1.number_of_columns(),  (size_t) 0);
}

TEST_F(MatrixTest, ConstructorSizeOnly) {
  Matrix<double> matrix_d0(kRows, kColumns);
  EXPECT_TRUE(elements_equal(matrix_d0, 0.));

  Matrix<complex> matrix_c0(kRows, kColumns);
  EXPECT_TRUE(elements_equal(matrix_c0, MAUS::Complex::complex(0.0)));
}

TEST_F(MatrixTest, ConstructorFill) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData[4]);
  EXPECT_TRUE(elements_equal(matrix_d0, kDoubleData[4]));

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData[4]);
  EXPECT_TRUE(elements_equal(matrix_c0, kComplexData[4]));
}

TEST_F(MatrixTest, IndexingRows) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  for (size_t row = 0; row < kRows; ++row) {
    Vector<double> row_vector = matrix_d0.row(row+1);
    for (size_t column = 0; column < kColumns; ++column) {
      EXPECT_EQ(row_vector[column], kDoubleData[row*kColumns + column]);
    }
  }

  // empty matrix
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3;
    matrix_d3.row(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // row out of bounds
  caught_exception = false;
  try {
    matrix_d0.row(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  for (size_t row = 0; row < kRows; ++row) {
    Vector<complex> row_vector = matrix_c0.row(row+1);
    for (size_t column = 0; column < kColumns; ++column) {
      EXPECT_EQ(row_vector[column], kComplexData[row*kColumns + column]);
    }
  }

  // empty matrix
  caught_exception = false;
  try {
    Matrix<double> matrix_c3;
    matrix_c3.row(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // row out of bounds
  caught_exception = false;
  try {
    matrix_c0.row(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, IndexingColumns) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  for (size_t column = 0; column < kColumns; ++column) {
    Vector<double> column_vector = matrix_d0.column(column+1);
    for (size_t row = 0; row < kRows; ++row) {
      EXPECT_EQ(column_vector[row], kDoubleData[row*kColumns + column]);
    }
  }

  // empty matrix
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3;
    matrix_d3.column(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // column out of bounds
  caught_exception = false;
  try {
    matrix_d0.column(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  for (size_t column = 0; column < kColumns; ++column) {
    Vector<complex> column_vector = matrix_c0.column(column+1);
    for (size_t row = 0; row < kRows; ++row) {
      EXPECT_EQ(column_vector[row], kComplexData[row*kColumns + column]);
    }
  }

  // empty matrix
  caught_exception = false;
  try {
    Matrix<double> matrix_c3;
    matrix_c3.column(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // column out of bounds
  caught_exception = false;
  try {
    matrix_c0.column(20);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, Submatrix) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  // union of the last two kRows and middle four kColumns
  Matrix<double> submatrix_d0 = matrix_d0.submatrix(
    kSubmatrixStartRow,    kSubmatrixRows,
    kSubmatrixStartColumn, kSubmatrixColumns);
  ASSERT_EQ((size_t) kSubmatrixRows,     submatrix_d0.number_of_rows());
  ASSERT_EQ((size_t) kSubmatrixColumns,  submatrix_d0.number_of_columns());
  int index;
  for (size_t row = 1; row <= kSubmatrixRows; ++row) {
    for (size_t column = 1; column <= kSubmatrixColumns; ++column) {
      index = (kSubmatrixStartRow+row-2)*kColumns
            + kSubmatrixStartColumn+column-2;
      EXPECT_EQ(kDoubleData[index], submatrix_d0(row, column));
    }
  }

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  // union of the last two kRows and middle four kColumns
  Matrix<complex> submatrix_c0 = matrix_c0.submatrix(
    kSubmatrixStartRow,    kSubmatrixRows,
    kSubmatrixStartColumn, kSubmatrixColumns);
  ASSERT_EQ(submatrix_c0.number_of_rows(),     (size_t) kSubmatrixRows);
  ASSERT_EQ(submatrix_c0.number_of_columns(),  (size_t) kSubmatrixColumns);
  for (size_t row = 1; row <= kSubmatrixRows; ++row) {
    for (size_t column = 1; column <= kSubmatrixColumns; ++column) {
      index = (kSubmatrixStartRow+row-2)*kColumns
            + kSubmatrixStartColumn+column-2;
      EXPECT_TRUE(::equal(submatrix_c0(row, column), kComplexData[index]));
    }
  }
}

TEST_F(MatrixTest, Comparison) {
  // *** double equality ***

  // self-equality
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  EXPECT_TRUE(matrix_d0 == matrix_d0);

  // identically prepared equality
  Matrix<double> matrix_d1(kRows, kColumns, kDoubleData);
  EXPECT_TRUE(matrix_d0 == matrix_d1);

  // empty equality
  Matrix<double> matrix_d2;
  Matrix<double> matrix_d3;
  EXPECT_TRUE(matrix_d2 == matrix_d3);

  // *** double inequality ***

  // different sizes
  Matrix<double> matrix_d4 = matrix_d0.submatrix(1, 1, kRows-1, kColumns-1);
  EXPECT_TRUE(matrix_d0 != matrix_d4);
  EXPECT_FALSE(matrix_d0 == matrix_d4);

  // empty/non-empty
  EXPECT_TRUE(matrix_d2 != matrix_d0);
  EXPECT_FALSE(matrix_d2 == matrix_d0);

  // same size, different elements
  Matrix<double> matrix_d5(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_d6(kRows, kColumns);
  for (size_t row = 1; row <= kSubmatrixRows; ++row) {
    for (size_t column = 1; column <= kSubmatrixColumns; ++column) {
      matrix_d6(row, column) = static_cast<double> (row*column);
    }
  }
  EXPECT_TRUE(matrix_d5 != matrix_d6);
  EXPECT_FALSE(matrix_d5 == matrix_d6);

  // *** complex equality ***

  // self-equality
  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  EXPECT_TRUE(matrix_c0 == matrix_c0);

  // identically prepared equality
  Matrix<complex> matrix_c1(kRows, kColumns, kComplexData);
  EXPECT_TRUE(matrix_c0 == matrix_c1);

  // empty equality
  Matrix<complex> matrix_c2;
  Matrix<complex> matrix_c3;
  EXPECT_TRUE(matrix_c2 == matrix_c3);

  // *** complex inequality ***

  // different sizes
  Matrix<complex> matrix_c4
    = matrix_c0.submatrix(1, 1, kRows-1, kColumns-1);
  EXPECT_TRUE(matrix_c0 != matrix_c4);
  EXPECT_FALSE(matrix_c0 == matrix_c4);

  // empty/non-empty
  EXPECT_TRUE(matrix_c2 != matrix_c0);
  EXPECT_FALSE(matrix_c2 == matrix_c0);

  // same size, different elements
  Matrix<complex> matrix_c5(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_c6(kRows, kColumns);
  for (size_t row = 1; row <= kSubmatrixRows; ++row) {
    for (size_t column = 1; column <= kSubmatrixColumns; ++column) {
      matrix_c6(row, column) = MAUS::Complex::complex(row*column);
    }
  }
  EXPECT_TRUE(matrix_c5 != matrix_c6);
  EXPECT_FALSE(matrix_c5 == matrix_c6);
}

TEST_F(MatrixTest, Assignment) {
  // *** double assignment ***

  // plain vanilla assignment
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_d1(kRows, kColumns);
  matrix_d1 = matrix_d0;
  EXPECT_TRUE(equal(matrix_d0, matrix_d1));

  // check special assignement to null matrix
  Matrix<double> matrix_d2;
  matrix_d2 = matrix_d0;
  EXPECT_TRUE(equal(matrix_d2, matrix_d0));

  // check bad assignment to differently sized matrix
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3(1, 12);
    matrix_d3 = matrix_d0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // *** complex assignment ***

  // plain vanilla assignment
  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_c1(kRows, kColumns);
  matrix_c1 = matrix_c0;
  EXPECT_TRUE(equal(matrix_c0, matrix_c1));

  // check special assignement to null matrix
  Matrix<complex> matrix_c2;
  matrix_c2 = matrix_c0;
  EXPECT_TRUE(equal(matrix_c2, matrix_c0));

  // check bad assignment to differently sized matrix
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(1, 12);
    matrix_c3 = matrix_c0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, Addition) {
  // add and assign
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_d1(kRows, kColumns);
  matrix_d1 += matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d1));

  // add
  Matrix<double> matrix_d2 = matrix_d0 + matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, kDoubleDatadoubled_));

  // bad addition of differently sized matrices
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3(kRows+1, kColumns);
    matrix_d3 += matrix_d0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<double> matrix_d3(kRows, kColumns+1);
    matrix_d3 += matrix_d0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // add and assign
  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_c1(kRows, kColumns);
  matrix_c1 += matrix_c0;
  ASSERT_TRUE(equal(matrix_c0, matrix_c1));

  // add
  Matrix<complex> matrix_c2 = matrix_c0 + matrix_c1;
  EXPECT_TRUE(equal(matrix_c2, kComplexDatadoubled_));

  // bad addition of differently sized matrices
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(kRows+1, kColumns);
    matrix_c3 += matrix_c0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(kRows, kColumns+1);
    matrix_c3 += matrix_c0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, Subtraction) {
  // subtract and assign
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_d1(kRows, kColumns, kDoubleDatadoubled_);
  Matrix<double> matrix_d2(matrix_d1);
  matrix_d2 -= matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d2));

  // subtract
  Matrix<double> matrix_d3 = matrix_d1 - matrix_d0;
  EXPECT_TRUE(equal(matrix_d3, kDoubleData));

  // bad subtraction of differently sized matrices
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3(kRows+1, kColumns);
    matrix_d3 -= matrix_d0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<double> matrix_d3(kRows, kColumns+1);
    matrix_d3 -= matrix_d0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // subtract and assign
  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_c1(kRows, kColumns, kComplexDatadoubled_);
  Matrix<complex> matrix_c2(matrix_c1);
  matrix_c2 -= matrix_c0;
  ASSERT_TRUE(equal(matrix_c0, matrix_c2));

  // subtract
  Matrix<complex> matrix_c3 = matrix_c1 - matrix_c0;
  EXPECT_TRUE(equal(matrix_c3, kComplexData));

  // bad subtraction of differently sized matrices
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(kRows+1, kColumns);
    matrix_c3 -= matrix_c0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(kRows, kColumns+1);
    matrix_c3 -= matrix_c0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, Multiplication) {
  // multiple and assign
  double d1[6] = {1, 2, 3, 7, 6, 5};
  double d2[6] = {3, 4, 5, 2, 8, 3};
  const Matrix<double>  matrix_d0(3, 2, d1);
  const Matrix<double>  matrix_d1(2, 3, d2);
  Matrix<double>        matrix_d2(3, 3);
  for (int i = 1; i <= 3; i++)
    for (int j = 1; j <= 3; j++)
      for (int k = 1; k <= 2; k++)
        matrix_d2(i, j) += matrix_d0(i, k)*matrix_d1(k, j);
  Matrix<double> matrix_d3(matrix_d0);
  matrix_d3 *= matrix_d1;
  ASSERT_TRUE(equal(matrix_d2, matrix_d3));

  // multiply
  Matrix<double> matrix_d4 = matrix_d0 * matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, matrix_d4));

  // bad multiplication of differently sized matrices
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3(kColumns, kRows+1);
    matrix_d3 *= matrix_d0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // multiply and assign
  complex c1[6];
  complex c2[6];
  for (int i = 0; i < 6; i++) {
    c1[i] = MAUS::Complex::complex(d1[i],       d2[i]);
  }
  for (int i = 0; i < 6; i++) {
    c2[i] = MAUS::Complex::complex(-d2[i]*d1[i], d1[i]);
  }
  const Matrix<complex> matrix_c0(3, 2, c1);
  const Matrix<complex> matrix_c1(2, 3, c2);
  Matrix<complex>       matrix_c2(3, 3);
  for (int i = 1; i <= 3; i++)
    for (int j = 1; j <= 3; j++)
      for (int k = 1; k <= 2; k++)
        matrix_c2(i, j) += matrix_c0(i, k) * matrix_c1(k, j);
  Matrix<complex> matrix_c3(matrix_c0);
  matrix_c3 *= matrix_c1;
  ASSERT_TRUE(equal(matrix_c2, matrix_c3));

  // multiply
  Matrix<complex> matrix_c4 = matrix_c0 * matrix_c1;
  EXPECT_TRUE(equal(matrix_c2, matrix_c4));

  // bad multiplication of differently sized matrices
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(kColumns, kRows+1);
    matrix_c3 *= matrix_c0;
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, ScalarMultiplication) {
  // *** double multiplication ***

  // multiply and assign
  const Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_d1(matrix_d0);
  matrix_d1 *= 2.;
  ASSERT_TRUE(equal(matrix_d1, kDoubleDatadoubled_));

  // multiplication on the right by a scalar
  Matrix<double> matrix_d2 = matrix_d0 * 2.;
  ASSERT_TRUE(equal(matrix_d2, kDoubleDatadoubled_));

  // multiplication on the left by a scalar
  Matrix<double> matrix_d3 = 2. * matrix_d0;
  ASSERT_TRUE(equal(matrix_d3, kDoubleDatadoubled_));

  // *** double multiplication ***

  // multiply and assign
  const Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_c1(matrix_c0);
  matrix_c1 *= kComplexScalingFactor;
  ASSERT_TRUE(equal(matrix_c1, kComplexDatamultiple_));

  // multiplication on the right by a scalar
  Matrix<complex> matrix_c2 = matrix_c0 * kComplexScalingFactor;
  ASSERT_TRUE(equal(matrix_c2, kComplexDatamultiple_));

  // multiplication on the left by a scalar
  Matrix<complex> matrix_c3 = kComplexScalingFactor * matrix_c0;
  ASSERT_TRUE(equal(matrix_c3, kComplexDatamultiple_));
}

TEST_F(MatrixTest, ScalarDivision) {
  const Matrix<double> matrix_d0(kRows, kColumns, kDoubleDatadoubled_);
  Matrix<double> matrix_d1(matrix_d0);
  matrix_d1 /= 2.;
  ASSERT_TRUE(equal(matrix_d1, kDoubleData));

  Matrix<double> matrix_d2 = matrix_d0 / 2.;
  ASSERT_TRUE(equal(matrix_d2, kDoubleData));

  const Matrix<complex> matrix_c0(kRows, kColumns, kComplexDatamultiple_);
  Matrix<complex> matrix_c1(matrix_c0);
  matrix_c1 /= kComplexScalingFactor;
  ASSERT_TRUE(equal(matrix_c1, kComplexData));

  Matrix<complex> matrix_c2 = matrix_c0 / kComplexScalingFactor;
  ASSERT_TRUE(equal(matrix_c2, kComplexData));
}

TEST_F(MatrixTest, ComplexComposition) {
  // test construction of a complex matrix from one double matrix (real part)
  const Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  const Matrix<double> matrix_real(kRows, kColumns, kDoubleData);
  Matrix<complex> matrix_c1(kRows, kColumns);
  for (size_t row = 1; row <= kRows; ++row) {
    for (size_t column = 1; column <= kColumns; ++column) {
      matrix_c1(row, column)
        = MAUS::Complex::complex(kDoubleData[(row-1)*kColumns+(column-1)]);
    }
  }
  Matrix<complex> matrix_c2 = MAUS::Complex::complex(matrix_real);
  ASSERT_TRUE(equal(matrix_c2, matrix_c1));
  const Matrix<complex> matrix_c3(matrix_real);
  ASSERT_TRUE(equal(matrix_c3, matrix_c1));

  // test construction of a complex matrix from two double matrices (real, imag)
  double kDoubleDatareversed[kDataSize];
  for (size_t index = 0; index < kDataSize; ++index) {
    kDoubleDatareversed[kDataSize-index-1] = kDoubleData[index];
  }
  const Matrix<double> matrix_imag(kRows, kColumns, kDoubleDatareversed);
  Matrix<complex> matrix_c4 = MAUS::Complex::complex(matrix_real, matrix_imag);
  ASSERT_TRUE(equal(matrix_c4, matrix_c0));
  Matrix<complex> matrix_c5(matrix_real, matrix_imag);
  ASSERT_TRUE(equal(matrix_c5, matrix_c0));

  // bad use of two differently sized real matrices
  bool caught_exception = false;
  Matrix<double> matrix_real_bad_rows(kRows+1, kColumns);
  try {
    Matrix<complex> matrix_c6
      = MAUS::Complex::complex(matrix_real_bad_rows, matrix_imag);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  Matrix<double> matrix_real_bad_columns(kRows, kColumns+1);
  try {
    Matrix<complex> matrix_c6
      = MAUS::Complex::complex(matrix_real_bad_columns, matrix_imag);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, ComplexDecomposition) {
  const Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  const Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);

  // the real part of kComplexData is just kDoubleData
  Matrix<double> matrix_real = real(matrix_c0);
  EXPECT_TRUE(equal(matrix_real, matrix_d0));

  // subtract off the imaginary part of kComplexData
  // which is again just kDoubleData
  Matrix<double> matrix_imag = imag(matrix_c0);
  // create a pure-imaginary matrix
  Matrix<double> zero_matrix(kRows, kColumns);
  Matrix<complex> matrix_pure_imag
    = MAUS::Complex::complex(zero_matrix, matrix_imag);
  Matrix<complex> matrix_c1 = matrix_c0 - matrix_pure_imag;
  Matrix<complex> matrix_c2 = MAUS::Complex::complex(matrix_d0);
  EXPECT_TRUE(equal(matrix_c1, matrix_c2));
}

TEST_F(MatrixTest, ComplexConjugation) {
  const Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  const Matrix<double> matrix_d0(kRows, kColumns, kDoubleDatadoubled_);
  Matrix<double> matrix_d1 = real(matrix_c0 + conj(matrix_c0));
  ASSERT_TRUE(equal(matrix_d1, matrix_d0));
}

TEST_F(MatrixTest, Determinant) {
  // double matrix determinant
  Matrix<double> matrix_d1(kSize, kSize, kDoubleData);
  for (size_t i = 1; i <= 2; ++i) {
    for (size_t j = 3; j <= 4; ++j) {
      matrix_d1(i, j) = 0.;
      matrix_d1(j, i) = 0.;
    }
  }
  // det is equal to det(upper block diag)*det(lower block diag)
  double upper_block_diag_det = matrix_d1(1, 1)*matrix_d1(2, 2)
                              - matrix_d1(1, 2)*matrix_d1(2, 1);
  double lower_block_diag_det = matrix_d1(3, 3)*matrix_d1(4, 4)
                              -matrix_d1(3, 4)*matrix_d1(4, 3);
  EXPECT_TRUE(::equal(upper_block_diag_det * lower_block_diag_det,
              determinant(matrix_d1)));

  // if 2 rows are identical, det is 0
  Matrix<double> matrix_d2(kSize, kSize, kDoubleData);
  for (size_t column = 1; column <= kSize; ++column) {
    matrix_d2(2, column) = matrix_d2(1, column);
  }
  EXPECT_TRUE(::equal(0., determinant(matrix_d2)));

  // bad non-square matrix determinant
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d3(kRows, kColumns);
    determinant(matrix_d3);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // complex matrix determinant
  Matrix<complex> matrix_c1(kSize, kSize, kComplexData);
  for (size_t i = 1; i <= 2; ++i) {
    for (size_t j = 3; j <= 4; ++j) {
      matrix_c1(i, j) = MAUS::Complex::complex(0.0, 0.0);
      matrix_c1(j, i) = MAUS::Complex::complex(0.0, 0.0);
    }
  }
  // det is equal to det(upper block diag)*det(lower block diag)
  MAUS::complex upper_block_diag_det_c = matrix_c1(1, 1)*matrix_c1(2, 2)
                                       - matrix_c1(1, 2)*matrix_c1(2, 1);
  MAUS::complex lower_block_diag_det_c = matrix_c1(3, 3)*matrix_c1(4, 4)
                                       - matrix_c1(3, 4)*matrix_c1(4, 3);
  EXPECT_TRUE(::equal(upper_block_diag_det_c * lower_block_diag_det_c,
              determinant(matrix_c1)));

  // if 2 rows are identical, det is 0
  Matrix<complex> matrix_c2(kSize, kSize, kComplexData);
  for (size_t column = 1; column <= kSize; ++column) {
    matrix_c2(2, column) = matrix_c2(1, column);
  }
  EXPECT_TRUE(::equal(MAUS::Complex::complex(0.0, 0.0),
                      determinant(matrix_c2)));

  // bad non-square matrix determinant
  caught_exception = false;
  try {
    Matrix<complex> matrix_c3(kRows, kColumns);
    determinant(matrix_c3);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, Inverse) {
  // M^-1 * M = Identity (double)
  Matrix<double> matrix_d1(kSize, kSize, kDoubleData);
  Matrix<double> matrix_d2 = inverse(matrix_d1);
  Matrix<double> matrix_d3 = matrix_d1 * matrix_d2;
  for (size_t row = 1; row <= kSize; ++row) {
    for (size_t column = 1; column <= kSize; ++column) {
      EXPECT_TRUE(::equal(row == column ? 1. : 0., matrix_d3(row, column)));
    }
  }

  // bad non-square matrix inverse
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d4(kRows, kColumns);
    inverse(matrix_d4);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // non-invertible matrix (infinite elements)
  caught_exception = false;
  try {
    const double bad_array[] = {
      std::numeric_limits<double>::infinity(),  1,
      1,                                        1};
    Matrix<double> matrix_d4(2, 2, bad_array);
    inverse(matrix_d4);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);

  // non-invertible matrix (NaN elements) -- not sure how to do this
}

TEST_F(MatrixTest, Trace) {
  Matrix<double> matrix_d1(kSize, kSize, kDoubleData);
  double tr = trace(matrix_d1);
  for (size_t i = 1; i <= kSize; i++) {
    tr -= matrix_d1(i, i);
  }
  EXPECT_DOUBLE_EQ(tr, 0.);
}

TEST_F(MatrixTest, Transpose) {
  // verify the transpose of a real-valued matrix
  Matrix<double> matrix_da(kSize, kSize, kDoubleData);
  Matrix<double> matrix_dt = transpose(matrix_da);
  ASSERT_EQ(matrix_da.number_of_rows(), matrix_dt.number_of_columns());
  ASSERT_EQ(matrix_da.number_of_columns(), matrix_dt.number_of_rows());
  for (size_t i = 1; i <= matrix_da.number_of_rows(); ++i)
    for (size_t j = 1; j <= matrix_da.number_of_columns(); ++j)
      EXPECT_EQ(matrix_da(i, j), matrix_dt(j, i));

  // verify the creation of a real-valued, row-vector matrix
  Vector<double> column_vector(kDoubleData, 6);
  Matrix<double> row_vector = transpose(column_vector);
  for (size_t j = 1; j <= 6; ++j) {
    EXPECT_EQ(row_vector(1, j), column_vector(j));
  }

  // verify the transpose of a complex-valued matrix
  Matrix<complex> matrix_ca(kSize, kSize, kComplexData);
  Matrix<complex> matrix_ct = transpose(matrix_ca);
  ASSERT_EQ(matrix_ca.number_of_rows(), matrix_ct.number_of_columns());
  ASSERT_EQ(matrix_ca.number_of_columns(), matrix_ct.number_of_rows());
  for (size_t i = 1; i <= matrix_ca.number_of_rows(); ++i)
    for (size_t j = 1; j <= matrix_ca.number_of_columns(); ++j)
      EXPECT_EQ(matrix_ca(i, j), matrix_ct(j, i));
}

TEST_F(MatrixTest, Dagger) {
  complex herm_data[4] = {
    MAUS::Complex::complex(12.1, 0.),      MAUS::Complex::complex(98.6, 100.0),
    MAUS::Complex::complex(98.6, -100.0),  MAUS::Complex::complex(22.4, 0.)
  };
  Matrix<complex> matrix(2, 2, herm_data);
  Matrix<complex> matrix_dagger = dagger(matrix);
  ASSERT_TRUE(equal(matrix_dagger, matrix));

  // verify the creation of a complex-valued, row-vector matrix
  Vector<complex> column_vector(kComplexData, 6);
  Matrix<complex> row_vector = dagger(column_vector);
  for (size_t j = 1; j <= 6; ++j) {
    EXPECT_EQ(row_vector(1, j), MAUS::conj(column_vector(j)));
  }
}

TEST_F(MatrixTest, HepMatrix) {
  ::CLHEP::HepRandom hep_random;
  const Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  ::CLHEP::HepMatrix matrix_hep0 = MAUS::CLHEP::HepMatrix(matrix_d0);
  ASSERT_EQ((size_t) matrix_hep0.num_row(), matrix_d0.number_of_rows());
  ASSERT_EQ((size_t) matrix_hep0.num_col(), matrix_d0.number_of_columns());
  for (size_t row = 1; row <= kRows; ++row) {
    for (size_t column = 1; column <= kColumns; ++column) {
      EXPECT_TRUE(::equal(matrix_hep0(row, column), matrix_d0(row, column)));
    }
  }
}

TEST_F(MatrixTest, Eigen) {
  // verify the eigensystem equation M*E = e*E
  Matrix<double> matrix_d0(kSize, kSize, kDoubleData);
  std::pair< Vector<complex>, Matrix<complex> > eigensys
    = eigensystem(matrix_d0);
  Vector<complex> eigenvals = eigensys.first;
  Matrix<complex> eigenvectors = eigensys.second;
  ASSERT_EQ(eigenvals.size(), kSize);
  ASSERT_EQ(eigenvectors.number_of_columns(), kSize);
  for (size_t i = 1; i < eigenvals.size(); ++i) {
    complex eigenvalue = eigenvals(i);
    Vector<complex> eigenvector = eigenvectors.column(i);
    EXPECT_TRUE(equal(MAUS::Complex::complex(matrix_d0) * eigenvector,
                      eigenvalue * eigenvector));
  }

  // verify that just getting the eigenvalues returns the same set from
  // eigensystem() (but perhaps in a different order)
  Vector<complex> eigenvals2 = eigenvalues(matrix_d0);
  ASSERT_EQ(eigenvals2.size(), kSize);
  for (size_t i = 1; i <= kSize; ++i) {
    bool found_match = false;
    for (size_t j = 1; j <= kSize; j++) {
      if (::equal(eigenvals2(i), eigenvals(j))) {
        found_match = true;
      }
    }
    EXPECT_TRUE(found_match);
  }

  // bad non-square matrix eigensystem
  bool caught_exception = false;
  try {
    Matrix<double> matrix_d4(kRows, kColumns);
    eigenvalues(matrix_d4);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
  caught_exception = false;
  try {
    Matrix<double> matrix_d4(kRows, kColumns);
    eigensystem(matrix_d4);
  } catch (MAUS::Exceptions::Exception exc) {
    caught_exception = true;
  }
  EXPECT_TRUE(caught_exception);
}

TEST_F(MatrixTest, Negation) {
  Matrix<double> matrix_d0(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_d1(-matrix_d0);
  for (size_t row = 1; row <= kRows; ++row) {
    for (size_t column = 1; column <= kColumns; ++column) {
      EXPECT_TRUE(::equal(matrix_d0(row, column), -matrix_d1(row, column)));
    }
  }

  Matrix<complex> matrix_c0(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_c1(-matrix_c0);
  for (size_t row = 1; row <= kRows; ++row) {
    for (size_t column = 1; column <= kColumns; ++column) {
      EXPECT_TRUE(::equal(matrix_c0(row, column), -matrix_c1(row, column)));
    }
  }
}

TEST_F(MatrixTest, VectorMultiplication) {
  // reverse the order of a vector's elements using a matrix
  Matrix<double> matrix_d0(kSize, kSize, kDoubleData);
  Matrix<double> matrix_reverse_d(kSize, kSize);
  for (size_t i = 1; i <= kSize; ++i) {
    matrix_reverse_d(i, kSize-i+1) = 1.;
  }
  Vector<double> vector_d0 = matrix_d0.row(1);
  Vector<double> vector_product_d = matrix_reverse_d * vector_d0;
  for (size_t i = 1; i <= kSize; ++i) {
    EXPECT_TRUE(::equal(vector_product_d(i), vector_d0(kSize-i+1)));
  }

  // reverse the order of a vector's elements using a matrix
  Matrix<complex> matrix_c0(kSize, kSize, kComplexData);
  Matrix<complex> matrix_reverse_c(kSize, kSize);
  for (size_t i = 1; i <= kSize; ++i) {
    matrix_reverse_c(i, kSize-i+1) = MAUS::Complex::complex(1., 0.);
  }
  Vector<complex> vector_c0 = matrix_c0.row(1);
  Vector<complex> vector_product_c = matrix_reverse_c * vector_c0;
  for (size_t i = 1; i <= kSize; ++i) {
    EXPECT_TRUE(::equal(vector_product_c(i), vector_c0(kSize-i+1)));
  }
}

TEST_F(MatrixTest, Streaming) {
  std::stringstream double_stream;
  Matrix<double> matrix_da(kRows, kColumns, kDoubleData);
  Matrix<double> matrix_db;
  double_stream << matrix_da;
  double_stream >> matrix_db;
  EXPECT_TRUE(equal(matrix_da, matrix_db));

  std::stringstream complex_stream;
  Matrix<complex> matrix_ca(kRows, kColumns, kComplexData);
  Matrix<complex> matrix_cb;
  complex_stream << matrix_ca;
  complex_stream >> matrix_cb;
  EXPECT_TRUE(equal(matrix_ca, matrix_cb));
}
