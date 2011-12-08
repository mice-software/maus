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
template class HermitianMatrix;
template <typename StdType> class Vector;


//*************************
// Conversion Functions
//*************************

/** @brief Returns a real-valued matrix containing the real part of the
 *         corresponding elements of the given complex-valued matrix.
 */
SymmetricMatrix real(const HermitianMatrix& complex_matrix);
/** @brief Returns a real-valued matrix containing the imaginary part of the
 *         corresponding elements of the given complex-valued matrix.
 */
SymmetricMatrix imag(const HermitianMatrix& complex_matrix);
/** @brief Returns a complex-valued matrix containing the complex conjugate
 *         of the elements of the given matrix.
 */
HermitianMatrix dagger(const HermitianMatrix& complex_matrix);

HermitianMatrix::HermitianMatrix(const SymmetricMatrix& real_matrix);

/** @brief Constructs a Hermitian matrix made up of two symmetric matrices. The
 *				 "real" matrix yields the real parts of the complex elements. The
 *				 "imaginary" matrix yields the imagniary parts of the complex elements
 *				 by multiplying it by i, ignoring any non-zero diagonal components
 *				 of the imaginary matrix, and conjugating the upper triangle.
 */
HermitianMatrix::HermitianMatrix(
	const SymmetricMatrix& real_matrix,
  const SymmetricMatrix& imaginary_matrix);

/** @brief returns the inverse of a matrix
 */
HermitianMatrix inverse(const HermitianMatrix& matrix);

namespace CLHEP
{
/** @brief Creates a CLHEP::HepMatrix from a MAUS::Matrix. This function is
 *         meant to look like a copy constructor for the HepMatrix class, but
 *         it is a free function in the namespace MAUS::CLHEP.
 *
 */
::CLHEP::HepSymMatrix HepSymMatrix(const HermitianMatrix& matrix);
}

//*************************
// Eigensystem Functions
//*************************

/** @brief returns a vector of eigenvalues. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
Vector<double> eigenvalues(const HermitianMatrix& matrix);

/** @brief returns a vector of std::pair containing a vector of eigenvalues
 *         and a matrix of eigenvectors. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
std::pair<Vector<double>, Matrix<complex> > eigensystem(
  const HermitianMatrix& matrix);

//*************************
// Unitary Operators
//*************************

MAUS::HermitianMatrix operator-(const HermitianMatrix& matrix);

//*************************
// Scalar Operators
//*************************

MAUS::HermitianMatrix  operator*(
  const double&						value,
  const HermitianMatrix& matrix);


/** @class HermitianMatrix extends Matrix by enforcing symmetric arrangements of
 *				 elements. All elements are stored despite the redundent nature of
 *				 symmetric matrices. This is so that base class functions will work
 *				 properly. They will just return an ordinary Matrix object and be
 *				 uncastable as a HermitianMatrix.
 */
class HermitianMatrix : public Matrix<complex>
{
public:
  /** @brief default constructor makes an empty HermitianMatrix of size 0
   */
	HermitianMatrix();

  /** @brief Copy constructor
   */
	HermitianMatrix(const HermitianMatrix& original_instance);

  /** @brief Construct a matrix and fill all fields with 0
   *
   *  @params size number of rows/columns
   */
	HermitianMatrix(const size_t size);

  /** @brief Construct a matrix and fill with identical data
   *
   *  @params size number of rows/columns
   *  @params data variable to be copied into all items in the matrix
   */
	HermitianMatrix(const size_t size, const complex& value);

  /** @brief Construct a matrix and fill with data from an array
   *
   *  @params size number of rows/columns
   *  @params data pointer to the start of a memory block of size
   *          size^2 with data laid out <row> <row> <row>. Only the lower
	 *					triangle of data is used to easily ensure element symmetry.
	 *					Note HermitianMatrix does not take ownership of memory in data.
   */
	HermitianMatrix(const size_t size, comlex const * const data);

  //*************************
  // Size Functions
  //*************************

  /** @brief returns number of rows/columns in the matrix
   */
  size_t size() const;
  
  //*************************
  // Assignment Operators
  //*************************
  HermitianMatrix& operator =(
		const HermitianMatrix&									 rhs);
  HermitianMatrix& operator+=(
    const HermitianMatrix&									 rhs);
  HermitianMatrix& operator-=(
    const HermitianMatrix&									 rhs);
  HermitianMatrix& operator*=(const complex& rhs);
  HermitianMatrix& operator/=(const complex& rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const HermitianMatrix operator+(
    const HermitianMatrix&											 rhs) const;
  const HermitianMatrix operator-(
    const HermitianMatrix&											 rhs) const;
  const HermitianMatrix operator*(const complex& rhs) const;
  const HermitianMatrix operator/(const complex& rhs) const;

  //*************************
  // Befriending
  //*************************

  //These use their Matrix counterparts and then use the protected base class
	//copy constructor to cast the return type as a HermitianMatrix
  friend HermitianMatrix inverse(
		const HermitianMatrix& matrix);
	friend HermitianMatrix MAUS::operator-(
		const HermitianMatrix& matrix);
	friend HermitianMatrix MAUS::operator*(
		const complex&				 value,
		const HermitianMatrix& matrix);

	//These use special low-level gsl functions for symmetric matricies
  friend Vector<double> eigenvalues(const HermitianMatrix& matrix);
  friend std::pair<Vector<double>, Matrix<complex> >
  eigensystem(const HermitianMatrix& matrix);

protected:
	
  /** @brief Base class copy constructor
   */
	HermitianMatrix(const Matrix<complex>& original_instance);

  //build the matrix with size^2 elements initialised to zero by default
  void build_matrix(const size_t size, const bool initialize=true);  

  //build the matrix with size^2 elements initialised to array data in
  //row major order
  void build_matrix(const size_t size,  complex const * const data);

};

} //end namespace MAUS
        
#endif
