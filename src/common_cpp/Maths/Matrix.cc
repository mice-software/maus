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
 * From MMatrix.cc (see Matrix.hh):
 *
 * To get the templates right in cc file, you need to declare a function
 * and then tell compiler which template objects to compile - otherwise
 * you will get a linker error
 *
 * I am trying to replace CLHEP matrix here, as CLHEP has less functionality
 * and is probably worse written than GSL. A similar thing exists in boost
 * library
 */

#include "Maths/Matrix.hh"

#include <iostream>
#include <limits>
#include <sstream>

#include "CLHEP/Matrix/Matrix.h"
#include "gsl/gsl_linalg.h"
#include "gsl/gsl_eigen.h"
#include "json/json.h"

#include "Utils/Exception.hh"
#include "Maths/Vector.hh"
#include "Utils/JsonWrapper.hh"

using MAUS::complex;
using MAUS::Vector;
using MAUS::MatrixBase;
using MAUS::Matrix;

namespace MAUS {

// ############################
//  MatrixBase (public)
// ############################

// *************************
//  Constructors
// *************************

template <typename StdType, typename GslType>
MatrixBase<StdType, GslType>::MatrixBase() : matrix_(NULL) {}
template MatrixBase<double, gsl_matrix>::MatrixBase();
template MatrixBase<complex, gsl_matrix_complex>::MatrixBase();

template <>
MatrixBase<double, gsl_matrix>::MatrixBase(
    const MatrixBase<double, gsl_matrix>& original_instance)
    : matrix_(NULL) {
  if (original_instance.matrix_ != NULL) {
    build_matrix(original_instance.matrix_->size1,
                 original_instance.matrix_->size2);
    gsl_matrix_memcpy(matrix_, original_instance.matrix_);
  }
}

template <>
MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
  const MatrixBase<complex, gsl_matrix_complex>& original_instance)
    : matrix_(NULL) {
  if (original_instance.matrix_ != NULL) {
    build_matrix(original_instance.matrix_->size1,
                 original_instance.matrix_->size2);
    gsl_matrix_complex_memcpy(matrix_, original_instance.matrix_);
  }
}

template <>
MatrixBase<double, gsl_matrix>::MatrixBase(
  const ::CLHEP::HepMatrix& hep_matrix) : matrix_(NULL) {
  size_t rows = hep_matrix.num_row();
  size_t columns = hep_matrix.num_col();

  build_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      (*this)(row, column) = hep_matrix(row, column);
    }
  }
}

template <>
MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
  const ::CLHEP::HepMatrix& hep_matrix) : matrix_(NULL) {
  size_t rows = hep_matrix.num_row();
  size_t columns = hep_matrix.num_col();

  build_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      (*this)(row, column) = MAUS::Complex::complex(hep_matrix(row, column));
    }
  }
}

template <typename StdType, typename GslType>
MAUS::MatrixBase<StdType, GslType>::MatrixBase(const size_t rows,
                                               const size_t columns)
    : matrix_(NULL) {
  build_matrix(rows, columns);
}
template MatrixBase<double, gsl_matrix>::MatrixBase(
  const size_t rows, const size_t columns);
template MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
  const size_t rows, const size_t columns);

template <typename StdType, typename GslType>
MAUS::MatrixBase<StdType, GslType>::MatrixBase(const size_t   rows,
                                               const size_t   columns,
                                               const StdType  value)
    : matrix_(NULL) {
  build_matrix(rows, columns);
  for (size_t row = 1; row <= rows; row++) {
    for (size_t column = 1; column <= columns; column++) {
      (*this)(row, column) = value;
    }
  }
}
template MatrixBase<double, gsl_matrix>::MatrixBase(
  const size_t rows, const size_t columns, const double value);
template MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
  const size_t rows, const size_t columns, const complex value);

template <typename StdType, typename GslType>
MatrixBase<StdType, GslType>::MatrixBase(const size_t           rows,
                                         const size_t           columns,
                                         StdType const * const  data)
    : matrix_(NULL) {
  build_matrix(rows, columns, data);
}
template MatrixBase<double, gsl_matrix>::MatrixBase(
  const size_t rows, const size_t columns, double const * const data);
template MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
  const size_t rows, const size_t columns, complex const * const data);

template <typename StdType, typename GslType>
MatrixBase<StdType, GslType>::~MatrixBase() {
  delete_matrix();
}
template MatrixBase<double, gsl_matrix>::~MatrixBase();
template MatrixBase<complex, gsl_matrix_complex>::~MatrixBase();

// *************************
//  Indexing Operators
// *************************

template <>
double& MatrixBase<double, gsl_matrix>::operator()(
  const size_t row, const size_t column) {
  if (matrix_ == NULL) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to index an empty matrix.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  } else if ((row < 1) || (row > matrix_->size1)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Row index out of bounds.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  } else if ((column < 1) || (column > matrix_->size2)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Column index out of bounds.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  }
  return *gsl_matrix_ptr(matrix_, row-1, column-1);
}

template <>
complex& MatrixBase<complex, gsl_matrix_complex>::operator()(
    const size_t row, const size_t column) {
  if (matrix_ == NULL) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to index an empty matrix.",
                 "MatrixBase<complex, gsl_matrix_complex>::operator()"));
  } else if ((row < 1) || (row > matrix_->size1)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Row index out of bounds.",
                 "MatrixBase<complex, gsl_matrix_complex>::operator()"));
  } else if ((column < 1) || (column > matrix_->size2)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Column index out of bounds.",
                 "MatrixBase<complex, gsl_matrix_complex>::operator()"));
  }
  return *gsl_matrix_complex_ptr(matrix_, row-1, column-1);
}

