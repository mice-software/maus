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

#ifndef PolynomialVector_hh
#define PolynomialVector_hh 1

#include "Interface/Matrix.hh"
#include "Interface/Vector.hh"

#include "Interface/Interpolator.hh"

#include <map>

namespace MAUS
{

/// PolynomialVector, an arbitrary order polynomial vector class.

/// PolynomialVector describes a vector of multivariate polynomials \f$y_i = a_0 + Sum (a_j x^j)\f$
/// i.e. maps a vector \f$\vec{x}\f$ onto a vector \f$\vec{y}\f$ with \f$\vec{y} = a_0 + sum( a_{j_1j_2...j_n} x_1^{j1} x_2^{j2} ... x_n^{jn}  )\f$.
/// Also algorithms to map a single index J into \f$j_1...j_n\f$.\n
/// \n
/// PolynomialVector represents the polynomial coefficients as a matrix of doubles so that\n
/// \f$ \vec{y} = \mathbf{A} \vec{w} \f$\n
/// where \f$\vec{w}\f$ is a vector with elements given by \n
/// \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
/// So the index \f$ i \f$ is actually itself a vector. The vectorisation of \f$ i \f$ is handled by IndexByPower and IndexByVector methods.
/// IndexByPower gives an index like:\n
/// \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
/// While IndexByVector gives an index like:\n
/// \f$ w_i = x_{i_1}x_{i_2} \ldots x_{i_n} \f$ \n
/// \n
/// Polynomial Vector includes several functions to do least squares fits.
/// Here we find a polynomial of arbitrary dimension and order from a set of points by the method of least squares.
/// Note that the problem must be overspecified, so the number of points must be >= number of polynomial coefficients.
/// A few interesting variations on this theme... have a look!\n
///\n

class PolynomialVector : public VectorMap
{
public:

    class PolynomialCoefficient;

    /// Construct polynomial vector passing polynomial coefficients as a matrix.
    PolynomialVector (int pointDimension, Matrix<double> polynomialCoefficients); 
    /// Construct polynomial vector passing polynomial coefficients as a list of PolynomialCoefficient objects.

    /// Any coefficients missing from the vector are set to 0. Maximum order of the polynomial is given by the 
    /// maximum order of the coefficients in the vector.
    PolynomialVector (std::vector<PolynomialCoefficient> coefficients); 
    /// Destructor - no memory allocated so doesn't do anything
    ~PolynomialVector() {;}

    /// Reinitialise the polynomial vector with new point (x) dimension and coefficients
    void             SetCoefficients(int pointDim, Matrix<double> coeff);
    /// Reinitialise the polynomial vector with new point (x) dimension and coefficients.

    /// Any coefficients missing from the vector are set to 0. Maximum order of the polynomial is given by the 
    /// maximum order of the coefficients in the vector.
    void             SetCoefficients(std::vector<PolynomialCoefficient> coeff);
    /// Set the coefficients.

    /// This method can't be used to change point dimension or 
    /// value dimension - any range mismatch will be ignored.
    void             SetCoefficients(Matrix<double> coeff);
    /// Return the coefficients as a matrix of doubles.
    Matrix<double> GetCoefficientsAsMatrix() const {return _polyCoeffs;}
    /// Return the coefficients as a vector of PolynomialCoefficients, including 0 values.
    std::vector<PolynomialCoefficient> GetCoefficientsAsVector() const {return _polyVector;}

    /// Fill value with \f$ y_i \f$ at some set of \f$ x_i \f$ (point).

    /// point should be array of length PointDimension().
    /// value should be array of length ValueDimension().
    void  F    (const double*   point,    double* value)                  const;
    /// Fill value with \f$ y_i \f$ at some set of \f$ x_i \f$ (point).

    /// point should be vector of length PointDimension().
    /// value should be vector of length ValueDimension().
    /// Note that there is no bound checking here.
    void  F    (const Vector<double>& point,   Vector<double>& value) const;
    /// Length of the input point (x) vector.
    unsigned int PointDimension()         const {return _pointDim;}
    /// Length of the output value (y) vector.
    unsigned int ValueDimension()         const {return _polyCoeffs.number_of_rows();}
    /// Index of highest power - 0 is const, 1 is linear, 2 is quadratic etc...
    unsigned int PolynomialOrder()        const {if(_polyKeyByVector.size()>0) return _polyKeyByVector.back().size()+1; else return 0;}
    /// Polymorphic copy constructor.

    /// This is a special copy constructor for inheritance structures, so that
    /// I can call vectorMap->Clone() and it will create a vectorMap of the appropriate child type without
    /// the caller needing to know what type vectorMap actually is.
    PolynomialVector* Clone()             const {return new PolynomialVector(*this);}
    /// Return a copy, centred on point.
    PolynomialVector* Recentred(double* point) const;
    /// Return operator $\f R = P(Q) \f$ - NOT IMPLEMENTED
    PolynomialVector  Chain(PolynomialVector Q) const;
    /// Return inverse of the polynomial truncated at order n (in general an infinite series that does not converge, so beware!) - NOT IMPLEMENTED
    PolynomialVector  Inverse(int max_order) const;

    /// Make a vector like \f$(c, x, x^2, x^3...)\f$.
    /// polyVector should be of size NumberOfPolynomialCoefficients().
    /// could be static but faster as member function (use lookup table for _polyKey).
    Vector<double>& MakePolyVector(const Vector<double>& point, Vector<double>& polyVector) const;
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
    static unsigned int     NumberOfPolynomialCoefficients(int pointDimension, int order);
    unsigned int            NumberOfPolynomialCoefficients() {return NumberOfPolynomialCoefficients(_pointDim, PolynomialOrder());}

    /// Write out the PolynomialVector (effectively just polyCoeffs).
    friend std::ostream& operator<<(std::ostream&,  const PolynomialVector&);

    /// Control the formatting of the polynomial vector. If PrintHeaders is true, then every time I write
    /// a PolynomialVector it will write the header also (default).
    static void PrintHeaders(bool willPrintHeaders) {_printHeaders = willPrintHeaders;}
    /// Write out the header (PolynomialByPower index for each element).
    void PrintHeader(std::ostream& out, char int_separator='.', char str_separator=' ', int length=14, bool pad_at_start=true) const;

    /// Return a PolynomialVector found from a set of points by method of least squares.

    /// Finds the polynomial pol that minimises sum_i ( w_i (\vec{y_i} - Pol(\vec{x_i}))^2
    /// y_i = values[i], x_i = points[i], w_i = weightFunction(x_i) OR weights[i], polynomialOrder is the order of pol
    /// Nb if w_i not defined, I assume that w_i = 1 i.e. unweighted
    /// points and values should be the same length; points[i] should all be the same length; values[i] should all be the same length
    /// weight function should be a vector map that has PointDimension of points and returns a weight of dimension 1
    static PolynomialVector* PolynomialLeastSquaresFit(const std::vector< std::vector<double> >& points, const std::vector< std::vector<double> >& values, 
                                                       int polynomialOrder, const VectorMap* weightFunction);
    static PolynomialVector* PolynomialLeastSquaresFit(const std::vector< std::vector<double> >&  points, const std::vector< std::vector<double> >& values, 
                                                       int polynomialOrder, const std::vector<double>& weights=std::vector<double>());
    /// Find a polynomial least squares fit given that I know certain coefficients already, (stored in coeffs)
    /// (e.g. I know the polynomial to 2nd order, I want to extend to 3rd order using a least squares)
    static PolynomialVector* ConstrainedPolynomialLeastSquaresFit(const std::vector< std::vector<double> >&  points, const std::vector< std::vector<double> >& values, 
                                                       int polynomialOrder, std::vector< PolynomialCoefficient > coeffs,
                                                       const std::vector<double>& weights=std::vector<double>());
    /// Find a polynomial least squares fit given that I know certain coefficients already
    /// After finding the fit, calculate "chi2" for each particle. Compare x_out with polynomial fit of x_in, x_pol, 
    /// using delta_chi2 = \vec(x_out - x_pol).T() V^{-1} \vec(x_out - x_pol). If sum(delta_chi2)/totalWeight > chi2Limit,
    /// calculate chi2_out = \vec(x_out).T() V^{-1} \vec(x_out) and discard particles with chi2_out > chi2Start*(discardStep^n)
    /// repeat until sum(delta_chi2)/totalWeight <= chi2Limit, each time incrementing n by 1
    /// if chi2Start < 0, start with maximum chi2; if chi2End != NULL, put the final value in chi2End
    static PolynomialVector* Chi2ConstrainedLeastSquaresFit
                                                      (const std::vector< std::vector<double> >&  xin, const std::vector< std::vector<double> >& xout, 
                                                       int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                                                       double chi2Start, double discardStep, double* chi2End, double chi2Limit, 
                                                       std::vector<double> weights, bool firstIsMean=false);


