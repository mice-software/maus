//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//xboa is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with xboa in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"

#include "Interface/Complex.hh"
#include "Interface/MMatrix.hh"
#include "Interface/Vector.hh"

using namespace MAUS;

//Defined in ComplexTest.cc
bool equal(const MAUS::complex c1, const MAUS::complex c2);
bool equal(double c1, double c2);

class VectorTest : public testing::Test
{
public:
  VectorTest()
    : d_mv1(4),
      d_mv2(Vector<double>(4, 4.)),
      d_mv3(Vector<double>(&da[0], 3)),
      d_mv6(d_mv1),
      c_mv1(Vector<MAUS::complex>(4)),
      c_mv2(Vector<MAUS::complex>(4, MAUS::Complex::complex(1.,1.))),
      c_mv3(Vector<MAUS::complex>(&ca[0], 3)),
      c_mv6(c_mv1)
  {
  }

protected:
  static const double da[];
  Vector<double> d_mv0;
  Vector<double> d_mv1;
  Vector<double> d_mv2;
  Vector<double> d_mv3;
  static const Vector<double> d_mv4;
  static const Vector<double> d_mv5;
  Vector<double> d_mv6;
  
  static const MAUS::complex ca[3];
  Vector<MAUS::complex> c_mv0;
  Vector<MAUS::complex> c_mv1;
  Vector<MAUS::complex> c_mv2;
  Vector<MAUS::complex> c_mv3;
  static const Vector<MAUS::complex> c_mv4;
  static const Vector<MAUS::complex> c_mv5;
  Vector<MAUS::complex> c_mv6;
};

//****************************************
//VectorTest static const initializations
//****************************************

const double VectorTest::da[3] = {1., 2., 3.};
const Vector<double> VectorTest::d_mv4
  = Vector<double>(std::vector<double>(&da[0], &da[3]));
const Vector<double> VectorTest::d_mv5 = d_mv4;

const MAUS::complex VectorTest::ca[3] = {MAUS::Complex::complex(1.,-1.),
                                         MAUS::Complex::complex(2.,0.),
                                         MAUS::Complex::complex(3.,1.)};
const Vector<MAUS::complex> VectorTest::c_mv4
  = Vector<MAUS::complex>(std::vector<MAUS::complex>(&ca[0], &ca[3]));
const Vector<MAUS::complex> VectorTest::c_mv5 = c_mv4;

//***********
//test cases
//***********

TEST_F(VectorTest, Size) {
  ASSERT_TRUE(d_mv1.size() == 4);
  ASSERT_TRUE(d_mv2.size() == 4);
  ASSERT_TRUE(d_mv3.size() == 3);
  ASSERT_TRUE(d_mv4.size() == 3);
  ASSERT_TRUE(c_mv1.size() == 4);
  ASSERT_TRUE(c_mv2.size() == 4);
  ASSERT_TRUE(c_mv3.size() == 3);
  ASSERT_TRUE(c_mv4.size() == 3);
}

TEST_F(VectorTest, Subvector) {
//FIXME: crashes with "Abort trap"
  size_t sub1=2, sub2=3;
  Vector<double> d_mv_sub = d_mv4.subvector(sub1,sub2);
  ASSERT_TRUE(d_mv_sub.size() == size_t(sub2-sub1));
  for(size_t i=0; i<d_mv_sub.size(); ++i)
    ASSERT_TRUE(d_mv_sub[i] == d_mv4[i+sub1]);

  Vector<MAUS::complex> c_mv_sub = c_mv4.subvector(sub1,sub2);
  ASSERT_TRUE(c_mv_sub.size() == size_t(sub2-sub1));
  for(size_t i=0; i<c_mv_sub.size(); ++i)
    ASSERT_TRUE(c_mv_sub[i] == c_mv4[i+sub1]);
}

