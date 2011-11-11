// MAUS WARNING: THIS IS LEGACY CODE.
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
//Wrapper for GSL vector
//
//GslVector class handles maths operators and a few other operators
//
//Use template to define two types:
// (i)  GslVector<double> is a vector of doubles
// (ii) GslVector<complex> is a vector of complex
//
//Maths operators and a few others are defined, but maths operators
//don't allow operations between types - i.e. you can't multiply
//a complex matrix by a double matrix. Instead use interface methods
//like real() and complex() to convert between types first


#ifndef GslVector_h
#define GslVector_h

#include <iostream>

#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include "Interface/Squeal.hh"
#include "Interface/Complex.hh"

namespace std
{
  template<class T> class complex;
}

namespace MAUS
{

//forward declaration because GslVector needs to see Matrix for T() method
//(and others)
template <class GslType>
class Matrix;

/** @class GslVector A templated wrapper for GSL's C vector types and functions.
 *                   Currently only gsl_vector and gsl_vector_complex are
 *                   implemented.
 */
template <typename GslType>
class GslVector
{
public:
  //*************************
  // Constructors
  //*************************
  GslVector();
  GslVector(size_t i);
  GslVector(const GslVector<GslType>& original_instance);
  //copy data from an array
  template<class GslType2>
  GslVector(const GslVector<GslType2>& differently_typed_vector);
  ~GslVector();

  //return number of elements
  size_t size() const;
  
  /** @brief Create a vector that contains a subset of the elements. The subset
   *         begins with the element at index <code>begin_index</code> and
   *         extends to the element at index <code>end_index - 1</code>. Thus
   *         the size of the new vector is simply
   *         <code>end_index - begin_index</code>.
   *  @params begin_index the index of the first element in the subset
   *  @params end_index the index of the last element in the subset plus 1
   */
  GslVector<GslType> subvector(size_t begin_index, size_t end_index) const;

  //*************************
  // Assignment Operators
  //*************************
  GslVector<GslType>& operator =(const GslVector<GslType>&  rhs);
  GslVector<GslType>& operator+=(const GslVector<GslType>&  rhs);
  GslVector<GslType>& operator-=(const GslVector<GslType>&  rhs);
  GslVector<GslType>& operator*=(const GslVector<GslType>&  rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const GslVector<GslType> operator+(const GslVector<GslType>&  rhs) const;
  const GslVector<GslType> operator-(const GslVector<GslType>&  rhs) const;
  const GslVector<GslType> operator*(const GslVector<GslType>&  rhs) const;
  
  //*************************
  // Comparison Operators
  //*************************
  const bool operator==(const GslVector<GslType>& rhs) const;
  const bool operator!=(const GslVector<GslType>& rhs) const;

protected:
  /** @brief create the GSL vector member of the given size
   */
  void build_vector(size_t size);
  
  /** @brief copy data from an array and put it into the GSL vector member
   */
  void build_vector(const GslType* data_start, const GslType* data_end);
  
  /** @brief delete the GSL vector member
   */
  void delete_vector();

  GslType * vector_;
};

template<typename StdType>
class VectorBase
{
  //*************************
  // Constructors
  //*************************
  GslVector(size_t i, StdType  value);
  GslVector(const StdType* array_beginning, const StdType* array_end);
  GslVector(const std::vector<StdType, std::allocator<StdType> >& std_vector);
  
  //*************************
  // Indexing Operators
  //*************************
  
  //Indexing starting with 1
  StdType&       operator()(const size_t i);
  const StdType& operator()(const size_t i) const;
  
  //Indexing starting with 0
  StdType&       operator[](const size_t i);
  const StdType& operator[](const size_t i) const;
  
  //*************************
  // Assignment Operators
  //*************************
  GslVector<StdType>& operator*=(const StdType&          rhs);
  GslVector<StdType>& operator/=(const StdType&          rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const GslVector<StdType> operator*(const StdType&          rhs) const;
  const GslVector<StdType> operator/(const StdType&          rhs) const;
};

template<>
class Vector
{
};

template<>
class Vector<double> :  public VectorBase<double>,
                        public GslVector<gsl_vector>
{
};

template<>
class Vector<MAUS::complex> : public VectorBase<MAUS::complex>,
                              public GslVector<gsl_vector_complex>
{
};

//*************************
// Conversion Functions
//*************************
  
//return vector of doubles filled with either real or imaginary part of vector
GslVector<double>  real(GslVector<complex> vector);
GslVector<double>  imag(GslVector<complex> vector);


} //namespace MAUS
  
//*************************
// Unitary Operators
//*************************
template <class GslType> MAUS::GslVector<GslType>  operator-(
  const MAUS::GslVector<GslType>&  vector);

//*************************
// Scalar Operators
//*************************
template <class GslType> MAUS::GslVector<GslType>  operator*(
  const GslType&                value,
  const MAUS::GslVector<GslType>&  vector);

//*************************
// Stream Operators
//*************************
template <class GslType>
std::ostream&  operator<<(std::ostream&               out,
                          const MAUS::GslVector<GslType>&  vector);

#endif
