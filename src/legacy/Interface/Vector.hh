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
//VectorBase class handles maths operators and a few other operators
//
//Use template to define two types:
// (i)  VectorBase<double> is a vector of doubles
// (ii) VectorBase<complex> is a vector of complex
//
//Maths operators and a few others are defined, but maths operators
//don't allow operations between types - i.e. you can't multiply
//a complex matrix by a double matrix. Instead use interface methods
//like real() and complex() to convert between types first


#ifndef _SRC_LEGACY_INTERFACE_VECTOR_HH_
#define _SRC_LEGACY_INTERFACE_VECTOR_HH_

#include <iostream>

#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include "Interface/Squeal.hh"
#include "Interface/Complex.hh"

namespace std
{
  template<typename T> class complex;
}

namespace MAUS
{

//*************************
// Forward Declarations
//*************************
template <typename StdType, typename GslType> class MatrixBase;
template <typename StdType> class Matrix;
template <typename StdType, typename GslType> class VectorBase;
template <typename StdType> class Vector;
template <typename StdType> Vector<StdType> operator*(
  const Matrix<StdType>& lhs,
  const Vector<StdType>& rhs);

/** @class VectorBase A templated wrapper for GSL's C vector types and functions.
 *                   Currently only gsl_vector and gsl_vector_complex are
 *                   implemented.
 */
template <typename StdType, typename GslType>
class VectorBase
{
public:
  //*************************
  // Constructors
  //*************************
  VectorBase();
  VectorBase(const VectorBase<StdType, GslType>& original_instance);
  VectorBase(const size_t i);
  VectorBase(const size_t i, const StdType  value);
  /** @brief Create an instance from an array of data. The size is the number
   *         of elements in the array.
   */
  VectorBase(const StdType* data, const size_t size);
  VectorBase(const std::vector<StdType, std::allocator<StdType> >& std_vector);
  ~VectorBase();
  
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
  // Size Functions
  //*************************

  //return number of elements
  size_t size() const;

  //*************************
  // Subvector Functions
  //*************************

  /** @brief Create a vector that contains a subset of the elements. The subset
   *         begins with the element at index <code>begin_index</code> and
   *         extends to the element at index <code>end_index - 1</code>. Thus
   *         the size of the new vector is simply
   *         <code>end_index - begin_index</code>.
   *  @params begin_index the index of the first element in the subset
   *  @params end_index the index of the last element in the subset plus 1
   */
  VectorBase<StdType, GslType> subvector(size_t begin_index, size_t end_index) const;

  //*************************
  // Assignment Operators
  //*************************
  VectorBase<StdType, GslType>& operator =(
    const VectorBase<StdType, GslType>&                   rhs);
  VectorBase<StdType, GslType>& operator+=(
    const VectorBase<StdType, GslType>&                   rhs);
  VectorBase<StdType, GslType>& operator-=(
    const VectorBase<StdType, GslType>&                   rhs);
  VectorBase<StdType, GslType>& operator*=(
    const VectorBase<StdType, GslType>&                   rhs);
  VectorBase<StdType, GslType>& operator/=(
    const VectorBase<StdType, GslType>&                   rhs);
  VectorBase<StdType, GslType>& operator*=(const StdType& rhs);
  VectorBase<StdType, GslType>& operator/=(const StdType& rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const VectorBase<StdType, GslType> operator+(
    const VectorBase<StdType, GslType>&                       rhs) const;
  const VectorBase<StdType, GslType> operator-(
    const VectorBase<StdType, GslType>&                       rhs) const;
  const VectorBase<StdType, GslType> operator*(
    const VectorBase<StdType, GslType>&                       rhs) const;
  const VectorBase<StdType, GslType> operator/(
    const VectorBase<StdType, GslType>&                       rhs) const;
  const VectorBase<StdType, GslType> operator*(const StdType& rhs) const;
  const VectorBase<StdType, GslType> operator/(const StdType& rhs) const;
  
  //*************************
  // Comparison Operators
  //*************************
  const bool operator==(const VectorBase<StdType, GslType>& rhs) const;
  const bool operator!=(const VectorBase<StdType, GslType>& rhs) const;

  //*************************
  // Befriending
  //*************************

  //These operations could be done using solely the public interface, but
  //we want to use the optimised GSL matrix/vector, low-level functions.
  friend VectorBase<double, gsl_vector> real(
    const VectorBase<complex, gsl_vector_complex>& complex_vector);
  friend VectorBase<double, gsl_vector> imag(
    const VectorBase<complex, gsl_vector_complex>& complex_vector);
  friend typename MAUS::Vector<StdType> MAUS::operator*<>(
    const MAUS::Matrix<StdType>& lhs,
    const MAUS::Vector<StdType>& rhs);

protected:
  /** @brief delete the GSL vector member
   */
  void delete_vector();

  /** @brief copy data from an array and put it into the GSL vector member
   */
  void build_vector(const size_t size, const StdType* data);

  /** @brief create the GSL vector member of the given size
   */
  void build_vector(const size_t size, const bool initialize=true);

  GslType * vector_;
};

//*****************************
// Specialization Declarations
//*****************************

//These are put here instead of Vector.cc to maintain the order of
//function definitions.
template <>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator=(
  const VectorBase<double, gsl_vector>& rhs);
template <>
VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator=(
  const VectorBase<complex, gsl_vector_complex>& rhs);
template <> void VectorBase<double, gsl_vector>::delete_vector();
template <> void VectorBase<complex, gsl_vector_complex>::delete_vector();

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
  Vector(const Vector<double>& original_instance)
    : VectorBase<double, gsl_vector>(original_instance) { }

