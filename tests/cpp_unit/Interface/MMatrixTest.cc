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

#include "Interface/MMatrix.hh"
#include "Interface/MVector.hh"

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

////////////////////// IMPORT OF OLD UNIT TESTS - NEEDS CLEANUP (CTR) //////////////

bool diff(m_complex c1, m_complex c2);// { return fabs(re(c1) - re(c2)) < 1e-9 && fabs(im(c1) - im(c2) ) < 1e-9; }
bool diff(double    c1, double    c2);// { return fabs(c1-c2) < 1e-9; }

template <class Tmplt> bool diff(MMatrix<Tmplt> m1, MMatrix<Tmplt> m2)
{ 
  bool not_diff = true;
  if(m1.num_row() != m2.num_row() || m1.num_col() != m2.num_col()) 
    return false;
  for(size_t i=1; i<m1.num_row(); i++) 
    for(size_t j=1; j<m1.num_col(); j++) 
      not_diff &= diff(m1(i,j), m2(i,j));
  return not_diff;
}

template <class Tmplt> bool diff(MVector<Tmplt> v1, MVector<Tmplt> v2)
{ 
  bool not_diff = true;
  if(v1.num_row() != v2.num_row() ) 
    return false;
  for(size_t i=1; i<v1.num_row(); i++) 
    not_diff &= diff(v1(i), v2(i));
  return not_diff;
}

void build_data(); //make some test data

bool test_MMatrix();
bool test_MMatrixConstructor();
bool test_MMatrixDereference(); //operator()
bool test_MMatrixSize(); //num_row(), num_col()
bool test_MMatrixAssignmentEq(); //operator A = B
bool test_MMatrixBoolEq(); //operator A==B, A!=B
bool test_MMatrixPlus(); //operator A+=B, A+B, A+=c, A+c, c+A
bool test_MMatrixMinus(); //operator A-=B, A-B, A-=c, A-c, c-A
bool test_MMatrixTimes(); //operator A*=B, A*B, A*c, c*A
bool test_MMatrixDivide(); //operator A/=c, A/c
bool test_MMatrixOStreamOp(); //operator <<
bool test_MMatrixTrace();
bool test_MMatrixDet();
bool test_MMatrixInv(); //inverse, invert
bool test_MMatrixT();
bool test_MVectorT(); //returns a matrix so tested here
bool test_MMatrixSub();
bool test_MMatrixEigen(); //eigenvector, eigenvalue functions
bool test_MMatrixToGSL();
bool test_MMatrixToMMatrix(); //real, imaginary, complex functions

bool test_MMatrix()
{
  build_data();

  std::vector<bool> testpass;
  testpass.push_back( test_MMatrixConstructor() );
  testpass.push_back( test_MMatrixDereference() );
  testpass.push_back( test_MMatrixSize() );
  testpass.push_back( test_MMatrixBoolEq() );
  testpass.push_back( test_MMatrixAssignmentEq() );
  testpass.push_back( test_MMatrixPlus() );
  testpass.push_back( test_MMatrixMinus() );
  testpass.push_back( test_MMatrixTimes() );
  testpass.push_back( test_MMatrixDivide() );
  testpass.push_back( test_MMatrixOStreamOp() );
  testpass.push_back( test_MMatrixTrace() );
  testpass.push_back( test_MMatrixDet() );
  testpass.push_back( test_MMatrixInv() );
  testpass.push_back( test_MMatrixT() );
  testpass.push_back( test_MVectorT() );
  testpass.push_back( test_MMatrixSub() );
  testpass.push_back( test_MMatrixEigen() );
  testpass.push_back( test_MMatrixToGSL() );
  testpass.push_back( test_MMatrixToMMatrix() );

  std::string testnames[20] = 
    {"test_MMatrixConstructor", "test_MMatrixDereference", "test_MMatrixSize", "test_MMatrixBoolEq", "test_MMatrixAssignmentEq",
     "test_MMatrixPlus", "test_MMatrixMinus", "test_MMatrixTimes", "test_MMatrixDivide", "test_MMatrixOStreamOp", "test_MMatrixTrace", 
     "test_MMatrixDet", "test_MMatrixInv", "test_MMatrixT", "test_MVectorT", "test_MMatrixSub", "test_MMatrixEigen", "test_MMatrixToGSL", 
     "test_MMatrixToMMatrix" };
  bool all_testpass = true;
  for(size_t i=0; i<testpass.size(); i++) 
  {
    all_testpass &= testpass[i];
  }
  return all_testpass;
}

