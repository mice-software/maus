// Copyright 2010 Chris Rogers
//
//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//G4MICE is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with G4MICE in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

//Issues:
// * No ASSERT_EQ(size_t, size_t) - gives compiler warning for overload size_t to int
// * 

#include <limits>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Interface/Complex.hh"
#include "Interface/Matrix.hh"
#include "Interface/Vector.hh"

#include "gtest/gtest.h" 
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Random/Random.h"

using namespace MAUS;
namespace MAUS { using namespace Complex; }

//Defined in ComplexTest.cc
bool equal(const MAUS::complex c1, const MAUS::complex c2);
bool equal(double c1, double c2);

class MatrixTest : public ::testing::Test
{
protected:
  template <typename Tmplt>
  void elements_equal(Matrix<Tmplt>& matrix, Tmplt value)
  {
    bool are_equal = true;
    if(   matrix.number_of_rows()    != rows_
       || matrix.number_of_columns() != columns_) 
    {
      return false;
    }
    for(size_t row=1; row<rows; ++row) 
      for(size_t column=1; column<columns; ++column) 
        are_equal &= equal(matrix(row,column), value);
    return are_equal;
  }

  template <typename Tmplt>
  void equal(Matrix<Tmplt>& matrix, Tmplt* array)
  {
    bool are_equal = true;
    if(   matrix.number_of_rows()    != rows_
       || matrix.number_of_columns() != columns_) 
    {
      return false;
    }
    for(size_t row=1; row<rows; ++row) 
      for(size_t column=1; column<columns; ++column) 
        are_equal &= equal(matrix(row,column), array[(row-1)*columns_+column]);
    return are_equal;
  }


  template <typename Tmplt>
  bool equal(Matrix<Tmplt> m1, Matrix<Tmplt> m2)
  { 
    bool are_equal = true;
    if(   m1.number_of_rows()    != m2.number_of_rows()
       || m1.number_of_columns() != m2.number_of_columns()) 
    {
      return false;
    }
    for(size_t i=1; i<m1.number_of_rows(); i++) 
      for(size_t j=1; j<m1.number_of_columns(); j++) 
        are_equal &= equal(m1(i,j), m2(i,j));
    return are_equal;
  }

  template <typename Tmplt>
  bool equal(Vector<Tmplt> v1, Vector<Tmplt> v2)
  { 
    bool are_equal = true;
    if(v1.size() != v2.size() )
    {
      return false;
    }
    for(size_t i=1; i<v1.number_of_rows(); i++) 
      are_equal &= equal(v1(i), v2(i));
    return are_equal;
  }

  static const size_t data_size_;
  static const double double_data_[24];
  static const complex complex_data_[24];
  static const size_t size_; //square matrix
  static const size_t rows_;
  static const size_t columns_;
  static const size_t submatrix_start_row_;
  static const size_t submatrix_rows_;
  static const size_t submatrix_start_column_;
  static const size_t submatrix_columns_;
};

template bool MatrixTest::elements_equal(Matrix<double> matrix, double value);
template bool MatrixTest::elements_equal(Matrix<complex> matrix, complex value);
template bool MatrixTest::equal(Matrix<double> matrix, double* array);
template bool MatrixTest::equal(Matrix<complex> matrix, complex* array);
template bool MatrixTest::equal(Matrix<double> m1, Matrix<double> m2);
template bool MatrixTest::equal(Matrix<complex> m1, Matrix<complex> m2);
template bool MatrixTest::equal(Vector<double> v1, Vector<double> v2);
template bool MatrixTest::equal(Vector<complex> v1, Vector<complex> v2);


//****************************************
//MatrixTest static const initializations
//****************************************

const double MatrixTest::double_data_[24] = {
  0.,   1.,   2.,   3.,     -5.,    -8.,    -13.,   -21.,
  34.5, 55.7, 89.3, 144.2,  -32.5,  -57.5,  -91.2,  -146.3,
  3.14, -1.59 2.65, -3.58,  9.79,   -3.23,  8.46,   -2.64
};

const complex MatrixTest::complex_data_[24] = {
  complex(0., -2.64),     complex(1., 8.46),      complex(2., -3.23),
  complex(3., 9.79),      complex(-5., -3.58),    complex(-8., 2.65),
  complex(-13., -1.59),   complex(-21, 3.14),

  complex(34.5, -146.3),  complex(55.7, -91.2),   complex(89.3, -57.5),
  complex(144.2, -32.5),  complex(-32.5, 144.2),  complex(-57.5, 89.3),
  complex(-91.2, 55.7),   complex(-146.3, 34.5)
  
  complex(3.14, -21.),    complex(-1.59, -13.),   complex(2.65, -8.),
  complex(-3.58, -5.),    complex(9.79, 3.),      complex(-3.23, 2.),
  complex(8.46, 1.),      complex(-2.64, 0.)
};

const size_t MatrixTest::data_size_ = 24;
const size_t MatrixTest::size_      = 4;
const size_t MatrixTest::rows_      = 3;
const size_t MatrixTest::columns_   = 8;
const size_t MatrixTest::submatrix_rows_         = 2;
const size_t MatrixTest::submatrix_columns_      = 4;
const size_t MatrixTest::submatrix_start_row_    = 2;
const size_t MatrixTest::submatrix_start_column_ = 3;

//***********
//test cases
//***********

TEST_F(MatrixTest, DefaultConstructor) {
  Matrix<double> matrix_d0;
  ASSERT_EQ(matrix_d0.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_d0.number_of_columns(),  (size_t) 0);
  
  Matrix<complex> matrix_c0;
  ASSERT_EQ(matrix_c0.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_c0.number_of_columns(),  (size_t) 0);
}

TEST_F(MatrixTest, ConstructorFromArray) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  EXPECT_TRUE(equal(matrix_d0, double_data_));

  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  EXPECT_TRUE(equal(matrix_c0, complex_data_));
}