  //*** VectorBase constructors ***

  Vector() : VectorBase<double, gsl_vector>() { }
  Vector(const VectorBase<double, gsl_vector>& base_vector)
    : VectorBase<double, gsl_vector>(base_vector) { }
  Vector(size_t i, double  value)
    : VectorBase<double, gsl_vector>(i, value) { }
  Vector(size_t i) : VectorBase<double, gsl_vector>(i) { }
  Vector(const double* data, const size_t size)
    : VectorBase<double, gsl_vector>(data, size) { }
  Vector(const std::vector<double, std::allocator<double> >& std_vector)
    : VectorBase<double, gsl_vector>(std_vector) { }
};

template<>
class Vector<complex> : public VectorBase<complex, gsl_vector_complex>
{
public:
  Vector(const Vector<complex>& original_instance)
    : VectorBase<complex, gsl_vector_complex>(original_instance) { }

  //*** VectorBase constructors ***

  Vector() : VectorBase<complex, gsl_vector_complex>() { }
  Vector(const VectorBase<complex, gsl_vector_complex>& base_vector)
    : VectorBase<complex, gsl_vector_complex>(base_vector) { }
  Vector(size_t i, complex  value)
    : VectorBase<complex, gsl_vector_complex>(i, value) { }
  Vector(size_t i) : VectorBase<complex, gsl_vector_complex>(i) { }
  Vector(const complex* data, const size_t size)
    : VectorBase<complex, gsl_vector_complex>(data, size) { }
  Vector(const std::vector<complex, std::allocator<complex> >& std_vector)
    : VectorBase<complex, gsl_vector_complex>(std_vector) { }
};

//*************************
// Conversion Functions
//*************************
  
//return vector of doubles filled with either real or imaginary part of vector
VectorBase<double, gsl_vector> real(
  const VectorBase<complex, gsl_vector_complex>& complex_vector);
VectorBase<double, gsl_vector> imag(
  const VectorBase<complex, gsl_vector_complex>& complex_vector);
  
  
//*************************
// Unitary Operators
//*************************
template <typename StdType> Vector<StdType>  operator-(
  const Vector<StdType>& vector);

//*************************
// Scalar Operators
//*************************
template <typename StdType> Vector<StdType>  operator*(
  const StdType&          value,
  const Vector<StdType>&  vector);

//*************************
// Stream Operators
//*************************
template <typename StdType, typename GslType>
std::ostream&  operator<<(std::ostream&                       out,
                          const VectorBase<StdType, GslType>& vector);
template <typename StdType, typename GslType>
std::istream&  operator>>(std::istream&                       in,
                          VectorBase<StdType, GslType>&       vector);

} //namespace MAUS

#endif
