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

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"

#include "Interface/Complex.hh"

using namespace MAUS::Complex;

bool diff(const MAUS::complex c1, const MAUS::complex c2)
{
  return     fabs(real(c1) - real(c2)) < 1e-9
          && fabs(imag(c1) - imag(c2) ) < 1e-9;
}

bool diff(double c1, double c2) { return fabs(c1-c2) < 1e-9; }

class ComplexTest : public testing::Test
{
public:
  ComplexTest() : c1(complex(3,  -2)),
                  c2(complex(3,  -2)),
                  c3(complex(3,   2)),
                  c4(complex(3,  -2)),
                  cs(complex(6.)),
                  cc(complex(3,  -2)),
                  voidout("/dev/null"),
                  out(voidout)
  { }
protected:
  MAUS::complex c1;
  MAUS::complex c2;
  MAUS::complex c3;
  MAUS::complex c4;
  MAUS::complex cs;
  MAUS::complex ct;
  const MAUS::complex cc;
  std::ofstream voidout;
  std::ostream& out;
};

TEST_F(ComplexTest, Equals) {
  //operator ==
  ASSERT_TRUE(c1 == c2);
  ASSERT_FALSE(c1 == c3);
  ASSERT_TRUE(c1 == c4);
}

TEST_F(ComplexTest, NotEquals) {
  //operator !=
  ASSERT_FALSE(c1 != c2);
  ASSERT_TRUE(c1 != c3);
  ASSERT_FALSE(c1 != c4);
}

TEST_F(ComplexTest, Components) {
  //real and imag functions
  EXPECT_TRUE(fabs((real(c1) - 3) ) < 1e-9);
  EXPECT_TRUE(fabs((imag(c1)   + 2) ) < 1e-9);
  EXPECT_TRUE(fabs((real(cc) - 3) ) < 1e-9);
  EXPECT_TRUE(fabs((imag(cc)   + 2) ) < 1e-9);
}

TEST_F(ComplexTest, Double) {
  ASSERT_TRUE(real(cs) == 6.);
  ASSERT_TRUE(fabs(imag(cs)) < 1e-9);
}

TEST_F(ComplexTest, Conjugate) {
  //complext conjugate
  EXPECT_TRUE(conj(complex(3,-2)) == complex(3,2));
  EXPECT_TRUE(conj(complex(3)) == complex(3));
}

TEST_F(ComplexTest, Assignment) {
  //operator =
  ct = c1;
  EXPECT_TRUE(ct == c1);
}

TEST_F(ComplexTest, Multiplication) {
  //multiplication
  ct = c1*2.;
  EXPECT_TRUE(real(ct) == real(c1)*2. && imag(ct) == imag(c1)*2.);
  ct *= 2.;
  EXPECT_TRUE(real(ct) == real(c1)*4. && imag(ct) == imag(c1)*4.);

  fprintf(stdout, "ct = (12,-8) == (%f,%f)\n", real(ct), imag(ct));
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
  EXPECT_TRUE(diff(c1, ct));
  ct /= 2.;
  ct *= 2.;
  EXPECT_TRUE(diff(ct,c1));
  ct  = 2./c1;
  ct *= c1;
  EXPECT_TRUE(diff(ct, complex(2,0)));
  ct  = c1;
  ct /= c3;
  ct *= c3;
  EXPECT_TRUE(diff(ct , c1));
  ct  = c1/c3;
  ct *= c3; 
  EXPECT_TRUE(diff(ct, c1));
}

TEST_F(ComplexTest, Addition) {
  ct =  complex(3,-2);
  ct += 2.;
  EXPECT_TRUE(diff(ct, complex(5,-2)));
  ct =  complex(3,-2) + 2.;
  EXPECT_TRUE(diff(ct, complex(5,-2)));
  ct =  2.+complex(3,-2);
  EXPECT_TRUE(diff(ct, complex(5,-2)));
  ct =  complex(3,-2) + complex(5,-1);
  EXPECT_TRUE(diff(ct, complex(8,-3)));
  ct  = complex(3,-2);
  ct += complex(5,-1);
  EXPECT_TRUE(diff(ct, complex(8,-3)));
}

TEST_F(ComplexTest, Negation) {
  ct =  -complex(3,-2);
  EXPECT_TRUE(diff(ct, complex(-3,2)));
}

TEST_F(ComplexTest, Subtraction) {
  ct =  complex(3,-2);
  ct -= 2.;
  EXPECT_TRUE(diff(ct, complex(1,-2)));
  ct =  complex(3,-2) - 2.;
  EXPECT_TRUE(diff(ct, complex(1,-2)));
  ct =  2. - complex(3,-2);
  EXPECT_TRUE(diff(ct, complex(-1,2)));
  ct =  complex(3,-2) - complex(5,-1);
  EXPECT_TRUE(diff(ct, complex(-2,-1)));
  ct  = complex(3,-2);
  ct -= complex(5,-1);
  EXPECT_TRUE(diff(ct, complex(-2,-1)));
}

TEST_F(ComplexTest, Streaming) {
  std::stringstream test_stream; 
  test_stream << c1;
  test_stream >> ct;
/*
  fprintf(stdout, "(3,-2) * 8.0 == (%f,%f)\n", real(ct), imag(ct));
  fflush(stdout);
 */
  EXPECT_TRUE(ct == c1);
}
