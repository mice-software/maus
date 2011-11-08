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


bool test_complex();

bool diff(const MAUS::complex& c1, const MAUS::complex& c2)
{
  return     fabs(MAUS::Complex::real(c1) - MAUS::Complex::real(c2)) < 1e-9
          && fabs(MAUS::Complex::imag(c1) - MAUS::Complex::imag(c2) ) < 1e-9;
}
bool diff(double c1, double c2) { return fabs(c1-c2) < 1e-9; }

bool test_complex()
{
  std::ofstream voidout("/dev/null");
  std::ostream& out = voidout;//std::cout; //

  bool testpass = true;
  out << "\n========= MAUS::complex ==========" << std::endl;
  out << std::boolalpha;

  MAUS::complex c1 = MAUS::Complex::complex(3,  -2);
  MAUS::complex c2 = MAUS::Complex::complex(3,  -2);
  MAUS::complex c3 = MAUS::Complex::complex(3,   2);
  MAUS::complex c4 = MAUS::Complex::complex(-3, -2);
  MAUS::complex ct;
  const MAUS::complex cc = MAUS::Complex::complex( 3, -2);

  //operator ==
  testpass &=   (c1 == c2);
  testpass &= ! (c1 == c3);
  testpass &= ! (c1 == c4);
  out << "operator ==      " << testpass << std::endl;

  //operator !=
  testpass &= ! (c1 != c2);
  testpass &=   (c1 != c3);
  testpass &=   (c1 != c4);
  out << "operator !=      " << testpass << std::endl;

  //real and im functions
  testpass &= fabs( (MAUS::Complex::real(c1) - 3) ) < 1e-9;
  testpass &= fabs( (MAUS::Complex::imag(c1)   + 2) ) < 1e-9;
  testpass &= fabs( (MAUS::Complex::real(cc) - 3) ) < 1e-9;
  testpass &= fabs( (MAUS::Complex::imag(cc)   + 2) ) < 1e-9;
  out << "real/im          " << testpass << std::endl;

  //complext conjugate
  testpass &= MAUS::Complex::conj(MAUS::Complex::complex(3,-2)) == MAUS::Complex::complex(3,2);
  testpass &= MAUS::Complex::conj(MAUS::Complex::complex(3))    == MAUS::Complex::complex(3);
  out << "conj             " << testpass << std::endl;

  //operator =
  ct = c1;
  testpass &= ct == c1;
  out << "operator =       " << testpass << std::endl;

  //multiplication
  ct = ct*2.;
  testpass &= MAUS::Complex::real(ct) == MAUS::Complex::real(c1)*2. && MAUS::Complex::imag(ct) == MAUS::Complex::imag(c1)*2.;
  ct *= 2.;
  testpass &= MAUS::Complex::real(ct) == MAUS::Complex::real(c1)*4. && MAUS::Complex::imag(ct) == MAUS::Complex::imag(c1)*4.;
  ct = 2.*ct;
  testpass &= MAUS::Complex::real(ct) == MAUS::Complex::real(c1)*8. && MAUS::Complex::imag(ct) == MAUS::Complex::imag(c1)*8.;

  ct  = c1;
  ct *= c3;
  testpass &= MAUS::Complex::real(ct) == MAUS::Complex::real(c1)*MAUS::Complex::real(c3)-MAUS::Complex::imag(c1)*MAUS::Complex::imag(c3) 
  && MAUS::Complex::imag(ct) == MAUS::Complex::imag(c1)*MAUS::Complex::real(c3)+MAUS::Complex::imag(c3)*MAUS::Complex::real(c1);
  ct  = c1*c3;
  testpass &= MAUS::Complex::real(ct) == MAUS::Complex::real(c1)*MAUS::Complex::real(c3)-MAUS::Complex::imag(c1)*MAUS::Complex::imag(c3) 
  && MAUS::Complex::imag(ct) == MAUS::Complex::imag(c1)*MAUS::Complex::real(c3)+MAUS::Complex::imag(c3)*MAUS::Complex::real(c1);
  out << "operator *, *=   " << testpass << std::endl;  
  
  ct = c1;
  ct = ct/2.;
  ct = ct*2.;
  testpass &= diff(c1, ct);
  ct /= 2.;
  ct *= 2.;
  testpass &= diff(ct,c1);
  ct  = 2./c1;
  ct *= c1;
  testpass &= diff(ct, MAUS::Complex::complex(2,0));
  ct  = c1;
  ct /= c3;
  ct *= c3;
  testpass &= diff(ct , c1);
  ct  = c1/c3;
  ct *= c3; 
  testpass &= diff(ct, c1);
  out << "operator /, /=   " << testpass << std::endl;  
  
  ct =  MAUS::Complex::complex(3,-2);
  ct += 2.;
  testpass &= diff(ct, MAUS::Complex::complex(5,-2));
  ct =  MAUS::Complex::complex(3,-2) + 2.;
  testpass &= diff(ct, MAUS::Complex::complex(5,-2));
  ct =  2.+MAUS::Complex::complex(3,-2);
  testpass &= diff(ct, MAUS::Complex::complex(5,-2));
  ct =  MAUS::Complex::complex(3,-2) + MAUS::Complex::complex(5,-1);
  testpass &= diff(ct, MAUS::Complex::complex(8,-3));
  ct  = MAUS::Complex::complex(3,-2);
  ct += MAUS::Complex::complex(5,-1);
  testpass &= diff(ct, MAUS::Complex::complex(8,-3));
  out << "operator +, +=   " << testpass << std::endl;  

  ct =  -MAUS::Complex::complex(3,-2);
  testpass &= diff(ct, MAUS::Complex::complex(-3,2));
  ct =  MAUS::Complex::complex(3,-2);
  ct -= 2.;
  testpass &= diff(ct, MAUS::Complex::complex(1,-2));
  ct =  MAUS::Complex::complex(3,-2) - 2.;
  testpass &= diff(ct, MAUS::Complex::complex(1,-2));
  ct =  2. - MAUS::Complex::complex(3,-2);
  testpass &= diff(ct, MAUS::Complex::complex(-1,2));
  ct =  MAUS::Complex::complex(3,-2) - MAUS::Complex::complex(5,-1);
  testpass &= diff(ct, MAUS::Complex::complex(-2,-1));
  ct  = MAUS::Complex::complex(3,-2);
  ct -= MAUS::Complex::complex(5,-1);
  testpass &= diff(ct, MAUS::Complex::complex(-2,-1));
  out << "operator -, -=   " << testpass << std::endl;  

  std::stringstream test_stream; 
  test_stream << c1;
  test_stream >> ct;
  testpass &= ct == c1;
  out << "operator <<, >>  " << testpass << std::endl;  

  return testpass; 
}

TEST(ComplexTest, all) {
  EXPECT_TRUE(test_complex());
}