TEST_F(MatrixTest, IndexingElements) {
  Matrix<double> matrix_d1(rows_, columns_, double_data_);
  const Matrix<double> matrix_d2(rows_, columns_, double_data_);
  for(size_t i=1; i<=rows_; ++i)
  {
    for(size_t j=0; j<columns_; ++j)
    {
      //check that it returns correct value
      EXPECT_EQ(matrix_d1(i+1,j+1), double_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_d1[i][j], double_data_[i*columns_ + j]);
      //check that it works with const matrix
      EXPECT_EQ(matrix_d2(i+1,j+1), double_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_d2[i][j], double_data_[i*columns_ + j]);

      //check that it can set the correct value
      matrix_d1(i+1,j+1) = 4.;
      EXPECT_EQ(matrix_d1(i+1,j+1), 4.);
      matrix_d1[i][j] = -6.;
      EXPECT_EQ(matrix_d1[i][j], -6.);
    }
  
  Matrix<complex> matrix_c1(rows_, columns_, complex_data_);
  //check that it works for const as well
  const Matrix<complex> matrix_c2(rows_, columns_, complex_data_);
  complex new_value1 = complex(4., -2.);
  complex new_value2 = complex(-6., 15.);
  for(size_t i=1; i<=rows_; ++i)
  {
    for(size_t j=0; j<columns_; ++j)
    {
      //check that it returns correct value
      EXPECT_EQ(matrix_c1(i+1,j+1), complex_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_c1[i][j], complex_data_[i*columns_ + j]);
      //check that it works with const matrix
      EXPECT_EQ(matrix_c2(i+1,j+1), complex_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_c2[i][j], complex_data_[i*columns_ + j]);

      //check that it can set the correct value
      matrix_c1(i+1,j+1) = new_value1;
      EXPECT_EQ(matrix_c1(i+1,j+1), new_value1);
      matrix_c1[i][j] = new_value2;
      EXPECT_EQ(matrix_c1[i][j], new_value2);
    }
  }
}

TEST_F(MatrixTest, CopyConstructor) {
  //should work if matrix is const
  const Matrix<double> matrix_d0(rows_, columns_, double_data_);
  const Matrix<double> matrix_d1(matrix_d0);
  EXPECT_TRUE(equal(matrix_d0, matrix_d1);
  
  //check that handles 0 length okay
  Matrix<double> matrix_d2;
  Matrix<double> matrix_d3(matrix_d2);
  EXPECT_TRUE(equal(matrix_d2, matrix_d3);
  
  //should work if matrix is const
  const Matrix<complex> matrix_c0(complex, rows_, columns_);
  const Matrix<complex> matrix_c1(matrix_c0);
  EXPECT_TRUE(equal(matrix_c0, matrix_c1);
  
  //check that handles 0 length okay
  Matrix<complex> matrix_c2;
  Matrix<complex> matrix_c3(matrix_c2);
  EXPECT_TRUE(equal(matrix_c2, matrix_c3);
}

TEST_F(MatrixTest, HepMatrixConstructor) {
  HepRandom hep_random;
  const HepMatrix matrix_hep0(rows_, columns_, hep_random);
  const Matrix<double> matrix_d0(hep_matrix);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) rows_);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) columns_);
  for(size_t i=1; i<=rows_; ++i)
    for(size_t j=1; j<=columns_; ++j)
      EXPECT_EQ(matrix_d0(i,j), matrix_hep0(i,j));

  //check that handles 0 length okay
  HepMatrix matrix_hep1;
  Matrix<double> matrix_d1(matrix_hep1);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) 0);

  const Matrix<complex> matrix_c0(hep_matrix);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) rows_);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) columns_);
  for(size_t i=1; i<=rows_; ++i)
    for(size_t j=1; j<=columns_; ++j)
      EXPECT_EQ(matrix_d0(i,j), complex(matrix_hep0(i,j)));
  
  //check that handles 0 length okay
  Matrix<complex> matrix_d1(matrix_hep1);
  ASSERT_EQ(matrix_c1.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_c1.number_of_columns(),  (size_t) 0);
}

TEST_F(MatrixTest, ConstructorSizeOnly) {
  Matrix<double> matrix_d0(rows_, columns_);
  EXPECT_TRUE(elements_equal(matrix_d0, 0.);
  
  Matrix<complex> matrix_c0(rows_, columns_);
  EXPECT_TRUE(elements_equal(matrix_c0, complex(0.0));
}

TEST_F(MatrixTest, ConstructorFill) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_[4]);
  EXPECT_TRUE(elements_equal(matrix_d0, double_data_[4]);
  
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_[4]);
  EXPECT_TRUE(elements_equal(matrix_c0, complex_data_[4]);
}

TEST_F(MatrixTest, IndexingRows) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  for (size_t row=0; row<rows_; ++row)
  {
    Vector<double> row_vector = matrix_d0.row(row);
    for (size_t column=0; column<columns_; ++column)
    {
      EXPECT_EQ(row_vector[column], double_data_[row*columns_ + column]);
    }
  }

  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  for (size_t row=0; row<rows_; ++row)
  {
    Vector<complex> row_vector = matrix_c0.row(row);
    for (size_t column=0; column<columns_; ++column)
    {
      EXPECT_EQ(row_vector[column], complex_data_[row*columns_ + column]);
    }
  }
}

TEST_F(MatrixTest, IndexingColumns) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  for (size_t column=0; column<columns_; ++column)
  {
    Vector<double> column_vector = matrix_d0.column(column);
    for (size_t row=0; row<rows_; ++row)
    {
      EXPECT_EQ(column_vector[row], double_data_[row*columns_ + column]);
    }
  }
  
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  for (size_t column=0; column<columns_; ++column)
  {
    Vector<complex> column_vector = matrix_c0.column(column);
    for (size_t row=0; row<rows_; ++row)
    {
      EXPECT_EQ(column_vector[row], complex_data_[row*columns_ + column]);
    }
  }
}

TEST_F(MatrixTest, Submatrix) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  //union of the last two rows_ and middle four columns_
  Matrix<double> submatrix = matrix_d0.submatrix(
    submatrix_start_row_,    submatrix_rows_,
    submatrix_start_column_, submatrix_columns_);
  ASSERT_EQ(submatrix.number_of_rows(),     (size_t) submatrix_rows_);
  ASSERT_EQ(submatrix.number_of_columns(),  (size_t) submatrix_columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      EXPECT_EQ(
        submatrix(row,column),
        double_data_[  (row+submatrix_start_row-1)*columns_
                     + column+submatrix_start_column]);
    }
  }

  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  //union of the last two rows_ and middle four columns_
  Matrix<complex> submatrix = matrix_c0.submatrix(
    submatrix_start_row_,    submatrix_rows_,
    submatrix_start_column_, submatrix_columns_);
  ASSERT_EQ(submatrix.number_of_rows(),     (size_t) submatrix_rows_);
  ASSERT_EQ(submatrix.number_of_columns(),  (size_t) submatrix_columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      EXPECT_EQ(
        submatrix(row,column),
        complex_data_[  (row+submatrix_start_row-1)*columns_
                      + column+submatrix_start_column]);
    }
  }
}

