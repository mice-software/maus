// MAUS WARNING: THIS IS LEGACY CODE.
//This file is a part of MAUS
//
//MAUS is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//MAUS is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with MAUS in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

#ifndef _SRC_COMMON_CPP_MATHS_SYMMETRIC_MATRIX_HH_
#define _SRC_COMMON_CPP_MATHS_SYMMETRIC_MATRIX_HH_

#include <iostream>
#include <vector>

#include "Interface/Complex.hh"
#include "Interface/Squeal.hh"
#include "Interface/Vector.hh"
#include "Interface/Matrix.hh"

namespace CLHEP
{
  class HepSymMatrix;
}

namespace MAUS
{

//*************************
// Forward Declarations
//*************************
template <typename StdType> class Matrix;
template <typename StdType> class SymmetricMatrix;
template <typename StdType> class Vector;


//*************************
// Conversion Functions
//*************************

/** @brief Returns a real-valued matrix containing the real part of the
 *         corresponding elements of the given complex-valued matrix.
 */
SymmetricMatrix<double> real(const SymmetricMatrix<complex>& complex_matrix);
/** @brief Returns a real-valued matrix containing the imaginary part of the
 *         corresponding elements of the given complex-valued matrix.
 */
SymmetricMatrix<double> imag(const SymmetricMatrix<complex>& complex_matrix);
/** @brief Returns a complex-valued matrix containing the complex conjugate
 *         of the elements of the given matrix.
 */
SymmetricMatrix<complex> conj(const SymmetricMatrix<complex>& complex_matrix);

namespace Complex
{

SymmetricMatrix<MAUS::complex> complex(
	const SymmetricMatrix<double>& real_matrix);
SymmetricMatrix<MAUS::complex> complex(
	const SymmetricMatrix<double>& real_matrix,
  const SymmetricMatrix<double>& imaginary_matrix);

} //namespace Complex

/** @brief returns the inverse of a matrix
 */
template <typename StdType>
SymmetricMatrix<StdType> inverse(const SymmetricMatrix<StdType>& matrix);

/** @brief returns the transpose of a matrix (such that M(i,j) = T(j,i))
 */
template <typename StdType>
SymmetricMatrix<StdType> transpose(const SymmetricMatrix<StdType>& matrix);

namespace CLHEP
{
/** @brief Creates a CLHEP::HepMatrix from a MAUS::Matrix. This function is
 *         meant to look like a copy constructor for the HepMatrix class, but
 *         it is a free function in the namespace MAUS::CLHEP.
 *
 */
::CLHEP::HepSymMatrix HepSymMatrix(const SymmetricMatrix<double>& matrix);
}

//*************************
// Eigensystem Functions
//*************************

/** @brief returns a vector of eigenvalues. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
Vector<double> eigenvalues(const SymmetricMatrix<double>& matrix);

/** @brief returns a vector of std::pair containing a vector of eigenvalues
 *         and a matrix of eigenvectors. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
std::pair<Vector<double>, Matrix<double> > eigensystem(
  const SymmetricMatrix<double>& matrix);

//*************************
// Unitary Operators
//*************************

template <typename StdType> MAUS::SymmetricMatrix<StdType> operator-(
  const SymmetricMatrix<StdType>& matrix);

//*************************
// Scalar Operators
//*************************

template <typename StdType> MAUS::SymmetricMatrix<StdType>  operator*(
  const StdType&									 value,
  const SymmetricMatrix<StdType>&  matrix);


/** @class SymmetricMatrix extends Matrix by enforcing symmetric arrangements of
 *				 elements. All elements are stored despite the redundent nature of
 *				 symmetric matrices. This is so that base class functions will work
 *				 properly. They will just return an ordinary Matrix object and be
 *				 uncastable as a SymmetricMatrix.
 */
template <typename StdType>
class SymmetricMatrix : public Matrix<StdType>
{
public:
  /** @brief default constructor makes an empty SymmetricMatrix of size 0
   */
  SymmetricMatrix();

  /** @brief Copy constructor
   */
  SymmetricMatrix(const SymmetricMatrix<StdType>& original_instance);

