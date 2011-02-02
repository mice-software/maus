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
//
//Wrapper for GSL vector and gsl_complex
//
//MVector class handles maths operators and a few other operators
//
//Use template to define two types:
// (i)  MVector<double> is a vector of doubles
// (ii) MVector<m_complex> is a vector of m_complex
//
//Maths operators and a few others are defined, but maths operators
//don't allow operations between types - i.e. you can't multiply
//a complex matrix by a double matrix. Instead use interface methods
//like real() and complex() to convert between types first


#ifndef MVector_h
#define MVector_h

#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include <iostream>
#include <vector>

#include "Squeal.hh"

/////////////// Complex Start //////////////////////

typedef gsl_complex m_complex; //typedef because I guess at some point I may want to do something else

inline m_complex m_complex_build(double r, double i) { m_complex c = {{r,i}}; return c;}
inline m_complex m_complex_build(double r)           { m_complex c = {{r,0.}}; return c;}
//Just overload the standard operators
inline m_complex operator *(m_complex c,  double    d)  {return gsl_complex_mul_real(c,d);}
inline m_complex operator *(double    d,  m_complex c)  {return gsl_complex_mul_real(c,d);}
inline m_complex operator *(m_complex c1, m_complex c2) {return gsl_complex_mul(c1,c2);}

inline m_complex operator /(m_complex c,  double    d)  {return gsl_complex_div_real(c,d);}
inline m_complex operator /(m_complex c1, m_complex c2) {return gsl_complex_div(c1,c2);}
inline m_complex operator /(double    d,  m_complex c)  {m_complex c1 = m_complex_build(d); return gsl_complex_div(c1,c);}

inline m_complex operator +(m_complex c,  double    d)  {return gsl_complex_add_real(c, d);}
inline m_complex operator +(double    d,  m_complex c)  {return gsl_complex_add_real(c, d);}
inline m_complex operator +(m_complex c1, m_complex c2) {return gsl_complex_add     (c1,c2);}

inline m_complex  operator -(m_complex  c)                {c.dat[0] = -c.dat[0]; c.dat[1] = -c.dat[1]; return c;}
inline m_complex  operator -(m_complex  c,  double    d)  {return gsl_complex_sub_real(c, d);}
inline m_complex  operator -(double     d,  m_complex c)  {return -gsl_complex_sub_real(c, d);}
inline m_complex  operator -(m_complex  c1, m_complex c2) {return gsl_complex_sub     (c1,c2);}

//suboptimal *= ... should do the substitution in place
inline m_complex& operator *=(m_complex& c,  double    d)  {c  = gsl_complex_mul_real(c,d); return c;}
inline m_complex& operator *=(m_complex& c1, m_complex c2) {c1 = gsl_complex_mul(c1,c2);    return c1;}

inline m_complex& operator /=(m_complex& c,  double    d)  {c  = gsl_complex_div_real(c,d); return c;}
inline m_complex& operator /=(m_complex& c1, m_complex c2) {c1 = gsl_complex_div(c1,c2);    return c1;}

inline m_complex& operator +=(m_complex& c,  double    d)  {c  = gsl_complex_add_real(c, d);  return c;}
inline m_complex& operator +=(m_complex& c1, m_complex c2) {c1 = gsl_complex_add     (c1,c2); return c1;}

inline m_complex& operator -=(m_complex& c,  double    d)  {c  = gsl_complex_sub_real(c, d);  return c;}
inline m_complex& operator -=(m_complex& c1, m_complex c2) {c1 = gsl_complex_sub     (c1,c2); return c1;}
//comparators
inline bool      operator ==(m_complex c1, m_complex c2) {return c1.dat[0] == c2.dat[0] && c1.dat[1] == c2.dat[1];}
inline bool      operator !=(m_complex c1, m_complex c2) {return !(c1==c2);}

//real and imaginary
inline double&       re(m_complex& c)       {return c.dat[0];}
inline double&       im(m_complex& c)       {return c.dat[1];}
inline const double& re(const m_complex& c) {return c.dat[0];}
inline const double& im(const m_complex& c) {return c.dat[1];}

//complex conjugate
inline m_complex      conj(const m_complex& c) {return m_complex_build(re(c), -im(c));}

std::ostream& operator<<(std::ostream& out, m_complex  c);
std::istream& operator>>(std::istream& in,  m_complex& c);


//////////////// Complex End ///////////////////

