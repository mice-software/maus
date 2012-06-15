/* This file is part of MAUS: http:// micewww.pp.rl.ac.uk:8080/projects/maus
 * 
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http:// www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 *
 * Based off of MVector by Chris Rogers which is a C++ wrapper for the GSL
 * vector C library (double and gsl_complex element types only).
 *
 * The main differences between MVector and Vector are as follows:
 * a) Vector avoids using void pointers by using a base class which is templated
 *    by both element type and GSL vector type. The GSL types are then hidden by
 *    templating the derived type only by element type.
 * c) In general, if a function can be implemented using only the public
 *    interface it is provided as a free function instead of a member
 *    function. Exceptions are functions which are associated with vector
 *    space properties such as subsets and algebraic operators.
 * d) Use of the MAUS namespace obviates the use of the initial 'M' in the class
 *    name (which presumably stood for MAUS).
 *
 * From MVector.hh:
 *
 * Maths operators and a few others are defined, but maths operators
 * don't allow operations between types - i.e. you can't multiply
 * a complex matrix by a double matrix. Instead use interface methods
 * like real() and complex() to convert between types first.
 */

#ifndef COMMON_CPP_MATHS_VECTOR_HH
#define COMMON_CPP_MATHS_VECTOR_HH

#include <iostream>
#include <memory>
#include <vector>

#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include "Maths/Complex.hh"

namespace CLHEP {
class HepVector;
}

namespace MAUS {
// *************************
//  Forward Declarations
// *************************
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
class VectorBase {
 public:
  // *************************
  //  Constructors
  // *************************
  VectorBase();
  VectorBase(const VectorBase<StdType, GslType>& original_instance);

  /** @brief Copy constructor for CLHEP::HepMatrix
   */
  explicit VectorBase(const ::CLHEP::HepVector& hep_vector);

  explicit VectorBase(const size_t i);

  VectorBase(const size_t i, const StdType  value);

  /** @brief Create an instance from an array of data. The size is the number
   *         of elements in the array.
   */
  VectorBase(const StdType* data, const size_t size);
  VectorBase(const std::vector<StdType, std::allocator<StdType> >& std_vector);
  ~VectorBase();

  // *************************
  //  Indexing Operators
  // *************************

  // Indexing starting with 1
  StdType&       operator()(const size_t i);
  const StdType& operator()(const size_t i) const;

  // Indexing starting with 0
  StdType&       operator[](const size_t i);
  const StdType& operator[](const size_t i) const;

  // *************************
  //  Size Functions
  // *************************

  // return number of elements
  const size_t size() const;

  // *************************
  //  Subvector Functions
  // *************************

  /** @brief Create a vector that contains a subset of the elements. The subset
   *         begins with the element at index <code>begin_index</code> and
   *         extends to the element at index <code>end_index - 1</code>. Thus
   *         the size of the new vector is simply
   *         <code>end_index - begin_index</code>.
   *  @params begin_index the index of the first element in the subset
   *  @params end_index the index of the last element in the subset plus 1
   */
  VectorBase<StdType, GslType> subvector(size_t begin_index, size_t end_index)
      const;

  // *************************
  //  Assignment Operators
  // *************************
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

  // *************************
  //  Algebraic Operators
  // *************************
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

  // *************************
  //  Comparison Operators
  // *************************
  const bool operator==(const VectorBase<StdType, GslType>& rhs) const;
  const bool operator!=(const VectorBase<StdType, GslType>& rhs) const;

  // *************************
  //  Befriending
  // *************************

  // These operations could be done using solely the public interface, but
  // we want to use the optimised GSL matrix/vector, low-level functions.
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
  void build_vector(const size_t size, const bool initialize = true);

  GslType * vector_;
};

// *****************************
//  Specialization Declarations
// *****************************

// These are put here instead of Vector.cc to maintain the order of
// function definitions.
template <> VectorBase<double, gsl_vector>::VectorBase(
  const VectorBase<double, gsl_vector>& original_instance);
template <> VectorBase<complex, gsl_vector_complex>::VectorBase(
  const VectorBase<complex, gsl_vector_complex>& original_instance);
template <> VectorBase<double, gsl_vector>::VectorBase(
    const ::CLHEP::HepVector& hep_vector);
template <> VectorBase<complex, gsl_vector_complex>::VectorBase(
    const ::CLHEP::HepVector& hep_matrix);
template <>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator=(
  const VectorBase<double, gsl_vector>& rhs);
template <>
VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator=(
  const VectorBase<complex, gsl_vector_complex>& rhs);
template <> void VectorBase<double, gsl_vector>::delete_vector();
template <> void VectorBase<complex, gsl_vector_complex>::delete_vector();
template <> void VectorBase<double, gsl_vector>::build_vector(
  const size_t size, const bool initialize);
template <> void VectorBase<complex, gsl_vector_complex>::build_vector(
  const size_t size, const bool initialize);

/** @class Vector Defines the association between GSL and standard C++ types.
 */
template<typename StdType>
class Vector {};

template<>
class Vector<double> :  public VectorBase<double, gsl_vector> {
 public:
  Vector(const Vector<double>& original_instance)
      : VectorBase<double, gsl_vector>(original_instance) {}

