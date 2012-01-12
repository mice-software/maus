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

#include "gtest/gtest.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Random/Random.h"

#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

using namespace MAUS;

//Defined in ComplexTest.cc
bool equal(double c1, double c2);

class SymmetricMatrixTest : public ::testing::Test
{
public:
  SymmetricMatrixTest()
  {
    for (size_t index=0; index<data_size_; ++index)
    {
      double_data_doubled_[index] = double_data_[index] * 2;
    }
  }

  bool elements_equal(const SymmetricMatrix& matrix, const double value)
  {
    bool are_equal = true;
    if(matrix.size() != size_) 
    {
      return false;
    }
    for(size_t row=1; row<=size_; ++row)
    {
      for(size_t column=1; column<=row; ++column)
      {
        are_equal &= ::equal(matrix(row, column), value);
        are_equal &= ::equal(matrix(column, row), value);
      }
    }
    return are_equal;
  }
  
  bool equal(const SymmetricMatrix& matrix, const double* array)
  {
    bool are_equal = true;
    if(matrix.size() != size_) 
    {
      return false;
    }
    for(size_t row=1; row<=size_; ++row)
    {
      for(size_t column=1; column<=row; ++column)
      {
        are_equal &= ::equal(matrix(row,column),
                             array[(row-1)*size_+(column-1)]);
        are_equal &= ::equal(matrix(column,row),
                             array[(row-1)*size_+(column-1)]);
      }
    }
    return are_equal;
  }

  bool equal(const SymmetricMatrix& m1, const SymmetricMatrix& m2)
  { 
    bool are_equal = true;
    if(m1.size() != m2.size()) 
    {
      return false;
    }
    for(size_t row=1; row<=m1.size(); ++row)
		{
      for(size_t column=1; column<=row; ++column)
			{
        are_equal &= ::equal(m1(row,column), m1(column,row));
        are_equal &= ::equal(m1(row,column), m2(row,column));
			}
		}
    return are_equal;
  }

  bool equal(const Vector<double>& v1, const Vector<double>& v2)
  { 
    bool are_equal = true;
    if(v1.size() != v2.size() )
    {
      return false;
    }
    for(size_t i=1; i<=v1.size(); ++i) 
      are_equal &= ::equal(v1(i), v2(i));
    return are_equal;
  }
protected:
  static const size_t data_size_;
  static const double double_data_[25];
  static const size_t size_;
  double double_data_doubled_[25];
};

//************************************************//
//SymmetricMatrixTest static const initializations//
//************************************************//

const double SymmetricMatrixTest::double_data_[25] = {
  0.,			1.,     2.,			3.,     -5.,
	1.,			-13.,   -21.,		34.5,		55.7,
	2.,			-21.,		-32.5,	-57.5,  -91.2,
	3.,			34.5,		-57.5,  2.65,		-3.58,
	-5.,		-55.7,  -91.2,  -3.58,	3.38
};

const size_t SymmetricMatrixTest::data_size_ = 25;
const size_t SymmetricMatrixTest::size_      = 5;

//***********
//test cases
//***********

TEST_F(SymmetricMatrixTest, DefaultConstructor) {
  SymmetricMatrix matrix_d0;
  ASSERT_EQ(matrix_d0.size(),     (size_t) 0);
}

TEST_F(SymmetricMatrixTest, CopyConstructor) {
  //should work if matrix is const
  const SymmetricMatrix matrix_d0(size_, double_data_);
  const SymmetricMatrix matrix_d1(matrix_d0);
  EXPECT_TRUE(equal(matrix_d0, matrix_d1));
  
  //check that handles 0 length okay
  SymmetricMatrix matrix_d2;
  SymmetricMatrix matrix_d3(matrix_d2);
  EXPECT_TRUE(equal(matrix_d2, matrix_d3));
}

TEST_F(SymmetricMatrixTest, HepSymMatrixConstructor) {
  ::CLHEP::HepRandom hep_random;
  const ::CLHEP::HepSymMatrix matrix_hep0(size_, hep_random);
  const SymmetricMatrix matrix_d0(matrix_hep0);
  ASSERT_EQ(matrix_d0.size(), (size_t) size_);
  for(size_t i=1; i<=size_; ++i)
    for(size_t j=1; j<=size_; ++j)
      EXPECT_EQ(matrix_d0(i,j), matrix_hep0(i,j));

  //check that handles 0 length okay
  ::CLHEP::HepSymMatrix matrix_hep1;
  SymmetricMatrix matrix_d1(matrix_hep1);
  ASSERT_EQ(matrix_d1.size(), (size_t) 0);
}

TEST_F(SymmetricMatrixTest, ConstructorSizeOnly) {
  SymmetricMatrix matrix_d0(size_);
  EXPECT_TRUE(elements_equal(matrix_d0, 0.));
}

TEST_F(SymmetricMatrixTest, ConstructorFill) {
  SymmetricMatrix matrix_d0(size_, double_data_[4]);
  EXPECT_TRUE(elements_equal(matrix_d0, double_data_[4]));
}

TEST_F(SymmetricMatrixTest, ConstructorFromArray) {
  SymmetricMatrix matrix_d0(size_, double_data_);
  EXPECT_TRUE(equal(matrix_d0, double_data_));
}

TEST_F(SymmetricMatrixTest, Set) {
  const SymmetricMatrix matrix_s0(size_, double_data_);
  SymmetricMatrix matrix_s1(size_);
  for(size_t row=1; row<=size_; ++row)
  {
    for(size_t column=1; column<=row; ++column)
    {
			matrix_s1.set(row, column, double_data_[(row-1)*size_+(column-1)]);
    }
  }
  EXPECT_TRUE(equal(matrix_s0, matrix_s1));
}