TEST_F(VectorTest, Indexing) {
  for(size_t i=0; i<d_mv2.size(); ++i) ASSERT_TRUE(d_mv2(i+1) == 4.);
  for(size_t i=0; i<d_mv3.size(); ++i) ASSERT_TRUE(d_mv3(i+1) == da[i]);
  for(size_t i=0; i<d_mv4.size(); ++i) ASSERT_TRUE(d_mv4(i+1) == da[i]);
  for(size_t i=0; i<d_mv5.size(); ++i) ASSERT_TRUE(d_mv5(i+1) == da[i]);
  for(size_t i=0; i<c_mv2.size(); ++i)
    ASSERT_TRUE(c_mv2(i+1) == MAUS::Complex::complex(1.,1.));
  for(size_t i=0; i<c_mv3.size(); ++i) ASSERT_TRUE(c_mv3(i+1) == ca[i]);
  for(size_t i=0; i<c_mv4.size(); ++i) ASSERT_TRUE(c_mv4(i+1) == ca[i]);
  for(size_t i=0; i<c_mv5.size(); ++i) ASSERT_TRUE(c_mv5(i+1) == ca[i]);

  for(size_t i=1; i<=d_mv2.size(); ++i) ASSERT_TRUE(d_mv2(i) == 4.);
  for(size_t i=1; i<=d_mv3.size(); ++i) ASSERT_TRUE(d_mv3(i) == da[i-1]);
  for(size_t i=1; i<=c_mv2.size(); ++i)
    ASSERT_TRUE(c_mv2(i) == MAUS::Complex::complex(1.,1.));
  for(size_t i=1; i<=c_mv3.size(); ++i) ASSERT_TRUE(c_mv3(i) == ca[i-1]);

  d_mv2(2+1) = 2.;
  EXPECT_TRUE(d_mv2(2+1) == 2.);
  d_mv2(2+1) = 4.;

  c_mv2(2+1) = MAUS::Complex::complex(-2.,2.);
  EXPECT_TRUE(c_mv2(2+1) == MAUS::Complex::complex(-2.,2.));
  c_mv2(2+1) = MAUS::Complex::complex(1.,1.);
}

TEST_F(VectorTest, Equals) {
  EXPECT_TRUE(d_mv5 == d_mv4);
  EXPECT_TRUE(c_mv5 == c_mv4);
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

TEST_F(VectorTest, Assignment) {
  d_mv6 = d_mv3;
  d_mv6 = d_mv6; //check for specific subtle bug when self-allocating
  EXPECT_TRUE(d_mv6 == d_mv3); //should be exactly ==
  for(size_t i=0; i<d_mv3.size(); ++i) d_mv6(i+1) = 2.*d_mv6(i+1);
  EXPECT_TRUE(d_mv6 != d_mv3); //verify deepcopy

  c_mv6 = c_mv3;
  c_mv6 = c_mv6; //check for specific subtle bug when self-allocating
  EXPECT_TRUE(c_mv6 == c_mv3); //should be exactly ==
  for(size_t i=0; i<c_mv3.size(); ++i) c_mv6(i+1) = 2.*c_mv6(i+1);
  EXPECT_TRUE(c_mv6 != c_mv3); //verify deepcopy)
}

TEST_F(VectorTest, Multiplication) {
  d_mv3 *= 2.;
  for(size_t i=0; i<d_mv3.size(); ++i)
    ASSERT_TRUE(fabs(d_mv3(i+1) - da[i]*2.) < 1e-9);

  c_mv3 *= MAUS::Complex::complex(2.,-2.);
  for(size_t i=0; i<c_mv3.size(); ++i)
    ASSERT_TRUE(equal(c_mv3(i+1), ca[i]*MAUS::Complex::complex(2.,-2.)));

  d_mv3 = d_mv4 * 2.;
  for(size_t i=0; i<d_mv3.size(); ++i)
    ASSERT_TRUE(fabs(d_mv3(i+1) - da[i]*2.) < 1e-9);

  c_mv3 = c_mv4 * MAUS::Complex::complex(2.,-2.);
  for(size_t i=0; i<c_mv3.size(); ++i)
    ASSERT_TRUE(equal(c_mv3(i+1), ca[i]*MAUS::Complex::complex(2.,-2.)));

  d_mv3 = 2. * d_mv4;
  for(size_t i=0; i<d_mv3.size(); ++i)
    ASSERT_TRUE(fabs(d_mv3(i+1) - da[i]*2.) < 1e-9);

  c_mv3 = MAUS::Complex::complex(2.,-2.) * c_mv4;
  for(size_t i=0; i<c_mv3.size(); ++i)
    ASSERT_TRUE(equal(c_mv3(i+1), ca[i]*MAUS::Complex::complex(2.,-2.)));

  d_mv3 = d_mv4;
  c_mv3 = c_mv4;
}

TEST_F(VectorTest, Division) {
  d_mv3 /= 2.;
  for(size_t i=0; i<d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]/2.) < 1e-9);
  d_mv3 = d_mv4;

  c_mv3 /= MAUS::Complex::complex(2., -3);
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]/MAUS::Complex::complex(2.,-3)));
  c_mv3 = c_mv4;

  d_mv3 = d_mv3 / 2.;
  for(size_t i=0; i<d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]/2.) < 1e-9);
  d_mv3 = d_mv4;

  c_mv3 = c_mv3 / MAUS::Complex::complex(2., -3);
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]/MAUS::Complex::complex(2.,-3)));
  c_mv3 = c_mv4;
}

