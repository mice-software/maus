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

#include "Interface/MMatrix.hh"
#include "Interface/MVector.hh"

bool test_m_complex();
bool test_MVector();

bool diff(m_complex c1, m_complex c2) { return fabs(re(c1) - re(c2)) < 1e-9 && fabs(im(c1) - im(c2) ) < 1e-9; }
bool diff(double    c1, double    c2) { return fabs(c1-c2) < 1e-9; }

template <class Tmplt> bool diff(MMatrix<Tmplt> m1, MMatrix<Tmplt> m2)
{ 
  if(m1.num_row() != m2.num_row() || m1.num_col() != m2.num_col()) 
    return false;
  for(size_t i=1; i<m1.num_row(); i++) 
    for(size_t j=1; j<m1.num_col(); j++) 
      return diff(m1(i,j), m2(i,j));
  return true;
}

std::ofstream voidout("/dev/null");
std::ostream& out = voidout;//std::cout; //

bool test_m_complex()
{
  bool testpass = true;
  out << "\n========= m_complex ==========" << std::endl;
  out << std::boolalpha;

  m_complex c1 = m_complex_build(3,  -2);
  m_complex c2 = m_complex_build(3,  -2);
  m_complex c3 = m_complex_build(3,   2);
  m_complex c4 = m_complex_build(-3, -2);
  m_complex ct;
  const m_complex cc = m_complex_build( 3, -2);

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
  testpass &= fabs( (re(c1) - 3) ) < 1e-9;
  testpass &= fabs( (im(c1)   + 2) ) < 1e-9;
  testpass &= fabs( (re(cc) - 3) ) < 1e-9;
  testpass &= fabs( (im(cc)   + 2) ) < 1e-9;
  out << "real/im          " << testpass << std::endl;

  //complext conjugate
  testpass &= conj(m_complex_build(3,-2)) == m_complex_build(3,2);
  testpass &= conj(m_complex_build(3))    == m_complex_build(3);
  out << "conj             " << testpass << std::endl;

  //operator =
  ct = c1;
  testpass &= ct == c1;
  out << "operator =       " << testpass << std::endl;

  //multiplication
  ct = ct*2.;
  testpass &= re(ct) == re(c1)*2. && im(ct) == im(c1)*2.;
  ct *= 2.;
  testpass &= re(ct) == re(c1)*4. && im(ct) == im(c1)*4.;
  ct = 2.*ct;
  testpass &= re(ct) == re(c1)*8. && im(ct) == im(c1)*8.;

  ct  = c1;
  ct *= c3;
  testpass &= re(ct) == re(c1)*re(c3)-im(c1)*im(c3) 
           && im(ct) == im(c1)*re(c3)+im(c3)*re(c1);
  ct  = c1*c3;
  testpass &= re(ct) == re(c1)*re(c3)-im(c1)*im(c3) 
           && im(ct) == im(c1)*re(c3)+im(c3)*re(c1);
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
  testpass &= diff(ct, m_complex_build(2,0));
  ct  = c1;
  ct /= c3;
  ct *= c3;
  testpass &= diff(ct , c1);
  ct  = c1/c3;
  ct *= c3; 
  testpass &= diff(ct, c1);
  out << "operator /, /=   " << testpass << std::endl;  
  
  ct =  m_complex_build(3,-2);
  ct += 2.;
  testpass &= diff(ct, m_complex_build(5,-2));
  ct =  m_complex_build(3,-2) + 2.;
  testpass &= diff(ct, m_complex_build(5,-2));
  ct =  2.+m_complex_build(3,-2);
  testpass &= diff(ct, m_complex_build(5,-2));
  ct =  m_complex_build(3,-2) + m_complex_build(5,-1);
  testpass &= diff(ct, m_complex_build(8,-3));
  ct  = m_complex_build(3,-2);
  ct += m_complex_build(5,-1);
  testpass &= diff(ct, m_complex_build(8,-3));
  out << "operator +, +=   " << testpass << std::endl;  

  ct =  -m_complex_build(3,-2);
  testpass &= diff(ct, m_complex_build(-3,2));
  ct =  m_complex_build(3,-2);
  ct -= 2.;
  testpass &= diff(ct, m_complex_build(1,-2));
  ct =  m_complex_build(3,-2) - 2.;
  testpass &= diff(ct, m_complex_build(1,-2));
  ct =  2. - m_complex_build(3,-2);
  testpass &= diff(ct, m_complex_build(-1,2));
  ct =  m_complex_build(3,-2) - m_complex_build(5,-1);
  testpass &= diff(ct, m_complex_build(-2,-1));
  ct  = m_complex_build(3,-2);
  ct -= m_complex_build(5,-1);
  testpass &= diff(ct, m_complex_build(-2,-1));
  out << "operator -, -=   " << testpass << std::endl;  

  std::stringstream test_stream; 
  test_stream << c1;
  test_stream >> ct;
  testpass &= ct == c1;
  out << "operator <<, >>  " << testpass << std::endl;  

  return testpass; 
}