size_t n1=8, n2=2;

double test_data_d[16] = 
{
    1,  1,   2,   3, -5, -8, -13, -21,
   34.5, 55.7,  89.3, 144.2, -32.5,-57.5, -91.2,-146.3
};

m_complex test_data_c[16];
void build_data()
{
  for(int i=0; i<16; i++) test_data_c[i] = m_complex_build(test_data_d[i], test_data_d[15-i]);
}


bool test_MMatrixConstructor()
{
  bool   testpass = true;
  size_t n1=8, n2=2;
  MMatrix<double> mm_d0;
  MMatrix<double> mm_d1( mm_d0 ); 
  MMatrix<double> mm_d2( 10, 20 );
  MMatrix<double> mm_d3( 10, 20, 3.);
  MMatrix<double> mm_d4( n1, n2, test_data_d);
  const MMatrix<double> mm_d5(mm_d4);
  MMatrix<double> mm_d6 = MMatrix<double>::Diagonal( 4, 2, -1);
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) {double d = mm_d2(i,j); d += 1;} //check for invalid read in valgrind here
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) testpass &= mm_d3(i,j) == 3.;
  for(size_t i1=1; i1<=n1; i1++) 
    for(size_t i2=1; i2<=n2; i2++) 
      testpass &= mm_d4(i1,i2) == test_data_d[(i2-1)*8 + i1-1];
  for(size_t i1=1; i1<=4; i1++) 
    for(size_t i2=1; i2<=4; i2++) 
      testpass &= (diff(mm_d6(i1,i2), -1.) || i1==i2) &&  (diff(mm_d6(i1,i2), 2) || i1!=i2);


  MMatrix<m_complex> mm_c0;
  MMatrix<m_complex> mm_c1( mm_c0 ); 
  MMatrix<m_complex> mm_c2( 10, 20 );
  MMatrix<m_complex> mm_c3( 10, 20, m_complex_build(3,-3));
  MMatrix<m_complex> mm_c4( n1, n2, test_data_c);
  const MMatrix<m_complex> mm_c5(mm_c4);
  MMatrix<m_complex> mm_c6 = MMatrix<m_complex>::Diagonal( 4, m_complex_build(2,-3), m_complex_build(1,-6));
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) {m_complex c = mm_c2(i,j); c += m_complex_build(1,1);} //check for invalid read in valgrind here
  for(size_t i=1; i<=10; i++) 
    for(size_t j=1; j<=20; j++) testpass &= mm_c3(i,j) == m_complex_build(3,-3);
  for(size_t i1=1; i1<=n1; i1++) 
    for(size_t i2=1; i2<=n2; i2++) 
      testpass &= mm_c4(i1,i2) == test_data_c[(i2-1)*8 + i1-1];
  for(size_t i1=1; i1<=4; i1++) 
    for(size_t i2=1; i2<=4; i2++) 
      testpass &= (diff(mm_c6(i1,i2), m_complex_build(1,-6)) || i1==i2) &&  (diff(mm_c6(i1,i2), m_complex_build(2,-3)) || i1!=i2);

  return testpass;
}

bool test_MMatrixDereference()
{
  bool testpass = true;
  MMatrix<double> mm_d1( n1, n2, test_data_d);
  const MMatrix<double> mm_d2( n1, n2, test_data_d);
  for(size_t i1=0; i1<n1; i1++)
    for(size_t i2=0; i2<n2; i2++)
    {
      testpass &= mm_d1(i1+1,i2+1) == test_data_d[(i2)*8 + i1];
      testpass &= mm_d2(i1+1,i2+1) == test_data_d[(i2)*8 + i1];
      mm_d1(i1+1,i2+1) = 4;
      testpass &= mm_d1(i1+1,i2+1) == 4;
    }
  MMatrix<m_complex> mm_c1( n1, n2, test_data_c);
  const MMatrix<m_complex> mm_c2( n1, n2, test_data_c);
  for(size_t i1=0; i1<n1; i1++)
    for(size_t i2=0; i2<n2; i2++)
    {
      testpass &= mm_c1(i1+1,i2+1) == test_data_c[(i2)*8 + i1];
      testpass &= mm_c2(i1+1,i2+1) == test_data_c[(i2)*8 + i1];
      mm_c1(i1+1,i2+1) = m_complex_build(4. -4);
      testpass &= mm_c1(i1+1,i2+1) == m_complex_build(4. -4);
    }
  return testpass;
}

