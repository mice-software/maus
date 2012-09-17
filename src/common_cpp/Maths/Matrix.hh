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
 * Based off of MMatrix by Chris Rogers which is a C++ wrapper for the GSL
 * matrix C library (double and gsl_complex element types only).
 *
 * The main differences between MMatrix and Matrix are as follows:
 * a) Matrix avoids using void pointers by using a base class which is templated
 *    by both element type and GSL matrix type. The GSL types are then hidden by
 *    templating the derived type only by element type.
 * c) In general, if a function can be implemented using only the public
 *    interface it is provided as a free function instead of a member
 *    function. Exceptions are functions which are associated with vector
 *    space properties such as subsets and algebraic operators.
 * d) Use of the MAUS namespace obviates the use of the initial 'M' in the class
 *    name (which presumably stood for MAUS).
 */

#ifndef COMMON_CPP_MATHS_MATRIX_HH_
#define COMMON_CPP_MATHS_MATRIX_HH_

#include <iostream>
#include <vector>
#include <utility>

#include "gsl/gsl_matrix.h"
#include "gsl/gsl_blas.h"

#include "Interface/Squeal.hh"
#include "Maths/Complex.hh"
#include "Maths/Vector.hh"

namespace CLHEP {
class HepMatrix;
}

namespace MAUS {

// *************************
//  Forward Declarations
// *************************
template <typename StdType, typename GslType> class MatrixBase;
template <typename StdType> class Matrix;
template <typename StdType, typename GslType> class VectorBase;
template <typename StdType> class Vector;

// *************************
//  GSL Helper Functions
// *************************

/** @brief Multiplies performs the double matrix multiplication a b, leaving
 *         the result in a. The intent is to mimick other standard GSL
 *         functions
 */
int gsl_matrix_mul(gsl_matrix ** matrix_A, const gsl_matrix * matrix_B);
/** @brief Multiplies performs the complex matrix multiplication a b, leaving
 *         the result in a. The intent is to mimick other standard GSL
 *         functions
 */
int gsl_matrix_complex_mul(gsl_matrix_complex ** matrix_A,
                           const gsl_matrix_complex * matrix_B);
/** @brief Multiplies the double vector b on the left by the double matrix a,
 *         leaving the result in b. This is slightly different from the standard
 *         GSL functions in that the result is stored in the second operand, not
 *         the first. The first operand is a matrix, so either the spacial
 *         relation between a and b must be disrupted or the convention must be
 *         violated.
 */
int gsl_matrix_mul(const gsl_matrix * matrix_A, gsl_vector ** vector_B);
/** @brief Multiplies the complex vector b on the left by the complex matrix a,
 *         leaving the result in b. This is slightly different from the standard
 *         GSL functions in that the result is stored in the second operand, not
 *         the first. The first operand is a matrix, so either the spacial
 *         relation between a and b must be disrupted or the convention must be
 *         violated.
 */
int gsl_matrix_complex_mul(const gsl_matrix_complex * matrix_A,
                           gsl_vector_complex ** vector_B);

// *************************
//  Conversion Functions
// *************************

/** @brief Returns a real-valued matrix containing the real part of the
 *         corresponding elements of the given complex-valued matrix.
 */
Matrix<double> real(const Matrix<complex>& complex_matrix);
/** @brief Returns a real-valued matrix containing the imaginary part of the
 *         corresponding elements of the given complex-valued matrix.
 */
Matrix<double> imag(const Matrix<complex>& complex_matrix);
/** @brief Returns a complex-valued matrix containing the complex conjugate
 *         of the elements of the given matrix.
 */
Matrix<complex> conj(const Matrix<complex>& complex_matrix);

namespace Complex {
Matrix<MAUS::complex> complex(const Matrix<double>& real_matrix);
Matrix<MAUS::complex> complex(const Matrix<double>& real_matrix,
                              const Matrix<double>& imaginary_matrix);
}  // namespace Complex

/** @brief Returns a complex-valued matrix containing the complex conjugate
 *         of the elements of the given matrix.
 */
Matrix<complex> dagger(const Matrix<complex>& matrix);

/** @brief returns the determinant of a matrix. Throws an exception if the
 *         matrix is not square
 */
template <typename StdType>
StdType determinant(const Matrix<StdType>& matrix);

/** @brief returns the inverse of a matrix
 */
template <typename StdType>
Matrix<StdType> inverse(const Matrix<StdType>& matrix);

/** @brief returns the sum of terms with row == column, even if the matrix is
 *         not square
 */
template <typename StdType>
StdType trace(const Matrix<StdType>& matrix);

/** @brief returns the transpose of a matrix (such that M(i,j) = T(j,i))
 */
template <typename StdType>
Matrix<StdType> transpose(const Matrix<StdType>& matrix);

namespace CLHEP {
/** @brief Creates a CLHEP::HepMatrix from a MAUS::Matrix. This function is
 *         meant to look like a copy constructor for the HepMatrix class, but
 *         it is a free function in the namespace MAUS::CLHEP.
 *
 */
::CLHEP::HepMatrix HepMatrix(const Matrix<double>& matrix);
}  // namespace CLHEP

// *************************
//  Eigensystem Functions
// *************************

/** @brief returns a vector of eigenvalues. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
Vector<complex> eigenvalues(const Matrix<double>& matrix);

/** @brief returns a vector of std::pair containing a vector of eigenvalues
 *         and a matrix of eigenvectors. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
std::pair<Vector<complex>, Matrix<complex> > eigensystem(
  const Matrix<double>& matrix);

// *************************
//  Unitary Operators
// *************************

template <typename StdType> MAUS::Matrix<StdType> operator-(
  const Matrix<StdType>& matrix);

// *************************
//  Scalar Operators
// *************************

template <typename StdType> MAUS::Matrix<StdType>  operator*(
  const StdType&          value,
  const Matrix<StdType>&  matrix);

// *************************
//  Matrix/Vector Operators
// *************************

/** @brief Multiply a column vector on the left by a matrix.
 */
template <typename StdType> Vector<StdType> operator*(
  const Matrix<StdType>& lhs,
  const Vector<StdType>& rhs);

/** @brief  Create a Matrix that represents a real-valued row vector that can
 *          then multiply a matrix on the left.
 */
Matrix<double> transpose(const Vector<double>& column_vector);

/** @brief  Create a Matrix that represents a complex-valued row vector that can
 *          then multiply a valued matrix on the left.
 */
Matrix<complex> dagger(const Vector<complex>& column_vector);

// *************************
//  Stream Operators
// *************************

// format is
//  num_row  num_col
//  m11 m12 m13 ...
//  m21 m22 m23 ...
//  ...
template <typename StdType>
std::ostream& operator<<(std::ostream&            out,
                         const Matrix<StdType>&   matrix);
template <typename StdType>
std::istream& operator>>(std::istream&            in,
                         Matrix<StdType>&         matrix);



/** @class Matrix C++ wrapper for GSL matrix
 *  Matrix class handles matrix algebra, maths operators and some
 *  higher level calculation like matrix inversion, eigenvector
 *  analysis etc
 *
 *  Use template to define two types:\n
 *  (i) Matrix<double> is a matrix of doubles\n
 *  (ii) Matrix<MAUS::complex> is a matrix of MAUS::complex\n
 *
 *  Maths operators and a few others are defined, but maths operators
 *  don't allow operations between types - i.e. you can't multiply
 *  a complex matrix by a double matrix. Instead use interface methods
 *  like real() and complex() to convert between types first
 */
template <typename StdType, typename GslType>
class MatrixBase {
 public:
  /** @brief default constructor makes an empty MatrixBase of size (0,0)
   */
  MatrixBase();