template <>
const double& MatrixBase<double, gsl_matrix>::operator()(
  const size_t row, const size_t column) const {
  if (matrix_ == NULL) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to index an empty matrix.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  } else if ((row < 1) || (row > matrix_->size1)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Row index out of bounds.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  } else if ((column < 1) || (column > matrix_->size2)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Column index out of bounds.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  }
  return *gsl_matrix_ptr(matrix_, row-1, column-1);
}

template <>
const complex& MatrixBase<complex, gsl_matrix_complex>::operator()(
  const size_t row, const size_t column) const {
  if (matrix_ == NULL) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to index an empty matrix.",
                 "MatrixBase<complex, gsl_matrix_complex>::operator()"));
  } else if ((row < 1) || (row > matrix_->size1)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Row index out of bounds.",
                 "MatrixBase<complex, gsl_matrix_complex>::operator()"));
  } else if ((column < 1) || (column > matrix_->size2)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Column index out of bounds.",
                 "MatrixBase<complex, gsl_matrix_complex>::operator()"));
  }
  return *gsl_matrix_complex_ptr(matrix_, row-1, column-1);
}

template <typename StdType, typename GslType> Vector<StdType>
MAUS::MatrixBase<StdType, GslType>::row(const size_t row) const {
  if (matrix_ == NULL) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to index an empty matrix.",
                 "MatrixBase<double, gsl_matrix>::row()"));
  } else if ((row < 1) || (row > matrix_->size1)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Row index out of bounds.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  }
  size_t columns = matrix_->size2;
  Vector<StdType> row_vector(columns);
  for (size_t column = 1; column <= columns; ++column) {
    row_vector(column) = (*this)(row, column);
  }

  return row_vector;
}
template Vector<double>
MatrixBase<double, gsl_matrix>::row(const size_t row) const;
template Vector<complex>
MatrixBase<complex, gsl_matrix_complex>::row(const size_t row) const;

template <typename StdType, typename GslType> Vector<StdType>
MatrixBase<StdType, GslType>::column(const size_t column) const {
  if (matrix_ == NULL) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempting to index and empty matrix.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  } else if ((column < 1) || (column > matrix_->size2)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Column index out of bounds.",
                 "MatrixBase<double, gsl_matrix>::operator()"));
  }
  size_t rows = matrix_->size1;
  Vector<StdType> column_vector(rows);
  for (size_t row = 1; row <= rows; ++row) {
    column_vector(row) = operator()(row, column);
  }

  return column_vector;
}
template Vector<double>
MatrixBase<double, gsl_matrix>::column(const size_t column) const;
template Vector<complex>
MatrixBase<complex, gsl_matrix_complex>::column(const size_t column) const;

// *************************
//  Size Functions
// *************************

template <typename StdType, typename GslType>
const size_t MatrixBase<StdType, GslType>::number_of_rows() const {
  if (matrix_ != NULL) {
    return matrix_->size1;
  }

  return 0;
}
template const size_t MatrixBase<double, gsl_matrix>::number_of_rows() const;
template
const size_t MatrixBase<complex, gsl_matrix_complex>::number_of_rows() const;

template <typename StdType, typename GslType>
const size_t MatrixBase<StdType, GslType>::number_of_columns() const {
  if (matrix_ != NULL) {
    return matrix_->size2;
  }

  return 0;
}
template const size_t MatrixBase<double, gsl_matrix>::number_of_columns() const;
template
const size_t MatrixBase<complex, gsl_matrix_complex>::number_of_columns() const;

// *************************
//  Submatrix Functions
// *************************

template <typename StdType, typename GslType>
MatrixBase<StdType, GslType> MatrixBase<StdType, GslType>::submatrix(
    size_t start_row,    size_t number_of_rows,
    size_t start_column, size_t number_of_columns) const {
  MatrixBase<StdType, GslType> submatrix(number_of_rows, number_of_columns);
  for (size_t row = 1; row <= number_of_rows; ++row) {
    for (size_t column = 1; column <= number_of_columns; ++column) {
      submatrix(row, column) = (*this)(start_row+row-1, start_column+column-1);
    }
  }
  return submatrix;
}
template MatrixBase<double, gsl_matrix>
MatrixBase<double, gsl_matrix>::submatrix(
  size_t start_row,    size_t number_of_rows,
  size_t start_column, size_t number_of_columns) const;
template MatrixBase<complex, gsl_matrix_complex>
MatrixBase<complex, gsl_matrix_complex>::submatrix(
  size_t start_row,    size_t number_of_rows,
  size_t start_column, size_t number_of_columns) const;

// *************************
//  Assignment Operators
// *************************

template <>
MatrixBase<double, gsl_matrix>& MatrixBase<double, gsl_matrix>::operator=(
  const MatrixBase<double, gsl_matrix>& rhs) {
  if (&rhs != this) {
    if (matrix_ == NULL) {
      // special case (like for a copy constructor call) where a non-null
      // matrix is assigned to a null matrix
      build_matrix(rhs.matrix_->size1, rhs.matrix_->size2);
    } else if (   (rhs.matrix_ == NULL)
               || (matrix_->size1 != rhs.matrix_->size1)
               || (matrix_->size2 != rhs.matrix_->size2)) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                   "Attempted to assign a matrix of a different size.",
                   "MatrixBase<double>::operator=()"));
    }

    gsl_matrix_memcpy(matrix_, rhs.matrix_);
  }

  return *this;
}