    /// Now use a Function
    /// Here the LLS can choose what points it wants to study
    /// Sweep out from centre of LLS trying to keep chi2 small
    /// As chi2 goes out of tolerance, try adding a new order polynomial
    /// Guarantees that I will return a Polynomial that fits within chi2 max and sometimes this means the polynomial order will be less than chi2 max
    /// If I can't find any fit at all, will return NULL
    /// On return, delta holds the value of delta at the limit of validity
    static PolynomialVector* Chi2SweepingLeastSquaresFit
                          (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                           double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps);
    /// Sweep out from centre of LLS trying to keep chi2 small
    /// Here I let the validity region expand independently in each direction (which is slower as I need to test many more cases)
    /// First do Chi2SweepingLeastSquaresFit; then expand the validity region sequentially in each direction. Idea is to make a
    /// good fit even in the case that delta doesn't scale appropriately between different directions
    static PolynomialVector* Chi2SweepingLeastSquaresFitVariableWalls
                          (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                           double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps, std::vector<double> max_delta);

    /// Now some utility functions
    /// Should probably go in a "utility function" namespace, that does not currently exist.

    /// Return the mean of some set of values, using some set of weights
    static Vector<double> Means      (std::vector<std::vector<double> > values, std::vector<double> weights);
    /// Return the covariance matrix of some set of values, using some set of weights and some mean
    /// If length of weights is not equal to length of values use weights = 1.
    /// If length of mean is not equal to length of first value recalculate mean
    static Matrix<double> Covariances(std::vector<std::vector<double> > values, std::vector<double> weights, Vector<double> mean);
    /// Make a hypercube of points with half width of delta in each dimension so that the number
    /// of points is at least multiplier*NumberOfPolynomialCoefficients(i_order, delta.size())
    /// - so that if multiplier >= 1 it will always define a polynomial of order i_order.
    /// Require evenly spaced ito position, sometimes this means I make more points on the shell
    static std::vector< std::vector<double> > PointBox(std::vector<double> delta, int i_order);
    /// Make a hyperellipsoid of points where magnitude of each vector \f$\vec{u}$\f is chosen such that 
    /// \f$\vec{u}^T \mathbf{D}^{-1} \vec{u}=1$\f and number of points is at least
    /// multiplier*NumberOfPolynomialCoefficients(i_order, delta.num_row()) - so that if multiplier >= 1 
    /// it will always define a polynomial of order i_order. delta should be a symmetric square matrix.
    /// Note that this is just a correctly scaled version of PointBox, which defines the distribution of
    /// points on the shell.
    static std::vector< std::vector<double> > PointShell(Matrix<double> scale_matrix, int i_order);
    /// Return chi2 of residuals of some set of output variables compared with the polynomial vector of some set of input variables 
    /// Here Chi2 Avg is defined as Sum( v^T M^{-1} v ) where v is the vector of residuals and M is covariance matrix of v
    double GetAvgChi2OfDifference(std::vector< std::vector<double> > in, std::vector< std::vector<double> > out);
    /// Should probably go in a "utility function" namespace, that does not currently exist.
    /// Print a sequence: with some string that separates elements of index and some character that pads the total vector
    /// length is the total length of the output, set to < 1 to ignore (means no padding)
    /// pad_at_start determines whether to pad at start (i.e. right align) or end (i.e. left align)
    template < class Container >
    static void PrintContainer(std::ostream& out, const Container& container, char T_separator, char str_separator, int length, bool pad_at_start);
    /// Return true if a and b are identical; a must have an iterator object dereferenced by *it
    /// must have increment prefix operator defined and with != operator defined by the object pointed to by it
    template <class TEMP_CLASS, class TEMP_ITER>
    static bool IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b, TEMP_ITER a_begin, TEMP_ITER a_end, TEMP_ITER b_begin, TEMP_ITER b_end);

    /// PolynomialCoefficient sub-class - useful, could be extended if needed (e.g. become an iterator etc)
    class PolynomialCoefficient
    {
      public:
        /// inVariablesByVector is x power indexed like e.g. \f$x_1^4 x_2^3 =\f$ {1,1,1,1,2,2,2}
        PolynomialCoefficient(std::vector<int> inVariablesByVector, int outVariable, double coefficient) 
            : _inVarByVec(inVariablesByVector), _outVar(outVariable), _coefficient(coefficient) {}
        /// Accessors
        /// Pass a parameter to *set*
        std::vector<int> InVariables() const {return _inVarByVec;}
        int              OutVariable() const {return _outVar;}
        double           Coefficient() const {return _coefficient;}

        std::vector<int> InVariables(std::vector<int> inVar ) {_inVarByVec  = inVar;  return _inVarByVec;}
        int              OutVariable(int              outVar) {_outVar      = outVar; return _outVar;}
        double           Coefficient(double           coeff ) {_coefficient = coeff;  return _coefficient;}

        /// transform coefficient from subspace space_in to subspace space_out, both subspaces of some larger space
        /// if any of coeff variables is not in space_out OR not in space_in, leave this coeff untouched and Squeal
        /// so for coeff({1,2},0,1.1), coeff.space_transform({0,2,3,5}, {4,7,1,2,3,0}) would return coeff({3,4},5,1.1)
        void             SpaceTransform(std::vector<int> space_in, std::vector<int> space_out);
        /// if var is in inVariables return true
        bool             HasInVariable(int var) const {for(unsigned int i=0; i<_inVarByVec.size(); i++) if(_inVarByVec[i] == var) return true; return false;}

      private:
        std::vector<int> _inVarByVec;
        int              _outVar;
        double           _coefficient;
    };


private:
    int                                _pointDim;
    std::vector< std::vector<int> >    _polyKeyByPower; //std::vector<int>[i_1] = j_1
    std::vector< std::vector<int> >    _polyKeyByVector; //std::vector<int>[j_1] = i_1
    Matrix<double>										 _polyCoeffs;
    std::vector<PolynomialCoefficient> _polyVector;
    static bool                        _printHeaders;
};

std::ostream& operator<<(std::ostream&, const PolynomialVector&);


//template function