TEST_F(MatrixTest, Comparison) {
  //*** double equality ***

  //self-equality
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d0);
  
  //identically prepared equality
  Matrix<double> matrix_d1(rows_, columns_, double_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d1);
  
  //empty equality
  Matrix<double> matrix_d2();
  Matrix<double> matrix_d3();
  EXPECT_TRUE(matrix_d2 == matrix_d3);

  //*** double inequality ***

  //different sizes
  Matrix<double> matrix_d4 = matrix_d0.submatrix(1, 1, rows_-1, columns-1);
  EXPECT_TRUE(matrix_d0 != matrix_d4);
  EXPECT_FALSE(matrix_d0 == matrix_d4);

  //empty/non-empty
  EXPECT_TRUE(matrix_d2 != matrix_d0);
  EXPECT_FALSE(matrix_d2 == matrix_d0);

  //same size, different elements
  Matrix<double> matrix_d5(rows_, columns_, double_data_);
  Matrix<double> matrix_d6(rows_, columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      matrix_d6(row, column) = (double) row*column;
    }
  }
  EXPECT_TRUE(matrix_d5 != matrix_d6);
  EXPECT_FALSE(matrix_d5 == matrix_d6);

  //*** complex equality ***
  
  //self-equality
  Matrix<complex> matrix_d0(rows_, columns_, complex_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d0);
  
  //identically prepared equality
  Matrix<complex> matrix_d1(rows_, columns_, complex_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d1);
  
  //empty equality
  Matrix<complex> matrix_d2();
  Matrix<complex> matrix_d3();
  EXPECT_TRUE(matrix_d2 == matrix_d3);
  
  //*** complex inequality ***
  
  //different sizes
  Matrix<complex> matrix_c4 = matrix_c0.submatrix(1, 1, rows_-1, columns-1);
  EXPECT_TRUE(matrix_c0 != matrix_c4);
  EXPECT_FALSE(matrix_c0 == matrix_c4);
  
  //empty/non-empty
  EXPECT_TRUE(matrix_c2 != matrix_c0);
  EXPECT_FALSE(matrix_c2 == matrix_c0);
  
  //same size, different elements
  Matrix<complex> matrix_c5(rows_, columns_, complex_data_);
  Matrix<complex> matrix_c6(rows_, columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      matrix_c6(row, column) = (complex) row*column;
    }
  }
  EXPECT_TRUE(matrix_c5 != matrix_c6);
  EXPECT_FALSE(matrix_c5 == matrix_c6);
}

//TODO: CONTINUE CLEANUP FROM HERE

TEST_F(MatrixTest, Assignment) {
  //plain vanilla assignment
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  Matrix<double> matrix_d1 = matrix_d0;
  EXPECT_TRUE(matrix_d0, matrix_d1);

  //plain vanilla assignment
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1 = matrix_c0;
  EXPECT_TRUE(matrix_c0, matrix_c1);
}

TEST_F(MatrixTest, Addition) {
  //add and assign
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  Matrix<double> matrix_d1();
  matrix_d1 += matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d1));

  //add
  double double_data_doubled[data_size_];
  for (size_t index=0; index<data_size_; ++index)
  {
    double_data_doubled = double_data_ * 2;
  }
  Matrix<double> matrix_d2 = matrix_d0 + matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, double_data_doubled));

  //add and assign
  Matrix<complex> matrix_d0(rows_, columns_, complex_data_);
  Matrix<complex> matrix_d1();
  matrix_d1 += matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d1));

  //add
  complex complex_data_doubled[data_size_];
  for (size_t index=0; index<data_size_; ++index)
  {
    complex_data_doubled = complex_data_ * 2;
  }
  Matrix<complex> matrix_d2 = matrix_d0 + matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, complex_data_doubled));
}

TEST_F(MatrixTest, Subtraction) {
  //subtract and assign
  double double_data_doubled[data_size_];
  for (size_t index=0; index<data_size_; ++index)
  {
    double_data_doubled = double_data_ * 2;
  }
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  Matrix<double> matrix_d1(rows_, columns_, double_data_doubled);
  Matrix<double> matrix_d2(matrix_d1);
  matrix_d2 -= matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d2));

  //subtract
  Matrix<double> matrix_d3 = matrix_d1 - matrix_d0;
  EXPECT_TRUE(equal(matrix_d3, double_data_));

  //subtract and assign
  complex complex_data_doubled[data_size_];
  for (size_t index=0; index<data_size_; ++index)
  {
    complex_data_doubled = complex_data_ * 2;
  }
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1( rows_, columns_, complex_data_doubled);
  Matrix<complex> matrix_c2(matrix_c1);
  matrix_c2 -= matrix_c0;
  ASSERT_TRUE(equal(matrix_c0, matrix_c2));

  //subtract
  Matrix<complex> matrix_c3 = matrix_c1 - matrix_c0;
  EXPECT_TRUE(equal(matrix_c3, complex_data_));
}

TEST_F(MatrixTest, Multiplication) {
  bool testpass = true;

  double d1[6] = {1,2,3,7,6,5};
  double d2[6] = {3,4,5,2,8,3};
  const Matrix<double>  matrix_d0(3, 2, d1);
  const Matrix<double>  matrix_d1(2, 3, d2);
  Matrix<double>        matrix_d2(3, 3);
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        matrix_d2(i,j) += matrix_d0(i,k)*matrix_d1(k,j);
  Matrix<double> matrix_d3(matrix_d0)
  matrix_d3 *= matrix_d1;
  ASSERT_TRUE(equal(matrix_d2, matrix_d3));
  
  Matrix<double> matrix_d4 = matrix_d0 * matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, matrix_d4));
       
  complex c1[6];
  complex c2[6];
  for(int i=0; i<6; i++) c1[i] = complex( d1[i],       d2[i]);
  for(int i=0; i<6; i++) c2[i] = complex(-d2[i]*d1[i], d1[i]);
  const Matrix<complex>  matrix_c0(3, 2, d1);
  const Matrix<complex>  matrix_c1(2, 3, d2);
  Matrix<complex>        matrix_c2(3, 3);
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        matrix_c2(i,j) += matrix_c0(i,k) * matrix_c1(k,j);
  Matrix<complex> matrix_c3(matrix_c0)
  matrix_c3 *= matrix_c1;
  ASSERT_TRUE(equal(matrix_c2, matrix_c3));
  
  Matrix<complex> matrix_c4 = matrix_c0 * matrix_c1;
  EXPECT_TRUE(equal(matrix_c2, matrix_c4));
}