template <> MatrixBase<complex, gsl_matrix_complex>&
MatrixBase<complex, gsl_matrix_complex>::operator=(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) {
  if (&rhs != this) {
    if (matrix_ == NULL) {
      // special case (like for a copy constructor call) where a non-null
      // matrix is assigned to a null matrix
      build_matrix(rhs.matrix_->size1, rhs.matrix_->size2);
    } else if (   (rhs.matrix_ == NULL)
               || (matrix_->size1 != rhs.matrix_->size1)
               || (matrix_->size2 != rhs.matrix_->size2)) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                   "Attempted to assign a matrix of a different size.",
                   "MatrixBase<complex>::operator=()"));
    }

    gsl_matrix_complex_memcpy(matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<double, gsl_matrix>&
MatrixBase<double, gsl_matrix>::operator+=(
  const MatrixBase<double, gsl_matrix>& rhs) {
  if (   (number_of_rows() != rhs.number_of_rows())
      || (number_of_columns() != rhs.number_of_columns())) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to add two matrices of different sizes",
                 "MatrixBase<double>::operator+=()"));
  }
  if (matrix_ != NULL && rhs.matrix_ != NULL) {
    gsl_matrix_add(matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<complex, gsl_matrix_complex>&
MatrixBase<complex, gsl_matrix_complex>::operator+=(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) {
  if (   (number_of_rows() != rhs.number_of_rows())
      || (number_of_columns() != rhs.number_of_columns())) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to add two matrices of different sizes",
                 "MatrixBase<complex>::operator+=()"));
  }
  if (matrix_ != NULL && rhs.matrix_ != NULL) {
    gsl_matrix_complex_add(matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<double, gsl_matrix>&
MatrixBase<double, gsl_matrix>::operator-=(
  const MatrixBase<double, gsl_matrix>& rhs) {
  if (   (number_of_rows() != rhs.number_of_rows())
      || (number_of_columns() != rhs.number_of_columns())) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to subtract two matrices of different sizes",
                 "MatrixBase<double,gsl_matrix>::operator-=()"));
  }
  if (matrix_ != NULL && rhs.matrix_ != NULL) {
    gsl_matrix_sub(matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<complex, gsl_matrix_complex>&
MatrixBase<complex, gsl_matrix_complex>::operator-=(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) {
  if (   (number_of_rows() != rhs.number_of_rows())
      || (number_of_columns() != rhs.number_of_columns())) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to subtract two matrices of different sizes",
                 "MatrixBase<complex,gsl_matrix_complex>::operator-=()"));
  }
  if (matrix_ != NULL && rhs.matrix_ != NULL) {
    gsl_matrix_complex_sub(matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<double, gsl_matrix>&
MatrixBase<double, gsl_matrix>::operator*=(
  const MatrixBase<double, gsl_matrix>& rhs) {
  if (number_of_columns() != rhs.number_of_rows()) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to multiply two matrices of incompatible sizes",
                 "MatrixBase<complex,gsl_matrix_complex>::operator*=()"));
  }
  if (matrix_ != NULL && rhs.matrix_ != NULL) {
    gsl_matrix_mul(&matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<complex, gsl_matrix_complex>&
MatrixBase<complex, gsl_matrix_complex>::operator*=(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) {
  if (number_of_columns() != rhs.number_of_rows()) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to multiply two matrices of incompatible sizes",
                 "MatrixBase<complex,gsl_matrix_complex>::operator*=()"));
  }
  if (matrix_ != NULL && rhs.matrix_ != NULL) {
    gsl_matrix_complex_mul(&matrix_, rhs.matrix_);
  }
  return *this;
}

template<> MatrixBase<double, gsl_matrix>&
MatrixBase<double, gsl_matrix>::operator*=(const double& rhs) {
  if (matrix_ != NULL) {
    gsl_matrix_scale(matrix_, rhs);
  }
  return *this;
}

template<> MatrixBase<complex, gsl_matrix_complex>&
MatrixBase<complex, gsl_matrix_complex>::operator *=(const complex& rhs) {
  if (matrix_ != NULL) {
    gsl_matrix_complex_scale(matrix_, rhs);
  }
  return *this;
}

template<> MatrixBase<double, gsl_matrix>&
MatrixBase<double, gsl_matrix>::operator/=(const double& rhs) {
  if (matrix_ != NULL) {
    gsl_matrix_scale(matrix_, 1./rhs);
  }
  return *this;
}

template<> MatrixBase<complex, gsl_matrix_complex>&
MatrixBase<complex, gsl_matrix_complex>::operator/=(const complex& rhs) {
  if (matrix_ != NULL) {
    gsl_matrix_complex_scale(matrix_, MAUS::Complex::complex(1.)/rhs);
  }
  return *this;
}

// *************************
//  Algebraic Operators
// *************************

template <typename StdType, typename GslType>
const MatrixBase<StdType, GslType> MatrixBase<StdType, GslType>::operator+(
    const MatrixBase<StdType, GslType>& rhs) const {
  return MatrixBase<StdType, GslType>(*this) += rhs;
}
template const MatrixBase<double, gsl_matrix>
MatrixBase<double, gsl_matrix>::operator+(
  const MatrixBase<double, gsl_matrix>& rhs) const;
template const MatrixBase<complex, gsl_matrix_complex>
MatrixBase<complex, gsl_matrix_complex>::operator+(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) const;

template <typename StdType, typename GslType>
const MatrixBase<StdType, GslType> MatrixBase<StdType, GslType>::operator-(
    const MatrixBase<StdType, GslType>& rhs) const {
  return MatrixBase<StdType, GslType>(*this) -= rhs;
}
template const MatrixBase<double, gsl_matrix>
MatrixBase<double, gsl_matrix>::operator-(
  const MatrixBase<double, gsl_matrix>& rhs) const;
template const MatrixBase<complex, gsl_matrix_complex>
MatrixBase<complex, gsl_matrix_complex>::operator-(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) const;


template <typename StdType, typename GslType>
const MatrixBase<StdType, GslType> MatrixBase<StdType, GslType>::operator*(
    const MatrixBase<StdType, GslType>& rhs) const {
  return MatrixBase<StdType, GslType>(*this) *= rhs;
}
template const MatrixBase<double, gsl_matrix>
MatrixBase<double, gsl_matrix>::operator*(
  const MatrixBase<double, gsl_matrix>& rhs) const;