  /** @brief Copy constructor makes a deep copy of mv
   */
  MatrixBase(const MatrixBase<StdType, GslType>& original_instance);

  /** @brief Copy constructor for CLHEP::HepMatrix
   */
  explicit MatrixBase(const ::CLHEP::HepMatrix& hep_matrix);

  /** @brief Construct a matrix and fill all fields with 0
   *
   *  @params nrows number of rows
   *  @params ncols number of columns
   */
  MatrixBase(const size_t nrows, const size_t ncols);

  /** @brief Construct a matrix and fill with identical data
   *
   *  @params nrows number of rows
   *  @params ncols number of columns
   *  @params data variable to be copied into all items in the matrix
   */
  MatrixBase(const size_t nrows, const size_t ncols, const StdType value);

  /** @brief Construct a matrix and fill with data from an array
   *
   *  @params nrows number of rows
   *  @params ncols number of columns
   *  @params data pointer to the start of a memory block of size
   *          nrows*ncols with data laid out <row> <row> <row>. Note MatrixBase
   *          does not take ownership of memory in data.
   */
  MatrixBase(const size_t nrows, const size_t ncols,
             StdType const * const data);

  /** @brief destructor
   */
  ~MatrixBase();

  // *************************
  //  Indexing Operators
  // *************************

  // *** Indexing starting with 1 ***

  /** @brief Returns the element at location (row, column) in the matrix.
   */
  StdType& operator()(const size_t row, const size_t column);
  /** @brief Returns the element at location (row, column) in the matrix
   *         as a constant.
   */
  const StdType& operator()(const size_t row, const size_t column) const;