TEST_F(MatrixTest, ScalarMultiplication) {
  double double_data_doubled[data_size_];
  for (size_t index=0; index<data_size_; ++index)
  {
    double_data_doubled = double_data_ * 2.;
  }

  const Matrix<double> matrix_d0( rows_, columns_, test_data);
  Matrix<double> matrix_d1(matrix_d0);
  matrix_d1 *= 2.;
  ASSERT_TRUE(equal(matrix_d1, double_data_doubled));

  Matrix<double> matrix_d2 = matrix_d0 * 2.;
  ASSERT_TRUE(equal(matrix_d2, double_data_doubled));
  

  complex complex_data_doubled[data_size_];
  for (size_t index=0; index<data_size_; ++index)
  {
    complex_data_doubled = complex_data_ * 2.;
  }

  const Matrix<complex> matrix_c0( rows_, columns_, test_data);
  Matrix<complex> matrix_c1(matrix_c0);
  matrix_c1 *= 2.;
  ASSERT_TRUE(equal(matrix_c1, complex_data_doubled));

  Matrix<complex> matrix_c2 = matrix_c0 * 2.;
  ASSERT_TRUE(equal(matrix_c2, complex_data_doubled));
}

TEST_F(MatrixTest, VectorMultiplication) {
  Vector<double>    mv_d = matrix_d2.get_mvector(1);
  Vector<double>    mv_d_mult = matrix_d1 * mv_d;
  testpass &= equal(mv_d_mult, matrix_d3.get_mvector(1));

  Vector<complex> mv_c = matrix_c2.get_mvector(1);
  Vector<complex> mv_c_mult = matrix_c1 * mv_c;
  testpass &= equal(mv_c_mult, matrix_c3.get_mvector(1));
}

TEST_F(MatrixTest, Division) {
}

TEST_F(MatrixTest, ComplexDecomposition) {
}

TEST_F(MatrixTest, Addition) {
}

//trace()
TEST_F(MatrixTest, Trace) {
  Matrix<double> matrix_d1(double_data_, size_, size_);
  double tr = matrix_d1.trace();
  for(size_t i=1; i<=size_; i++) tr -= matrix_d1(i,i);
  EXPECT_DOUBLE_EQ(tr, 0.);
}

//det() - check for simple test case
TEST_F(MatrixTest, Determinant) {
  Matrix<double> matrix_d1( size_, size_, double_data_);
  for(int i=1; i<=2; i++) for(int j=3; j<=4; j++) {
    matrix_d1(i,j) = 0.;
    matrix_d1(j,i) = 0.;
  }
  //det is equal to det(upper block diag)*det(lower block diag)
  EXPECT_NEAR( ( matrix_d1(1,1)*matrix_d1(2,2)
                -matrix_d1(1,2)*matrix_d1(2,1))*(matrix_d1(3,3)*matrix_d1(4,4)
                -matrix_d1(3,4)*matrix_d1(4,3)), matrix_d1.determinant(),
                1e-6);
  Matrix<double> matrix_d2( size_, size_, double_data_);
  for(size_t i=1; i<=size_; i++) matrix_d2(2,i) = matrix_d2(1,i);
  EXPECT_NEAR( 0., matrix_d2.determinant(),1e-6);  //if 2 rows_ identical, det is 0
}

//
TEST_F(MatrixTest, Inversion) {
  Matrix<double> matrix_d1( size_, size_, double_data_);
  Matrix<double> matrix_d2 = matrix_d1;
  matrix_d1.invert();
  matrix_d2 = matrix_d1*matrix_d2; //M^-1 * M = Identity
  for(int i=1; i<=4; i++)
    for(int j=1; j<=4; j++)
      EXPECT_NEAR( i==j ? 1. : 0.,matrix_d2(i,j), 1e-9);

  Matrix<double> matrix_d1( size_, size_, double_data_);
  Matrix<double> matrix_d2 = matrix_d1.inverse()*matrix_d1; //M^-1 * M = Identity
  for(int i=1; i<=4; i++)
    for(int j=1; j<=4; j++)
      EXPECT_NEAR( i==j ? 1. : 0.,matrix_d2(i,j), 1e-9);
}

////////////////////// IMPORT OF OLD UNIT TESTS - NEEDS CLEANUP (CTR) //////////////

void build_data(); //make some test data

bool test_MatrixAssignmentEq(); //operator A = B
bool test_MatrixBoolEq(); //operator A==B, A!=B
bool test_MatrixPlus(); //operator A+=B, A+B, A+=c, A+c, c+A
bool test_MatrixMinus(); //operator A-=B, A-B, A-=c, A-c, c-A
bool test_MatrixTimes(); //operator A*=B, A*B, A*c, c*A
bool test_MatrixDivide(); //operator A/=c, A/c
bool test_MatrixOStreamOp(); //operator <<
bool test_MatrixTrace();
bool test_MatrixDet();
bool test_MatrixInv(); //inverse, invert
bool test_MatrixT();
bool test_VectorT(); //returns a matrix so tested here
bool test_MatrixSub();
bool test_MatrixEigen(); //eigenvector, eigenvalue functions
bool test_MatrixToGSL();
bool test_MatrixToMatrix(); //real, imaginary, complex functions

bool test_Matrix()
{
  build_data();

  std::vector<bool> testpass;
  testpass.push_back( test_MatrixConstructor() );
  testpass.push_back( test_MatrixDereference() );
  testpass.push_back( test_MatrixSize() );
  testpass.push_back( test_MatrixBoolEq() );
  testpass.push_back( test_MatrixAssignmentEq() );
  testpass.push_back( test_MatrixPlus() );
  testpass.push_back( test_MatrixMinus() );
  testpass.push_back( test_MatrixTimes() );
  testpass.push_back( test_MatrixDivide() );
  testpass.push_back( test_MatrixOStreamOp() );
  testpass.push_back( test_MatrixTrace() );
  testpass.push_back( test_MatrixDet() );
  testpass.push_back( test_MatrixInv() );
  testpass.push_back( test_MatrixT() );
  testpass.push_back( test_VectorT() );
  testpass.push_back( test_MatrixSub() );
  testpass.push_back( test_MatrixEigen() );
  testpass.push_back( test_MatrixToGSL() );
  testpass.push_back( test_MatrixToMatrix() );

  bool all_testpass = true;
  for(size_t i=0; i<testpass.size(); i++) 
  {
    all_testpass &= testpass[i];
  }
  return all_testpass;
}