bool test_MMatrixSize()
{
  bool testpass = true;
  const MMatrix<double>    mm_d0;
  const MMatrix<double>    mm_d1( n1, n2, test_data_d);
  testpass &= mm_d0.num_row() == 0  && mm_d0.num_col() == 0;
  testpass &= mm_d1.num_row() == n1 && mm_d1.num_col() == n2;
  const MMatrix<m_complex> mm_c0;
  const MMatrix<m_complex> mm_c1( n1, n2, test_data_c);
  testpass &= mm_c0.num_row() == 0  && mm_c0.num_col() == 0;
  testpass &= mm_c1.num_row() == n1 && mm_c1.num_col() == n2;
  return testpass;
}

bool test_MMatrixBoolEq()
{
  bool testpass = true;
  const MMatrix<double>    mm_d0a;
  const MMatrix<double>    mm_d0b;
  const MMatrix<double>    mm_d1a( n1, n2, test_data_d);
  MMatrix<double>          mm_d1b( n1, n2, test_data_d);
  const MMatrix<double>    mm_d1c( n2, n1, 1);
  const MMatrix<m_complex> mm_c0a;
  const MMatrix<m_complex> mm_c0b;
  const MMatrix<m_complex> mm_c1a( n1, n2, test_data_c);
  MMatrix<m_complex> mm_c1b( n1, n2, test_data_c);
  const MMatrix<m_complex>    mm_c1c( n2, n1, m_complex_build(0,0));

  testpass &= mm_d0a == mm_d0b && !(mm_d0a != mm_d0b);
  testpass &= mm_d1a == mm_d1b && !(mm_d1a != mm_d1b);
  testpass &= mm_d1a != mm_d1c && !(mm_d1a == mm_d1c);
  testpass &= mm_c0a == mm_c0b && !(mm_c0a != mm_c0b);
  testpass &= mm_c1a == mm_c1b && !(mm_c1a != mm_c1b);
  testpass &= mm_c1a != mm_c1c && !(mm_c1a == mm_c1c);


  for(size_t i=1; i<mm_d1a.num_row(); i++) 
    for(size_t j=1; j<mm_d1a.num_col(); j++)
    {
      mm_d1b(i,j) = 0;
      testpass &= mm_d1a != mm_d1b && !(mm_d1a == mm_d1b);
      mm_d1b(i,j) = mm_d1a(i,j);
    }
  for(size_t i=1; i<mm_c1a.num_row(); i++) 
    for(size_t j=1; j<mm_c1a.num_col(); j++)
    {
      mm_c1b(i,j) = m_complex_build(0,0);
      testpass &= mm_c1a != mm_c1b && !(mm_c1a == mm_c1b);
      mm_c1b(i,j) = mm_c1a(i,j);
    }

  return testpass;
}

bool test_MMatrixAssignmentEq()
{

  bool testpass = true;
  const MMatrix<double>    mm_d0a;
  MMatrix<double>          mm_d0b;
  const MMatrix<double>    mm_d1a( n1, n2, test_data_d);
  MMatrix<double>          mm_d1b;
  const MMatrix<m_complex> mm_c0a;
  MMatrix<m_complex>       mm_c0b;
  const MMatrix<m_complex> mm_c1a( n1, n2, test_data_c);
  MMatrix<m_complex>       mm_c1b;

  mm_d0b = mm_d0a;
  testpass &= mm_d0b == mm_d0a;
  mm_d1b = mm_d1a;
  testpass &= mm_d1b == mm_d1a;
  mm_c0b = mm_c0a;
  testpass &= mm_c0b == mm_c0a;
  mm_c1b = mm_c1a;
  testpass &= mm_c1b == mm_c1a;
  mm_d1b = mm_d1b; //test for memory problem
  mm_c1b = mm_c1b; //test for memory problem

  return testpass;
}