bool test_MVector() 
{
  bool      testpass = true;
  out << "\n========= MVector ==========" << std::endl;

  double    da[3] = {1.,2.,3.};
  m_complex ca[3] = {m_complex_build(1.,-1.), m_complex_build(2.,0.),  m_complex_build(3., 1.)};//
  MVector<double>    d_mv0;
  MVector<double>    d_mv1(4);
  MVector<double>    d_mv2(4, 4.);
  MVector<double>    d_mv3(&da[0], &da[3]);
  const MVector<double>    d_mv4(std::vector<double>(&da[0], &da[3]) );
  MVector<double>    d_mv5(d_mv4);

  MVector<m_complex> c_mv0;
  MVector<m_complex> c_mv1(4);
  MVector<m_complex> c_mv2(4, m_complex_build(1.,1.) );
  MVector<m_complex> c_mv3(&ca[0], &ca[3]);
  const MVector<m_complex> c_mv4(std::vector<m_complex>(&ca[0], &ca[3]) );
  MVector<m_complex> c_mv5(c_mv4);
  out << "constructor      " << testpass << std::endl;  

  testpass &= d_mv1.num_row() == 4 && d_mv2.num_row() == 4 && d_mv3.num_row() == 3 && d_mv4.num_row() == 3;
  testpass &= c_mv1.num_row() == 4 && c_mv2.num_row() == 4 && c_mv3.num_row() == 3 && c_mv4.num_row() == 3;
  out << "num_row()        " << testpass << std::endl; 

  for(size_t i=0; i<d_mv2.num_row(); i++) testpass &= d_mv2(i+1) == 4.;
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= d_mv3(i+1) == da[i];
  for(size_t i=0; i<d_mv4.num_row(); i++) testpass &= d_mv4(i+1) == da[i];
  for(size_t i=0; i<d_mv5.num_row(); i++) testpass &= d_mv5(i+1) == da[i];
  for(size_t i=0; i<c_mv2.num_row(); i++) testpass &= c_mv2(i+1) == m_complex_build(1.,1.);
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= c_mv3(i+1) == ca[i];
  for(size_t i=0; i<c_mv4.num_row(); i++) testpass &= c_mv4(i+1) == ca[i];
  for(size_t i=0; i<c_mv5.num_row(); i++) testpass &= c_mv5(i+1) == ca[i];

  for(size_t i=1; i<=d_mv2.num_row(); i++) testpass &= d_mv2(i) == 4.;
  for(size_t i=1; i<=d_mv3.num_row(); i++) testpass &= d_mv3(i) == da[i-1];
  for(size_t i=1; i<=c_mv2.num_row(); i++) testpass &= c_mv2(i) == m_complex_build(1.,1.);
  for(size_t i=1; i<=c_mv3.num_row(); i++) testpass &= c_mv3(i) == ca[i-1];

  d_mv2(2+1) = 2.;
  c_mv2(2+1) = m_complex_build(-2.,2.);
  testpass &= d_mv2(2+1) == 2. && c_mv2(2+1) == m_complex_build(-2.,2.); //and that should be exact
  d_mv2(2+1) = 4.;
  c_mv2(2+1) = m_complex_build(1.,1.);

  out << "operator()       " << testpass << std::endl; 

  d_mv3(1) *= -1;
  c_mv3(1) *= -1;
  testpass &= d_mv5 == d_mv4;
  testpass &= d_mv5 != d_mv0 && d_mv5 != d_mv1 && d_mv5 != d_mv2 && d_mv5 != d_mv3;
  testpass &= c_mv5 == c_mv4;
  testpass &= c_mv5 != c_mv0 && c_mv5 != c_mv1 && c_mv5 != c_mv2 && c_mv5 != c_mv3;
  d_mv3(1) *= -1;
  c_mv3(1) *= -1;
  out << "operator==, !=   " << testpass << std::endl;

  MVector<double>    d_mv6( d_mv1 );
  d_mv6 = d_mv3;
  d_mv6 = d_mv6; //check for specific subtle bug when self-allocating
  testpass &= d_mv6 == d_mv3; //should be exactly ==
  for(size_t i=0; i<d_mv3.num_row(); i++) d_mv6(i+1) = 2.*d_mv6(i+1);
  testpass &= d_mv6 != d_mv3; //should not be equal (i.e. check it really is a deepcopy)

  MVector<m_complex>    c_mv6( c_mv1 );
  c_mv6 = c_mv3;
  c_mv6 = c_mv6; //check for specific subtle bug when self-allocating
  testpass &= c_mv6 == c_mv3; //should be exactly ==
  for(size_t i=0; i<c_mv3.num_row(); i++) c_mv6(i+1) = 2.*c_mv6(i+1);
  testpass &= c_mv6 != c_mv3; //should not be equal (i.e. check it really is a deepcopy)

  out << "operator=        " << testpass << std::endl;

  d_mv3 *= 2.;
  c_mv3 *= m_complex_build(2.,-2.);
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]*2.) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]*m_complex_build(2.,-2.) );
  d_mv3 = d_mv4 * 2.;
  c_mv3 = c_mv4 * m_complex_build(2.,-2.);
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]*2.) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]*m_complex_build(2.,-2.) );
  d_mv3 = 2. * d_mv4;
  c_mv3 = m_complex_build(2.,-2.) * c_mv4;
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]*2.) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]*m_complex_build(2.,-2.) );
  d_mv3 = d_mv4;
  c_mv3 = c_mv4;
  out << "operator*, *=    " << testpass << std::endl;


  d_mv3 /= 2.;
  c_mv3 /= m_complex_build(2., -3);
  d_mv3 = d_mv3 * 2.;
  c_mv3 = c_mv3 * m_complex_build(2., -3);
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]);
  d_mv3 = d_mv3 / 2.;
  c_mv3 = c_mv3 / m_complex_build(2., -3);
  d_mv3 = d_mv3 * 2.;
  c_mv3 = c_mv3 * m_complex_build(2., -3);
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]);
  out << "operator/, /=    " << testpass << std::endl;

  d_mv3 += (d_mv3/2.);
  c_mv3 += (c_mv3/m_complex_build(3,-2));
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]*(1.+1./2.) ) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]*(1.+1./m_complex_build(3,-2)) );
  d_mv3  = d_mv4;
  c_mv3  = c_mv4;
  d_mv3  = d_mv3+(d_mv3/2.);
  c_mv3  = c_mv3+(c_mv3/m_complex_build(3,-2));
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs(d_mv3(i+1) - da[i]*(1.+1./2.) ) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff(c_mv3(i+1), ca[i]*(1.+1./m_complex_build(3,-2)) );
  d_mv3  = d_mv4;
  c_mv3  = c_mv4;
  out << "operator+, +=    " << testpass << std::endl;

  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs((-d_mv3)(i+1) + d_mv3(i+1) ) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff( (-c_mv3)(i+1), -(c_mv3(i+1)) );

  d_mv3 -= d_mv4;
  c_mv3 -= c_mv4;
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs( d_mv3(i+1) ) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff( (-c_mv3)(i+1), m_complex_build(0,0) );
  d_mv3  = d_mv4;
  c_mv3  = c_mv4;
  d_mv3  = d_mv3 - d_mv4;
  c_mv3  = c_mv3 - c_mv4;
  for(size_t i=0; i<d_mv3.num_row(); i++) testpass &= fabs( d_mv3(i+1) ) < 1e-9;
  for(size_t i=0; i<c_mv3.num_row(); i++) testpass &= diff( (-c_mv3)(i+1), m_complex_build(0,0) );

  out << "operator-, -=    " << testpass << std::endl;

  std::stringstream ss;
  ss  << d_mv4 << "\n";
  MVector<double>    d_mvio;
  MVector<m_complex> c_mvio;
  ss >> d_mvio;
  ss << c_mv4 << std::endl;
  ss >> c_mvio;
  for(size_t i=1; i<=d_mvio.num_row(); i++) testpass &= diff(d_mvio(i), d_mv4(i));
  for(size_t i=1; i<=c_mvio.num_row(); i++) testpass &= diff(c_mvio(i), c_mv4(i));
  out << "operator<<       " << testpass << std::endl;

  const gsl_vector*         c_vec_d = MVector_to_gsl(d_mv4);
  const gsl_vector_complex* c_vec_c = MVector_to_gsl(c_mv4);
  
  testpass &= c_mv4.num_row() == c_vec_c->size && d_mv4.num_row() == c_vec_d->size;
  for(size_t i=0; i<d_mv4.num_row(); i++) testpass &= fabs( d_mv4(i+1)-gsl_vector_get(c_vec_d, i) ) < 1e-9;
  for(size_t i=0; i<c_mv4.num_row(); i++) testpass &= diff( c_mv4(i+1), gsl_vector_complex_get(c_vec_c, i) );

  out << "gsl interface    " << testpass << std::endl;

  int sub1=2, sub2=3;
  MVector<double>    d_mv_sub = d_mv4.sub(sub1,sub2);
  MVector<m_complex> c_mv_sub = c_mv4.sub(sub1,sub2);
  testpass &= d_mv_sub.num_row() == size_t(sub2-sub1+1);
  testpass &= c_mv_sub.num_row() == size_t(sub2-sub1+1);
  for(size_t i=1; i<=d_mv_sub.num_row(); i++) testpass &= d_mv_sub(i) == d_mv4(i+sub1-1);
  for(size_t i=1; i<=c_mv_sub.num_row(); i++) testpass &= c_mv_sub(i) == c_mv4(i+sub1-1);
  out << "sub              " << testpass << std::endl;

  MVector<double>    d_mv_re   = re(c_mv4);
  MVector<double>    d_mv_im   = im(c_mv4);
  MVector<m_complex> c_mv_comp = complex(d_mv_re, d_mv_im);
  MVector<m_complex> c_mv_real = complex(d_mv_re);
  testpass &= c_mv_comp == c_mv4;
  testpass &= re(c_mv_real) == d_mv_re && im(c_mv_real) == MVector<double>(c_mv_real.num_row(), 0);

  out << "complex          " << testpass << std::endl;
  
  out << "T() is tested in MMatrix" << std::endl;

  out << "Sample output from ostream\n" << d_mv4 << std::endl << c_mv4 << std::endl;

  return testpass;
}

TEST(MVectorTest, old_unit_tests) {
  EXPECT_TRUE(test_m_complex());
  EXPECT_TRUE(test_MVector());
}