  /** @brief Copy constructor for CLHEP::HepSymMatrix
   */
  SymmetricMatrix(const ::CLHEP::HepSymMatrix& hep_matrix );

  /** @brief Construct a matrix and fill all fields with 0
   *
   *  @params size number of rows/columns
   */
  SymmetricMatrix(const size_t size);

  /** @brief Construct a matrix and fill with identical data
   *
   *  @params size number of rows/columns
   *  @params data variable to be copied into all items in the matrix
   */
  SymmetricMatrix(const size_t size, const StdType value);

  /** @brief Construct a matrix and fill with data from an array
   *
   *  @params size number of rows/columns
   *  @params data pointer to the start of a memory block of size
   *          size^2 with data laid out <row> <row> <row>. Only the lower
	 *					triangle of data is used to easily ensure element symmetry.
	 *					Note SymmetricMatrix does not take ownership of memory in data.
   */
  SymmetricMatrix(const size_t size, StdType const * const data);

  //*************************
  // Size Functions
  //*************************

  /** @brief returns number of rows/columns in the matrix
   */
  size_t size() const;
  
  //*************************
  // Assignment Operators
  //*************************
  SymmetricMatrix<StdType>& operator =(
		const SymmetricMatrix<StdType>&										rhs);
  SymmetricMatrix<StdType>& operator+=(
    const SymmetricMatrix<StdType>&                   rhs);
  SymmetricMatrix<StdType>& operator-=(
    const SymmetricMatrix<StdType>&                   rhs);
  SymmetricMatrix<StdType>& operator*=(const StdType& rhs);
  SymmetricMatrix<StdType>& operator/=(const StdType& rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const SymmetricMatrix<StdType> operator+(
    const SymmetricMatrix<StdType>&                       rhs) const;
  const SymmetricMatrix<StdType> operator-(
    const SymmetricMatrix<StdType>&                       rhs) const;
  const SymmetricMatrix<StdType> operator*(const StdType& rhs) const;
  const SymmetricMatrix<StdType> operator/(const StdType& rhs) const;

  //*************************
  // Befriending
  //*************************

  //These use their Matrix counterparts and then use the protected base class
	//copy constructor to cast the return type as a SymmetricMatrix
	friend SymmetricMatrix<double> real(
		const SymmetricMatrix<complex>& complex_matrix);
	friend SymmetricMatrix<double> imag(
		const SymmetricMatrix<complex>& complex_matrix);
	friend SymmetricMatrix<complex> conj(
		const SymmetricMatrix<complex>& complex_matrix);
	friend SymmetricMatrix<MAUS::complex> MAUS::Complex::complex(
		const SymmetricMatrix<double>& real_matrix);
	friend SymmetricMatrix<MAUS::complex> MAUS::Complex::complex(
		const SymmetricMatrix<double>& real_matrix,
		const SymmetricMatrix<double>& imaginary_matrix);
  friend SymmetricMatrix<StdType> inverse<>(
		const SymmetricMatrix<StdType>& matrix);
  friend SymmetricMatrix<StdType> transpose<>(
		const SymmetricMatrix<StdType>& matrix);
	friend SymmetricMatrix<StdType> MAUS::operator-<>(
		const SymmetricMatrix<StdType>& matrix);
	friend SymmetricMatrix<StdType> MAUS::operator*<>(
		const StdType&									 value,
		const SymmetricMatrix<StdType>&  matrix);

	//These use special low-level gsl functions for symmetric matricies
  friend Vector<double> eigenvalues(const SymmetricMatrix<double>& matrix);
  friend std::pair<Vector<double>, Matrix<double> >
  eigensystem(const SymmetricMatrix<double>& matrix);

protected:
	
  /** @brief Base class copy constructor
   */
  SymmetricMatrix(const Matrix<StdType>& original_instance);

  //build the matrix with size^2 elements initialised to zero by default
  void build_matrix(const size_t size, const bool initialize=true);  

  //build the matrix with size^2 elements initialised to array data in
  //row major order
  void build_matrix(const size_t size, StdType const * const data);

};

} //end namespace MAUS
        
#endif