bool test_MMatrixPlus()
{
  bool testpass = true;
  MMatrix<double> mm_d1a( n1, n2, test_data_d);
  MMatrix<double> mm_d1b( mm_d1a );
  mm_d1a += mm_d1a;
  for(size_t i=1; i<=mm_d1b.num_row(); i++)  
    for(size_t j=1; j<=mm_d1b.num_col(); j++)  
      testpass &= fabs(mm_d1a(i,j) - 2.*mm_d1b(i,j)) < 1e-9; 
  mm_d1a = mm_d1b + mm_d1a;
  for(size_t i=1; i<=mm_d1b.num_row(); i++)  
    for(size_t j=1; j<=mm_d1b.num_col(); j++)  
      testpass &= fabs(mm_d1a(i,j) - 3.*mm_d1b(i,j)) < 1e-9; 

  MMatrix<m_complex> mm_c1a( n1, n2, test_data_c);
  MMatrix<m_complex> mm_c1b( mm_c1a );
  mm_c1a += mm_c1a;
  for(size_t i=1; i<=mm_c1b.num_row(); i++)  
    for(size_t j=1; j<=mm_c1b.num_col(); j++)  
      testpass &= diff(mm_c1a(i,j), 2.*mm_c1b(i,j) ); 
  mm_c1a = mm_c1b + mm_c1a;
  for(size_t i=1; i<=mm_c1b.num_row(); i++)  
    for(size_t j=1; j<=mm_c1b.num_col(); j++)  
      testpass &= diff(mm_c1a(i,j), 3.*mm_c1b(i,j)); 
  
  return testpass;  
}

bool test_MMatrixMinus()
{
  bool testpass = true;
  MMatrix<double> mm_d1a( n1, n2, test_data_d);
  MMatrix<double> mm_d1b( mm_d1a );
  mm_d1a -= mm_d1a;
  for(size_t i=1; i<=mm_d1b.num_row(); i++)  
    for(size_t j=1; j<=mm_d1b.num_col(); j++)  
      testpass &= fabs(mm_d1a(i,j)) < 1e-9; 
  mm_d1a = -mm_d1b;
  for(size_t i=1; i<=mm_d1b.num_row(); i++)  
    for(size_t j=1; j<=mm_d1b.num_col(); j++)  
      testpass &= fabs(mm_d1a(i,j)+mm_d1b(i,j)) < 1e-9; 
  mm_d1a = mm_d1b;
  mm_d1a = mm_d1b - mm_d1a;
  for(size_t i=1; i<=mm_d1b.num_row(); i++)  
    for(size_t j=1; j<=mm_d1b.num_col(); j++)  
      testpass &= fabs(mm_d1a(i,j)) < 1e-9; 

  MMatrix<m_complex> mm_c1a( n1, n2, test_data_c);
  MMatrix<m_complex> mm_c1b( mm_c1a );
  mm_c1a -= mm_c1a;
  for(size_t i=1; i<=mm_c1b.num_row(); i++)  
    for(size_t j=1; j<=mm_c1b.num_col(); j++)  
      testpass &= diff(mm_c1a(i,j), m_complex_build(0,0) ); 
  mm_c1a = -mm_c1b;
  for(size_t i=1; i<=mm_d1b.num_row(); i++)  
    for(size_t j=1; j<=mm_d1b.num_col(); j++)  
      testpass &= diff(mm_c1a(i,j), -mm_c1b(i,j)); 
  mm_c1a = mm_c1b;
  mm_c1a = mm_c1b - mm_c1a;
  for(size_t i=1; i<=mm_c1b.num_row(); i++)  
    for(size_t j=1; j<=mm_c1b.num_col(); j++)  
      testpass &= diff(mm_c1a(i,j), m_complex_build(0,0) ); 
  
  return testpass;  
}