template const MatrixBase<complex, gsl_matrix_complex>
MatrixBase<complex, gsl_matrix_complex>::operator*(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) const;

template <typename StdType, typename GslType>
const MatrixBase<StdType, GslType> MatrixBase<StdType, GslType>::operator*(
    const StdType& rhs) const {
  return MatrixBase<StdType, GslType>(*this) *= rhs;
}
template const MatrixBase<double, gsl_matrix>
MatrixBase<double, gsl_matrix>::operator*(
  const double& rhs) const;
template const MatrixBase<complex, gsl_matrix_complex>
MatrixBase<complex, gsl_matrix_complex>::operator*(
  const complex& rhs) const;

template <typename StdType, typename GslType>
const MatrixBase<StdType, GslType> MatrixBase<StdType, GslType>::operator/(
    const StdType& rhs) const {
  return MatrixBase<StdType, GslType>(*this) /= rhs;
}
template const MatrixBase<double, gsl_matrix>
MatrixBase<double, gsl_matrix>::operator/(
  const double& rhs) const;
template const MatrixBase<complex, gsl_matrix_complex>
MatrixBase<complex, gsl_matrix_complex>::operator/(
  const complex& rhs) const;

// *************************
//  Comparison Operators
// *************************

template <typename StdType, typename GslType>
const bool MatrixBase<StdType, GslType>::operator==(
  const MatrixBase<StdType, GslType>& rhs) const {
  size_t rows = number_of_rows();
  size_t columns = number_of_columns();
  if (   (number_of_rows() != rhs.number_of_rows())
      || (number_of_columns() != rhs.number_of_columns())) {
    return false;
  }

  if (matrix_ != NULL) {
    for (size_t row = 1; row <= rows; ++row) {
      for (size_t column = 1; column <= columns; ++column) {
        if ((*this)(row, column) != rhs(row, column)) {
          return false;
        }
      }
    }
  }
  return true;
}
template const bool MatrixBase<double, gsl_matrix>::operator==(
  const MatrixBase<double, gsl_matrix>& rhs) const;
template const bool MatrixBase<complex, gsl_matrix_complex>::operator==(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) const;

template <typename StdType, typename GslType>
const bool MatrixBase<StdType, GslType>::operator!=(
  const MatrixBase<StdType, GslType>& rhs) const {
  return !((*this) == rhs);
}
template const bool MatrixBase<double, gsl_matrix>::operator!=(
  const MatrixBase<double, gsl_matrix>& rhs) const;
template const bool MatrixBase<complex, gsl_matrix_complex>::operator!=(
  const MatrixBase<complex, gsl_matrix_complex>& rhs) const;

// ############################
//  MatrixBase (protected)
// ############################

template <>
void MatrixBase<double, gsl_matrix>::delete_matrix() {
  if (matrix_ != NULL) {
    gsl_matrix_free(matrix_);
  }

  matrix_ = NULL;
}

template <>
void MatrixBase<complex, gsl_matrix_complex>::delete_matrix() {
  if (matrix_ != NULL) {
    gsl_matrix_complex_free(matrix_);
  }

  matrix_ = NULL;
}

template <>
void MatrixBase<double, gsl_matrix>::build_matrix(
    const size_t rows, const size_t columns, const bool initialize) {
  delete_matrix();

  if ((rows > (size_t) 0) && (columns > (size_t) 0)) {
    if (initialize) {
      matrix_ = gsl_matrix_calloc(rows, columns);
    } else {
      matrix_ = gsl_matrix_alloc(rows, columns);
    }
  }
}

template <>
void MatrixBase<complex, gsl_matrix_complex>::build_matrix(
    const size_t rows, const size_t columns, const bool initialize) {
  delete_matrix();

  if ((rows > (size_t) 0) && (columns > (size_t) 0)) {
    if (initialize) {
      matrix_ = gsl_matrix_complex_calloc(rows, columns);
    } else {
      matrix_ = gsl_matrix_complex_alloc(rows, columns);
    }
  }
}

template <typename StdType, typename GslType>
void MatrixBase<StdType, GslType>::build_matrix(const size_t          rows,
                                                const size_t          columns,
                                                StdType const * const data) {
  build_matrix(rows, columns, false);
  for (size_t row = 0; row < rows; ++row) {
    for (size_t column = 0; column < columns; ++column) {
      (*this)(row+1, column+1) = data[row*columns + column];
    }
  }
}
template void MatrixBase<double, gsl_matrix>::build_matrix(
  const size_t rows, const size_t columns, double const * const data);
template void MatrixBase<complex, gsl_matrix_complex>::build_matrix(
  const size_t rows, const size_t columns, complex const * const data);

template <typename StdType, typename GslType>
void MatrixBase<StdType, GslType>::gsl_error_handler(const char *   reason,
                                                     const char *   file,
                                                     int            line,
                                                     int            gsl_errno) {
  throw(Exceptions::Exception(
      Exceptions::recoverable,
      reason,
      "MatrixBase<StdType, GslType>::gsl_error_handler()"));
}
template void MatrixBase<double, gsl_matrix>::gsl_error_handler(
    const char * reason, const char * file, int line, int gsl_errno);
template void MatrixBase<complex, gsl_matrix_complex>::gsl_error_handler(
    const char * reason, const char * file, int line, int gsl_errno);

template <typename StdType, typename GslType>
GslType * MatrixBase<StdType, GslType>::matrix() {
  return matrix_;
}
template gsl_matrix * MatrixBase<double, gsl_matrix>::matrix();
template gsl_matrix_complex * MatrixBase<complex, gsl_matrix_complex>::matrix();

// ############################
//  Matrix (public)
// ############################