TEST_F(VectorTest, Addition) {
  d_mv3 += (d_mv3/2.);
  for(size_t i=0; i<d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]*(1.+1./2.)) < 1e-9);
  d_mv3  = d_mv4;

  c_mv3 += (c_mv3/MAUS::Complex::complex(3,-2));
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]*(1.+1./MAUS::Complex::complex(3,-2))));
  c_mv3  = c_mv4;

  d_mv3  = d_mv3 + (d_mv3/2.);
  for(size_t i=0; i<d_mv3.size(); ++i)
  {
    EXPECT_TRUE(fabs(d_mv3[i] - da[i]*(1.+1./2.) ) < 1e-9);
    }
  d_mv3  = d_mv4;

  c_mv3  = c_mv3 + (c_mv3/MAUS::Complex::complex(3,-2));
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal(c_mv3[i], ca[i]*(1.+1./MAUS::Complex::complex(3,-2))));
  c_mv3  = c_mv4;
}

TEST_F(VectorTest, Inversion) {
  for(size_t i=0; i<d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(  (-d_mv3)(i+1)
                     + d_mv3(i+1)) < 1e-9);
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal((-c_mv3)(i+1), -(c_mv3(i+1))));  
}

TEST_F(VectorTest, Subtraction) {
  d_mv3 -= d_mv4;
  c_mv3 -= c_mv4;
  for(size_t i=0; i<d_mv3.size(); ++i)
    EXPECT_TRUE(fabs(d_mv3(i+1)) < 1e-9);
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal((-c_mv3)(i+1), MAUS::Complex::complex(0,0)));
  d_mv3 = d_mv4;
  c_mv3 = c_mv4;
  d_mv3 = d_mv3 - d_mv4;
  c_mv3 = c_mv3 - c_mv4;
  for(size_t i=0; i<d_mv3.size(); ++i)
    EXPECT_TRUE(fabs( d_mv3(i+1) ) < 1e-9);
  for(size_t i=0; i<c_mv3.size(); ++i)
    EXPECT_TRUE(equal((-c_mv3)(i+1), MAUS::Complex::complex(0,0)));
}

TEST_F(VectorTest, Streaming) {
  std::stringstream ss;
  ss  << d_mv4 << "\n";
  Vector<double>    d_mvio;
  Vector<MAUS::complex> c_mvio;
  ss >> d_mvio;
  ss << c_mv4 << std::endl;
  ss >> c_mvio;
  for(size_t i=1; i<=d_mvio.size(); ++i)
    ASSERT_TRUE(equal(d_mvio(i), d_mv4(i)));
  for(size_t i=1; i<=c_mvio.size(); ++i)
    ASSERT_TRUE(equal(c_mvio(i), c_mv4(i)));
}