bool test_MMatrixTimes()
{
  bool testpass = true;

  MMatrix<double> mm_da( n1, n2, test_data_d);
  const MMatrix<double> mm_db( mm_da );
  mm_da *= 2.;
  for(size_t i=1; i<=mm_db.num_row(); i++)  
    for(size_t j=1; j<=mm_db.num_col(); j++)  
      testpass &= fabs(mm_da(i,j) - 2.*mm_db(i,j)) < 1e-9; 
  mm_da = mm_da*0.5;
  testpass &= diff(mm_da, mm_db); 
       
  double d1[6] = {1,2,3,7,6,5};
  double d2[6] = {3,4,5,2,8,3};
  MMatrix<double>       mm_d1( 3, 2, d1);
  MMatrix<double>       mm_d2( 2, 3, d2);
  MMatrix<double>       mm_d3( 3, 3, 0.);
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        mm_d3(i,j) += mm_d1(i,k)*mm_d2(k,j);
  mm_d1*=mm_d2;
  testpass &= diff(mm_d1, mm_d3);
  mm_d1 = MMatrix<double>(3,2,d1);
  testpass &= diff(mm_d1*mm_d2, mm_d3);

  MMatrix<m_complex> mm_ca( n1, n2, test_data_c);
  const MMatrix<m_complex> mm_cb( mm_ca );
  mm_ca *= m_complex_build(2.,-5.);
  for(size_t i=1; i<=mm_db.num_row(); i++)
    for(size_t j=1; j<=mm_db.num_col(); j++)
      testpass &= diff(mm_ca(i,j), m_complex_build(2.,-5.)*mm_cb(i,j)); 
  mm_ca = mm_ca*(1./m_complex_build(2.,-5.));
  testpass &= diff(mm_ca, mm_cb); 
       
  m_complex c1[6];
  m_complex c2[6];
  for(int i=0; i<6; i++) c1[i] = m_complex_build( d1[i],       d2[i]);
  for(int i=0; i<6; i++) c2[i] = m_complex_build(-d2[i]*d1[i], d1[i]);
  MMatrix<m_complex> mm_c1( 3, 2, c1);
  MMatrix<m_complex> mm_c2( 2, 3, c2);
  MMatrix<m_complex> mm_c3( 3, 3, m_complex_build(0,0));
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        mm_c3(i,j) += mm_c1(i,k)*mm_c2(k,j);
  mm_c1*=mm_c2;
  testpass &= diff(mm_c1, mm_c3);
  mm_c1 = MMatrix<m_complex>(3,2,c1);
  testpass &= diff(mm_c1*mm_c2, mm_c3);

  MVector<double>    mv_d = mm_d2.get_mvector(1);
  MVector<m_complex> mv_c = mm_c2.get_mvector(1);
  MVector<double>    mv_d_mult = mm_d1 * mv_d;
  MVector<m_complex> mv_c_mult = mm_c1 * mv_c;
  testpass &= diff(mv_d_mult, mm_d3.get_mvector(1));
  testpass &= diff(mv_c_mult, mm_c3.get_mvector(1));

  return testpass;  
}

bool test_MMatrixDivide()
{
  bool testpass = true;
  MMatrix<double> mm_da( n1, n2, test_data_d);
  const MMatrix<double> mm_db( mm_da );
  mm_da /= 2.;
  mm_da *= 2.;
  testpass &= diff(mm_da, mm_db);
  mm_da  = mm_da/2.;
  mm_da *= 2.;
  testpass &= diff(mm_da, mm_db); 

  MMatrix<m_complex> mm_ca( n1, n2, test_data_c);
  const MMatrix<m_complex> mm_cb( mm_ca );
  mm_ca /= m_complex_build(2.,-5.);
  mm_ca *= m_complex_build(2., -5.);
  testpass &= diff(mm_ca, mm_cb);
  mm_ca  = mm_ca/m_complex_build(2.,-5.);
  mm_ca *= m_complex_build(2., -5.);
  testpass &= diff(mm_ca, mm_cb); 

  return testpass;
}

bool test_MMatrixOStreamOp()
{
  bool testpass = true;
  std::stringstream test;
  MMatrix<double> mm_da( n1, n2, test_data_d );
  MMatrix<double> mm_db;
  test << mm_da;
  test >> mm_db;
  testpass &= diff(mm_da, mm_db);

  MMatrix<m_complex> mm_ca( n1, n2, test_data_c );
  MMatrix<m_complex> mm_cb;
  test << mm_ca;
  test >> mm_cb;
  testpass &= diff(mm_ca, mm_cb);

  return testpass;
}

bool test_MMatrixTrace()
{
  bool testpass = true;
  const MMatrix<double> mm_da( 4, 4, test_data_d );
  double trace_d = 0;
  for(size_t i=1; i<=4; i++) trace_d += mm_da(i,i);  
  testpass &= diff(trace_d, mm_da.trace());

  const MMatrix<m_complex> mm_ca( 4, 4, test_data_c );
  m_complex trace_c = m_complex_build(0);
  for(size_t i=1; i<=4; i++) trace_c += mm_ca(i,i);  
  testpass &= diff(trace_c, mm_ca.trace());
  return testpass;
}