size_t rows_=8, columns_=2;


bool test_MatrixConstructor()
{
  bool   testpass = true;
  size_t rows_=8, columns_=2;
  Matrix<double> matrix_d0;
  Matrix<double> matrix_d1( matrix_d0 ); 
  Matrix<double> matrix_d2( 10, 20 );
  Matrix<double> matrix_d3( 10, 20, 3.);
  Matrix<double> matrix_d4( rows_, columns_, double_data_);
  const Matrix<double> matrix_d5(matrix_d4);
  Matrix<double> matrix_d6 = Matrix<double>::Diagonal( 4, 2, -1);
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) {double d = matrix_d2(i,j); d += 1;} //check for invalid read in valgrind here
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) testpass &= matrix_d3(i,j) == 3.;
  for(size_t i1=1; i1<=rows_; i1++) 
    for(size_t i2=1; i2<=columns_; i2++) 
      testpass &= matrix_d4(i1,i2) == double_data_[(i2-1)*8 + i1-1];
  for(size_t i1=1; i1<=4; i1++) 
    for(size_t i2=1; i2<=4; i2++) 
      testpass &= (equal(matrix_d6(i1,i2), -1.) || i1==i2) &&  (equal(matrix_d6(i1,i2), 2) || i1!=i2);


  Matrix<complex> matrix_c0;
  Matrix<complex> matrix_c1( matrix_c0 ); 
  Matrix<complex> matrix_c2( 10, 20 );
  Matrix<complex> matrix_c3( 10, 20, MAUS::Complex::complex(3,-3));
  Matrix<complex> matrix_c4( rows_, columns_, complex_data_);
  const Matrix<complex> matrix_c5(matrix_c4);
  Matrix<complex> matrix_c6 = Matrix<complex>::Diagonal( 4, MAUS::Complex::complex(2,-3), MAUS::Complex::complex(1,-6));
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) {complex c = matrix_c2(i,j); c += MAUS::Complex::complex(1,1);} //check for invalid read in valgrind here
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) testpass &= matrix_c3(i,j) == MAUS::Complex::complex(3,-3);
  for(size_t i1=1; i1<=rows_; i1++) 
    for(size_t i2=1; i2<=columns_; i2++) 
      testpass &= matrix_c4(i1,i2) == complex_data_[(i2-1)*8 + i1-1];
  for(size_t i1=1; i1<=4; i1++) 
    for(size_t i2=1; i2<=4; i2++) 
      testpass &= (equal(matrix_c6(i1,i2), MAUS::Complex::complex(1,-6)) || i1==i2) &&  (equal(matrix_c6(i1,i2), MAUS::Complex::complex(2,-3)) || i1!=i2);

  return testpass;
}

bool test_MatrixDereference()
{
  bool testpass = true;
  Matrix<double> matrix_d1( rows_, columns_, test_data);
  const Matrix<double> matrix_d2( rows_, columns_, test_data);
  for(size_t i1=0; i1<rows_; i1++)
    for(size_t i2=0; i2<columns_; i2++)
    {
      testpass &= matrix_d1(i1+1,i2+1) == test_data[(i2)*8 + i1];
      testpass &= matrix_d2(i1+1,i2+1) == test_data[(i2)*8 + i1];
      matrix_d1(i1+1,i2+1) = 4;
      testpass &= matrix_d1(i1+1,i2+1) == 4;
    }
  Matrix<complex> matrix_c1( rows_, columns_, complex_data_);
  const Matrix<complex> matrix_c2( rows_, columns_, complex_data_);
  for(size_t i1=0; i1<rows_; i1++)
    for(size_t i2=0; i2<columns_; i2++)
    {
      testpass &= matrix_c1(i1+1,i2+1) == complex_data_[(i2)*8 + i1];
      testpass &= matrix_c2(i1+1,i2+1) == complex_data_[(i2)*8 + i1];
      matrix_c1(i1+1,i2+1) = MAUS::Complex::complex(4. -4);
      testpass &= matrix_c1(i1+1,i2+1) == MAUS::Complex::complex(4. -4);
    }
  return testpass;
}

bool test_MatrixSize()
{
  bool testpass = true;
  const Matrix<double>    matrix_d0;
  const Matrix<double>    matrix_d1( rows_, columns_, test_data);
  testpass &= matrix_d0.number_of_rows() == 0  && matrix_d0.number_of_columns() == 0;
  testpass &= matrix_d1.number_of_rows() == rows_ && matrix_d1.number_of_columns() == columns_;
  const Matrix<complex> matrix_c0;
  const Matrix<complex> matrix_c1( rows_, columns_, complex_data_);
  testpass &= matrix_c0.number_of_rows() == 0  && matrix_c0.number_of_columns() == 0;
  testpass &= matrix_c1.number_of_rows() == rows_ && matrix_c1.number_of_columns() == columns_;
  return testpass;
}