template <class Tmplt>
class MMatrix; //forward declaration because MVector needs to see MMatrix for T() method (and others)

//////////////// MVector Start ///////////////////

template <class Tmplt>
class MVector
{
public:
  MVector() : _vector(NULL)                               {;}
  MVector( const MVector<Tmplt>& mv);
  MVector( const Tmplt* ta_beg, const Tmplt* ta_end ) : _vector(NULL) { build_vector(ta_beg, ta_end); } //copy from data and put it in the vector
  MVector( std::vector<Tmplt> tv) : _vector(NULL)         { build_vector(&tv[0], &tv.back()+1); }
  MVector( size_t i );
  MVector( size_t i, Tmplt  value );
  template <class Tmplt2> MVector(MVector<Tmplt2>);
  ~MVector();

  //return number of elements
  size_t       num_row() const;
  //return reference to ith element; indexing starts from 1, runs to num_row (inclusive); not bound checked
  Tmplt&       operator()(size_t i);
  const Tmplt& operator()(size_t i) const;

  MVector<Tmplt> sub(size_t n1, size_t n2) const; //return a vector running from n1 to n2 (inclusive)
  MMatrix<Tmplt> T() const; //return a matrix that is the transpose of the vector
  MVector<Tmplt>& operator= (const MVector<Tmplt>& mv); //set *this to be equal to mv

  friend MVector<m_complex>& operator *=(MVector<m_complex>& v, m_complex      c);
  friend MVector<double>&    operator *=(MVector<double>&    v, double         d);

  friend MVector<m_complex>& operator +=(MVector<m_complex>& v1, MVector<m_complex> v2);
  friend MVector<double>&    operator +=(MVector<double>&    v1, MVector<double>    v2);

  friend MVector<m_complex>  operator * (MMatrix<m_complex>  m,  MVector<m_complex> v);
  friend MVector<double>     operator * (MMatrix<double>     m,  MVector<double>    v);

  friend class MMatrix<Tmplt>;
  friend class MMatrix<double>;

//  friend gsl_vector*         MVectorToGSL(MVector<double>&     );
//  friend gsl_vector_complex* MVectorToGSL(MVector<gsl_complex>&);
  friend const gsl_vector*         MVector_to_gsl(const MVector<double>&     );
  friend const gsl_vector_complex* MVector_to_gsl(const MVector<gsl_complex>&);

private:
  void build_vector ( size_t size ); //copy from data and put it in the vector
  void build_vector ( const Tmplt* data_start, const Tmplt* data_end ); //copy from data and put it in the vector

  void delete_vector( );

  static gsl_vector*         get_vector(const MVector<double>&    m);
  static gsl_vector_complex* get_vector(const MVector<m_complex>& m);


  void* _vector;
};

//Operators do what you would expect - i.e. normal mathematical functions
template <class Tmplt> bool operator ==(const MVector<Tmplt>& v1, const MVector<Tmplt>& v2);
template <class Tmplt> bool operator !=(const MVector<Tmplt>& v1, const MVector<Tmplt>& v2);

MVector<m_complex>& operator *=(MVector<m_complex>& v, m_complex c);
MVector<double>&    operator *=(MVector<double>&    v, double    d);
template <class Tmplt> MVector<Tmplt> operator *(MVector<Tmplt> v, Tmplt          t);
template <class Tmplt> MVector<Tmplt> operator *(Tmplt          t, MVector<Tmplt> v);

template <class Tmplt> MVector<Tmplt>& operator /=(MVector<Tmplt>& v, Tmplt          t);
template <class Tmplt> MVector<Tmplt>  operator / (MVector<Tmplt>  v, Tmplt          t);
template <class Tmplt> MVector<Tmplt>  operator / (Tmplt           t, MVector<Tmplt> v);

MVector<m_complex>& operator +=(MVector<m_complex>& v1, MVector<m_complex> v2);
MVector<double>&    operator +=(MVector<double>&    v1, MVector<double>    v2);
template <class Tmplt> MVector<Tmplt>  operator + (MVector<Tmplt>  v1, MVector<Tmplt> v2);

template <class Tmplt> MVector<Tmplt>& operator -=(MVector<Tmplt>& v1, MVector<Tmplt> v2);
template <class Tmplt> MVector<Tmplt>  operator - (MVector<Tmplt>  v1, MVector<Tmplt> v2);
template <class Tmplt> MVector<Tmplt>  operator - (const MVector<Tmplt>& v);