bool test_MMatrixDet()
{
  bool   testpass = true;
  const MMatrix<double> mm_da(2,2, test_data_d);
  double det_d = mm_da(1,1)*mm_da(2,2) - mm_da(2,1)*mm_da(1,2);
  testpass &= diff(mm_da.determinant(), det_d);

  const MMatrix<m_complex> mm_ca(2,2, test_data_c);
  m_complex det_c = mm_ca(1,1)*mm_ca(2,2) - mm_ca(2,1)*mm_ca(1,2);
  testpass &= diff(mm_ca.determinant(), det_c);
  return testpass;
}

bool test_MMatrixInv()
{
  bool testpass = true;
  MMatrix<double> mm_da(4,4,test_data_d);
  MMatrix<double> mm_dinv = mm_da.inverse();
  MMatrix<double> mm_db( mm_da );
  mm_da.invert();
  testpass &= diff(mm_db*mm_dinv, MMatrix<double>::Diagonal(4, 1, 0)); //4D identity matrix
  testpass &= diff(mm_db*mm_da,   MMatrix<double>::Diagonal(4, 1, 0)); //4D identity matrix

  MMatrix<m_complex> mm_ca(4,4,test_data_c);
  MMatrix<m_complex> mm_cinv = mm_ca.inverse();
  MMatrix<m_complex> mm_cb( mm_ca );
  mm_ca.invert();
  testpass &= diff(mm_cb*mm_cinv, MMatrix<m_complex>::Diagonal(4, m_complex_build(1), m_complex_build(0))); //4D identity matrix
  testpass &= diff(mm_cb*mm_ca,   MMatrix<m_complex>::Diagonal(4, m_complex_build(1), m_complex_build(0))); //4D identity matrix

  return testpass;
}

bool test_MMatrixT()
{
  bool testpass = true;
  MMatrix<double> mm_da(n1,n2,test_data_d);  
  MMatrix<double> mm_dt = mm_da.T();
  testpass &= mm_da.num_row() == mm_dt.num_col() && mm_da.num_col() == mm_dt.num_row();
  for(size_t i1 = 1; i1<=mm_da.num_row(); i1++)
    for(size_t i2 = 1; i2<=mm_da.num_col(); i2++)
      testpass &= mm_da(i1,i2) == mm_dt(i2,i1);

  MMatrix<m_complex> mm_ca(n1,n2,test_data_c);  
  MMatrix<m_complex> mm_ct = mm_ca.T();
  testpass &= mm_ca.num_row() == mm_ct.num_col() && mm_ca.num_col() == mm_ct.num_row();
  for(size_t i1 = 1; i1<=mm_ca.num_row(); i1++)
    for(size_t i2 = 1; i2<=mm_ca.num_col(); i2++)
      testpass &= mm_ca(i1,i2) == mm_ct(i2,i1);


  return testpass;
}

bool test_MVectorT()
{
  bool testpass = true;
  MVector<double> mv_da(test_data_d, &test_data_d[n1*n2]);  
  MMatrix<double> mm_dt = mv_da.T();
  testpass &= mv_da.num_row() == mm_dt.num_col() && 1 == mm_dt.num_row();
  for(size_t i1 = 1; i1<=mm_dt.num_row(); i1++)
    for(size_t i2 = 1; i2<=mm_dt.num_col(); i2++)
      testpass &= mv_da(i2) == mm_dt(i1,i2);

  MVector<m_complex> mv_ca(test_data_c, &test_data_c[n1*n2]);  
  MMatrix<m_complex> mm_ct = mv_ca.T();
  testpass &= mv_ca.num_row() == mm_ct.num_col() && 1 == mm_ct.num_row();
  for(size_t i1 = 1; i1<=mm_ct.num_row(); i1++)
    for(size_t i2 = 1; i2<=mm_ct.num_col(); i2++)
      testpass &= mv_da(i2) == mm_dt(i1,i2);

  return testpass;
}