bool test_MatrixBoolEq()
{
  bool testpass = true;
  const Matrix<double>    matrix_d0a;
  const Matrix<double>    matrix_d0b;
  const Matrix<double>    matrix_d1a( rows_, columns_, test_data);
  Matrix<double>          matrix_d1b( rows_, columns_, test_data);
  const Matrix<double>    matrix_d1c( columns_, rows_, 1);
  const Matrix<complex> matrix_c0a;
  const Matrix<complex> matrix_c0b;
  const Matrix<complex> matrix_c1a( rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1b( rows_, columns_, complex_data_);
  const Matrix<complex>    matrix_c1c( columns_, rows_, MAUS::Complex::complex(0,0));

  testpass &= matrix_d0a == matrix_d0b && !(matrix_d0a != matrix_d0b);
  testpass &= matrix_d1a == matrix_d1b && !(matrix_d1a != matrix_d1b);
  testpass &= matrix_d1a != matrix_d1c && !(matrix_d1a == matrix_d1c);
  testpass &= matrix_c0a == matrix_c0b && !(matrix_c0a != matrix_c0b);
  testpass &= matrix_c1a == matrix_c1b && !(matrix_c1a != matrix_c1b);
  testpass &= matrix_c1a != matrix_c1c && !(matrix_c1a == matrix_c1c);


  for(size_t i=1; i<matrix_d1a.number_of_rows(); i++) 
    for(size_t j=1; j<matrix_d1a.number_of_columns(); j++)
    {
      matrix_d1b(i,j) = 0;
      testpass &= matrix_d1a != matrix_d1b && !(matrix_d1a == matrix_d1b);
      matrix_d1b(i,j) = matrix_d1a(i,j);
    }
  for(size_t i=1; i<matrix_c1a.number_of_rows(); i++) 
    for(size_t j=1; j<matrix_c1a.number_of_columns(); j++)
    {
      matrix_c1b(i,j) = MAUS::Complex::complex(0,0);
      testpass &= matrix_c1a != matrix_c1b && !(matrix_c1a == matrix_c1b);
      matrix_c1b(i,j) = matrix_c1a(i,j);
    }

  return testpass;
}

bool test_MatrixAssignmentEq()
{

  bool testpass = true;
  const Matrix<double>    matrix_d0a;
  Matrix<double>          matrix_d0b;
  const Matrix<double>    matrix_d1a( rows_, columns_, test_data);
  Matrix<double>          matrix_d1b;
  const Matrix<complex> matrix_c0a;
  Matrix<complex>       matrix_c0b;
  const Matrix<complex> matrix_c1a( rows_, columns_, complex_data_);
  Matrix<complex>       matrix_c1b;

  matrix_d0b = matrix_d0a;
  testpass &= matrix_d0b == matrix_d0a;
  matrix_d1b = matrix_d1a;
  testpass &= matrix_d1b == matrix_d1a;
  matrix_c0b = matrix_c0a;
  testpass &= matrix_c0b == matrix_c0a;
  matrix_c1b = matrix_c1a;
  testpass &= matrix_c1b == matrix_c1a;
  matrix_d1b = matrix_d1b; //test for memory problem
  matrix_c1b = matrix_c1b; //test for memory problem

  return testpass;
}

bool test_MatrixPlus()
{
  bool testpass = true;
  Matrix<double> matrix_d1a( rows_, columns_, test_data);
  Matrix<double> matrix_d1b( matrix_d1a );
  matrix_d1a += matrix_d1a;
  for(size_t i=1; i<=matrix_d1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_d1b.number_of_columns(); j++)  
      testpass &= fabs(matrix_d1a(i,j) - 2.*matrix_d1b(i,j)) < 1e-9; 
  matrix_d1a = matrix_d1b + matrix_d1a;
  for(size_t i=1; i<=matrix_d1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_d1b.number_of_columns(); j++)  
      testpass &= fabs(matrix_d1a(i,j) - 3.*matrix_d1b(i,j)) < 1e-9; 

  Matrix<complex> matrix_c1a( rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1b( matrix_c1a );
  matrix_c1a += matrix_c1a;
  for(size_t i=1; i<=matrix_c1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_c1b.number_of_columns(); j++)  
      testpass &= equal(matrix_c1a(i,j), 2.*matrix_c1b(i,j) ); 
  matrix_c1a = matrix_c1b + matrix_c1a;
  for(size_t i=1; i<=matrix_c1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_c1b.number_of_columns(); j++)  
      testpass &= equal(matrix_c1a(i,j), 3.*matrix_c1b(i,j)); 
  
  return testpass;  
}

bool test_MatrixMinus()
{
  bool testpass = true;
  Matrix<double> matrix_d1a( rows_, columns_, test_data);
  Matrix<double> matrix_d1b( matrix_d1a );
  matrix_d1a -= matrix_d1a;
  for(size_t i=1; i<=matrix_d1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_d1b.number_of_columns(); j++)  
      testpass &= fabs(matrix_d1a(i,j)) < 1e-9; 
  matrix_d1a = -matrix_d1b;
  for(size_t i=1; i<=matrix_d1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_d1b.number_of_columns(); j++)  
      testpass &= fabs(matrix_d1a(i,j)+matrix_d1b(i,j)) < 1e-9; 
  matrix_d1a = matrix_d1b;
  matrix_d1a = matrix_d1b - matrix_d1a;
  for(size_t i=1; i<=matrix_d1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_d1b.number_of_columns(); j++)  
      testpass &= fabs(matrix_d1a(i,j)) < 1e-9; 

  Matrix<complex> matrix_c1a( rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1b( matrix_c1a );
  matrix_c1a -= matrix_c1a;
  for(size_t i=1; i<=matrix_c1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_c1b.number_of_columns(); j++)  
      testpass &= equal(matrix_c1a(i,j), MAUS::Complex::complex(0,0) ); 
  matrix_c1a = -matrix_c1b;
  for(size_t i=1; i<=matrix_d1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_d1b.number_of_columns(); j++)  
      testpass &= equal(matrix_c1a(i,j), -matrix_c1b(i,j)); 
  matrix_c1a = matrix_c1b;
  matrix_c1a = matrix_c1b - matrix_c1a;
  for(size_t i=1; i<=matrix_c1b.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_c1b.number_of_columns(); j++)  
      testpass &= equal(matrix_c1a(i,j), MAUS::Complex::complex(0,0) ); 
  
  return testpass;  
}

bool test_MatrixTimes()
{
  bool testpass = true;

  //scalar multiplication
  Matrix<double> matrix_da( rows_, columns_, test_data);
  const Matrix<double> matrix_db( matrix_da );
  matrix_da *= 2.;
  for(size_t i=1; i<=matrix_db.number_of_rows(); i++)  
    for(size_t j=1; j<=matrix_db.number_of_columns(); j++)  
      testpass &= fabs(matrix_da(i,j) - 2.*matrix_db(i,j)) < 1e-9; 
  matrix_da = matrix_da*0.5;
  testpass &= equal(matrix_da, matrix_db); 

  //matrix multiplication
  double d1[6] = {1,2,3,7,6,5};
  double d2[6] = {3,4,5,2,8,3};
  Matrix<double>       matrix_d1( 3, 2, d1);
  Matrix<double>       matrix_d2( 2, 3, d2);
  Matrix<double>       matrix_d3( 3, 3, 0.);
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        matrix_d3(i,j) += matrix_d1(i,k)*matrix_d2(k,j);
  matrix_d1*=matrix_d2;
  testpass &= equal(matrix_d1, matrix_d3);
  matrix_d1 = Matrix<double>(3,2,d1);
  testpass &= equal(matrix_d1*matrix_d2, matrix_d3);

  //scalar multiplication
  Matrix<complex> matrix_ca( rows_, columns_, complex_data_);
  const Matrix<complex> matrix_cb( matrix_ca );
  matrix_ca *= MAUS::Complex::complex(2.,-5.);
  for(size_t i=1; i<=matrix_db.number_of_rows(); i++)
    for(size_t j=1; j<=matrix_db.number_of_columns(); j++)
      testpass &= equal(matrix_ca(i,j), MAUS::Complex::complex(2.,-5.)*matrix_cb(i,j)); 
  matrix_ca = matrix_ca*(1./MAUS::Complex::complex(2.,-5.));
  testpass &= equal(matrix_ca, matrix_cb); 
       
  //matrix multiplication
  complex c1[6];
  complex c2[6];
  for(int i=0; i<6; i++) c1[i] = MAUS::Complex::complex( d1[i],       d2[i]);
  for(int i=0; i<6; i++) c2[i] = MAUS::Complex::complex(-d2[i]*d1[i], d1[i]);
  Matrix<complex> matrix_c1( 3, 2, c1);
  Matrix<complex> matrix_c2( 2, 3, c2);
  Matrix<complex> matrix_c3( 3, 3, MAUS::Complex::complex(0,0));
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        matrix_c3(i,j) += matrix_c1(i,k)*matrix_c2(k,j);
  matrix_c1*=matrix_c2;
  testpass &= equal(matrix_c1, matrix_c3);
  matrix_c1 = Matrix<complex>(3,2,c1);
  testpass &= equal(matrix_c1*matrix_c2, matrix_c3);

  //vector multiplication
  Vector<double>    mv_d = matrix_d2.get_mvector(1);
  Vector<complex> mv_c = matrix_c2.get_mvector(1);
  Vector<double>    mv_d_mult = matrix_d1 * mv_d;
  Vector<complex> mv_c_mult = matrix_c1 * mv_c;
  testpass &= equal(mv_d_mult, matrix_d3.get_mvector(1));
  testpass &= equal(mv_c_mult, matrix_c3.get_mvector(1));

  return testpass;  
}

bool test_MatrixDivide()
{
  bool testpass = true;
  Matrix<double> matrix_da( rows_, columns_, test_data);
  const Matrix<double> matrix_db( matrix_da );
  matrix_da /= 2.;
  matrix_da *= 2.;
  testpass &= equal(matrix_da, matrix_db);
  matrix_da  = matrix_da/2.;
  matrix_da *= 2.;
  testpass &= equal(matrix_da, matrix_db); 

  Matrix<complex> matrix_ca( rows_, columns_, complex_data_);
  const Matrix<complex> matrix_cb( matrix_ca );
  matrix_ca /= MAUS::Complex::complex(2.,-5.);
  matrix_ca *= MAUS::Complex::complex(2., -5.);
  testpass &= equal(matrix_ca, matrix_cb);
  matrix_ca  = matrix_ca/MAUS::Complex::complex(2.,-5.);
  matrix_ca *= MAUS::Complex::complex(2., -5.);
  testpass &= equal(matrix_ca, matrix_cb); 

  return testpass;
}

bool test_MatrixOStreamOp()
{
  bool testpass = true;
  std::stringstream test;
  Matrix<double> matrix_da( rows_, columns_, test_data );
  Matrix<double> matrix_db;
  test << matrix_da;
  test >> matrix_db;
  testpass &= equal(matrix_da, matrix_db);

  Matrix<complex> matrix_ca( rows_, columns_, complex_data_ );
  Matrix<complex> matrix_cb;
  test << matrix_ca;
  test >> matrix_cb;
  testpass &= equal(matrix_ca, matrix_cb);

  return testpass;
}

bool test_MatrixTrace()
{
  bool testpass = true;
  const Matrix<double> matrix_da( 4, 4, test_data );
  double trace_d = 0;
  for(size_t i=1; i<=4; i++) trace_d += matrix_da(i,i);  
  testpass &= equal(trace_d, matrix_da.trace());

  const Matrix<complex> matrix_ca( 4, 4, complex_data_ );
  complex trace_c = MAUS::Complex::complex(0);
  for(size_t i=1; i<=4; i++) trace_c += matrix_ca(i,i);  
  testpass &= equal(trace_c, matrix_ca.trace());
  return testpass;
}


bool test_MatrixDet()
{
  bool   testpass = true;
  const Matrix<double> matrix_da(2,2, test_data);
  double det_d = matrix_da(1,1)*matrix_da(2,2) - matrix_da(2,1)*matrix_da(1,2);
  testpass &= equal(matrix_da.determinant(), det_d);

  const Matrix<complex> matrix_ca(2,2, complex_data_);
  complex det_c = matrix_ca(1,1)*matrix_ca(2,2) - matrix_ca(2,1)*matrix_ca(1,2);
  testpass &= equal(matrix_ca.determinant(), det_c);
  return testpass;
}

bool test_MatrixInv()
{
  bool testpass = true;
  Matrix<double> matrix_da(4,4,test_data);
  Matrix<double> matrix_dinv = matrix_da.inverse();
  Matrix<double> matrix_db( matrix_da );
  matrix_da.invert();
  testpass &= equal(matrix_db*matrix_dinv, Matrix<double>::Diagonal(4, 1, 0)); //4D identity matrix
  testpass &= equal(matrix_db*matrix_da,   Matrix<double>::Diagonal(4, 1, 0)); //4D identity matrix

  Matrix<complex> matrix_ca(4,4,complex_data_);
  Matrix<complex> matrix_cinv = matrix_ca.inverse();
  Matrix<complex> matrix_cb( matrix_ca );
  matrix_ca.invert();
  testpass &= equal(matrix_cb*matrix_cinv, Matrix<complex>::Diagonal(4, MAUS::Complex::complex(1), MAUS::Complex::complex(0))); //4D identity matrix
  testpass &= equal(matrix_cb*matrix_ca,   Matrix<complex>::Diagonal(4, MAUS::Complex::complex(1), MAUS::Complex::complex(0))); //4D identity matrix

  return testpass;
}

bool test_MatrixT()
{
  bool testpass = true;
  Matrix<double> matrix_da(rows_,columns_,test_data);  
  Matrix<double> matrix_dt = matrix_da.T();
  testpass &= matrix_da.number_of_rows() == matrix_dt.number_of_columns() && matrix_da.number_of_columns() == matrix_dt.number_of_rows();
  for(size_t i1 = 1; i1<=matrix_da.number_of_rows(); i1++)
    for(size_t i2 = 1; i2<=matrix_da.number_of_columns(); i2++)
      testpass &= matrix_da(i1,i2) == matrix_dt(i2,i1);

  Matrix<complex> matrix_ca(rows_,columns_,complex_data_);  
  Matrix<complex> matrix_ct = matrix_ca.T();
  testpass &= matrix_ca.number_of_rows() == matrix_ct.number_of_columns() && matrix_ca.number_of_columns() == matrix_ct.number_of_rows();
  for(size_t i1 = 1; i1<=matrix_ca.number_of_rows(); i1++)
    for(size_t i2 = 1; i2<=matrix_ca.number_of_columns(); i2++)
      testpass &= matrix_ca(i1,i2) == matrix_ct(i2,i1);


  return testpass;
}

bool test_VectorT()
{
  bool testpass = true;
  Vector<double> mv_da(test_data, &test_data[rows_*columns_]);  
  Matrix<double> matrix_dt = mv_da.T();
  testpass &= mv_da.number_of_rows() == matrix_dt.number_of_columns() && 1 == matrix_dt.number_of_rows();
  for(size_t i1 = 1; i1<=matrix_dt.number_of_rows(); i1++)
    for(size_t i2 = 1; i2<=matrix_dt.number_of_columns(); i2++)
      testpass &= mv_da(i2) == matrix_dt(i1,i2);

  Vector<complex> mv_ca(complex_data_, &complex_data_[rows_*columns_]);  
  Matrix<complex> matrix_ct = mv_ca.T();
  testpass &= mv_ca.number_of_rows() == matrix_ct.number_of_columns() && 1 == matrix_ct.number_of_rows();
  for(size_t i1 = 1; i1<=matrix_ct.number_of_rows(); i1++)
    for(size_t i2 = 1; i2<=matrix_ct.number_of_columns(); i2++)
      testpass &= mv_da(i2) == matrix_dt(i1,i2);

  return testpass;
}

bool test_MatrixSub()
{
  bool testpass = true;
  size_t k1=3,k2=4,m1=2,m2=4;
  Matrix<double> matrix_da(4,4,test_data);
  Matrix<double> matrix_db = matrix_da.sub(k1,k2,m1,m2);
  for(size_t i=k1; i<=k2; i++)
    for(size_t j=m1; j<=m2; j++)
      testpass &= matrix_db(i-k1+1, j-m1+1)  == matrix_da(i,j);
  testpass &= matrix_db.number_of_rows() == k2-k1+1 && matrix_db.number_of_columns() == m2-m1+1;

  Matrix<complex> matrix_ca(4,4,complex_data_);
  Matrix<complex> matrix_cb = matrix_ca.sub(k1,k2,m1,m2);
  for(size_t i=k1; i<=k2; i++)
    for(size_t j=m1; j<=m2; j++)
      testpass &= matrix_cb(i-k1+1, j-m1+1) == matrix_ca(i,j);
  testpass &= matrix_cb.number_of_rows() == k2-k1+1 && matrix_cb.number_of_columns() == m2-m1+1;

  return testpass;
}

bool test_MatrixEigen()
{
  bool testpass = true;
  Matrix<double> md(4, 4, test_data);
  std::pair< Vector<complex>, Matrix<complex> > evec  = md.eigenvectors();
  Vector<complex> eval  = md.eigenvalues();
  for(size_t i=1; i<evec.first.number_of_rows(); i++)
    testpass &= equal(complex(md) * evec.second.get_mvector(1), evec.second.get_mvector(1)*evec.first(1) ) ;

  Vector<complex> eval2 = evec.first;
  for(size_t i=1; i<=eval2.number_of_rows(); i++)
  {
    bool test_here = false;
    for(size_t j=1; j<=eval2.number_of_rows(); j++)
      if( equal( eval2(i),eval(j) ) ) test_here = true;
    testpass &= test_here; //pass test if for each value in eval2 there is also a value in eval and size_ of eval2 == size_ of eval
  }    
  testpass &= eval.number_of_rows() == eval2.number_of_rows();
  return testpass;
}


bool test_MatrixToGSL() 
{
  bool testpass = true;
  Matrix<double> matrix_da(rows_, columns_, test_data);  
  Matrix<double> matrix_db;
  const gsl_matrix* md =  Matrix_to_gsl(matrix_da);
  try {const gsl_matrix* m0 =  Matrix_to_gsl(matrix_db); m0 = NULL; testpass = false;}
  catch(...) {}
  for(size_t i=1; i<=matrix_da.number_of_rows(); i++)
    for(size_t j=1; j<=matrix_da.number_of_columns(); j++)
      testpass &= matrix_da(i,j) == gsl_matrix_get(md, i-1, j-1);

  Matrix<complex> matrix_ca(rows_, columns_, complex_data_);  
  Matrix<complex> matrix_cb;
  const gsl_matrix_complex* mc1 =  Matrix_to_gsl(matrix_ca);
  try {const gsl_matrix_complex* mc2 =  Matrix_to_gsl(matrix_cb); mc2 = NULL; testpass = false;}
  catch(...) {}
  for(size_t i=1; i<=matrix_ca.number_of_rows(); i++)
    for(size_t j=1; j<=matrix_ca.number_of_columns(); j++)
      testpass &= matrix_ca(i,j) == gsl_matrix_complex_get(mc1, i-1, j-1);

  return testpass;
}

bool test_MatrixToMatrix()
{
  bool testpass = true;
  Matrix<complex> matrix_ca(rows_, columns_, complex_data_);  
  Matrix<double>    matrix_dr = re(matrix_ca);  
  Matrix<double>    matrix_di = im(matrix_ca);  
  Matrix<complex> matrix_cb = complex(matrix_dr, matrix_di);
  testpass &= equal(matrix_cb, matrix_ca);

  Matrix<complex> matrix_cc = complex(matrix_dr);
  for(size_t i=1; i<matrix_ca.number_of_rows(); i++)
    for(size_t j=1; j<matrix_ca.number_of_columns(); j++)
      testpass &= equal( matrix_cc(i,j), MAUS::Complex::complex( MAUS::real( matrix_ca(i,j) ) ) );

  for(size_t i=1; i<=matrix_ca.number_of_columns(); i++)
  {
    Vector<double>    mv_d = matrix_dr.get_mvector(i);
    Vector<complex> mv_c = matrix_ca.get_mvector(i);
    for(size_t j=1; j<matrix_ca.number_of_rows(); j++)
    {
      testpass &= equal( matrix_ca(j,i), mv_c(j) );
      testpass &= equal( matrix_dr(j,i), mv_d(j) );
    }
  }
  return testpass;
}


TEST(MatrixTest, old_unit_tests)
{
  EXPECT_TRUE(test_Matrix());
}