//format is 
//num_row
//v1
//v2
//...
template <class Tmplt> std::ostream& operator<<(std::ostream& out, MVector<Tmplt>  v);
template <class Tmplt> std::istream& operator>>(std::istream& out, MVector<Tmplt>& v);

//Interfaces between MVector<m_complex> and MVector<double>
MVector<m_complex> complex(MVector<double> real);
MVector<m_complex> complex(MVector<double> real, MVector<double> imaginary);
//return vector of doubles filled with either real or imaginary part of mv
MVector<double>    re     (MVector<m_complex> mv);
MVector<double>    im     (MVector<m_complex> mv);

//Interface to gsl
const gsl_vector*         MVector_to_gsl(const MVector<double>&      vd);
const gsl_vector_complex* MVector_to_gsl(const MVector<gsl_complex>& vc);

///////////////// MVector End ///////////////// Nb: some inlined functions below...

//////////////////////////// MVector Inlined Functions //////////////

template <class Tmplt>
inline MVector<Tmplt>::~MVector() { delete_vector();}
template <>
inline void MVector<double>::delete_vector() { if(_vector != NULL) gsl_vector_free( (gsl_vector*)_vector); }
template <>
inline void MVector<m_complex>::delete_vector() { if(_vector != NULL) gsl_vector_complex_free( (gsl_vector_complex*)_vector); }

MVector<m_complex> inline & operator *=(MVector<m_complex>& v, gsl_complex c)
{gsl_vector_complex_scale((gsl_vector_complex*)v._vector,  c); return v;}
MVector<double>    inline & operator *=(MVector<double>&    v, double d) 
{gsl_vector_scale((gsl_vector*)v._vector,  d); return v;}

template <class Tmplt> MVector<Tmplt> inline operator *(MVector<Tmplt> v, Tmplt d) 
{return v*=d;}
template <class Tmplt> MVector<Tmplt> inline operator *(Tmplt d,       MVector<Tmplt> v) 
{return v*d;}

template <class Tmplt> MVector<Tmplt> inline & operator /=(MVector<Tmplt>& v, Tmplt d) 
{return v*=1./d;}
template <class Tmplt> MVector<Tmplt> inline   operator / (MVector<Tmplt>  v, Tmplt d) 
{return v/=d;}

MVector<m_complex> inline & operator +=(MVector<m_complex>& v1, MVector<m_complex> v2)
{gsl_vector_complex_add((gsl_vector_complex*)v1._vector,  (gsl_vector_complex*)v2._vector); return v1;}
MVector<double> inline &    operator +=(MVector<double>&    v1, MVector<double>    v2)
{gsl_vector_add((gsl_vector*)v1._vector,  (gsl_vector*)v2._vector); return v1;}
template <class Tmplt> MVector<Tmplt>     inline operator +(MVector<Tmplt> v1, MVector<Tmplt> v2) 
{return v1+=v2;}

template <class Tmplt> MVector<Tmplt> inline operator - (const MVector<Tmplt>& v)
{MVector<Tmplt> vo(v); for(size_t i=1; i<=vo.num_row(); i++) vo(i) = -vo(i); return vo;}
template <class Tmplt> MVector<Tmplt> inline & operator -=(MVector<Tmplt>& v1, MVector<Tmplt> v2) 
{return v1+= -v2;}
template <class Tmplt> MVector<Tmplt> inline   operator - (MVector<Tmplt>  v1, MVector<Tmplt> v2) 
{return v1-=v2;}

template <class Tmplt> bool inline operator!=(const MVector<Tmplt>& c1, const MVector<Tmplt>& c2)
{return !(c1==c2);}

template <class Tmplt>
gsl_vector inline* MVector<Tmplt>::get_vector(const MVector<double>&    m)
{
  if(m._vector == NULL) 
    throw(Squeal(Squeal::recoverable, "Attempt to access uninitialised matrix", "MVector::get_vector"));
  return (gsl_vector*)m._vector;
}

template <class Tmplt>
gsl_vector_complex inline* MVector<Tmplt>::get_vector(const MVector<m_complex>& m)
{
  if(m._vector == NULL) 
    throw(Squeal(Squeal::recoverable, "Attempt to access uninitialised vector", "MVector::get_vector"));
  return (gsl_vector_complex*)m._vector;

}



#endif