bool test_MMatrixSub()
{
  bool testpass = true;
  size_t k1=3,k2=4,m1=2,m2=4;
  MMatrix<double> mm_da(4,4,test_data_d);
  MMatrix<double> mm_db = mm_da.sub(k1,k2,m1,m2);
  for(size_t i=k1; i<=k2; i++)
    for(size_t j=m1; j<=m2; j++)
      testpass &= mm_db(i-k1+1, j-m1+1)  == mm_da(i,j);
  testpass &= mm_db.num_row() == k2-k1+1 && mm_db.num_col() == m2-m1+1;

  MMatrix<m_complex> mm_ca(4,4,test_data_c);
  MMatrix<m_complex> mm_cb = mm_ca.sub(k1,k2,m1,m2);
  for(size_t i=k1; i<=k2; i++)
    for(size_t j=m1; j<=m2; j++)
      testpass &= mm_cb(i-k1+1, j-m1+1) == mm_ca(i,j);
  testpass &= mm_cb.num_row() == k2-k1+1 && mm_cb.num_col() == m2-m1+1;

  return testpass;
}

bool test_MMatrixEigen()
{
  bool testpass = true;
  MMatrix<double> md(4, 4, test_data_d);
  std::pair< MVector<m_complex>, MMatrix<m_complex> > evec  = md.eigenvectors();
  MVector<m_complex> eval  = md.eigenvalues();
  for(size_t i=1; i<evec.first.num_row(); i++)
    testpass &= diff(complex(md) * evec.second.get_mvector(1), evec.second.get_mvector(1)*evec.first(1) ) ;

  MVector<m_complex> eval2 = evec.first;
  for(size_t i=1; i<=eval2.num_row(); i++)
  {
    bool test_here = false;
    for(size_t j=1; j<=eval2.num_row(); j++)
      if( diff( eval2(i),eval(j) ) ) test_here = true;
    testpass &= test_here; //pass test if for each value in eval2 there is also a value in eval and size of eval2 == size of eval
  }    
  testpass &= eval.num_row() == eval2.num_row();
  return testpass;
}


bool test_MMatrixToGSL() 
{
  bool testpass = true;
  MMatrix<double> mm_da(n1, n2, test_data_d);  
  MMatrix<double> mm_db;
  const gsl_matrix* md =  MMatrix_to_gsl(mm_da);
  try {const gsl_matrix* m0 =  MMatrix_to_gsl(mm_db); m0 = NULL; testpass = false;}
  catch(...) {}
  for(size_t i=1; i<=mm_da.num_row(); i++)
    for(size_t j=1; j<=mm_da.num_col(); j++)
      testpass &= mm_da(i,j) == gsl_matrix_get(md, i-1, j-1);

  MMatrix<m_complex> mm_ca(n1, n2, test_data_c);  
  MMatrix<m_complex> mm_cb;
  const gsl_matrix_complex* mc1 =  MMatrix_to_gsl(mm_ca);
  try {const gsl_matrix_complex* mc2 =  MMatrix_to_gsl(mm_cb); mc2 = NULL; testpass = false;}
  catch(...) {}
  for(size_t i=1; i<=mm_ca.num_row(); i++)
    for(size_t j=1; j<=mm_ca.num_col(); j++)
      testpass &= mm_ca(i,j) == gsl_matrix_complex_get(mc1, i-1, j-1);

  return testpass;
}

bool test_MMatrixToMMatrix()
{
  bool testpass = true;
  MMatrix<m_complex> mm_ca(n1, n2, test_data_c);  
  MMatrix<double>    mm_dr = re(mm_ca);  
  MMatrix<double>    mm_di = im(mm_ca);  
  MMatrix<m_complex> mm_cb = complex(mm_dr, mm_di);
  testpass &= diff(mm_cb, mm_ca);

  MMatrix<m_complex> mm_cc = complex(mm_dr);
  for(size_t i=1; i<mm_ca.num_row(); i++)
    for(size_t j=1; j<mm_ca.num_col(); j++)
      testpass &= diff( mm_cc(i,j), m_complex_build( re( mm_ca(i,j) ) ) );

  for(size_t i=1; i<=mm_ca.num_col(); i++)
  {
    MVector<double>    mv_d = mm_dr.get_mvector(i);
    MVector<m_complex> mv_c = mm_ca.get_mvector(i);
    for(size_t j=1; j<mm_ca.num_row(); j++)
    {
      testpass &= diff( mm_ca(j,i), mv_c(j) );
      testpass &= diff( mm_dr(j,i), mv_d(j) );
    }
  }
  return testpass;
}


TEST(MMatrixTest, old_unit_tests)
{
  EXPECT_TRUE(test_MMatrix());
}