TEST_F(SymmetricMatrixTest, Assignment) {
  //plain vanilla assignment
  SymmetricMatrix matrix_d0(size_, double_data_);
  SymmetricMatrix matrix_d1 = matrix_d0;
  EXPECT_TRUE(equal(matrix_d0, matrix_d1));
}

TEST_F(SymmetricMatrixTest, Addition) {
  //add and assign
  SymmetricMatrix matrix_d0(size_, double_data_);
  SymmetricMatrix matrix_d1(size_);
  matrix_d1 += matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d1));

  //add
  SymmetricMatrix matrix_d2 = matrix_d0 + matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, double_data_doubled_));
}

TEST_F(SymmetricMatrixTest, Subtraction) {
  //subtract and assign
  SymmetricMatrix matrix_d0(size_, double_data_);
  SymmetricMatrix matrix_d1(size_, double_data_doubled_);
  SymmetricMatrix matrix_d2(matrix_d1);
  matrix_d2 -= matrix_d0;
  ASSERT_TRUE(equal(matrix_d2, matrix_d0));

  //subtract
  SymmetricMatrix matrix_d3 = matrix_d1 - matrix_d0;
  EXPECT_TRUE(equal(matrix_d3, double_data_));
}

TEST_F(SymmetricMatrixTest, ScalarMultiplication) {
  //multiply and assign
  const SymmetricMatrix matrix_d0(size_, double_data_);
  SymmetricMatrix matrix_d1(matrix_d0);
  matrix_d1 *= 2.;
  ASSERT_TRUE(equal(matrix_d1, double_data_doubled_));

  //multiplication on the right by a scalar
  SymmetricMatrix matrix_d2 = matrix_d0 * 2.;
  ASSERT_TRUE(equal(matrix_d2, double_data_doubled_));

  //multiplication on the left by a scalar
  SymmetricMatrix matrix_d3 = 2. * matrix_d0;
  ASSERT_TRUE(equal(matrix_d3, double_data_doubled_));
}

TEST_F(SymmetricMatrixTest, ScalarDivision) {
  const SymmetricMatrix matrix_d0(size_, double_data_doubled_);
  SymmetricMatrix matrix_d1(matrix_d0);
  matrix_d1 /= 2.;
  ASSERT_TRUE(equal(matrix_d1, double_data_));

  SymmetricMatrix matrix_d2 = matrix_d0 / 2.;
  ASSERT_TRUE(equal(matrix_d2, double_data_));
}

TEST_F(SymmetricMatrixTest, Inverse) {
  SymmetricMatrix matrix_d1(size_, double_data_);
  SymmetricMatrix matrix_d2 = inverse(matrix_d1);
	//M^-1 * M = Identity
  Matrix<double> matrix_d3 = (Matrix<double>) matrix_d1 * matrix_d2;
  for(size_t row=1; row<=size_; ++row)
  {
    for(size_t column=1; column<=size_; ++column)
    {
      EXPECT_TRUE(::equal(row==column ? 1. : 0., matrix_d3(row, column)));
    }
  }
}

TEST_F(SymmetricMatrixTest, HepSymMatrix) {
  ::CLHEP::HepRandom hep_random;
  const SymmetricMatrix matrix_d0(size_, double_data_);
  ::CLHEP::HepSymMatrix matrix_hep0 = MAUS::CLHEP::HepSymMatrix(matrix_d0);
  ASSERT_EQ((size_t) matrix_hep0.num_row(), matrix_d0.size());
  ASSERT_EQ((size_t) matrix_hep0.num_col(), matrix_d0.size());
  for(size_t row=1; row<=size_; ++row)
  {
    for(size_t column=1; column<=size_; ++column)
    {
      EXPECT_TRUE(::equal(matrix_hep0(row, column), matrix_d0(row, column)));
    }
  }
}

TEST_F(SymmetricMatrixTest, Eigen) {
  //verify the eigensystem equation M*E = e*E
  SymmetricMatrix matrix_d0(size_, double_data_);
  std::pair< Vector<double>, Matrix<double> > eigensys
    = eigensystem(matrix_d0);
  Vector<double> eigenvals = eigensys.first;
  Matrix<double> eigenvectors = eigensys.second;
  ASSERT_EQ(eigenvals.size(), size_);
  ASSERT_EQ(eigenvectors.number_of_columns(), size_);
  for(size_t i=1; i<eigenvals.size(); ++i)
  {
    double eigenvalue = eigenvals(i);
    Vector<double> eigenvector = eigenvectors.column(i);
    EXPECT_TRUE(equal(matrix_d0 * eigenvector,
                      eigenvalue * eigenvector));
  }

  //verify that just getting the eigenvalues returns the same set from
  //eigensystem() (but perhaps in a different order)
  Vector<double> eigenvals2 = eigenvalues(matrix_d0);
  ASSERT_EQ(eigenvals2.size(), size_);
  for(size_t i=1; i<=size_; ++i)
  {
    bool found_match = false;
    for (size_t j=1; j<=size_; j++)
    {
      if (::equal(eigenvals2(i), eigenvals(j)))
      {
        found_match = true;
      }
    }
    EXPECT_TRUE(found_match);
  }    
}

TEST_F(SymmetricMatrixTest, Negation) {
  SymmetricMatrix matrix_d0(size_, double_data_);
  SymmetricMatrix matrix_d1(-matrix_d0);
  for(size_t row=1; row<=size_; ++row)
  {
    for(size_t column=1; column<=row; ++column)
    {
      EXPECT_TRUE(::equal(-matrix_d1(row, column), matrix_d0(row, column)));
      EXPECT_TRUE(::equal(-matrix_d1(column, row), matrix_d0(row, column)));
    }
  }
}