template <class TEMP_CLASS, class TEMP_ITER>
bool PolynomialVector::IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b, TEMP_ITER a_begin, TEMP_ITER a_end, TEMP_ITER b_begin, TEMP_ITER b_end)
{
  TEMP_ITER a_it=a_begin;
  TEMP_ITER b_it=b_begin;
  while(a_it!=a_end && b_it!=b_end) {
    if( *a_it != *b_it ) return false;
    ++a_it;
    ++b_it;
  }
  if ( a_it != a_end || b_it != b_end ) return false;
  return true;
}

} // namespace MAUS












//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Legacy PolynomialVector
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "Interface/MMatrix.hh"
#include "Interface/MVector.hh"
#include "Interface/Interpolator.hh"
#include "Interface/Squeal.hh"
#include "Interface/Mesh.hh"

#include <iomanip>
#include <sstream>
#include <math.h>

#include "gsl/gsl_sf_gamma.h"

#include <map>

/// PolynomialVector, an arbitrary order polynomial vector class.

/// PolynomialVector describes a vector of multivariate polynomials \f$y_i = a_0 + Sum (a_j x^j)\f$
/// i.e. maps a vector \f$\vec{x}\f$ onto a vector \f$\vec{y}\f$ with \f$\vec{y} = a_0 + sum( a_{j_1j_2...j_n} x_1^{j1} x_2^{j2} ... x_n^{jn}  )\f$.
/// Also algorithms to map a single index J into \f$j_1...j_n\f$.\n
/// \n
/// PolynomialVector represents the polynomial coefficients as a matrix of doubles so that\n
/// \f$ \vec{y} = \mathbf{A} \vec{w} \f$\n
/// where \f$\vec{w}\f$ is a vector with elements given by \n
/// \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
/// So the index \f$ i \f$ is actually itself a vector. The vectorisation of \f$ i \f$ is handled by IndexByPower and IndexByVector methods.
/// IndexByPower gives an index like:\n
/// \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
/// While IndexByVector gives an index like:\n
/// \f$ w_i = x_{i_1}x_{i_2} \ldots x_{i_n} \f$ \n
/// \n
/// Polynomial Vector includes several functions to do least squares fits.
/// Here we find a polynomial of arbitrary dimension and order from a set of points by the method of least squares.
/// Note that the problem must be overspecified, so the number of points must be >= number of polynomial coefficients.
/// A few interesting variations on this theme... have a look!\n
///\n

class PolynomialVector : public VectorMap
{
public:

    class PolynomialCoefficient;

    /// Construct polynomial vector passing polynomial coefficients as a matrix.
    PolynomialVector (int pointDimension, MMatrix<double> polynomialCoefficients); 
    /// Construct polynomial vector passing polynomial coefficients as a list of PolynomialCoefficient objects.

    /// Any coefficients missing from the vector are set to 0. Maximum order of the polynomial is given by the 
    /// maximum order of the coefficients in the vector.
    PolynomialVector (std::vector<PolynomialCoefficient> coefficients); 
    /// Destructor - no memory allocated so doesn't do anything
    ~PolynomialVector() {;}

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
    std::vector<PolynomialCoefficient> GetCoefficientsAsVector() const {return _polyVector;}

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
    unsigned int PolynomialOrder()        const {if(_polyKeyByVector.size()>0) return _polyKeyByVector.back().size()+1; else return 0;}
    /// Polymorphic copy constructor.

    /// This is a special copy constructor for inheritance structures, so that
    /// I can call vectorMap->Clone() and it will create a vectorMap of the appropriate child type without
    /// the caller needing to know what type vectorMap actually is.
    PolynomialVector* Clone()             const {return new PolynomialVector(*this);}
    /// Return a copy, centred on point.
    PolynomialVector* Recentred(double* point) const;
    /// Return operator $\f R = P(Q) \f$ - NOT IMPLEMENTED
    PolynomialVector  Chain(PolynomialVector Q) const;
    /// Return inverse of the polynomial truncated at order n (in general an infinite series that does not converge, so beware!) - NOT IMPLEMENTED
    PolynomialVector  Inverse(int max_order) const;

    /// Make a vector like \f$(c, x, x^2, x^3...)\f$.
    /// polyVector should be of size NumberOfPolynomialCoefficients().
    /// could be static but faster as member function (use lookup table for _polyKey).
    MVector<double>& MakePolyVector(const MVector<double>& point, MVector<double>& polyVector) const;
    /// Make a vector like \f$(c, x, x^2, x^3...)\f$.
    /// PolyVector should be of size NumberOfPolynomialCoefficients().
    /// Could be static but faster as member function (use lookup table for _polyKey).
    double*          MakePolyVector(const double* point, double* polyVector) const;

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
    static unsigned int     NumberOfPolynomialCoefficients(int pointDimension, int order);
    unsigned int            NumberOfPolynomialCoefficients() {return NumberOfPolynomialCoefficients(_pointDim, PolynomialOrder());}

    /// Write out the PolynomialVector (effectively just polyCoeffs).
    friend std::ostream& operator<<(std::ostream&,  const PolynomialVector&);

    /// Control the formatting of the polynomial vector. If PrintHeaders is true, then every time I write
    /// a PolynomialVector it will write the header also (default).
    static void PrintHeaders(bool willPrintHeaders) {_printHeaders = willPrintHeaders;}
    /// Write out the header (PolynomialByPower index for each element).
    void PrintHeader(std::ostream& out, char int_separator='.', char str_separator=' ', int length=14, bool pad_at_start=true) const;

    /// Return a PolynomialVector found from a set of points by method of least squares.

    /// Finds the polynomial pol that minimises sum_i ( w_i (\vec{y_i} - Pol(\vec{x_i}))^2
    /// y_i = values[i], x_i = points[i], w_i = weightFunction(x_i) OR weights[i], polynomialOrder is the order of pol
    /// Nb if w_i not defined, I assume that w_i = 1 i.e. unweighted
    /// points and values should be the same length; points[i] should all be the same length; values[i] should all be the same length
    /// weight function should be a vector map that has PointDimension of points and returns a weight of dimension 1
    static PolynomialVector* PolynomialLeastSquaresFit(const std::vector< std::vector<double> >& points, const std::vector< std::vector<double> >& values, 
                                                       int polynomialOrder, const VectorMap* weightFunction);
    static PolynomialVector* PolynomialLeastSquaresFit(const std::vector< std::vector<double> >&  points, const std::vector< std::vector<double> >& values, 
                                                       int polynomialOrder, const std::vector<double>& weights=std::vector<double>());
    /// Find a polynomial least squares fit given that I know certain coefficients already, (stored in coeffs)
    /// (e.g. I know the polynomial to 2nd order, I want to extend to 3rd order using a least squares)
    static PolynomialVector* ConstrainedPolynomialLeastSquaresFit(const std::vector< std::vector<double> >&  points, const std::vector< std::vector<double> >& values, 
                                                       int polynomialOrder, std::vector< PolynomialCoefficient > coeffs,
                                                       const std::vector<double>& weights=std::vector<double>());
    /// Find a polynomial least squares fit given that I know certain coefficients already
    /// After finding the fit, calculate "chi2" for each particle. Compare x_out with polynomial fit of x_in, x_pol, 
    /// using delta_chi2 = \vec(x_out - x_pol).T() V^{-1} \vec(x_out - x_pol). If sum(delta_chi2)/totalWeight > chi2Limit,
    /// calculate chi2_out = \vec(x_out).T() V^{-1} \vec(x_out) and discard particles with chi2_out > chi2Start*(discardStep^n)
    /// repeat until sum(delta_chi2)/totalWeight <= chi2Limit, each time incrementing n by 1
    /// if chi2Start < 0, start with maximum chi2; if chi2End != NULL, put the final value in chi2End
    static PolynomialVector* Chi2ConstrainedLeastSquaresFit
                                                      (const std::vector< std::vector<double> >&  xin, const std::vector< std::vector<double> >& xout, 
                                                       int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                                                       double chi2Start, double discardStep, double* chi2End, double chi2Limit, 
                                                       std::vector<double> weights, bool firstIsMean=false);


