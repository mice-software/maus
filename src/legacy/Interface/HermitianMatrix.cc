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
#include "gsl/gsl_linalg.h"
#include "gsl/gsl_eigen.h"

#include "Interface/Matrix.hh"
#include "Interface/HermitianMatrix.hh"
#include "Interface/SymmetricMatrix.hh"
#include "Interface/Squeal.hh"
#include "Interface/Vector.hh"

using namespace MAUS;

namespace MAUS
{

//****************************
// Forward Declarations
//****************************


//############################
// Free Functions
//############################

//****************************
// Conversion Functions
//****************************

HermitianMatrix dagger(const HermitianMatrix& matrix)
{
	Matrix<complex> transpose_matrix
		= transpose((Matrix<complex>) matrix);
	return conj(transpose_matrix);
}

HermitianMatrix inverse(const HermitianMatrix& matrix)
{
	return inverse((Matrix<complex>) matrix);
}

//*************************
// Eigensystem Functions
//*************************

//FIXME: use Hermitian GSL functions
Vector<double> eigenvalues(const HermitianMatrix& matrix)
{
  size_t size = matrix.size();
  HermitianMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(size);
  gsl_eigen_herm_workspace * workspace = gsl_eigen_herm_alloc(size);
  int ierr = gsl_eigen_herm(temp_matrix.matrix_, eigenvalues, workspace);
  gsl_eigen_herm_free(workspace);
  if(ierr != 0)
  {
    gsl_vector_free(eigenvalues);
    throw(Squeal(Squeal::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvalues"));
  }
  Vector<double> eigenvalue_vector((double*) eigenvalues->data, size);
  gsl_vector_free(eigenvalues);
  return eigenvalue_vector;
}

std::pair<Vector<double>, Matrix<complex> > eigensystem(
  const HermitianMatrix& matrix)
{
  size_t size = matrix.size();
  HermitianMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(size);
  gsl_matrix_complex * eigenvectors = gsl_matrix_complex_calloc(size, size);
  gsl_eigen_hermv_workspace * workspace = gsl_eigen_hermv_alloc(size);
  int ierr = gsl_eigen_hermv(temp_matrix.matrix_,
														 eigenvalues, eigenvectors,
                             workspace);
  gsl_eigen_hermv_free(workspace);
  if(ierr != 0)
  {
    gsl_vector_free(eigenvalues);
    gsl_matrix_complex_free(eigenvectors);
    throw(Squeal(Squeal::recoverable,
                 "Failed to calculate eigenvalue",
                 "MAUS::eigenvectors"));
  }
  Vector<double> eigenvalue_vector((double*) eigenvalues->data, size);
  gsl_vector_free(eigenvalues);
  Matrix<complex> eigenvector_matrix(size, size,
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

//############################
// HermitianMatrix Functions
//############################

//*************************
// Constructors
//*************************

HermitianMatrix::HermitianMatrix() : Matrix<complex>()
{ }

HermitianMatrix::HermitianMatrix(const HermitianMatrix& original_instance)
  : Matrix<complex>(original_instance)
{ }

HermitianMatrix::HermitianMatrix(const SymmetricMatrix& real_matrix)
	: Matrix<complex>()
{
	const Matrix<double> matrix = (Matrix<double>) real_matrix;
	(*this) = MAUS::Complex::complex(matrix);
}

HermitianMatrix::HermitianMatrix(const SymmetricMatrix& real_matrix,
																 const SymmetricMatrix& imaginary_matrix)
{
	size_t size = real_matrix.size();
	if (imaginary_matrix.size() != size)
	{
    throw(Squeal(Squeal::recoverable,
                 "Attempted to build a Hermitian matrix using real and imaginary matrices of different sizes",
                 "MAUS::HermitianMatrix::HermitianMatrix(<double>, <double>)"));
	}

	build_matrix(size);
	complex element;
	for (size_t row=1; row<=size; ++row)
	{
		for (size_t column=1; column<=row; ++column)
		{
			if (row == column)
			{
				//make sure the diagonal elements' imaginary parts are zero
				(*this)(row, column)
					= MAUS::Complex::complex(real_matrix(row, column), 0.);
			}
			else
			{
				element = Complex::complex(real_matrix(row, column),
																	 imaginary_matrix(row, column));
				//set the lower diagonal elements
				(*this)(row, column) = element;
				//set the upper diagonal elements
				(*this)(column, row) = conj(element);
			}
		}
	}
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
			if (row != column)
			{
				(*this)(row, column) = value;
				(*this)(column,row) = conj(value);
			}
			else
			{
				//make sure the imaginary part of diagonal elements are zero
				(*this)(row, column) = Complex::complex(real(value));
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

const size_t HermitianMatrix::size() const
{
	return number_of_rows();
}

//*************************
// Assignment Operators
//*************************

HermitianMatrix& HermitianMatrix::operator=(const HermitianMatrix& rhs)
{
	((Matrix<complex>) *this) = (Matrix<complex>) rhs;
	return *this;
}

HermitianMatrix& HermitianMatrix::operator+=(const HermitianMatrix& rhs)
{
	((Matrix<complex>) *this) += (Matrix<complex>) rhs;
	return *this;
}

HermitianMatrix& HermitianMatrix::operator-=(const HermitianMatrix& rhs)
{
	((Matrix<complex>) *this) -= (Matrix<complex>) rhs;
	return *this;
}

//*************************
// Algebraic Operators
//*************************

const HermitianMatrix HermitianMatrix::operator+(
  const HermitianMatrix& rhs) const
{
  return HermitianMatrix(*this) += rhs;
}

const HermitianMatrix HermitianMatrix::operator-(
  const HermitianMatrix& rhs) const
{
  return HermitianMatrix(*this) -= rhs;
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
			if (row != column)
			{
				(*this)(row+1, column+1) = element;
				(*this)(column+1, row+1) = conj(element);
			}
			else
			{
				//make sure the imaginary part of diagonal elements are zero
				(*this)(row+1, column+1) = Complex::complex(real(element));
			}
    }
  }
}

} //namespace MAUS