  // Indexing to entire rows/columns
  /** @brief Returns a vector representing the desired matrix row.
   */
  Vector<StdType> row(const size_t row) const;
  /** @brief Returns a vector representing the desired matrix column.
   */
  Vector<StdType> column(const size_t column) const;

  // *************************
  //  Size Functions
  // *************************

  /** @brief returns number of rows in the matrix
   */
  const size_t number_of_rows() const;

  /** @brief returns number of columns in the matrix
   */
  const size_t number_of_columns() const;

  // *************************
  //  Submatrix Functions
  // *************************

  /** @brief Returns a matrix that is a subset of the original matrix.
   */
  MatrixBase<StdType, GslType> submatrix(size_t start_row,
                                         size_t number_of_rows,
                                         size_t start_column,
                                         size_t number_of_columns)
                                        const;

  // *************************
  //  Assignment Operators
  // *************************
  MatrixBase<StdType, GslType>& operator =(
    const MatrixBase<StdType, GslType>&                   rhs);
  MatrixBase<StdType, GslType>& operator+=(
    const MatrixBase<StdType, GslType>&                   rhs);
  MatrixBase<StdType, GslType>& operator-=(
    const MatrixBase<StdType, GslType>&                   rhs);
  MatrixBase<StdType, GslType>& operator*=(
    const MatrixBase<StdType, GslType>&                   rhs);
  MatrixBase<StdType, GslType>& operator*=(const StdType& rhs);
  MatrixBase<StdType, GslType>& operator/=(const StdType& rhs);

  // *************************
  //  Algebraic Operators
  // *************************
  const MatrixBase<StdType, GslType> operator+(
    const MatrixBase<StdType, GslType>&                       rhs) const;
  const MatrixBase<StdType, GslType> operator-(
    const MatrixBase<StdType, GslType>&                       rhs) const;
  const MatrixBase<StdType, GslType> operator*(
    const MatrixBase<StdType, GslType>&                       rhs) const;
  const MatrixBase<StdType, GslType> operator*(const StdType& rhs) const;
  const MatrixBase<StdType, GslType> operator/(const StdType& rhs) const;

  // *************************
  //  Comparison Operators
  // *************************
  const bool operator==(const MatrixBase<StdType, GslType>& rhs) const;
  const bool operator!=(const MatrixBase<StdType, GslType>& rhs) const;

  // *************************
  //  Befriending
  // *************************

  // These operations could be done using solely the public interface, but
  // we want to use the optimised GSL matrix/vector, low-level functions.
  friend StdType determinant<>(const Matrix<StdType>& matrix);
  friend Matrix<StdType> inverse<>(const Matrix<StdType>& matrix);
  friend Matrix<StdType> transpose<>(const Matrix<StdType>& matrix);
  friend Vector<complex> eigenvalues(const Matrix<double>& matrix);
  friend std::pair<Vector<complex>, Matrix<complex> >
  eigensystem(const Matrix<double>& matrix);
  friend typename MAUS::Vector<StdType> MAUS::operator*<>(
    const MAUS::Matrix<StdType>& lhs,
    const MAUS::Vector<StdType>& rhs);

 protected:
  // delete the matrix and set it to null
  void delete_matrix();

  // build the matrix with size i,j, elements initialised to zero
  void build_matrix(const size_t i,
                    const size_t j,
                    const bool initialize = true);

  // build the matrix with size i,j, elements initialised to array data in
  // row major order
  void build_matrix(const size_t i, const size_t j, StdType const * const data);

  void gsl_error_handler(const char * reason, const char * file,
                         int line, int gsl_errno);

  GslType * matrix();

  GslType * matrix_;
};

// *****************************
//  Specialization Declarations
// *****************************

template <> MatrixBase<double, gsl_matrix>::MatrixBase(
    const MatrixBase<double, gsl_matrix>& original_instance);
template <> MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
  const MatrixBase<complex, gsl_matrix_complex>& original_instance);
template <> MatrixBase<double, gsl_matrix>::MatrixBase(
    const ::CLHEP::HepMatrix& hep_matrix);
template <> MatrixBase<complex, gsl_matrix_complex>::MatrixBase(
    const ::CLHEP::HepMatrix& hep_matrix);
template <> double& MatrixBase<double, gsl_matrix>::operator()(
    const size_t row, const size_t column);
template <> complex& MatrixBase<complex, gsl_matrix_complex>::operator()(
    const size_t row, const size_t column);
template <> const double& MatrixBase<double, gsl_matrix>::operator()(
    const size_t row, const size_t column) const;
template <> const complex& MatrixBase<complex, gsl_matrix_complex>::operator()(
    const size_t row, const size_t column) const;