    /// Now use a Function
    /// Here the LLS can choose what points it wants to study
    /// Sweep out from centre of LLS trying to keep chi2 small
    /// As chi2 goes out of tolerance, try adding a new order polynomial
    /// Guarantees that I will return a Polynomial that fits within chi2 max and sometimes this means the polynomial order will be less than chi2 max
    /// If I can't find any fit at all, will return NULL
    /// On return, delta holds the value of delta at the limit of validity
    static PolynomialVector* Chi2SweepingLeastSquaresFit
                          (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                           double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps);
    /// Sweep out from centre of LLS trying to keep chi2 small
    /// Here I let the validity region expand independently in each direction (which is slower as I need to test many more cases)
    /// First do Chi2SweepingLeastSquaresFit; then expand the validity region sequentially in each direction. Idea is to make a
    /// good fit even in the case that delta doesn't scale appropriately between different directions
    static PolynomialVector* Chi2SweepingLeastSquaresFitVariableWalls
                          (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                           double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps, std::vector<double> max_delta);

    /// Now some utility functions
    /// Should probably go in a "utility function" namespace, that does not currently exist.

    /// Return the mean of some set of values, using some set of weights
    static MVector<double> Means      (std::vector<std::vector<double> > values, std::vector<double> weights);
    /// Return the covariance matrix of some set of values, using some set of weights and some mean
    /// If length of weights is not equal to length of values use weights = 1.
    /// If length of mean is not equal to length of first value recalculate mean
    static MMatrix<double> Covariances(std::vector<std::vector<double> > values, std::vector<double> weights, MVector<double> mean);
    /// Make a hypercube of points with half width of delta in each dimension so that the number
    /// of points is at least multiplier*NumberOfPolynomialCoefficients(i_order, delta.size())
    /// - so that if multiplier >= 1 it will always define a polynomial of order i_order.
    /// Require evenly spaced ito position, sometimes this means I make more points on the shell
    static std::vector< std::vector<double> > PointBox(std::vector<double> delta, int i_order);
    /// Make a hyperellipsoid of points where magnitude of each vector \f$\vec{u}$\f is chosen such that 
    /// \f$\vec{u}^T \mathbf{D}^{-1} \vec{u}=1$\f and number of points is at least
    /// multiplier*NumberOfPolynomialCoefficients(i_order, delta.num_row()) - so that if multiplier >= 1 
    /// it will always define a polynomial of order i_order. delta should be a symmetric square matrix.
    /// Note that this is just a correctly scaled version of PointBox, which defines the distribution of
    /// points on the shell.
    static std::vector< std::vector<double> > PointShell(MMatrix<double> scale_matrix, int i_order);
    /// Return chi2 of residuals of some set of output variables compared with the polynomial vector of some set of input variables 
    /// Here Chi2 Avg is defined as Sum( v^T M^{-1} v ) where v is the vector of residuals and M is covariance matrix of v
    double GetAvgChi2OfDifference(std::vector< std::vector<double> > in, std::vector< std::vector<double> > out);
    /// Should probably go in a "utility function" namespace, that does not currently exist.
    /// Print a sequence: with some string that separates elements of index and some character that pads the total vector
    /// length is the total length of the output, set to < 1 to ignore (means no padding)
    /// pad_at_start determines whether to pad at start (i.e. right align) or end (i.e. left align)
    template < class Container >
    static void PrintContainer(std::ostream& out, const Container& container, char T_separator, char str_separator, int length, bool pad_at_start);
    /// Return true if a and b are identical; a must have an iterator object dereferenced by *it
    /// must have increment prefix operator defined and with != operator defined by the object pointed to by it
    template <class TEMP_CLASS, class TEMP_ITER>
    static bool IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b, TEMP_ITER a_begin, TEMP_ITER a_end, TEMP_ITER b_begin, TEMP_ITER b_end);

    /// PolynomialCoefficient sub-class - useful, could be extended if needed (e.g. become an iterator etc)
    class PolynomialCoefficient
    {
      public:
        /// inVariablesByVector is x power indexed like e.g. \f$x_1^4 x_2^3 =\f$ {1,1,1,1,2,2,2}
        PolynomialCoefficient(std::vector<int> inVariablesByVector, int outVariable, double coefficient) 
            : _inVarByVec(inVariablesByVector), _outVar(outVariable), _coefficient(coefficient) {}
        /// Accessors
        /// Pass a parameter to *set*
        std::vector<int> InVariables() const {return _inVarByVec;}
        int              OutVariable() const {return _outVar;}
        double           Coefficient() const {return _coefficient;}

        std::vector<int> InVariables(std::vector<int> inVar ) {_inVarByVec  = inVar;  return _inVarByVec;}
        int              OutVariable(int              outVar) {_outVar      = outVar; return _outVar;}
        double           Coefficient(double           coeff ) {_coefficient = coeff;  return _coefficient;}

        /// transform coefficient from subspace space_in to subspace space_out, both subspaces of some larger space
        /// if any of coeff variables is not in space_out OR not in space_in, leave this coeff untouched and Squeal
        /// so for coeff({1,2},0,1.1), coeff.space_transform({0,2,3,5}, {4,7,1,2,3,0}) would return coeff({3,4},5,1.1)
        void             SpaceTransform(std::vector<int> space_in, std::vector<int> space_out);
        /// if var is in inVariables return true
        bool             HasInVariable(int var) const {for(unsigned int i=0; i<_inVarByVec.size(); i++) if(_inVarByVec[i] == var) return true; return false;}

      private:
        std::vector<int> _inVarByVec;
        int              _outVar;
        double           _coefficient;
    };


private:
    int                                _pointDim;
    std::vector< std::vector<int> >    _polyKeyByPower; //std::vector<int>[i_1] = j_1
    std::vector< std::vector<int> >    _polyKeyByVector; //std::vector<int>[j_1] = i_1
    MMatrix<double>                    _polyCoeffs;
    std::vector<PolynomialCoefficient> _polyVector;
    static bool                        _printHeaders;
};

std::ostream& operator<<(std::ostream&, const PolynomialVector&);


//template function

template <class TEMP_CLASS, class TEMP_ITER>
bool PolynomialVector::IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b, TEMP_ITER a_begin, TEMP_ITER a_end, TEMP_ITER b_begin, TEMP_ITER b_end)
{
  TEMP_ITER a_it=a_begin;
  TEMP_ITER b_it=b_begin;
  while(a_it!=a_end && b_it!=b_end) {
    if( *a_it != *b_it ) return false;
    ++a_it;
    ++b_it;
  }
  if ( a_it != a_end || b_it != b_end ) return false;
  return true;
}

////////// POLYNOMIALVECTOR START ///////////

bool PolynomialVector::_printHeaders=true;

PolynomialVector::PolynomialVector(int numberOfInputVariables, MMatrix<double> polynomialCoefficients)
        :  _pointDim(numberOfInputVariables), _polyKeyByPower(), _polyKeyByVector(), _polyCoeffs(polynomialCoefficients)
{
  SetCoefficients(numberOfInputVariables, polynomialCoefficients);
}

PolynomialVector::PolynomialVector(std::vector<PolynomialCoefficient> coefficients)
        :  _pointDim(0), _polyKeyByPower(), _polyKeyByVector(), _polyCoeffs()
{
  SetCoefficients(coefficients);
}