Matrix<complex>::Matrix(
  const MatrixBase<double, gsl_matrix>& real_matrix)
  : MatrixBase<complex, gsl_matrix_complex>() {
  size_t rows = real_matrix.number_of_rows();
  size_t columns = real_matrix.number_of_columns();
  build_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      (*this)(row, column) = MAUS::Complex::complex(real_matrix(row, column));
    }
  }
}

Matrix<complex>::Matrix(
    const MatrixBase<double, gsl_matrix>& real_matrix,
    const MatrixBase<double, gsl_matrix>& imaginary_matrix)
    : MatrixBase<complex, gsl_matrix_complex>() {
  size_t rows = real_matrix.number_of_rows();
  size_t columns = real_matrix.number_of_columns();
  if (   (rows != imaginary_matrix.number_of_rows())
      || (columns != imaginary_matrix.number_of_columns())) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to build a complex matrix using "
      "real and imaginary matrices of different sizes",
      "Matrix<complex>::Matrix()"));
  }
  build_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      (*this)(row, column) = MAUS::Complex::complex(
        real_matrix(row, column), imaginary_matrix(row, column));
    }
  }
}

Matrix<double> Matrix<double>::submatrix(size_t start_row,
                             size_t number_of_rows,
                             size_t start_column,
                             size_t number_of_columns)
                             const {
  return MatrixBase<double, gsl_matrix>::submatrix(
    start_row, number_of_rows, start_column, number_of_columns);
}

Matrix<complex> Matrix<complex>::submatrix(size_t start_row,
                             size_t number_of_rows,
                             size_t start_column,
                             size_t number_of_columns)
                             const {
  return MatrixBase<complex, gsl_matrix_complex>::submatrix(
    start_row, number_of_rows, start_column, number_of_columns);
}

const Matrix<double> Matrix<double>::operator+(const Matrix<double>& rhs)
    const {
  return MatrixBase<double, gsl_matrix>::operator+(rhs);
}

const Matrix<double> Matrix<double>::operator-(const Matrix<double>& rhs)
    const {
  return MatrixBase<double, gsl_matrix>::operator-(rhs);
}

const Matrix<double> Matrix<double>::operator*(const Matrix<double>& rhs)
    const {
  return MatrixBase<double, gsl_matrix>::operator*(rhs);
}

const Matrix<double> Matrix<double>::operator*(const double& rhs) const {
  return MatrixBase<double, gsl_matrix>::operator*(rhs);
}

const Matrix<double> Matrix<double>::operator/(const double& rhs) const {
  return MatrixBase<double, gsl_matrix>::operator/(rhs);
}

const Matrix<complex> Matrix<complex>::operator+(const Matrix<complex>& rhs)
    const {
  return MatrixBase<complex, gsl_matrix_complex>::operator+(rhs);
}

const Matrix<complex> Matrix<complex>::operator-(const Matrix<complex>& rhs)
    const {
  return MatrixBase<complex, gsl_matrix_complex>::operator-(rhs);
}

const Matrix<complex> Matrix<complex>::operator*(const Matrix<complex>& rhs)
    const {
  return MatrixBase<complex, gsl_matrix_complex>::operator*(rhs);
}

const Matrix<complex> Matrix<complex>::operator*(const complex& rhs)
    const {
  return MatrixBase<complex, gsl_matrix_complex>::operator*(rhs);
}

const Matrix<complex> Matrix<complex>::operator/(const complex& rhs) const {
  return MatrixBase<complex, gsl_matrix_complex>::operator/(rhs);
}


// ############################
//  Template Declarations
// ############################

template class MatrixBase<double, gsl_matrix>;
template class MatrixBase<complex, gsl_matrix_complex>;

template class Matrix<double>;
template class Matrix<complex>;

// ############################
//  Free Functions
// ############################

// *************************
//  GSL Helper Functions
// *************************

int gsl_matrix_mul(gsl_matrix ** matrix_A, const gsl_matrix * matrix_B) {
  int return_code;
  gsl_matrix* matrix_AB = NULL;
  matrix_AB = gsl_matrix_alloc((*matrix_A)->size1, matrix_B->size2);
  if (matrix_AB != NULL) {
    return_code =
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
                   1., *matrix_A, matrix_B,
                   0., matrix_AB);
    gsl_matrix_free(*matrix_A);
    *matrix_A = gsl_matrix_alloc(matrix_AB->size1, matrix_AB->size2);
    gsl_matrix_memcpy(*matrix_A, matrix_AB);
    gsl_matrix_free(matrix_AB);
  } else {
    return_code = 0;
  }
  return return_code;
}

int gsl_matrix_complex_mul(gsl_matrix_complex ** matrix_A,
                           const gsl_matrix_complex * matrix_B) {
  int return_code;
  gsl_matrix_complex * matrix_AB = NULL;
  matrix_AB = gsl_matrix_complex_alloc((*matrix_A)->size1, matrix_B->size2);
  if (matrix_AB != NULL) {
    return_code =
    gsl_blas_zgemm(CblasNoTrans, CblasNoTrans,
                   MAUS::Complex::complex(1.), *matrix_A, matrix_B,
                   MAUS::Complex::complex(0.), matrix_AB);
    gsl_matrix_complex_free(*matrix_A);
    *matrix_A = gsl_matrix_complex_alloc(matrix_AB->size1, matrix_AB->size2);
    gsl_matrix_complex_memcpy(*matrix_A, matrix_AB);
    gsl_matrix_complex_free(matrix_AB);
  } else {
    return_code = 0;
  }
  return return_code;
}

