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
//Some more details on implementation
//
//To get the templates right in cc file, you need to declare a function
//and then tell compiler which template objects to compile - otherwise
//you will get a linker error
//
//I am trying to replace CLHEP matrix here, as CLHEP has less functionality
//and is probably worse written than GSL. A similar thing exists in boost
//library
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
// Forward Declarations
//*************************

//*************************
// Constructors
//*************************
  
template <typename StdType>
SymmetricMatrix<StdType>::SymmetricMatrix() : Matrix<StdType>()
{ }
template SymmetricMatrix<double>::SymmetricMatrix();
template SymmetricMatrix<complex>::SymmetricMatrix();

template <typename StdType>
SymmetricMatrix<StdType>::SymmetricMatrix(
  const SymmetricMatrix<StdType>& original_instance)
  : Matrix<StdType>(original_instance)
{ }
template SymmetricMatrix<double>::SymmetricMatrix(
  const SymmetricMatrix<double>& original_instance);
template SymmetricMatrix<complex>::SymmetricMatrix(
  const SymmetricMatrix<complex>& original_instance);

template <>
SymmetricMatrix<double>::SymmetricMatrix(
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
  
template <>
SymmetricMatrix<complex>::SymmetricMatrix(
  const ::CLHEP::HepSymMatrix& hep_matrix ) : Matrix<complex>()
{
  size_t size = hep_matrix.num_row();
	
  build_matrix(size, false);
	complex element;
  for(size_t row=1; row<=size; ++row)
  {
    for(size_t column=1; column<= row; ++column)
    {
			element = Complex::complex(hep_matrix(row,column));
      (*this)(row,column) = element;
			if (row != column)
			{
				(*this)(column,row) = element;
			}
    }
  }
}

template <typename StdType>
SymmetricMatrix<StdType>::SymmetricMatrix(const size_t size)
  : Matrix<StdType>(size, size)
{ }
template SymmetricMatrix<double>::SymmetricMatrix(
  const size_t size);
template SymmetricMatrix<complex>::SymmetricMatrix(
  const size_t size);

template <typename StdType>
SymmetricMatrix<StdType>::SymmetricMatrix(const size_t	size,
																					const StdType	value)
  : Matrix<StdType>()
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
template SymmetricMatrix<double>::SymmetricMatrix(
  const size_t size, const double value);
template SymmetricMatrix<complex>::SymmetricMatrix(
  const size_t size, const complex value);

template <typename StdType>
SymmetricMatrix<StdType>::SymmetricMatrix(const size_t					size,
																					StdType const * const data)
  : Matrix<StdType>()
{
  build_matrix(size, data);
}
template SymmetricMatrix<double>::SymmetricMatrix(
  const size_t size, double const * const data);
template SymmetricMatrix<complex>::SymmetricMatrix(
  const size_t size, complex const * const data);

//*************************
// Size Functions
//*************************

template <typename StdType>
size_t SymmetricMatrix<StdType>::size() const
{
	return Matrix<StdType>::number_of_rows();
}
template size_t SymmetricMatrix<double>::size() const;
template size_t SymmetricMatrix<complex>::size() const;

//*************************
// Assignment Operators
//*************************

template <typename StdType>
SymmetricMatrix<StdType>& SymmetricMatrix<StdType>::operator=(
  const SymmetricMatrix<StdType>& rhs)
{
	((Matrix<StdType>) *this) = (Matrix<StdType>) rhs;
	return *this;
}
template SymmetricMatrix<double>& SymmetricMatrix<double>::operator=(
  const SymmetricMatrix<double>& rhs);
template SymmetricMatrix<complex>& SymmetricMatrix<complex>::operator=(
  const SymmetricMatrix<complex>& rhs);

template <typename StdType>
SymmetricMatrix<StdType>& SymmetricMatrix<StdType>::operator+=(
	const SymmetricMatrix<StdType>& rhs)
{
	((Matrix<StdType>) *this) += (Matrix<StdType>) rhs;
	return *this;
}
template SymmetricMatrix<double>& SymmetricMatrix<double>::operator+=(
	const SymmetricMatrix<double>& rhs);
template SymmetricMatrix<complex>& SymmetricMatrix<complex>::operator+=(
	const SymmetricMatrix<complex>& rhs);

template <typename StdType>
SymmetricMatrix<StdType>& SymmetricMatrix<StdType>::operator-=(
	const SymmetricMatrix<StdType>& rhs)
{
	((Matrix<StdType>) *this) -= (Matrix<StdType>) rhs;
	return *this;
}
template SymmetricMatrix<double>& SymmetricMatrix<double>::operator-=(
	const SymmetricMatrix<double>& rhs);
template SymmetricMatrix<complex>& SymmetricMatrix<complex>::operator-=(
	const SymmetricMatrix<complex>& rhs);

template <typename StdType>
SymmetricMatrix<StdType>& SymmetricMatrix<StdType>::operator*=(
	const StdType& rhs)
{
	((Matrix<StdType>) *this) *= rhs;
  return *this;
}
template SymmetricMatrix<double>& SymmetricMatrix<double>::operator*=(
	const double& rhs);
template SymmetricMatrix<complex>& SymmetricMatrix<complex>::operator*=(
	const complex& rhs);

template <typename StdType>
SymmetricMatrix<StdType>& SymmetricMatrix<StdType>::operator/=(
	const StdType& rhs)
{
	((Matrix<StdType>) *this) /= rhs;
  return *this;
}
template SymmetricMatrix<double>& SymmetricMatrix<double>::operator/=(
	const double& rhs);
template SymmetricMatrix<complex>& SymmetricMatrix<complex>::operator/=(
	const complex& rhs);

//*************************
// Algebraic Operators
//*************************

template <typename StdType>
const SymmetricMatrix<StdType> SymmetricMatrix<StdType>::operator+(
  const SymmetricMatrix<StdType>& rhs) const
{
  return SymmetricMatrix<StdType>(*this) += rhs;
}
template const SymmetricMatrix<double>
SymmetricMatrix<double>::operator+(
  const SymmetricMatrix<double>& rhs) const;
template const SymmetricMatrix<complex>
SymmetricMatrix<complex>::operator+(
  const SymmetricMatrix<complex>& rhs) const;

template <typename StdType>
const SymmetricMatrix<StdType> SymmetricMatrix<StdType>::operator-(
  const SymmetricMatrix<StdType>& rhs) const
{
  return SymmetricMatrix<StdType>(*this) -= rhs;
}
template const SymmetricMatrix<double>
SymmetricMatrix<double>::operator-(
  const SymmetricMatrix<double>& rhs) const;
template const SymmetricMatrix<complex>
SymmetricMatrix<complex>::operator-(
  const SymmetricMatrix<complex>& rhs) const;

template <typename StdType>
const SymmetricMatrix<StdType> SymmetricMatrix<StdType>::operator*(
  const StdType& rhs) const
{
  return SymmetricMatrix<StdType>(*this) *= rhs;
}
template const SymmetricMatrix<double>
SymmetricMatrix<double>::operator*(
  const double& rhs) const;
template const SymmetricMatrix<complex>
SymmetricMatrix<complex>::operator*(
  const complex& rhs) const;

template <typename StdType>
const SymmetricMatrix<StdType> SymmetricMatrix<StdType>::operator/(
  const StdType& rhs) const
{
  return SymmetricMatrix<StdType>(*this) /= rhs;
}
template const SymmetricMatrix<double>
SymmetricMatrix<double>::operator/(
  const double& rhs) const;
template const SymmetricMatrix<complex>
SymmetricMatrix<complex>::operator/(
  const complex& rhs) const;

//############################
// SymmetricMatrix (protected)
//############################

template <typename StdType> SymmetricMatrix<StdType>::SymmetricMatrix(
	const Matrix<StdType>& original_instance) : Matrix<StdType>(original_instance)
{ }

template <typename StdType>
void SymmetricMatrix<StdType>::build_matrix(
  const size_t size, const bool initialize)
{
	Matrix<StdType>::build_matrix(size, size, initialize);
}
template void SymmetricMatrix<double>::build_matrix(
  const size_t size, const bool initialize);
template void SymmetricMatrix<complex>::build_matrix(
  const size_t size, const bool initialize);

template <typename StdType>
void SymmetricMatrix<StdType>::build_matrix(const size_t          size,
                                            StdType const * const data)
{
  build_matrix(size, false);
	StdType element;
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
template void SymmetricMatrix<double>::build_matrix(
  const size_t size, double const * const data);
template void SymmetricMatrix<complex>::build_matrix(
  const size_t size, complex const * const data);

//############################
// Template Declarations
//############################

template class SymmetricMatrix<double>;
template class SymmetricMatrix<complex>;

//############################
// Free Functions
//############################

//****************************
// Conversion Functions
//****************************

SymmetricMatrix<double> real(const SymmetricMatrix<complex>& complex_matrix)
{
	return real((Matrix<complex>) complex_matrix);
}

SymmetricMatrix<double> imag(const SymmetricMatrix<complex>& complex_matrix)
{
	return imag((Matrix<complex>) complex_matrix);
}

SymmetricMatrix<complex> conj(const SymmetricMatrix<complex>& matrix)
{
	return conj((Matrix<complex>) matrix);
}

namespace Complex
{

SymmetricMatrix<MAUS::complex> complex(
	const SymmetricMatrix<double>& real_matrix)
{
	return MAUS::Complex::complex((Matrix<double>) real_matrix);
}

SymmetricMatrix<MAUS::complex> complex(
	const SymmetricMatrix<double>& real_matrix,
  const SymmetricMatrix<double>& imaginary_matrix)
{
	return MAUS::Complex::complex((Matrix<double>) real_matrix,
																(Matrix<double>) imaginary_matrix);
}

} //namespace Matrix

template <typename StdType>
SymmetricMatrix<StdType> inverse(const SymmetricMatrix<StdType>& matrix)
{
	return inverse((Matrix<StdType>) matrix);
}
template SymmetricMatrix<double> inverse(
	const SymmetricMatrix<double>& matrix);
template SymmetricMatrix<complex> inverse(
	const SymmetricMatrix<complex>& matrix);

template <typename StdType>
SymmetricMatrix<StdType> transpose(const SymmetricMatrix<StdType>& matrix)
{
	return transpose((Matrix<StdType>) matrix);
}
template SymmetricMatrix<double> transpose(
  const SymmetricMatrix<double>& matrix);
template SymmetricMatrix<complex> transpose(
  const SymmetricMatrix<complex>& matrix);

namespace CLHEP
{

::CLHEP::HepSymMatrix HepSymMatrix(const SymmetricMatrix<double>& matrix)
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

//BOOKMARK

//*************************
// Eigensystem Functions
//*************************

Vector<double> eigenvalues(const SymmetricMatrix<double>& matrix)
{
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if(rows != columns)
  {
    throw(Squeal(Squeal::recoverable,
                 "Attempt to get eigenvalues of non-square matrix",
                 "MAUS::eigenvalues") );
  }
  SymmetricMatrix<double> temp_matrix(matrix);
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
  const SymmetricMatrix<double>& matrix)
{
  size_t rows = matrix.number_of_rows();
  size_t columns = matrix.number_of_columns();
  if(rows != columns)
  {
    throw(Squeal(Squeal::recoverable,
                 "Attempt to get eigensystem of non-square matrix",
                 "MAUS::eigensystem") );
  }
  SymmetricMatrix<double> temp_matrix(matrix);
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

template <typename StdType>
SymmetricMatrix<StdType> operator-(const SymmetricMatrix<StdType>& matrix)
{
	return -((Matrix<StdType>) matrix);
}
template SymmetricMatrix<double> operator-(
	const SymmetricMatrix<double>& matrix);
template SymmetricMatrix<complex> operator-(
	const SymmetricMatrix<complex>& matrix);

//*************************
// Scalar Operators
//*************************

template <typename StdType>
SymmetricMatrix<StdType> operator*(const StdType&									 lhs,
																	 const SymmetricMatrix<StdType>& rhs)
{
		return lhs * (Matrix<StdType>) rhs;
}
template SymmetricMatrix<double> operator*(
  const double& lhs, const SymmetricMatrix<double>& rhs);
template SymmetricMatrix<complex> operator*(
  const complex& lhs, const SymmetricMatrix<complex>& rhs);

} //namespace MAUS