void PolynomialVector::SetCoefficients(int pointDim, MMatrix<double> coeff)
{
  int nPCoeff      = coeff.num_col();
  _pointDim        = pointDim;
  _polyKeyByPower  = std::vector< std::vector<int> >();
  _polyKeyByVector = std::vector< std::vector<int> >();
  _polyVector      = std::vector<PolynomialCoefficient>();

  for(int i=0; i<nPCoeff; i++)
    _polyKeyByPower.push_back(IndexByPower (i, pointDim));
  for(int i=0; i<nPCoeff; i++) 
    _polyKeyByVector.push_back(IndexByVector(i, pointDim));

  for(size_t i=0; i<_polyCoeffs.num_row(); i++) 
    for(int j=0; j<nPCoeff; j++)
      _polyVector.push_back( PolynomialVector::PolynomialCoefficient(_polyKeyByVector[j], i, _polyCoeffs(i+1, j+1) ) );
}

void PolynomialVector::SetCoefficients(std::vector<PolynomialCoefficient> coeff)
{
  _pointDim        = 0;
  _polyKeyByPower  = std::vector< std::vector<int> >();
  _polyKeyByVector = std::vector< std::vector<int> >();
  _polyVector      = coeff;

  int maxPolyOrder = 0;
  int valueDim     = 0;
  for(unsigned int i=0; i<coeff.size(); i++)
  {
    int polyOrder = coeff[i].InVariables().size();
    for(unsigned int j=0; j<coeff[i].InVariables().size(); j++)
      if(coeff[i].InVariables()[j] > _pointDim) _pointDim = coeff[i].InVariables()[j];
    if(coeff[i].OutVariable()    > valueDim)  valueDim  = coeff[i].OutVariable();
    if(polyOrder > maxPolyOrder) maxPolyOrder = polyOrder;
  }
  _pointDim++; //PointDim indexes from 0
  _polyCoeffs = MMatrix<double>(valueDim+1, NumberOfPolynomialCoefficients(_pointDim, maxPolyOrder+1), 0.);

  for(size_t i=0; i<_polyCoeffs.num_col(); i++)
    _polyKeyByPower.push_back(IndexByPower (i, _pointDim));
  for(size_t i=0; i<_polyCoeffs.num_col(); i++) 
  {
    _polyKeyByVector.push_back(IndexByVector(i, _pointDim));
    for(unsigned int j=0; j<coeff.size(); j++) 
      if(coeff[j].InVariables() == _polyKeyByVector.back()) 
      {
        int dim = coeff[j].OutVariable();
        _polyCoeffs(dim+1,i+1) = coeff[j].Coefficient();
      }
  }
}

void PolynomialVector::SetCoefficients(MMatrix<double> coeff)
{
  for(size_t i=0; i<coeff.num_row() && i<_polyCoeffs.num_row(); i++)
    for(size_t j=0; j<coeff.num_col() && j<_polyCoeffs.num_col(); j++)
      _polyCoeffs(i+1,j+1) = coeff(i+1,j+1);
}

PolynomialVector* PolynomialVector::Recentred(double * point) const
{
    throw(Squeal(Squeal::nonRecoverable, "Recentred not implemented", "PolynomialVector::Recentred"));
}

void  PolynomialVector::F(const double*   point,    double* value)          const
{
    MVector<double> pointV(PointDimension(), 1), valueV(ValueDimension(), 1);
    for(unsigned int i=0; i<PointDimension(); i++) pointV(i+1) = point[i];
    F(pointV, valueV);
    for(unsigned int i=0; i<ValueDimension(); i++) value[i]  = valueV(i+1);
}

void  PolynomialVector::F(const MVector<double>& point,   MVector<double>& value) const
{
    MVector<double> polyVector(_polyKeyByVector.size(), 1); 
    MakePolyVector(point, polyVector);
    MVector<double> answer = _polyCoeffs*polyVector;
    for(unsigned int i=0; i<ValueDimension(); i++) value(i+1) = answer(i+1);
}


MVector<double>&  PolynomialVector::MakePolyVector(const MVector<double>& point, MVector<double>& polyVector) const
{
    for(unsigned int i=0; i<_polyKeyByVector.size(); i++)
        for(unsigned int j=0; j<_polyKeyByVector[i].size(); j++)
            polyVector(i+1) *= point( _polyKeyByVector[i][j]+1 );
    return polyVector;
}

double*  PolynomialVector::MakePolyVector(const double* point, double* polyVector) const
{
    for(unsigned int i=0; i<_polyKeyByVector.size(); i++)
    {
        polyVector[i] = 1.;
        for(unsigned int j=0; j<_polyKeyByVector[i].size(); j++)
            polyVector[i] *= point[ _polyKeyByVector[i][j] ];
    }
    return polyVector;
}

//Turn int index into a std::vector<int> 'a' of length 'd' with values x_1^a_1 x_2^a_2 ... x_d^a_d
// e.g. x_1^4 x_2^3 = {4,3,0,0}
std::vector<int> PolynomialVector::IndexByPower(int index, int nInputVariables)
{
    std::vector<int> powerIndex(nInputVariables, 0);
    std::vector<int> vectorIndex = IndexByVector(index, nInputVariables);
    for(unsigned int i=0; i<vectorIndex.size(); i++)
        powerIndex[vectorIndex[i]]++;
    return powerIndex;
}

//Turn int index into an index for element of polynomial
// e.g. x_1^4 x_2^3 = {1,1,1,1,2,2,2}
std::vector<int> PolynomialVector::IndexByVector(int index, int nInputVariables)
{
    if(index == 0) return std::vector<int>();
    std::vector<int> indices(1,-1);
    nInputVariables--;
    for(int i=0; i<index; i++)
    {
        if     (indices.front() == nInputVariables) { indices = std::vector<int>(indices.size()+1, 0); indices.back()--;}
        if     (indices.back()  == nInputVariables)
        {
            int j=indices.size()-1;
            while(indices[j] == nInputVariables) j--;
            for(int k=indices.size()-1; k>=j; k--) indices[k] = indices[j]+1;
        }
        else    indices.back()++;
    }
    return indices;
}

unsigned int PolynomialVector::NumberOfPolynomialCoefficients(int pointDimension, int order)
{
    int n=0;
    if(order<=0) return 0;
    for(int i=1; i<order; i++)
        n += gsl_sf_choose(pointDimension+i-1, i);
    return n+1;
}

std::ostream& operator<<(std::ostream& out, const PolynomialVector& pv)
{
    if(PolynomialVector::_printHeaders) pv.PrintHeader(out, '.', ' ', 14, true);
    out << '\n' << pv.GetCoefficientsAsMatrix();
    return out;
}

template <class Container >
void PolynomialVector::PrintContainer(std::ostream& out, const Container& container, char T_separator, char str_separator, int length, bool pad_at_start)
{
//  class Container::iterator it;
  std::stringstream strstr1("");
  std::stringstream strstr2("");
  class Container::const_iterator it1 = container.begin();
  class Container::const_iterator it2 = it1; 
  while(it1!=container.end())
  {
    it2++;
    if(it1 != container.end() && it2 != container.end())
         strstr1 << (*it1) << T_separator;
    else strstr1 << (*it1);
    it1++;
  }

  if(int(strstr1.str().size()) > length && length > 0) strstr2 << strstr1.str().substr(1, length);
  else                                                 strstr2 << strstr1.str();

  if(!pad_at_start) out << strstr2.str();
  for(int i=0; i<length - int(strstr2.str().size()); i++) out << str_separator;
  if(pad_at_start) out << strstr2.str();
}

