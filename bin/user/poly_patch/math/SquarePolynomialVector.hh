// MAUS WARNING: THIS IS LEGACY CODE.
// Copyright 2010-2011 Chris Rogers
// 
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see 
// <http://www.gnu.org/licenses/>.

#ifndef SquarePolynomialVector_hh
#define SquarePolynomialVector_hh 1

#include "math/MMatrix.hh"
#include "math/MVector.hh"
#include "math/PolynomialVector.hh"

#include <map>

/** SquarePolynomialVector, an arbitrary order polynomial vector class.
 *
 *  PolynomialVector describes a vector of multivariate polynomials\n
 *  \f$y_i = a_0 + Sum (a_j x^j)\f$
 *  i.e. maps a vector \f$\vec{x}\f$ onto a vector \f$\vec{y}\f$ with\n
 *  \f$\vec{y} = a_0 + sum( a_{j_1j_2...j_n} x_1^{j_1} x_2^{j_2} ... x_n^{j_n})\f$.
 *  Also algorithms to map a single index J into \f$j_1...j_n\f$.\n
 *  \n
 *  PolynomialVector represents the polynomial coefficients as a matrix of doubles so that\n
 *  \f$ \vec{y} = \mathbf{A} \vec{w} \f$\n
 *  where \f$\vec{w}\f$ is a vector with elements given by \n
 *  \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
 *  So the index \f$ i \f$ is actually itself a vector. The vectorisation of \f$ i \f$ is handled by IndexByPower and IndexByVector methods.
 *  IndexByPower gives an index like:\n
 *  \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
 *  While IndexByVector gives an index like:\n
 *  \f$ w_i = x_{i_1}x_{i_2} \ldots x_{i_n} \f$ \n
 *  \n
 */

class SquarePolynomialVector {
  public:
    typedef PolynomialVector::PolynomialCoefficient PolynomialCoefficient;

    /** Default constructor
     *
     *  Leaves everything empty, call SetCoefficients to set up properly.
     */
    SquarePolynomialVector();

    /** Copy constructor
     */
    SquarePolynomialVector (const SquarePolynomialVector& pv); 
    /// Construct polynomial vector passing polynomial coefficients as a matrix.
    SquarePolynomialVector (int pointDimension, MMatrix<double> polynomialCoefficients); 
    /// Construct polynomial vector passing polynomial coefficients as a list of PolynomialCoefficient objects.

    /// Any coefficients missing from the vector are set to 0. Maximum order of the polynomial is given by the 
    /// maximum order of the coefficients in the vector.
    SquarePolynomialVector (std::vector<PolynomialCoefficient> coefficients); 
    /// Destructor - no memory allocated so doesn't do anything
    ~SquarePolynomialVector() {;}

    /// Reinitialise the polynomial vector with new point (x) dimension and coefficients
    void             SetCoefficients(int pointDim, MMatrix<double> coeff);
    /// Reinitialise the polynomial vector with new point (x) dimension and coefficients.

    /// Any coefficients missing from the vector are set to 0. Maximum order of the polynomial is given by the 
    /// maximum order of the coefficients in the vector.
    void             SetCoefficients(std::vector<PolynomialCoefficient> coeff);
    /// Set the coefficients.

    /// This method can't be used to change point dimension or 
    /// value dimension - any range mismatch will be ignored.
    void             SetCoefficients(MMatrix<double> coeff);
    /// Return the coefficients as a matrix of doubles.
    MMatrix<double>                    GetCoefficientsAsMatrix() const {return _polyCoeffs;}
    /// Return the coefficients as a vector of PolynomialCoefficients, including 0 values.
    std::vector<PolynomialCoefficient> GetCoefficientsAsVector() const;

    /// Fill value with \f$ y_i \f$ at some set of \f$ x_i \f$ (point).

    /// point should be array of length PointDimension().
    /// value should be array of length ValueDimension().
    void  F    (const double*   point,    double* value)                  const;

    /// Fill value with \f$ y_i \f$ at some set of \f$ x_i \f$ (point).