int gsl_matrix_mul(const gsl_matrix * matrix_A, gsl_vector ** vector_b) {
  int return_code;
  gsl_vector * vector_Ab = NULL;
  vector_Ab = gsl_vector_alloc(matrix_A->size1);
  if (vector_Ab != NULL) {
    return_code =
    gsl_blas_dgemv(CblasNoTrans, 1., matrix_A, *vector_b, 0., vector_Ab);
    gsl_vector_free(*vector_b);
    *vector_b = gsl_vector_alloc(vector_Ab->size);
    if (vector_Ab != NULL) {
      gsl_vector_memcpy(*vector_b, vector_Ab);
    } else {
      return_code = 0;
    }
    gsl_vector_free(vector_Ab);
  } else {
    return_code = 0;
  }
  return return_code;
}

int gsl_matrix_complex_mul(const gsl_matrix_complex * matrix_A,
                           gsl_vector_complex ** vector_b) {
  int return_code;
  gsl_vector_complex * vector_Ab = NULL;
  vector_Ab = gsl_vector_complex_alloc(matrix_A->size1);
  if (vector_Ab != NULL) {
    return_code =
    gsl_blas_zgemv(CblasNoTrans,
                   MAUS::Complex::complex(1.), matrix_A, *vector_b,
                   MAUS::Complex::complex(0.), vector_Ab);
    gsl_vector_complex_free(*vector_b);
    *vector_b = gsl_vector_complex_alloc(vector_Ab->size);
    if (vector_Ab != NULL) {
      gsl_vector_complex_memcpy(*vector_b, vector_Ab);
    } else {
      return_code = 0;
    }
    gsl_vector_complex_free(vector_Ab);
  } else {
    return_code = 0;
  }
  return return_code;
}

// ****************************
//  Conversion Functions
// ****************************

Matrix<double> real(const Matrix<complex>& complex_matrix) {
  size_t rows = complex_matrix.number_of_rows();
  size_t columns = complex_matrix.number_of_columns();
  Matrix<double> double_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      double_matrix(row, column) = MAUS::real(complex_matrix(row, column));
    }
  }
  return double_matrix;
}

Matrix<double> imag(const Matrix<complex>& complex_matrix) {
  size_t rows = complex_matrix.number_of_rows();
  size_t columns = complex_matrix.number_of_columns();
  Matrix<double> double_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      double_matrix(row, column) = MAUS::imag(complex_matrix(row, column));
    }
  }
  return double_matrix;
}

Matrix<complex> conj(const Matrix<complex>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  Matrix<complex> conjugate_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      conjugate_matrix(row, column) = MAUS::conj(matrix(row, column));
    }
  }
  return conjugate_matrix;
}

namespace Complex {

Matrix<MAUS::complex> complex(const Matrix<double>& real_matrix) {
  size_t rows = real_matrix.number_of_rows();
  size_t columns = real_matrix.number_of_columns();
  Matrix<MAUS::complex> complex_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      complex_matrix(row, column)
        = MAUS::Complex::complex(real_matrix(row, column));
    }
  }
  return complex_matrix;
}

Matrix<MAUS::complex> complex(const Matrix<double>& real_matrix,
                              const Matrix<double>& imaginary_matrix) {
  size_t rows = real_matrix.number_of_rows();
  size_t columns = real_matrix.number_of_columns();
  if (   (rows != imaginary_matrix.number_of_rows())
      || (columns != imaginary_matrix.number_of_columns())) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to build a complex matrix using "
      "real and imaginary matrices of different sizes",
      "MAUS::MAUS::Complex::complex"));
  }
  Matrix<MAUS::complex> complex_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      complex_matrix(row, column)
        = MAUS::Complex::complex(real_matrix(row, column),
                                 imaginary_matrix(row, column));
    }
  }
  return complex_matrix;
}

}  // namespace Complex

template <> double determinant(
    const Matrix<double>& matrix) {
  int signum = 1;

  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if (rows != columns) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempt to get determinant of non-square matrix",
                 "MAUS::determinant()"));
  }
  gsl_permutation * p = gsl_permutation_alloc(rows);
  Matrix<double> tmp(matrix);
  gsl_linalg_LU_decomp(tmp.matrix_, p, &signum);
  double d = gsl_linalg_LU_det(tmp.matrix_, signum);
  gsl_permutation_free(p);
  return d;
}

template <> complex determinant(
    const Matrix<complex>& matrix) {
  int signum = 1;

  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if (rows != columns) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempt to get determinant of non-square matrix",
                 "MAUS::determinant()"));
  }
  gsl_permutation * p = gsl_permutation_alloc(rows);
  Matrix<complex> tmp(matrix);
  gsl_linalg_complex_LU_decomp(tmp.matrix_, p, &signum);
  gsl_permutation_free(p);
  return gsl_linalg_complex_LU_det(tmp.matrix_, signum);
}

template <>
Matrix<double> inverse(
    const Matrix<double>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if (rows != columns) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to get inverse of non-square matrix",
                 "MAUS::inverse()"));
  }
  if (determinant(matrix) == 0.) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to get inverse of singular matrix",
                 "MAUS::inverse()"));
  }

  Matrix<double> inverse(matrix);
  gsl_permutation* perm = gsl_permutation_alloc(rows);
  Matrix<double> lu(matrix);  // hold LU decomposition
  int s = 0;
  gsl_linalg_LU_decomp(lu.matrix_, perm, &s);
  gsl_linalg_LU_invert(lu.matrix_, perm, inverse.matrix_);
  gsl_permutation_free(perm);
  double test_value;
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      test_value = matrix(row, column);
      if (test_value != test_value) {
        throw(Exceptions::Exception(Exceptions::recoverable,
          "Failed to invert matrix: result contained NaN elements - singular?",
          "MAUS::inverse()"));
      } else if (test_value == std::numeric_limits<double>::infinity()) {
        throw(Exceptions::Exception(Exceptions::recoverable,
          "Failed to invert matrix: "
          "result contained infinite elements - singular?",
          "MAUS::inverse()"));
      }
    }
  }
  return inverse;
}