void PolynomialVector::PrintHeader(std::ostream& out, char int_separator, char str_separator, int length, bool pad_at_start) const
{
  if(_polyKeyByPower.size() > 0) PrintContainer< std::vector<int> >(out, _polyKeyByPower[0], int_separator, str_separator, length-1, pad_at_start);
  for(unsigned int i=1; i<_polyKeyByPower.size(); i++)
    PrintContainer<std::vector<int> >(out, _polyKeyByPower[i], int_separator, str_separator, length, pad_at_start);
}

MVector<double> PolynomialVector::Means(std::vector<std::vector<double> > values, std::vector<double> weights)
{
  if(values.size() < 1)    throw(Squeal(Squeal::recoverable, "No input values", "PolynomialVector::Means"));
  if(values[0].size() < 1) throw(Squeal(Squeal::recoverable, "Dimension < 1",   "PolynomialVector::Means"));
  if(weights.size() != values.size()) weights = std::vector<double>(values.size(), 1.);
  int                npoints     = values.size();
  int                dim         = values[0].size();
  MVector<double>    means(dim,0);
  double             totalWeight = 0;
  for(int x=0; x<npoints; x++) totalWeight += weights[x];
  for(int x=0; x<npoints; x++)
    for(int i=0; i<dim; i++)
    {
      means(i+1) += values[x][i]*weights[x]/totalWeight;
    }

  return means;
}

MMatrix<double> PolynomialVector::Covariances(std::vector<std::vector<double> > values, std::vector<double> weights, MVector<double> means)
{
  size_t                 npoints = values.size();
  if(npoints < 1) throw(Squeal(Squeal::recoverable, "No input values", "PolynomialVector::Covariances()"));
  size_t                 dim     = values[0].size();
  if(dim < 1)     throw(Squeal(Squeal::recoverable, "Dimension < 1", "PolynomialVector::Covariances()"));    
  if(means.num_row() != dim)           means   = Means(values, weights);
  if(weights.size()  != values.size()) weights = std::vector<double>(values.size(), 1.);

  MMatrix<double>    cov(dim,dim,0.);

  double             totalWeight = 0;
  for(size_t x=0; x<npoints; x++)
    totalWeight += weights[x];

  for(size_t x=0; x<npoints; x++)
    for(size_t i=0; i<dim; i++)
      for(size_t j=0; j<dim; j++) //make a sym matrix for speed
        cov(i+1,j+1) += values[x][i]*values[x][j]*weights[x]/totalWeight;

  for(size_t i=0; i<dim; i++)
    for(size_t j=0; j<dim; j++)
      cov(i+1,j+1) -= means(i+1)*means(j+1);

  return cov;
}

PolynomialVector* PolynomialVector::PolynomialLeastSquaresFit(const std::vector< std::vector<double> >&  points, const std::vector< std::vector<double> >& values, 
                                                              int polynomialOrder, const VectorMap* weightFunction)
{
  if(weightFunction == NULL) return PolynomialLeastSquaresFit(points, values, polynomialOrder);
  else
  {
    std::vector<double> weights(points.size());
    for(unsigned int i=0; i<points.size(); i++) weightFunction->F(&points[i][0], &weights[i]);
    return PolynomialLeastSquaresFit(points, values, polynomialOrder, weights);
  }
}

PolynomialVector* PolynomialVector::PolynomialLeastSquaresFit(const std::vector< std::vector<double> >& points, const std::vector< std::vector<double> >& values, 
                                                              int polynomialOrder, const std::vector<double>& weights)
{
  //Algorithm: We have F2 = sum_i ( f_k f_l) where f are polynomial terms; FY = sum_i (f_)

  int pointDim = points[0].size();
  int valueDim = values[0].size();
  int nPoints  = points.size();
  int nCoeffs  = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);

  MMatrix<double> Fy(nCoeffs, valueDim, 0.);
  MMatrix<double> F2(nCoeffs, nCoeffs,  0.);

  MMatrix<double> dummy(valueDim, nCoeffs, 0.);
  for(int i=0; i<valueDim; i++) for(int j=0; j<nCoeffs; j++) dummy(i+1, j+1) = 1;
  PolynomialVector* temp = new PolynomialVector(pointDim, dummy);

  std::vector<double>    tempFx(nCoeffs,0); //tempfx = x_i^j
  std::vector<double>    wt    (nPoints,1);
  if(weights.size() > 0) wt = weights; 

  //sum over points, values
  for(int i=0; i<nPoints; i++)
  {
    temp->MakePolyVector(&points[i][0], &tempFx[0]);
    for(int k=0; k<nCoeffs;  k++) 
      for(int j=0; j<nCoeffs;  j++) 
        F2(j+1, k+1) += tempFx[k]*tempFx[j]*wt[i];
    for(int j=0; j<nCoeffs; j++)
      for(int k=0; k<valueDim; k++)
        Fy(j+1,k+1) += values[i][k]*tempFx[j]*wt[i];
  }

  try{ F2.invert(); }
  catch(Squeal squee) { delete temp; throw(Squeal(Squeal::recoverable, "Could not find least squares fit for data", "PolynomialVector::PolynomialLeastSquaresFit")); }
  MMatrix<double> A = F2 * Fy;
  delete temp;
  temp = new PolynomialVector(pointDim, A.T());
  return temp;
}

PolynomialVector* PolynomialVector::ConstrainedPolynomialLeastSquaresFit(const std::vector< std::vector<double> >&  points, 
                                    const std::vector< std::vector<double> >& values, 
                                    int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs,
                                    const std::vector<double>& weights)
{
  //Algorithm: we want g(x) = old_f(x) + new_f(x), 
  //where old_f has polynomial terms in poly, new_f has all the rest
  //Use value - f(point) as input and do LLS forcing old_f(x) polynomial terms to 0
  //Nb: in this constrained case we have to go through the output vector and treat each like a 1D vector
  int pointDim   = points[0].size();
  int valueDim   = values[0].size();
  int nPoints    = points.size();
  if(nPoints<1)                   throw(Squeal(Squeal::recoverable, "No data for LLS Fit", "PolynomialVector::ConstrainedPolynomialLeastSquaresFit(...)"));
  if(int(values.size())!=nPoints) throw(Squeal(Squeal::recoverable, "Misaligned array in LLS Fit", "PolynomialVector::ConstrainedPolynomialLeastSquaresFit(...)")); 
  int nCoeffsNew = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);
  MMatrix<double> A(valueDim, nCoeffsNew, 0.);

  std::vector<double>    wt    (nPoints,   1);
  if(weights.size() > 0) wt = weights; 

  PolynomialVector  newPolyVector1D(pointDim, MMatrix<double>(1, nCoeffsNew)); //guaranteed to be of right order
  std::vector< std::vector<double> > tempFx(nPoints, std::vector<double>(nCoeffsNew) );
  for(int i=0; i<nPoints; i++)
    newPolyVector1D.MakePolyVector(&points[i][0], &tempFx[i][0]);

  for(int dim=0; dim<valueDim ; dim++)
  {
    std::vector<PolynomialVector::PolynomialCoefficient> oldCoeffs1D;
    for(unsigned int i=0; i<coeffs.size(); i++)
      if(coeffs[i].OutVariable() == dim && int(coeffs[i].InVariables().size()) < polynomialOrder) 
        oldCoeffs1D.push_back(coeffs[i]);
    PolynomialVector oldPolyVector1D(oldCoeffs1D);
    int              nCoeffsOld = oldCoeffs1D.size();
    int              deltaCoeff = nCoeffsNew - nCoeffsOld;
    if(deltaCoeff <= 0) break;


    std::vector<int> needsCalculation; //index of variables that need calculation
    for(int i=0; i<nCoeffsNew; i++) 
    {
      bool exists = true;
      for(unsigned int j=0; j<oldCoeffs1D.size(); j++)
        if(IndexByVector(i, pointDim) == oldCoeffs1D[j].InVariables()) exists = false;
      if(exists) needsCalculation.push_back(i);
    }

    MVector<double> Fy(deltaCoeff, 0);
    MMatrix<double> F2(deltaCoeff, deltaCoeff,  0.);

    for(int i=0; i<nPoints && needsCalculation.size()>0; i++) //optimisation note - this algorithm spends all its time in this loop
    {
      std::vector<double> oldValue(valueDim);
      oldPolyVector1D.F(&points[i][0], &oldValue[0]);
      for(int k=0; k<deltaCoeff;  k++)
      {
        Fy(k+1) += (values[i][dim] - oldValue[dim])*tempFx[i][needsCalculation[k]]*wt[i]; //ynew - yold
        for(int j=0; j<deltaCoeff;  j++) 
          F2(j+1,k+1) += tempFx[i][needsCalculation[k]]*tempFx[i][needsCalculation[j]]*wt[i];
      }
    }
    try{ F2.invert(); }
    catch(Squeal squee) { 
      throw(Squeal(Squeal::recoverable, "Could not find least squares fit for data", "PolynomialVector::ConstrainedPolynomialLeastSquaresFit")); }
    MVector<double> AVec = F2 * Fy;
    for(int i=0; i<deltaCoeff; i++) A(dim+1, needsCalculation[i]+1) = AVec(i+1);
  }

  PolynomialVector tempPoly(coeffs);
  for(unsigned int i=0; i<coeffs.size(); i++)
    for(unsigned int j=0; j<tempPoly._polyKeyByVector.size(); j++)
      if(tempPoly._polyKeyByVector[j] == coeffs[i].InVariables())
      {
        A(coeffs[i].OutVariable()+1,j+1) = coeffs[i].Coefficient();
      }

  return new PolynomialVector(pointDim, A);
}