  // *** VectorBase functions ***

  Vector() : VectorBase<double, gsl_vector>() {}
  explicit Vector(const ::CLHEP::HepVector& hep_vector)
      : VectorBase<double, gsl_vector>(hep_vector) {}
  Vector(size_t i, double  value)
      : VectorBase<double, gsl_vector>(i, value) {}
  explicit Vector(size_t i) : VectorBase<double, gsl_vector>(i) {}
  Vector(const double* data, const size_t size)
      : VectorBase<double, gsl_vector>(data, size) {}
  Vector(const std::vector<double, std::allocator<double> >& std_vector)
      : VectorBase<double, gsl_vector>(std_vector) {}

  const Vector<double> operator+(const Vector<double>& rhs) const;
  const Vector<double> operator-(const Vector<double>& rhs) const;
  const Vector<double> operator*(const Vector<double>& rhs) const;
  const Vector<double> operator/(const Vector<double>& rhs) const;
  const Vector<double> operator*(const double& rhs) const;
  const Vector<double> operator/(const double& rhs) const;

  // *** Vector<double> functions ***

  Vector(const VectorBase<double, gsl_vector>& base_vector)
      : VectorBase<double, gsl_vector>(base_vector) {}
};

template<>
class Vector<complex> : public VectorBase<complex, gsl_vector_complex> {
 public:
  Vector(const Vector<complex>& original_instance)
    : VectorBase<complex, gsl_vector_complex>(original_instance) {}

  // *** VectorBase functions ***

  Vector() : VectorBase<complex, gsl_vector_complex>() {}
  explicit Vector(const ::CLHEP::HepVector& hep_vector)
      : VectorBase<complex, gsl_vector_complex>(hep_vector) {}
  Vector(size_t i, complex  value)
      : VectorBase<complex, gsl_vector_complex>(i, value) {}
  explicit Vector(size_t i) : VectorBase<complex, gsl_vector_complex>(i) {}
  Vector(const complex* data, const size_t size)
      : VectorBase<complex, gsl_vector_complex>(data, size) { }
  Vector(const std::vector<complex, std::allocator<complex> >& std_vector)
      : VectorBase<complex, gsl_vector_complex>(std_vector) {}

  const Vector<complex> operator+(const Vector<complex>& rhs) const;
  const Vector<complex> operator-(const Vector<complex>& rhs) const;
  const Vector<complex> operator*(const Vector<complex>& rhs) const;
  const Vector<complex> operator/(const Vector<complex>& rhs) const;
  const Vector<complex> operator*(const complex& rhs) const;
  const Vector<complex> operator/(const complex& rhs) const;

  // *** Vector<complex> functions ***

  Vector(const VectorBase<complex, gsl_vector_complex>& base_vector)
      : VectorBase<complex, gsl_vector_complex>(base_vector) {}
};

// *************************
//  Conversion Functions
// *************************

// return vector of doubles filled with either real or imaginary part of vector
VectorBase<double, gsl_vector> real(
    const VectorBase<complex, gsl_vector_complex>& complex_vector);
VectorBase<double, gsl_vector> imag(
    const VectorBase<complex, gsl_vector_complex>& complex_vector);

// *************************
//  Unitary Operators
// *************************
template <typename StdType> Vector<StdType>  operator-(
    const Vector<StdType>& vector);

// *************************
//  Scalar Operators
// *************************
template <typename StdType> Vector<StdType>  operator*(
    const StdType&          value,
    const Vector<StdType>&  vector);

// *************************
//  Stream Operators
// *************************
template <typename StdType, typename GslType>
std::ostream&  operator<<(std::ostream&                       out,
                          const VectorBase<StdType, GslType>& vector);
template <typename StdType, typename GslType>
std::istream&  operator>>(std::istream&                       in,
                          VectorBase<StdType, GslType>&       vector);

}  // namespace MAUS

#endif