    /// point should be vector of length PointDimension().
    /// value should be vector of length ValueDimension().
    /// Note that there is no bound checking here.
    void  F    (const MVector<double>& point,   MVector<double>& value) const;
    /// Length of the input point (x) vector.
    unsigned int PointDimension()         const {return _pointDim;}
    /// Length of the output value (y) vector.
    unsigned int ValueDimension()         const {return _polyCoeffs.num_row();}
    /// Index of highest power - 0 is const, 1 is linear, 2 is quadratic etc...
    unsigned int PolynomialOrder()        const;
    /// Polymorphic copy constructor.

    /// This is a special copy constructor for inheritance structures
    SquarePolynomialVector* Clone()             const {return new SquarePolynomialVector(*this);}
 
    /// Make a vector like \f$(c, x, x^2, x^3...)\f$.
    /// polyVector should be of size NumberOfPolynomialCoefficients().
    /// could be static but faster as member function (use lookup table for _polyKey).
    MVector<double>& MakePolyVector(const MVector<double>& point, MVector<double>& polyVector) const;
    /// Make a vector like \f$(c, x, x^2, x^3...)\f$.
    /// PolyVector should be of size NumberOfPolynomialCoefficients().
    /// Could be static but faster as member function (use lookup table for _polyKey).
    double* MakePolyVector(const double* point, double* polyVector) const;

    /// Transforms from a 1d index of polynomial coefficients to an nd index.
    /// This is slow - you should use it to build a lookup table.
    /// For polynomial term \f$x_1^i x_2^j ... x_d^n\f$ index like [i][j] ... [n]
    /// e.g. \f$x_1^4 x_2^3 = x_1^4 x_2^3 x_3^0 x_4^0\f$ = {4,3,0,0}.
    static std::vector<int> IndexByPower (int index, int nInputVariables);
    /// Transforms from a 1d index of polynomial coefficients to an nd index.
    /// This is slow - you should use it to build a lookup table.
    /// For polynomial term \f$x_i x_j...x_n\f$ index like [i][j] ... [n]
    /// e.g. \f$x_1^4 x_2^3\f$ = {1,1,1,1,2,2,2}
    static std::vector<int> IndexByVector(int index, int nInputVariables);
    /// Returns the number of coefficients required for an arbitrary dimension, order polynomial
    /// e.g. \f$a_0 + a_1 x + a_2 y + a_3 z + a_4 x^2 + a_5 xy + a_6 y^2 + a_7 xz + a_8 yz + a_9 z^2\f$
    /// => NumberOfPolynomialCoefficients(3,2) = 9 (indexing starts from 0).
    static unsigned int NumberOfPolynomialCoefficients(int pointDimension, int order);

    /// Write out the PolynomialVector (effectively just polyCoeffs).
    friend std::ostream& operator<<(std::ostream&,  const SquarePolynomialVector&);

    /// Control the formatting of the polynomial vector. If PrintHeaders is true, then every time I write
    /// a PolynomialVector it will write the header also (default).
    static void PrintHeaders(bool willPrintHeaders) {_printHeaders = willPrintHeaders;}
    /// Write out the header (PolynomialByPower index for each element).
    void PrintHeader(std::ostream& out, char int_separator='.', char str_separator=' ', int length=14, bool pad_at_start=true) const;

private:
    static void IndexByPowerRecursive(std::vector<int> check, size_t check_index, size_t poly_power, std::vector<std::vector<int> >& nearby_points);

    int                                _pointDim;
    MMatrix<double>                    _polyCoeffs;
    static std::vector< std::vector< std::vector<int> > > _polyKeyByPower; //std::vector<int>[i_1] = j_1
    static std::vector< std::vector< std::vector<int> > > _polyKeyByVector; //std::vector<int>[j_1] = i_1
    static bool                        _printHeaders;
};

std::ostream& operator<<(std::ostream&, const SquarePolynomialVector&);

#endif // SquarePolynomialVector_hh