PolynomialVector* PolynomialVector::Chi2ConstrainedLeastSquaresFit
                                    (const std::vector< std::vector<double> >&  xin, const std::vector< std::vector<double> >& xout, 
                                      int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                                      double chi2Start, double discardStep, double* chi2End, double chi2Limit, 
                                      std::vector<double> weights, bool firstIsMean)
{
  int    nParticles   = xin.size();
  int    dimensionOut = xout[0].size();
  if(int(weights.size()) != nParticles) weights = std::vector<double>(xin.size(), 1.);
  std::vector<double> weights_in = std::vector<double>(weights);
  std::vector<double> amps(nParticles);
  MVector<double> means(dimensionOut,0);
  if(!firstIsMean)
     means = Means      (xout, weights);
  else for(int i=0; i<dimensionOut; i++) means(i+1) = xout[0][i];

  MMatrix<double> covInv = Covariances(xout, weights, means);
  try{ covInv.invert(); }
  catch(Squeal squee) {throw(Squeal(Squeal::recoverable, "Failed to find least squares fit for data", "PolynomialVector::Chi2ConstrainedLeastSquaresFit"));}

  double totalWeight = 0.;
  double discard       = chi2Start;
  double chi2          = chi2Limit*2.;
  int    nCoefficients = PolynomialVector::NumberOfPolynomialCoefficients(xin[0].size(), polynomialOrder)*xout[0].size();
  int    nGood         = nParticles;

  if(discard <= 0.) for(int i=0; i<nParticles; i++) //if chi2Start ill-defined, just use maximum chi2 in sample
  {
    MVector<double> xoutVec    (dimensionOut, 0);
    for(int j=0; j<dimensionOut; j++)
      xoutVec(j+1) = xout[i][j] - means(j+1);
    amps[i]      = (xoutVec.T() * covInv * xoutVec)(1);
    if(amps[i] > discard) discard = amps[i];
    totalWeight += weights[i];
  }
  discard /= discardStep; //Set discard to the largest amplitude in the data

  PolynomialVector* pvec = new PolynomialVector(std::vector<PolynomialVector::PolynomialCoefficient>() );
  while( nGood >= nCoefficients && chi2 > chi2Limit)
  {
    chi2               = 0.;
    nGood              = nParticles;
    if(pvec != NULL) delete pvec;

    //optimisation note - algorithm spends all its time doing the CPLS Fit
    try                 { pvec = PolynomialVector::ConstrainedPolynomialLeastSquaresFit(xin, xout, polynomialOrder, coeffs, weights); }
    catch(Squeal squee) { pvec = NULL; chi2 = chi2Limit*2.;}
    for(int i=0; i<nParticles && pvec!=NULL; i++)
    {
      if(fabs(weights[i]) > 1.e-9) 
      {
        std::vector<double> pout(dimensionOut, 0.);
        pvec->F(&xin[i][0], &pout[0]);
        MVector<double> residualVec(dimensionOut,0);
        for(int j=0; j<dimensionOut; j++) residualVec(j+1) = pout[j] - xout[i][j];
        chi2             += weights[i]*weights[i]/totalWeight/totalWeight * (residualVec.T() * covInv * residualVec)(1); //watch weights handling here
      }
    }
    for(int i=0; i<nParticles; i++)
      if(amps[i] > discard) {totalWeight -= weights[i]; weights[i] = 0.; nGood--;}
    discard /= discardStep;
    std::cout << "ConstrainedPolynomialLeastSquaresFit - chi2: " << chi2 << " cut: " << discard << " cut_step: " << discardStep << " weight: " << totalWeight
              << " pvec: " << pvec << std::endl;
  }
  if(chi2 > chi2Limit || pvec == NULL)
  {
    std::stringstream err;
    err << "PolynomialVector::Chi2ConstrainedLeastSquaresFit failed to converge. Best fit had <chi2>="
                                << chi2 << " compared to limit " << chi2Limit << std::endl;
    throw(Squeal(Squeal::recoverable, err.str(), "PolynomialVector::Chi2ConstrainedLeastSquaresFit(...)"));
  }
  if(chi2End != NULL) *chi2End = discard; //save discard at end for future use
  return pvec;
}

PolynomialVector* PolynomialVector::Chi2SweepingLeastSquaresFit
                          (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                           double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps)
{
  //build particles in shell
  //try to do chi2 fit
  //if chi2 fit works, increase shell size
  //else increase polynomial order, revise fit
  PolynomialVector*   pvec1 = NULL;
  PolynomialVector*   pvec2 = NULL;
  int    step = 0; 
  for(int i_order=1; i_order<=polynomialOrder; i_order++)
  {
    std::vector<std::vector<double> > in;
    std::vector<std::vector<double> > out;
    double chi2 = -1;
    while(chi2 < chi2Max && step < maxNumberOfSteps) {
      in.push_back(std::vector<double>(delta.size(), 0.));
      step++;
      std::vector<std::vector<double> > in_mod = PointBox(delta, i_order+1);
      for(size_t i=0; i<in_mod.size(); i++) in.push_back(in_mod[i]);
      vec.FAppend(in, out);
      if(pvec2 != NULL && pvec1 != NULL) delete  pvec2;
      if(pvec1 != NULL) pvec2 = pvec1;
      pvec1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
      chi2  = pvec1->GetAvgChi2OfDifference(in, out);
      if(chi2 < chi2Max)
        for(size_t i=0; i<delta.size(); i++) delta[i] *= deltaFactor;
    }
    delete pvec1;
    pvec1 = NULL;
  }
  for(size_t i=0; i<delta.size(); i++) delta[i] /= deltaFactor;
  return pvec2;
}