template <>
Matrix<complex> inverse(
    const Matrix<complex>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if (rows != columns) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempt to get inverse of non-square matrix",
                 "MAUS::inverse()"));
  }
  Matrix<complex> inverse(matrix);
  gsl_permutation* perm = gsl_permutation_alloc(rows);
  Matrix<complex> lu(matrix);  // hold LU decomposition
  int s = 0;
  gsl_linalg_complex_LU_decomp(lu.matrix_, perm, &s);
  gsl_linalg_complex_LU_invert(lu.matrix_, perm, inverse.matrix_);
  gsl_permutation_free(perm);
  complex test_value;
  const double infinity = std::numeric_limits<double>::infinity();
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      test_value = matrix(row, column);
      if (test_value != test_value) {
        throw(Exceptions::Exception(Exceptions::recoverable,
          "Failed to invert matrix: result contained NaN elements - singular?",
          "MAUS::inverse()"));
      } else if (   real(test_value) == infinity
                 || imag(test_value) == infinity) {
        throw(Exceptions::Exception(Exceptions::recoverable,
          "Failed to invert matrix: "
          "result contained infinite elements - singular?",
          "MAUS::inverse()"));
      }
    }
  }
  return inverse;
}

template <typename StdType>
StdType trace(const Matrix<StdType>& matrix) {
  StdType trace = matrix(1, 1);
  size_t rows = matrix.number_of_rows();
  for (size_t row = 2; row <= rows; ++row) {
    trace += matrix(row, row);
  }
  return trace;
}
template double trace(const Matrix<double>& matrix);
template complex trace(const Matrix<complex>& matrix);

template <> Matrix<double> transpose(
    const Matrix<double>& matrix) {
  Matrix<double> matrix_transpose(matrix.number_of_columns(),
                                  matrix.number_of_rows());
  gsl_matrix_transpose_memcpy(matrix_transpose.matrix_, matrix.matrix_);
  return matrix_transpose;
}

template <> Matrix<complex> transpose(
    const Matrix<complex>& matrix) {
  Matrix<complex> matrix_transpose(matrix.number_of_columns(),
                                  matrix.number_of_rows());
  gsl_matrix_complex_transpose_memcpy(matrix_transpose.matrix_, matrix.matrix_);
  return matrix_transpose;
}

Matrix<complex> dagger(const Matrix<complex>& matrix) {
  Matrix<complex> transpose_matrix = transpose(matrix);
  return MAUS::conj(transpose_matrix);
}

namespace CLHEP {
::CLHEP::HepMatrix HepMatrix(const Matrix<double>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();

  ::CLHEP::HepMatrix hep_matrix(rows, columns, 0);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      hep_matrix(row, column) = matrix(row, column);
    }
  }
  return hep_matrix;
}
}  // namespace MAUS::CLHEP

// *************************
//  Eigensystem Functions
// *************************

Vector<complex> eigenvalues(const Matrix<double>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if (rows != columns) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempt to get eigenvalues of non-square matrix",
                 "MAUS::eigenvalues"));
  }
  Matrix<double> temp_matrix(matrix);
  gsl_vector_complex * eigenvalues = gsl_vector_complex_alloc(rows);
  gsl_eigen_nonsymm_workspace * workspace = gsl_eigen_nonsymm_alloc(rows);
  gsl_eigen_nonsymm_params(0, 1, workspace);
  int ierr = gsl_eigen_nonsymm(temp_matrix.matrix_, eigenvalues, workspace);
  gsl_eigen_nonsymm_free(workspace);
  if (ierr != 0) {
    gsl_vector_complex_free(eigenvalues);
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvalues"));
  }
  Vector<complex> eigenvalue_vector(
    reinterpret_cast <complex*>(eigenvalues->data),
    rows);
  gsl_vector_complex_free(eigenvalues);
  return eigenvalue_vector;
}

Matrix<double> QR_least_squares(
    const Matrix<double>& design_matrix, const Matrix<double>& value_matrix) {
  size_t rows = design_matrix.number_of_rows();
  size_t columns = design_matrix.number_of_columns();

  Matrix<double> A(design_matrix);
  Matrix<double> Y(value_matrix);
  Matrix<double> Q(rows, rows);
  Matrix<double> R(rows, columns);
  size_t tau_size = columns;
  if (rows < tau_size) {
    tau_size = rows;
  }
  Vector<double> tau(tau_size);
  gsl_permutation * p = gsl_permutation_alloc(columns);
  int signum;
  gsl_vector * norm = gsl_vector_alloc(columns);
  gsl_linalg_QRPT_decomp2(A.matrix(), Q.matrix(), R.matrix(), tau.vector(),
                          p, &signum, norm);

  Matrix<double> P(columns, columns, 0.);
  for (size_t index = 0; index < columns; ++index) {
    P(index+1, gsl_permutation_get(p, index)+1) = 1.;
  }
  Matrix<double> C = transpose(P)*inverse(transpose(R)*R)
                   * transpose(R)*transpose(Q)*Y;
  gsl_vector_free(norm);
  gsl_permutation_free(p);

  return transpose(C);
}

