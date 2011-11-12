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
  GslVector<GslType>& operator/=(const GslVector<GslType>&  rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const GslVector<GslType> operator+(const GslVector<GslType>&  rhs) const;
  const GslVector<GslType> operator-(const GslVector<GslType>&  rhs) const;
  const GslVector<GslType> operator*(const GslVector<GslType>&  rhs) const;
  const GslVector<GslType> operator/(const GslVector<GslType>&  rhs) const;
  
  //*************************
  // Comparison Operators
  //*************************
  const bool operator==(const GslVector<GslType>& rhs) const;
  const bool operator!=(const GslVector<GslType>& rhs) const;
  
  //*************************
  // Befriending
  //*************************
  friend GslVector<gsl_vector> real(
           const GslVector<gsl_vector_complex>& complex_vector);
  friend GslVector<gsl_vector> imag(
           const GslVector<gsl_vector_complex>& complex_vector);

protected:
  /** @brief delete the GSL vector member
   */
  void delete_vector();

  /** @brief create the GSL vector member of the given size
   */
  void build_vector(size_t size);

  GslType * vector_;
};

//*****************************
// Specialization Declarations
//*****************************

//These are put here instead of Vector.cc to maintain the order of
//function definitions.
template <>
GslVector<gsl_vector>& GslVector<gsl_vector>::operator=(
  const GslVector<gsl_vector>& rhs);
template <>
GslVector<gsl_vector_complex>& GslVector<gsl_vector_complex>::operator=(
  const GslVector<gsl_vector_complex>& rhs);
template <> void GslVector<gsl_vector>::delete_vector();
template <> void GslVector<gsl_vector_complex>::delete_vector();

/** @class VectorBase Implements C++ standard type function extensions of
 *                    GslVector.
 */
template<typename StdType, typename GslType>
class VectorBase : public GslVector<GslType>
{
public:
  //*************************
  // Constructors
  //*************************
  VectorBase();
  VectorBase(const VectorBase<StdType, GslType>& original_instance);
  VectorBase(size_t i, StdType  value);
  VectorBase(const StdType* array_beginning, const StdType* array_end);
  VectorBase(const std::vector<StdType, std::allocator<StdType> >& std_vector);

  //*** GslVector constructors ***

  VectorBase(size_t i);
  VectorBase(const GslVector<GslType>& original_instance);
  
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
  VectorBase<StdType, GslType>& operator*=(const StdType& rhs);
  VectorBase<StdType, GslType>& operator/=(const StdType& rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const VectorBase<StdType, GslType> operator*(const StdType& rhs) const;
  const VectorBase<StdType, GslType> operator/(const StdType& rhs) const;
protected:
  /** @brief copy data from an array and put it into the GSL vector member
   */
  void build_vector(const StdType* data_start, const StdType* data_end);
};

/** @class Vector Defines the association between GSL and standard C++ types.
 */
template<typename StdType>
class Vector
{
};

template<>
class Vector<double> :  public VectorBase<double, gsl_vector>
{
public:
  //*** VectorBase constructors ***

  Vector() : VectorBase<double, gsl_vector>() { }
  Vector(size_t i, double  value)
    : VectorBase<double, gsl_vector>(i, value) { }
  Vector(const double* array_beginning, const double* array_end)
    : VectorBase<double, gsl_vector>(array_beginning, array_end) { }
  Vector(const std::vector<double, std::allocator<double> >& std_vector)
    : VectorBase<double, gsl_vector>(std_vector) { }

  //*** GslVector constructors ***

  Vector(size_t i) : VectorBase<double, gsl_vector>(i) { }
  Vector(const Vector<double>& original_instance)
    : VectorBase<double, gsl_vector>(original_instance) { }

  //Because of slicing, we need deep copies of the base classes
  Vector(const GslVector<gsl_vector>& base_vector)
    : VectorBase<double, gsl_vector>(base_vector)
  {
  }

  Vector(const VectorBase<double, gsl_vector>& base_vector)
    : VectorBase<double, gsl_vector>(base_vector)
  {
  }
};

template<>
class Vector<complex> : public VectorBase<complex, gsl_vector_complex>
{
public:
  //*** VectorBase constructors ***

  Vector() : VectorBase<complex, gsl_vector_complex>() { }
  Vector(size_t i, complex  value)
    : VectorBase<complex, gsl_vector_complex>(i, value) { }
  Vector(const complex* array_beginning, const complex* array_end)
    : VectorBase<complex, gsl_vector_complex>(array_beginning, array_end) { }
  Vector(const std::vector<complex, std::allocator<complex> >& std_vector)
    : VectorBase<complex, gsl_vector_complex>(std_vector) { }

  //*** GslVector constructors ***

  Vector(size_t i) : VectorBase<complex, gsl_vector_complex>(i) { }
  Vector(const Vector<complex>& original_instance)
    : VectorBase<complex, gsl_vector_complex>(original_instance) { }

  //Because of slicing, we need deep copies of the base classes
  Vector(const GslVector<gsl_vector_complex>& base_vector)
    : VectorBase<complex, gsl_vector_complex>(base_vector)
  {
  }

  Vector(const VectorBase<complex, gsl_vector_complex>& base_vector)
    : VectorBase<complex, gsl_vector_complex>(base_vector)
  {
  }
};

//*************************
// Conversion Functions
//*************************
  
//return vector of doubles filled with either real or imaginary part of vector
GslVector<gsl_vector> real(
  const GslVector<gsl_vector_complex>& complex_vector);
GslVector<gsl_vector> imag(
  const GslVector<gsl_vector_complex>& complex_vector);
  

} //namespace MAUS
  
//*************************
// Unitary Operators
//*************************
template <class StdType> MAUS::Vector<StdType>  operator-(
  const MAUS::Vector<StdType>& vector);

//*************************
// Scalar Operators
//*************************
template <class StdType> MAUS::Vector<StdType>  operator*(
  const StdType&                value,
  const MAUS::Vector<StdType>&  vector);

//*************************
// Stream Operators
//*************************
template <class GslType>
std::ostream&  operator<<(std::ostream&                   out,
                          const MAUS::GslVector<GslType>& vector);

#endif
