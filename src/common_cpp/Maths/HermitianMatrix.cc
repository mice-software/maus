/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include "Maths/HermitianMatrix.hh"

#include <limits>

#include "gsl/gsl_eigen.h"

#include "Utils/Exception.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

namespace MAUS {
// ****************************
//  Forward Declarations
// ****************************

// ############################
//  Free Functions
// ############################

// ****************************
//  Conversion Functions
// ****************************

SymmetricMatrix real(const HermitianMatrix& matrix) {
  // requires friend access to SymmetricMatrix AND HermitianMatrix
  return SymmetricMatrix(real((Matrix<complex>) matrix));
}

HermitianMatrix inverse(const HermitianMatrix& matrix) {
  return inverse((Matrix<complex>) matrix);
}

// *************************
//  Eigensystem Functions
// *************************

Vector<double> eigenvalues(const HermitianMatrix& matrix) {
  size_t size = matrix.size();
  HermitianMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(size);
  gsl_eigen_herm_workspace * workspace = gsl_eigen_herm_alloc(size);
  int ierr = gsl_eigen_herm(temp_matrix.matrix_, eigenvalues, workspace);
  gsl_eigen_herm_free(workspace);
  if (ierr != 0) {
    gsl_vector_free(eigenvalues);
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvalues"));
  }
  Vector<double> eigenvalue_vector(eigenvalues->data, size);
  gsl_vector_free(eigenvalues);
  return eigenvalue_vector;
}

std::pair<Vector<double>, Matrix<complex> > eigensystem(
    const HermitianMatrix& matrix) {
  size_t size = matrix.size();
  HermitianMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(size);
  gsl_matrix_complex * eigenvectors = gsl_matrix_complex_calloc(size, size);
  gsl_eigen_hermv_workspace * workspace = gsl_eigen_hermv_alloc(size);
  int ierr = gsl_eigen_hermv(temp_matrix.matrix_,
                             eigenvalues, eigenvectors,
                             workspace);
  gsl_eigen_hermv_free(workspace);
  if (ierr != 0) {
    gsl_vector_free(eigenvalues);
    gsl_matrix_complex_free(eigenvectors);
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvectors"));
  }
  Vector<double> eigenvalue_vector(eigenvalues->data, size);
  gsl_vector_free(eigenvalues);
  Matrix<complex> eigenvector_matrix(
    size, size, reinterpret_cast <complex *>(eigenvectors->data));
  gsl_matrix_complex_free(eigenvectors);
  return std::pair<Vector<double>, Matrix<complex> >(eigenvalue_vector,
                                                     eigenvector_matrix);
}

// *************************
//  Unitary Operators
// *************************

HermitianMatrix operator-(const HermitianMatrix& matrix) {
  return -((Matrix<complex>) matrix);
}

// ############################
//  HermitianMatrix Functions
// ############################

// *************************
//  Constructors
// *************************

HermitianMatrix::HermitianMatrix() : Matrix<complex>() {}

HermitianMatrix::HermitianMatrix(const HermitianMatrix& original_instance)
    : Matrix<complex>(original_instance) {}

HermitianMatrix::HermitianMatrix(
    const Matrix<complex>& original_instance) : Matrix<complex>() {
  Matrix<complex>::operator=(original_instance);
}

HermitianMatrix::HermitianMatrix(const size_t size)
    : Matrix<complex>(size, size) {}

HermitianMatrix::HermitianMatrix(const size_t  size, const complex& value)
    : Matrix<complex>() {
  build_matrix(size);
  for (size_t row = 1; row <= size; row++) {
    for (size_t column = 1; column <= row; column++) {
      if (row != column) {
        Matrix<complex>::operator()(row, column) = value;
        Matrix<complex>::operator()(column, row) = conj(value);
      } else {
        // make sure the imaginary part of diagonal elements are zero
        Matrix<complex>::operator()(row, column)
          = Complex::complex(real(value));
      }
    }
  }
}

HermitianMatrix::HermitianMatrix(const size_t            size,
                                 complex const * const  data)
    : Matrix<complex>() {
  build_matrix(size, data);
}

// *************************
//  Indexing Operators
// *************************

complex HermitianMatrix::operator()(const size_t row, const size_t column)
    const {
  return Matrix<complex>::operator()(row, column);
}

// *************************
//  Size Functions
// *************************

const size_t HermitianMatrix::size() const {
  return number_of_rows();
}

// *************************
//  Element Set Functions
// *************************

void HermitianMatrix::set(size_t row, size_t column, complex value) {
  Matrix<complex>::operator()(row, column) = value;
  Matrix<complex>::operator()(column, row) = conj(value);
}

// *************************
//  Assignment Operators
// *************************

HermitianMatrix& HermitianMatrix::operator=(const HermitianMatrix& rhs) {
  Matrix<complex>::operator=(rhs);
  return *this;
}

HermitianMatrix& HermitianMatrix::operator+=(const HermitianMatrix& rhs) {
  Matrix<complex>::operator+=(rhs);
  return *this;
}

HermitianMatrix& HermitianMatrix::operator-=(const HermitianMatrix& rhs) {
  Matrix<complex>::operator-=(rhs);
  return *this;
}

// *************************
//  Algebraic Operators
// *************************

const HermitianMatrix HermitianMatrix::operator+(
    const HermitianMatrix& rhs) const {
  return HermitianMatrix(*this) += rhs;
}

const HermitianMatrix HermitianMatrix::operator-(
    const HermitianMatrix& rhs) const {
  return HermitianMatrix(*this) -= rhs;
}

// ############################
//  HermitianMatrix (protected)
// ############################

void HermitianMatrix::build_matrix(
    const size_t size, const bool initialize) {
  Matrix<complex>::build_matrix(size, size, initialize);
}

void HermitianMatrix::build_matrix(const size_t         size,
                                   complex const * const data) {
  build_matrix(size, false);
  complex element;
  for (size_t row = 0; row < size; ++row) {
    for (size_t column = 0; column <= row; ++column) {
      element = data[row*size + column];
      if (row != column) {
        Matrix<complex>::operator()(row+1, column+1) = element;
        Matrix<complex>::operator()(column+1, row+1) = conj(element);
      } else {
        // make sure the imaginary part of diagonal elements are zero
        Matrix<complex>::operator()(row+1, column+1)
          = Complex::complex(real(element));
      }
    }
  }
}

}  // namespace MAUS
