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
#include "Interface/SymmetricMatrix.hh"
#include "Interface/Squeal.hh"
#include "Interface/Vector.hh"

using namespace MAUS;

namespace MAUS
{

//*************************
// Constructors
//*************************
  
SymmetricMatrix::SymmetricMatrix() : Matrix<double>()
{ }

SymmetricMatrix::SymmetricMatrix(
  const SymmetricMatrix& original_instance)
  : Matrix<double>(original_instance)
{ }

SymmetricMatrix::SymmetricMatrix(
	const Matrix<double>& original_instance) : Matrix<double>()
{
	Matrix<double>::operator=(original_instance);
}

SymmetricMatrix::SymmetricMatrix(
  const ::CLHEP::HepSymMatrix& hep_matrix) : Matrix<double>()
{
  size_t size = hep_matrix.num_row();

  build_matrix(size, false);
	double element;
  for(size_t row=1; row<=size; ++row)
  {
    for(size_t column=1; column<=row; ++column)
    {
			element = hep_matrix(row, column);
      (*this)(row,column) = element;
			if (row != column)
			{
				(*this)(column,row) = element;
			}
    }
  }
}

SymmetricMatrix::SymmetricMatrix(const size_t size)
  : Matrix<double>(size, size)
{ }

SymmetricMatrix::SymmetricMatrix(const size_t	size, const double& value)
  : Matrix<double>()
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

SymmetricMatrix::SymmetricMatrix(const size_t					 size,
																	double const * const data)
  : Matrix<double>()
{
  build_matrix(size, data);
}

//*************************
// Size Functions
//*************************

const size_t SymmetricMatrix::size() const
{
	return number_of_rows();
}

//*************************
// Assignment Operators
//*************************

SymmetricMatrix& SymmetricMatrix::operator=(const SymmetricMatrix& rhs)
{
	Matrix<double>::operator=(rhs);
	return *this;
}

SymmetricMatrix& SymmetricMatrix::operator+=(const SymmetricMatrix& rhs)
{
	Matrix<double>::operator+=(rhs);
	return *this;
}

SymmetricMatrix& SymmetricMatrix::operator-=(const SymmetricMatrix& rhs)
{
	Matrix<double>::operator-=(rhs);
	return *this;
}

SymmetricMatrix& SymmetricMatrix::operator*=(const double& rhs)
{
	Matrix<double>::operator*=(rhs);
  return *this;
}

SymmetricMatrix& SymmetricMatrix::operator/=(const double& rhs)
{
	Matrix<double>::operator/=(rhs);
  return *this;
}

//*************************
// Algebraic Operators
//*************************

const SymmetricMatrix SymmetricMatrix::operator+(
	const SymmetricMatrix& rhs) const
{
  return SymmetricMatrix(*this) += rhs;
}

const SymmetricMatrix SymmetricMatrix::operator-(
  const SymmetricMatrix& rhs) const
{
  return SymmetricMatrix(*this) -= rhs;
}

const SymmetricMatrix SymmetricMatrix::operator*(const double& rhs) const
{
  return SymmetricMatrix(*this) *= rhs;
}

const SymmetricMatrix SymmetricMatrix::operator/(const double& rhs) const
{
  return SymmetricMatrix(*this) /= rhs;
}

//############################
// SymmetricMatrix (protected)
//############################

void SymmetricMatrix::build_matrix(
  const size_t size, const bool initialize)
{
	Matrix<double>::build_matrix(size, size, initialize);
}

void SymmetricMatrix::build_matrix(const size_t         size,
                                   double const * const data)
{
  build_matrix(size, false);
	 double element;
  for(size_t row=0; row<size; ++row)
  {
    for(size_t column=0; column<=row; ++column)
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

//############################
// Free Functions
//############################

//****************************
// Conversion Functions
//****************************

SymmetricMatrix inverse(const SymmetricMatrix& matrix)
{
	return inverse((Matrix<double>) matrix);
}

namespace CLHEP
{

::CLHEP::HepSymMatrix HepSymMatrix(const SymmetricMatrix& matrix)
{
  size_t size = matrix.size();
  
  ::CLHEP::HepSymMatrix hep_matrix(size);
	double element;
  for(size_t row=1; row<=size; ++row)
  {
    for(size_t column=1; column<=row; ++column)
    {
			element = matrix(row, column);
      hep_matrix(row, column) = element;
			if (row != column)
			{
				hep_matrix(column, row) = element;
			}
    }
  }
  return hep_matrix;
}

}

//*************************
// Eigensystem Functions
//*************************

Vector<double> eigenvalues(const SymmetricMatrix& matrix)
{
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if(rows != columns)
  {
    throw(Squeal(Squeal::recoverable,
                 "Attempt to get eigenvalues of non-square matrix",
                 "MAUS::eigenvalues") );
  }
  SymmetricMatrix temp_matrix(matrix);
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

std::pair<Vector<double>, Matrix<double> > eigensystem(
  const SymmetricMatrix& matrix)
{
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if(rows != columns)
  {
    throw(Squeal(Squeal::recoverable,
                 "Attempt to get eigensystem of non-square matrix",
                 "MAUS::eigensystem") );
  }
  SymmetricMatrix temp_matrix(matrix);
  gsl_vector * eigenvalues = gsl_vector_alloc(rows);
  gsl_matrix * eigenvectors = gsl_matrix_calloc(rows, columns);
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
  Matrix<double> eigenvector_matrix(rows, columns,
                                    (double*) eigenvectors->data);
  gsl_matrix_free(eigenvectors);
  return std::pair<Vector<double>, Matrix<double> >(
           eigenvalue_vector, eigenvector_matrix);
}

//*************************
// Unitary Operators
//*************************

SymmetricMatrix operator-(const SymmetricMatrix& matrix)
{
	return -((Matrix<double>) matrix);
}

//*************************
// Scalar Operators
//*************************

SymmetricMatrix operator*(const double&					 lhs,
													const SymmetricMatrix& rhs)
{
	return lhs * (Matrix<double>) rhs;
}

} //namespace MAUS
