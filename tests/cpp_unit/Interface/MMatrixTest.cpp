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

#include "gtest/gtest.h" 

#include <limits>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "src/common/Interface/MMatrix.hh"
#include "src/common/Interface/MVector.hh"

namespace {

/////////////// MMatrix<double> Tests ///////////////

class MMatrixDoubleTest : public ::testing::Test {
protected:
  MMatrixDoubleTest() : n_sq(4), n1(2), n2(8)
 {}
  virtual ~MMatrixDoubleTest() {}
  virtual void SetUp()    {};//  {test_data_d = {}} //make some test data
  virtual void TearDown() {}

  static double test_data_d[16];
  const size_t        n_sq; //square matrix
  const size_t        n1, n2; //rectangular matrix
};

double MMatrixDoubleTest::test_data_d[16] = {
    1,  1,   2,   3, -5, -8, -13, -21,
   34.5, 55.7,  89.3, 144.2, -32.5,-57.5, -91.2,-146.3 
};

//Test MMatrix<double>::MMatrix<double>()
//Default constructor should end up with 0*0 length matrix
TEST_F(MMatrixDoubleTest, DefaultConstructorTest) {
  MMatrix<double> mm_d0;
  ASSERT_TRUE(mm_d0.num_row() == 0); //ASSERT_EQ gives compiler warning for unsigned int == int
  ASSERT_TRUE(mm_d0.num_col() == 0);
}

//Test MMatrix<double>::MMatrix<double>(int, int)
//Should make an uninitialised matrix
TEST_F(MMatrixDoubleTest, ConstructorSizeOnlyTest) {
  MMatrix<double> mm_d0( 10, 20 );
  ASSERT_TRUE(mm_d0.num_row() == 10);
  ASSERT_TRUE(mm_d0.num_col() == 20);
}

//Test MMatrix<double>::MMatrix<double>(int, int, double a)
//Should make a matrix initialised with every element filled with a
TEST_F(MMatrixDoubleTest, ConstructorAllEqualTest) { //constructor with all members equal
  MMatrix<double> mm_d0( 10, 20, -3. );
  ASSERT_TRUE(mm_d0.num_row() == 10);
  ASSERT_TRUE(mm_d0.num_col() == 20);
  for(size_t i=1; i<=mm_d0.num_row(); i++)
    for(size_t j=1; j<=mm_d0.num_row(); j++)
      EXPECT_EQ(mm_d0(i,j), -3.); //want to test exactly equal
}

//Test MMatrix<double>::MMatrix<double>(int, int, double* a)
//Should make a matrix initialised with elements filled from buffer a
TEST_F(MMatrixDoubleTest, ConstructorDataTest) { //constructor with buffer
  MMatrix<double> mm_d0( n1, n2, test_data_d);
  ASSERT_TRUE(mm_d0.num_row() == n1);
  ASSERT_TRUE(mm_d0.num_col() == n2);
  for(size_t i=1; i<=n1; i++)
    for(size_t j=1; j<=n2; j++)
      EXPECT_EQ(mm_d0(i,j), test_data_d[i-1+(j-1)*n1]); //want to test exactly equal
}

//Test MMatrix<double>::MMatrix<double>(const MMatrix<double>& matrix)
//Should make a matrix initialised with elements filled from matrix
//Should work if matrix is empty, should work if matrix is const
TEST_F(MMatrixDoubleTest, CopyConstructorTest) {
  const MMatrix<double> mm_d0( n1, n2, test_data_d);
  const MMatrix<double> mm_d1( mm_d0 );
  ASSERT_TRUE(mm_d1.num_row() == n1);
  ASSERT_TRUE(mm_d1.num_col() == n2);
  for(size_t i=1; i<=n1; i++)
    for(size_t j=1; j<=n2; j++)
      EXPECT_EQ(mm_d1(i,j), mm_d0(i,j));

  MMatrix<double> mm_d2; //check that handles 0 length okay
  MMatrix<double> mm_d3(mm_d2);
  ASSERT_TRUE(mm_d3.num_row() == 0);
  ASSERT_TRUE(mm_d3.num_col() == 0);
}

//Test MMatrix<double>::Diagonal(size_t size, double on_diagonal, double off_diagonal)
//Should make diagonal matrix with appropriate on_diagonal and off_diagonal elements
TEST_F(MMatrixDoubleTest, DiagonalTest) { //constructor with buffer
  MMatrix<double> mm_d0 = MMatrix<double>::Diagonal( 4, 2., -1.);
  ASSERT_TRUE(mm_d0.num_row() == 4);
  ASSERT_TRUE(mm_d0.num_col() == 4);
  
  for(size_t i1=1; i1<=4; i1++)
    for(size_t i2=1; i2<=4; i2++) {
      if(i1!=i2) EXPECT_EQ(mm_d0(i1,i2), -1.);
      if(i1==i2) EXPECT_EQ(mm_d0(i1,i2),  2.);
    }
}

//Bracket operator MMatrix<double>()
//Check that it works with const matrix, check that it returns correct value, check that 
//it can set the correct value
TEST_F(MMatrixDoubleTest, MMatrixBracketOperatorTest) {
  MMatrix<double> mm_d1( n1, n2, test_data_d);
  const MMatrix<double> mm_d2( n1, n2, test_data_d); //check that it works for const as well
  for(size_t i1=1; i1<=n1; i1++)
    for(size_t i2=1; i2<=n2; i2++) {
      EXPECT_EQ( mm_d1(i1,i2), test_data_d[i1-1+(i2-1)*n1] );
      EXPECT_EQ( mm_d2(i1,i2), test_data_d[i1-1+(i2-1)*n1] );
      mm_d1(i1,i2) = 4.;
      EXPECT_EQ( mm_d1(i1,i2), 4.);
    }
}

//trace()
TEST_F(MMatrixDoubleTest, MMatrixTraceTest) {
  MMatrix<double> mm_d1( n_sq, n_sq, test_data_d);
  double tr = mm_d1.trace();
  for(size_t i=1; i<=n_sq; i++) tr -= mm_d1(i,i);
  EXPECT_DOUBLE_EQ(tr, 0.);
}

//det() - check for simple test case
TEST_F(MMatrixDoubleTest, MMatrixDetTest) {
  MMatrix<double> mm_d1( n_sq, n_sq, test_data_d);
  for(int i=1; i<=2; i++) for(int j=3; j<=4; j++) {
    mm_d1(i,j) = 0.;
    mm_d1(j,i) = 0.;
  }
  EXPECT_NEAR( (mm_d1(1,1)*mm_d1(2,2)-mm_d1(1,2)*mm_d1(2,1))*(mm_d1(3,3)*mm_d1(4,4)-mm_d1(3,4)*mm_d1(4,3)), mm_d1.determinant(), 1e-6); //det is equal to det(upper block diag)*det(lower block diag)
  MMatrix<double> mm_d2( n_sq, n_sq, test_data_d);
  for(size_t i=1; i<=n_sq; i++) mm_d2(2,i) = mm_d2(1,i);
  EXPECT_NEAR( 0., mm_d2.determinant(),1e-6);  //if 2 rows identical, det is 0
}

//
TEST_F(MMatrixDoubleTest, MMatrixInvertTest) {
  MMatrix<double> mm_d1( n_sq, n_sq, test_data_d);
  MMatrix<double> mm_d2 = mm_d1;
  mm_d1.invert();
  mm_d2 = mm_d1*mm_d2; //M^-1 * M = Identity
  for(int i=1; i<=4; i++)
    for(int j=1; j<=4; j++)
      EXPECT_NEAR( i==j ? 1. : 0.,mm_d2(i,j), 1e-9);
}

//
TEST_F(MMatrixDoubleTest, MMatrixInverseTest) {
  MMatrix<double> mm_d1( n_sq, n_sq, test_data_d);
  MMatrix<double> mm_d2 = mm_d1.inverse()*mm_d1; //M^-1 * M = Identity
  for(int i=1; i<=4; i++)
    for(int j=1; j<=4; j++)
      EXPECT_NEAR( i==j ? 1. : 0.,mm_d2(i,j), 1e-9);
}



} //namespace