std::pair<Vector<complex>, Matrix<complex> > eigensystem(
    const Matrix<double>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if (rows != columns) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempt to get eigensystem of non-square matrix",
                 "MAUS::eigensystem"));
  }
  Matrix<double> temp_matrix(matrix);
  gsl_vector_complex * eigenvalues = gsl_vector_complex_alloc(rows);
  gsl_matrix_complex * eigenvectors = gsl_matrix_complex_calloc(rows, columns);
  gsl_eigen_nonsymmv_workspace * workspace = gsl_eigen_nonsymmv_alloc(rows);
  int ierr = gsl_eigen_nonsymmv(temp_matrix.matrix_,
                                eigenvalues, eigenvectors,
                                workspace);
  gsl_eigen_nonsymmv_free(workspace);
  if (ierr != 0) {
    gsl_vector_complex_free(eigenvalues);
    gsl_matrix_complex_free(eigenvectors);
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvectors"));
  }
  Vector<complex> eigenvalue_vector(
    reinterpret_cast <complex*>(eigenvalues->data),
    rows);
  gsl_vector_complex_free(eigenvalues);
  Matrix<complex> eigenvector_matrix(
    rows, columns, reinterpret_cast <complex*>(eigenvectors->data));
  gsl_matrix_complex_free(eigenvectors);
  return std::pair<Vector<complex>, Matrix<complex> >(
           eigenvalue_vector, eigenvector_matrix);
}

// *************************
//  Unitary Operators
// *************************

template <typename StdType>
Matrix<StdType> operator-(const Matrix<StdType>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  Matrix<StdType> negated_matrix(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      negated_matrix(row, column) = -matrix(row, column);
    }
  }
  return negated_matrix;
}
template Matrix<double> operator-(const Matrix<double>& matrix);
template Matrix<complex> operator-(const Matrix<complex>& matrix);

// *************************
//  Scalar Operators
// *************************

template <typename StdType>
Matrix<StdType> operator*(const StdType& lhs, const Matrix<StdType>& rhs) {
  Matrix<StdType> product(rhs);
  return product *= lhs;
}
template Matrix<double> operator*(
  const double& lhs, const Matrix<double>& rhs);
template Matrix<complex> operator*(
  const complex& lhs, const Matrix<complex>& rhs);

// *************************
//  Matrix/Vector Operators
// *************************

template <> Vector<double> operator*(
    const Matrix<double>& lhs,
    const Vector<double>& rhs) {
  Vector<double> product(rhs);
  if (   (lhs.matrix_ != NULL) && (rhs.vector_ != NULL)
      && (lhs.number_of_columns() == rhs.size())) {
    MAUS::gsl_matrix_mul(lhs.matrix_, &product.vector_);
  }
  return product;
}

template <> Vector<complex> operator*(
    const Matrix<complex>& lhs,
    const Vector<complex>& rhs) {
  Vector<complex> product(rhs);
  if (   (lhs.matrix_ != NULL) && (rhs.vector_ != NULL)
      && (lhs.number_of_columns() == rhs.size())) {
    MAUS::gsl_matrix_complex_mul(lhs.matrix_, &product.vector_);
  }
  return product;
}

Matrix<double> transpose(
    const Vector<double>& column_vector) {
  size_t columns = column_vector.size();
  Matrix<double> row_vector(1, columns);
  for (size_t index = 1; index <= columns; ++index) {
    row_vector(1, index) = column_vector(index);
  }

  return row_vector;
}

Matrix<complex> dagger(
    const Vector<complex>& column_vector) {
  size_t columns = column_vector.size();
  Matrix<complex> row_vector(1, columns);
  for (size_t index = 1; index <= columns; ++index) {
    row_vector(1, index) = conj(column_vector(index));
  }

  return row_vector;
}


// *************************
//  Stream Operators
// *************************

template <typename StdType>
std::ostream& operator<<(std::ostream&            out,
                        const Matrix<StdType>&    matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  out << rows << " " << columns << "\n";
  for (size_t row = 1; row <= rows; ++row) {
    out << "  ";
    for (size_t column = 1; column <= columns; ++column) {
      out << matrix(row, column) << "\t";
    }
    out << "\n";
  }
  return out;
}
template std::ostream& operator<<(
  std::ostream& out, const Matrix<complex>& matrix);

template <> std::ostream& operator<<(
    std::ostream& out, const Matrix<double>& matrix) {
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  out << "[";
  for (size_t row = 1; row <= rows; ++row) {
    out << "[";
    for (size_t column = 1; column <= columns; ++column) {
      out << matrix(row, column);
      if (column < columns) {
        out << ", ";
      }
    }
    out << "]";
    if (row < rows) {
      out << "," << std::endl;
    }
  }
  out << "]" << std::endl;
  return out;
}

template <typename StdType>
std::istream& operator>>(std::istream&    in,
                         Matrix<StdType>& matrix) {
  size_t rows;
  size_t columns;
  in >> rows >> columns;
  matrix = Matrix<StdType>(rows, columns);
  for (size_t row = 1; row <= rows; ++row) {
    for (size_t column = 1; column <= columns; ++column) {
      in >> matrix(row, column);
    }
  }
  return in;
}
template std::istream& operator>>(
  std::istream& in, Matrix<complex>& matrix);

template <> std::istream& operator>>(
    std::istream& in, Matrix<double>& matrix) {
  std::streamsize buffer_size = 1024;
  char buffer[buffer_size];
  std::stringbuf document;
  while (!in.eof()) {
    in.read(buffer, buffer_size);
    std::streamsize read_bytes_count = in.gcount();
    document.sputn(buffer, read_bytes_count);
  }

  Json::Value json_document = JsonWrapper::StringToJson(document.str());
  Json::Value::ArrayIndex rows = json_document.size();
  Json::Value::ArrayIndex columns;
  if (rows > 0) {
    columns = json_document[Json::Value::ArrayIndex(0)].size();
    matrix = Matrix<double>(rows, columns);
    for (size_t row = 1; row <= rows; ++row) {
      Json::Value json_row = json_document[Json::Value::ArrayIndex(row-1)];
      for (size_t column = 1; column <= columns; ++column) {
        Json::Value json_element = json_row[Json::Value::ArrayIndex(column-1)];
        matrix(row, column) = json_element.asDouble();
      }
    }
  }

  return in;
}

}  // namespace MAUS
