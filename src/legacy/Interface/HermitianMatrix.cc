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

#include <limits>

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "gsl/gsl_linalg.h"
#include "gsl/gsl_eigen.h"

#include "Interface/Matrix.hh"
#include "Interface/HermitianMatrix.hh"
#include "Interface/Squeal.hh"
#include "Interface/Vector.hh"

using namespace MAUS;

namespace MAUS
{

//############################
// Free Functions
//############################

//****************************
// Conversion Functions
//****************************

SymmetricMatrix real(const HermitianMatrix& hermitian_matrix)
{
	return real((Matrix<complex>) hermitian_matrix);
}

Matrix<double> imag(const HermitianMatrix& hermitian_matrix)
{
	//FIXME: conjugate the upper
	return imag((Matrix<complex>) complex_matrix);
}

HermitianMatrix dagger(const HermitianMatrix& matrix)
{
	return conj((Matrix<complex>) matrix);
}

HermitianMatrix inverse(const HermitianMatrix& matrix)
{
	return inverse((Matrix<complex>) matrix);
}

}

//*************************
// Eigensystem Functions
//*************************

//FIXME: use Hermitian GSL functions
Vector<double> eigenvalues(const HermitianMatrix& matrix)
{
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if(rows != columns)
  {
    throw(Squeal(Squeal::recoverable,
                 "Attempt to get eigenvalues of non-square matrix",
                 "MAUS::eigenvalues") );
  }
  HermitianMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(rows);
  gsl_eigen_symm_workspace * workspace = gsl_eigen_symm_alloc(rows);
  int ierr = gsl_eigen_symm(temp_matrix.matrix_, eigenvalues, workspace);
  gsl_eigen_symm_free(workspace);
  if(ierr != 0)
  {
    gsl_vector_free(eigenvalues);
    throw(Squeal(Squeal::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvalues"));
  }
  Vector<double> eigenvalue_vector((double*) eigenvalues->data, rows);
  gsl_vector_free(eigenvalues);
  return eigenvalue_vector;
}

std::pair<Vector<double>, Matrix<complex> > eigensystem(
  const HermitianMatrix& matrix)
{
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if(rows != columns)
  {
    throw(Squeal(Squeal::recoverable,
                 "Attempt to get eigensystem of non-square matrix",
                 "MAUS::eigensystem") );
  }
  HermitianMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(rows);
  gsl_matrix_complex * eigenvectors = gsl_matrix_complex_calloc(rows, columns);
  gsl_eigen_symmv_workspace * workspace = gsl_eigen_symmv_alloc(rows);
  int ierr = gsl_eigen_symmv(temp_matrix.matrix_,
														 eigenvalues, eigenvectors,
                             workspace);
  gsl_eigen_symmv_free(workspace);
  if(ierr != 0)
  {
    gsl_vector_free(eigenvalues);
    gsl_matrix_free(eigenvectors);
    throw(Squeal(Squeal::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvectors"));
  }
  Vector<double> eigenvalue_vector((double*) eigenvalues->data, rows);
  gsl_vector_free(eigenvalues);
  Matrix<complex> eigenvector_matrix(rows, columns,
                                    (complex*) eigenvectors->data);
  gsl_matrix_complex_free(eigenvectors);
  return std::pair<Vector<double>, Matrix<complex> >(
           eigenvalue_vector, eigenvector_matrix);
}

//*************************
// Unitary Operators
//*************************

HermitianMatrix operator-(const HermitianMatrix& matrix)
{
	return -((Matrix<complex>) matrix);
}

//*************************
// Scalar Operators
//*************************

HermitianMatrix operator*(const copmlex&				 lhs,
													const HermitianMatrix& rhs)
{
	return lhs * (Matrix<complex>) rhs;
}

//############################
// HermitianMatrix Functions
//############################

//*************************
// Constructors
//*************************

HermitianMatrix::HermitianMatrix() : Matrix<complex>()
{ }

HermitianMatrix::HermitianMatrix(
  const HermitianMatrix& original_instance)
  : Matrix<complex>(original_instance)
{ }

HermitianMatrix::HermitianMatrix(
	const SymmetricMatrix<double>& real_matrix)
{
	//FIXME
	return MAUS::Complex::complex((Matrix<double>) real_matrix);
}

HermitianMatrix::HermitianMatrix(
	const SymmetricMatrix<double>& real_matrix,
  const SymmetricMatrix<double>& imaginary_matrix)
{
	//FIXME
	return MAUS::Complex::complex((Matrix<double>) real_matrix,
																(Matrix<double>) imaginary_matrix);
}

HermitianMatrix::HermitianMatrix(const size_t size)
  : Matrix<complex>(size, size)
{ }

HermitianMatrix::HermitianMatrix(const size_t	size, const complex& value)
  : Matrix<complex>()
{
  build_matrix(size);
  for(size_t row=1; row<=size; row++)
  {
    for(size_t column=1; column<=row; column++)
    {
      (*this)(row, column) = value;
			if (row != column)
			{
				(*this)(column,row) = value;
			}
    }
  }
}

HermitianMatrix::HermitianMatrix(const size_t					 size,
																	complex const * const data)
  : Matrix<complex>()
{
  build_matrix(size, data);
}

//*************************
// Size Functions
//*************************

size_tconst Matrix::size() const
{
	return number_of_rows();
}

//*************************
// Assignment Operators
//*************************

HermitianMatrix&const Matrix::operator=(
  const HermitianMatrix& rhs)
{
	((Matrix<complex>) *this) = (Matrix<complex>) rhs;
	return *this;
}

HermitianMatrix&const Matrix::operator+=(
	const HermitianMatrix& rhs)
{
	((Matrix<complex>) *this) += (Matrix<complex>) rhs;
	return *this;
}

HermitianMatrix&const Matrix::operator-=(
	const HermitianMatrix& rhs)
{
	((Matrix<complex>) *this) -= (Matrix<complex>) rhs;
	return *this;
}

HermitianMatrix&const Matrix::operator*=(
	const complex& rhs)
{
	((Matrix<complex>) *this) *= rhs;
  return *this;
}

HermitianMatrix&const Matrix::operator/=(
	const complex& rhs)
{
	((Matrix<complex>) *this) /= rhs;
  return *this;
}

//*************************
// Algebraic Operators
//*************************

const HermitianMatrixconst Matrix::operator+(
  const HermitianMatrix& rhs) const
{
  returnconst Matrix(*this) += rhs;
}

const HermitianMatrix HermitianMatrix::operator-(
  const HermitianMatrix& rhs) const
{
  return HermitianMatrix(*this) -= rhs;
}

const HermitianMatrix HermitianMatrix::operator*(
  const complex& rhs) const
{
  return HermitianMatrix(*this) *= rhs;
}

const HermitianMatrix HermitianMatrix::operator/(
  const complex& rhs) const
{
  return HermitianMatrix(*this) /= rhs;
}

//############################
// HermitianMatrix (protected)
//############################

HermitianMatrix::HermitianMatrix(
	const Matrix<complex>& original_instance) : Matrix<complex>(original_instance)
{ }

void HermitianMatrix::build_matrix(
  const size_t size, const bool initialize)
{
	Matrix<complex>::build_matrix(size, size, initialize);
}

void HermitianMatrix::build_matrix(const size_t         size,
                                   complex const * const data)
{
  build_matrix(size, false);
	 complex element;
  for(size_t row=0; row<size; ++row)
  {
    for(size_t column=0; column<row; ++column)
    {
			element = data[row*size + column];
      (*this)(row+1, column+1) = element;
			if (row != column)
			{
				(*this)(column+1, row+1) = element;
			}
    }
  }
}

} //namespace MAUS