template <> void MatrixBase<double, gsl_matrix>::delete_matrix();
template <> void MatrixBase<complex, gsl_matrix_complex>::delete_matrix();
template <> void MatrixBase<double, gsl_matrix>::build_matrix(
    const size_t rows, const size_t columns, const bool initialize);
template <> void MatrixBase<complex, gsl_matrix_complex>::build_matrix(
    const size_t rows, const size_t columns, const bool initialize);

/** @class Matrix Defines the association between GSL and standard C++ types.
 */
template<typename StdType>
class Matrix {};

template<>
class Matrix<double> :  public MatrixBase<double, gsl_matrix> {
 public:
  Matrix(const Matrix<double>& original_instance)
    : MatrixBase<double, gsl_matrix>(original_instance) {}

  // *** MatrixBase functions ***

  Matrix() : MatrixBase<double, gsl_matrix>() {}
  explicit Matrix(const ::CLHEP::HepMatrix& hep_matrix )
    : MatrixBase<double, gsl_matrix>(hep_matrix) {}
  Matrix(const size_t rows, const size_t columns, const double value)
    : MatrixBase<double, gsl_matrix>(rows, columns, value) {}
  Matrix(const size_t rows, const size_t columns)
    : MatrixBase<double, gsl_matrix>(rows, columns) {}
  Matrix(const size_t rows, const size_t columns, double const * const data)
    : MatrixBase<double, gsl_matrix>(rows, columns, data) {}

  Matrix<double> submatrix(size_t start_row,
                           size_t number_of_rows,
                           size_t start_column,
                           size_t number_of_columns)
                           const;

  const Matrix<double> operator+(const Matrix<double>& rhs) const;
  const Matrix<double> operator-(const Matrix<double>& rhs) const;
  const Matrix<double> operator*(const Matrix<double>& rhs) const;
  const Matrix<double> operator*(const double& rhs) const;
  const Matrix<double> operator/(const double& rhs) const;

  // *** Matrix<double> functions ***

  Matrix(const MatrixBase<double, gsl_matrix>& base_vector)
    : MatrixBase<double, gsl_matrix>(base_vector) {}

  friend Vector<complex> eigenvalues(const Matrix<double>& matrix);
  friend Matrix<double> QR_least_squares(
      const Matrix<double>& design_matrix, const Matrix<double>& value_matrix);
};

template<>
class Matrix<complex> : public MatrixBase<complex, gsl_matrix_complex> {
 public:
  Matrix(const Matrix<complex>& original_instance)
    : MatrixBase<complex, gsl_matrix_complex>(original_instance) {}

  // *** MatrixBase functions ***

  Matrix() : MatrixBase<complex, gsl_matrix_complex>() {}
  explicit Matrix(const ::CLHEP::HepMatrix& hep_matrix )
    : MatrixBase<complex, gsl_matrix_complex>(hep_matrix) {}
  Matrix(const size_t rows, const size_t columns, const complex value)
    : MatrixBase<complex, gsl_matrix_complex>(rows, columns, value) {}
  Matrix(const size_t rows, const size_t columns)
    : MatrixBase<complex, gsl_matrix_complex>(rows, columns) {}
  Matrix(const size_t rows, const size_t columns, complex const * const data)
    : MatrixBase<complex, gsl_matrix_complex>(rows, columns, data) {}

  Matrix<complex> submatrix(size_t start_row,
                            size_t number_of_rows,
                            size_t start_column,
                            size_t number_of_columns)
                            const;

  const Matrix<complex> operator+(const Matrix<complex>& rhs) const;
  const Matrix<complex> operator-(const Matrix<complex>& rhs) const;
  const Matrix<complex> operator*(const Matrix<complex>& rhs) const;
  const Matrix<complex> operator*(const complex& rhs) const;
  const Matrix<complex> operator/(const complex& rhs) const;

  // *** Matrix<complex> functions ***

  Matrix(const MatrixBase<complex, gsl_matrix_complex>& base_vector)
    : MatrixBase<complex, gsl_matrix_complex>(base_vector) {}

  /** @brief Construct a matrix with complex elements (containing no imaginary
   *         component) corresponding to elements of the given real-valued
   *         matrix.
   */
  Matrix(const MatrixBase<double, gsl_matrix>& real_matrix);

  /** @brief Constructs a matrix with complex elements corresponding to elements
   *         of the given real-valued matrices. In effect the complex-valued
   *         matrix that is returned is R + i I, where R and I are real-valued
   *         matrices correspending the the function arguments.
   */
  Matrix(const MatrixBase<double, gsl_matrix>& real_matrix,
         const MatrixBase<double, gsl_matrix>& imaginary_matrix);
};


}  // end namespace MAUS

#endif