PolynomialVector* PolynomialVector::Chi2SweepingLeastSquaresFitVariableWalls
                      (const VectorMap& vec, int polynomialOrder, std::vector< PolynomialVector::PolynomialCoefficient > coeffs, 
                       double chi2Max, std::vector<double>& delta, double deltaFactor, int maxNumberOfSteps, std::vector<double> max_delta)
{
  PolynomialVector*   pvec1 = Chi2SweepingLeastSquaresFit(vec, polynomialOrder, coeffs, chi2Max, delta, deltaFactor, maxNumberOfSteps);
  if(pvec1==NULL) return NULL;
  PolynomialVector*   pvec2 = NULL;
  int    step = 0; 
  for(int i_order=pvec1->PolynomialOrder(); i_order<=polynomialOrder; i_order++)
  {
    for(size_t i=0; i<delta.size(); i++) {
      double chi2 = -1;
      while(chi2 < chi2Max && step < maxNumberOfSteps && delta[i]*deltaFactor<max_delta[i]) {
        if(chi2 < chi2Max)
          delta[i] *= deltaFactor;
        step++;
        std::vector<std::vector<double> > in = PointBox(delta, i_order+1);
        in.push_back(std::vector<double>(delta.size(), 0.));
        std::vector<std::vector<double> > out;
        vec.FAppend(in, out);
        if(pvec2 != NULL && pvec1 != NULL) delete  pvec2;
        if(pvec1 != NULL) pvec2 = pvec1;
        pvec1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
        chi2  = pvec1->GetAvgChi2OfDifference(in, out);
        if(chi2 > chi2Max)
          delta[i] /= deltaFactor;
      }
    }
    delete pvec1;
    pvec1 = NULL;
  }
  return pvec2;
}


void PolynomialVector::PolynomialCoefficient::SpaceTransform(std::vector<int> space_in, std::vector<int> space_out)
{
  std::map<int, int> mapping; //probably optimise this
  for(unsigned int i=0; i<space_out.size(); i++)
    for(unsigned int j=0; j<space_in.size(); j++)
      if(space_out[i] == space_in[j]) mapping[j] = i; //mapping[space_in_index] returns space_out_index

  std::vector<int> in_variables(_inVarByVec.size());
  for(unsigned int con=0; con<in_variables.size(); con++)
  {
    if(mapping.find(in_variables[con]) != mapping.end()) in_variables[con] = mapping[in_variables[con]];
    else                                                throw(Squeal(Squeal::recoverable, "Input variable not found in space transform", 
                                                                             "PolynomialVector::PolynomialCoefficient::SpaceTransform"));
  }

  if(mapping.find(_outVar) != mapping.end()) _outVar = mapping[_outVar];
  else                                      throw(Squeal(Squeal::recoverable, "Output variable not found in space transform", 
                                                                  "PolynomialVector::PolynomialCoefficient::SpaceTransform"));

  _inVarByVec = in_variables;
}

//Return chi2 of some set of output variables compared with the polynomial vector of some set of input variables 
double PolynomialVector::GetAvgChi2OfDifference(std::vector< std::vector<double> > in, std::vector< std::vector<double> > out)
{
  std::vector< std::vector<double> > out_p;
  if(in.size() < 1)           throw(Squeal(Squeal::recoverable, "No data in input",                      "PolynomialVector::GetAvgChi2OfDifference(...)"));
  if(in.size() != out.size()) throw(Squeal(Squeal::recoverable, "Input data and output data misaligned for calculation of chi2 difference", "PolynomialVector::GetAvgChi2OfDifference(...)"));
  for(size_t i=0; i<in.size(); i++) 
    if(in[i].size() != PointDimension() || out[i].size() != ValueDimension())
      throw(Squeal(Squeal::recoverable, "Bad input data for calculation of chi2 difference",  "PolynomialVector::GetAvgChi2OfDifference(...)"));
  this->FAppend(in, out_p);

  std::vector< std::vector<double> > diff(in.size());
  for(size_t i=0; i<out_p.size(); i++)
  {
    diff[i] = std::vector<double>( ValueDimension() );
    for(size_t j=0; j<ValueDimension(); j++)
      diff[i][j] = out[i][j]-out_p[i][j];
  }
  
  double chi2 = 0;
  MMatrix<double> cov_inv = Covariances(diff, std::vector<double>(diff.size(), 1.), MVector<double>(diff.size(), 0.));

  for(size_t i=0; i<diff.size(); i++)
  {
    MVector<double> diff_mv(&(diff[i][0]), &diff[i][0]+diff[i].size());
    chi2 += (diff_mv.T()*cov_inv*diff_mv)(1);
  }
  chi2 /= double(diff.size());
  return chi2;
}

//Make a shell of points on the outside of a hypercube with dimension same as delta length, corners
//of hypercube are at delta[0], delta[1], ... delta[n] and -delta[0], ..., -delta[n] 
//number of points is at least 2*NumberOfPolynomialCoefficients(i_order, delta.size())-1
//so that it will always define a polynomial of order i_order (one point +ve, one -ve hence factor 2)
//Require evenly spaced points, sometimes this means I make more points on the shell
static const double PI = atan(1)*4.;
std::vector< std::vector<double> > PolynomialVector::PointBox(std::vector<double> delta, int i_order)
{
  int min_size         = 3*NumberOfPolynomialCoefficients(i_order, delta.size());
  int n_points_per_dim = 2;
  int dim              = delta.size();
  while(pow(n_points_per_dim, dim)-pow(n_points_per_dim-2, dim) <= min_size) n_points_per_dim++;
  int max_point        = n_points_per_dim/2;
  if( 2*(n_points_per_dim/2) != n_points_per_dim ) //odd n_points
    max_point = (n_points_per_dim-1)/2;

  std::vector< std::vector<double> > pos;
  std::vector<int>    grid_size(dim, n_points_per_dim);
  std::vector<double> grid_min (delta);
  std::vector<double> grid_spacing(delta);
  for(size_t i=0; i<grid_spacing.size(); i++) grid_spacing[i] *= 2./double(n_points_per_dim-1);
  for(size_t i=0; i<grid_min    .size(); i++) grid_min    [i] *= -1.;
  NDGrid grid(dim, &grid_size[0], &grid_spacing[0], &grid_min[0]);
  for(Mesh::Iterator it=grid.Begin(); it<grid.End(); it++) {
    std::vector<int> place = it.State();
    for(size_t i=0; i<place.size(); i++) if(place[i] == 1 || place[i] == n_points_per_dim) { //I am on the outside of the grid
      pos.push_back(it.Position());
      break; //end inner i loop to detect if it is on outside of grid
    }
  }
  return pos;
}

//Algorithm - take the PointBox output and scale so that length is 1 in scale_matrix coordinate system
std::vector< std::vector<double> > PolynomialVector::PointShell(MMatrix<double> scale_matrix, int i_order) {
  size_t          point_dim = scale_matrix.num_row();
  MMatrix<double> scale_inv = scale_matrix.inverse();
  std::vector<std::vector<double> > point_box = PointBox(std::vector<double>(point_dim, 1.), i_order);
  for(size_t i=0; i<point_box.size(); i++) {
    MVector<double> point(&point_box[i][0], &point_box[i][0]+point_dim);
    double scale  = (point.T()*scale_inv*point)(1);
    point        /= pow(scale, double(point_dim));
    for(size_t j=0; j<point_dim; j++) point_box[i][j]=point(j+1); 
  }
  return point_box;
}
////////// POLYNOMIALVECTOR END ////////////





#endif




