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

#ifndef SRC_COMMON_CPP_MATHS_POLYNOMIAL_VECTOR_HH
#define SRC_COMMON_CPP_MATHS_POLYNOMIAL_VECTOR_HH

#include <map>
#include <vector>

#include "Interface/Interpolator.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

namespace MAUS {

/** @brief Calculates TransferMaps using any of several least squares fit
 *  functions.
 *  
 *  Here we find a polynomial of arbitrary dimension and order from a set of
 *  points by the method of least squares. The particular version of least
 *  squares used is specified in the constructor.
 *
 *  Note that the problem must be overspecified, so the number of points must be
 *  >= number of polynomial coefficients.
 *  A few interesting variations on this theme... have a look!\n
 * \n
 */

class LeastSquaresTransferMapCalculator {
 public:
  // *************************
  //  Constructors
  // *************************

  LeastSquaresTransferMapCalculator() { }

  /** @brief no memory allocated so doesn't do anything.
   */
  ~LeastSquaresTransferMapCalculator() { }

  /** @brief  Finds the polynomial pol that minimises
   *          sum_i ( w_i (\vec{y_i} - Pol(\vec{x_i}))^2.
   *          y_i = values[i],
   *          x_i = points[i],
   *          w_i = weightFunction(x_i) OR weights[i],
   *          polynomialOrder is the order of pol
   *  @param[in] points input variables of which the polynomials are a function
   *  @param[in] values output variables of which the polynomials are an
   *             expansion
   *  @param[in] polynomialOrder the maximum order of the polynomials
   *  @param[in] weightFunction weight as a function of point index
   * Nb if w_i not defined, I assume that w_i = 1 i.e. unweighted points and
   * values should be the same length; points[i] should all be the same length;
   * values[i] should all be the same length weight function should be a vector
   * map that has PointDimension of points and returns a weight of dimension 1
   */
  static LeastSquaresTransferMapCalculator* PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                                        polynomialOrder,
    const VectorMap*                          weightFunction);

  static LeastSquaresTransferMapCalculator* PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                                        polynomialOrder,
    const std::vector<double>& weights = std::vector<double>());

  /** Find a polynomial least squares fit given that I know certain coefficients
   *  already (stored in coeffs). For example, say I know the polynomial to 
   *  2nd order, I want to extend to 3rd order using a least squares fit.
   */
  static LeastSquaresTransferMapCalculator* ConstrainedPolynomialLeastSquaresFit
      (const std::vector< std::vector<double> >&  points,
       const std::vector< std::vector<double> >& values,
       unsigned int polynomialOrder,
       std::vector< PolynomialCoefficient > coeffs,
       const std::vector<double>& weights = std::vector<double>());

  /** Find a polynomial least squares fit given that I know certain coefficients
   *  already. After finding the fit, calculate "chi2" for each particle.
   *  Compare x_out with polynomial fit of x_in, x_pol, using
   *  \f$\delta_{\chi^2} = (\vec{x_{out}} - \vec{x_{pol}}).T() \mathbf{V}^{-1} (\vec{x_{out}} - \vec{x_{pol}}).\f$
   *  If
   *  \f$\sum(\delta_{\chi^2})/totalWeight > \chi^2_{limit}, \f$
   *  calculate
   *  \f$\chi^2_{out} = \vec(x_out).T() V^{-1} \vec(x_out)\f$
   *  and discard particles with
   *  \f$\chi^2_{out} > chi^2_{start}*(discardStep^n)\f$
   *  repeat until
   *  \f$\sum(\delta_{\chi^2})/totalWeight <= chi^2_{limit},\f$
   *  each time incrementing n by 1
   *  if \f$\chi^2_{start} < 0,\f$ start with maximum chi^2; if
   *  chi2End != NULL, put the final value in chi2End
   */
  static LeastSquaresTransferMapCalculator* Chi2ConstrainedLeastSquaresFit
      (const std::vector< std::vector<double> >&  xin,
       const std::vector< std::vector<double> >& xout,
       unsigned int polynomialOrder,
       std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient > coeffs,
       double chi2Start, double discardStep, double* chi2End, double chi2Limit,
       std::vector<double> weights, bool firstIsMean = false);

  /** Find a polynomial fit to an arbitrary function vec.
   *  Here the LLS can choose what points it wants to study
   *  Sweep out from centre of LLS trying to keep chi2 small
   *  As chi2 goes out of tolerance, try adding a new order polynomial
   *  Guarantees that I will return a Polynomial that fits within chi2 max and
   *  sometimes this means the polynomial order will be less than chi2 max. If
   *  I can't find any fit at all, will return NULL. On return, delta holds the
   *  value of delta at the limit of validity
   */
  static LeastSquaresTransferMapCalculator* Chi2SweepingLeastSquaresFit
      (const VectorMap& vec, unsigned int polynomialOrder,
       std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient > coeffs,
       double chi2Max, std::vector<double>& delta, double deltaFactor,
       int maxNumberOfSteps);

  /** Sweep out from centre of LLS trying to keep chi2 small
   *  Here I let the validity region expand independently in each direction
   *  (which is slower as I need to test many more cases) First do
   *  Chi2SweepingLeastSquaresFit; then expand the validity region sequentially
   *  in each direction. Idea is to make a good fit even in the case that delta
   *  doesn't scale appropriately between different directions
   */
  static LeastSquaresTransferMapCalculator* Chi2SweepingLeastSquaresFitVariableWalls
      (const VectorMap& vec, unsigned int polynomialOrder,
       std::vector< LeastSquaresTransferMapCalculator::PolynomialCoefficient > coeffs,
       double chi2Max, std::vector<double>& delta, double deltaFactor,
       int maxNumberOfSteps, std::vector<double> max_delta);

  /**  @brief  Return the mean of some set of values, using some set of weights
   */
  static Vector<double> Means(std::vector<std::vector<double> > values,
                              std::vector<double> weights);

  /** @brief  Return the covariance matrix of some set of values, using some set
   *          of weights and some mean.
   * If length of weights is not equal to length of values use weights = 1.
   * If length of mean is not equal to length of first value recalculate mean
   */
  static SymmetricMatrix Covariances(
      const std::vector<std::vector<double> >&  values,
      const std::vector<double>&                weights,
      const Vector<double>&                      mean);

  /** Make a hypercube of points with half width of delta in each dimension so
   *  that the number of points is at least
   *  multiplier*NumberOfPolynomialCoefficients(i_order, delta.size())
   *  - so that if multiplier >= 1 it will always define a polynomial of order
   *  i_order. Require evenly spaced ito position, sometimes this means I make
   *  more points on the shell
   */
  static std::vector< std::vector<double> > PointBox(std::vector<double> delta,
                                                     int i_order);

  /** Make a hyperellipsoid of points where magnitude of each vector
   *  \f$\vec{u}\f$
   *  is chosen such that
   *  \f$\vec{u}^T \mathbf{D}^{-1} \vec{u}=1\f$
   *  and number of points is at least
   *  multiplier*NumberOfPolynomialCoefficients(i_order, delta.num_row())
   *  - so that if multiplier >= 1
   *  it will always define a polynomial of order i_order. delta should be a
   *  symmetric square matrix. Note that this is just a correctly scaled version
   *  of PointBox, which defines the distribution of points on the shell.
   */
  static std::vector< std::vector<double> >
                           PointShell(Matrix<double> scale_matrix, int i_order);

  /** Return chi2 of residuals of some set of output variables compared with
   *  the polynomial vector of some set of input variables Here Chi2 Avg is
   *  defined as
   *  \f$sum( \vec{v}^T \mathbf{M}^{-1} \vec{v})\f$ where v is the vector of
   *  residuals and M is covariance matrix of v
   */
  double GetAvgChi2OfDifference(std::vector< std::vector<double> > in,
                                std::vector< std::vector<double> > out);

  /** Print a sequence in a column with some predefined width.
   *  @param out output stream to which sequence is printed
   *  @param container sequence to be printed. Container class must have
   *         begin() and end() methods that return a Container::const_iterator
   *         that reference the beginning and end of the container. The
   *         const_iterator methods must dereference to an object with the
   *         <<(std::ostream&) operator defined. The const_iterator must have
   *         a suffix increment (foo++) operator defined.
   *  @param T_separator separator that separates contained elements
   *  @param str_separator separator that pads the overall output
   *  @param length to which the output should be padded. Output exceeding this
   *         length will not be truncated
   *  @param pad_at_start set to true to pad at the start (i.e. right align);
   *         set to false to pad at the end (i.e. left align)
   *  Should probably go in a "utility function" namespace, that does not
   *  currently exist.
   */
  template < class Container >
  static void PrintContainer(std::ostream& out, const Container& container,
                             char T_separator, char str_separator,
                             size_t length, bool pad_at_start);

  /** Return true if sequences a and b are identical;
   *  a must have an iterator object
   *  dereferenced by *it must have increment prefix operator defined and with
   *  != operator defined by the object pointed to by it
   */
  template <class TEMP_CLASS, class TEMP_ITER>
  static bool IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b,
        TEMP_ITER a_begin, TEMP_ITER a_end, TEMP_ITER b_begin, TEMP_ITER b_end);

  // PolynomialCoefficient sub-class - useful, could be extended if needed
  // (e.g. become an iterator etc)

  /** Polynomial coefficient sub class represents a coefficient in a
   *  LeastSquaresTransferMapCalculator object. Each coefficient has an input variable that it
   *  represents (i.e. powers on x), an output variable that it yields (i.e.
   *  the y value) and a value.
   */
  class PolynomialCoefficient {
    public:
      /** Constructor
       *  @param inVariablesByVector is x power indexed like e.g.
       *         \f$x_1^4 x_2^3 =\f$ {1,1,1,1,2,2,2}
       *  @param outVariable y variable
       *  @param coefficient value
       *  e.g. y_2 = x_1^2 + 2 x_1 x_2 would have two coefficients, both with
       *  outVariable 2; but with inVariable {1,1} and coefficient 1.0; or with
       *  inVariable {1, 2} and coefficient 2.0
       */
      PolynomialCoefficient(std::vector<int> inVariablesByVector,
                                            int outVariable, double coefficient)
          : _inVarByVec(inVariablesByVector), _outVar(outVariable),
            _coefficient(coefficient) {}
      /** @returns the vector of input variables
       */
      std::vector<int> InVariables() const {return _inVarByVec;}

      /** @returns the output variable
       */
      int OutVariable() const {return _outVar;}

      /** @returns the value of the coefficient
       */
      double Coefficient() const {return _coefficient;}

      /** Set the input variables
       */
      std::vector<int> InVariables(std::vector<int> inVar ) {
        _inVarByVec  = inVar;
        return _inVarByVec;
      }

      /** Set the output variable
       */
      int OutVariable(int  outVar) {
        _outVar      = outVar;
        return _outVar;
      }

      /** Set the coefficient
       */
      double Coefficient(double coeff) {
        _coefficient = coeff;
        return _coefficient;
      }

      /** transform coefficient from subspace space_in to subspace space_out,
       * both subspaces of some larger space if any of coeff variables is not
       * in space_out OR not in space_in, leave this coeff untouched and Squeal
       * so for coeff({1,2},0,1.1),
       * coeff.space_transform({0,2,3,5}, {4,7,1,2,3,0}) would return
       * coeff({3,4},5,1.1)
       * @param space_in vector indexing the vector of each dimension in the
       *        input subspace
       * @param space_out vector indexing the vector of each dimension in the
       *        output subspace
       */
      void             SpaceTransform(std::vector<int> space_in,
                                      std::vector<int> space_out);
      /** if var is in inVariables return true
       */
      bool             HasInVariable(int var) const {
        for (size_t i = 0; i < _inVarByVec.size(); i++)
          if (_inVarByVec[i] == var) return true;
          return false;
      }

    private:
      std::vector<int> _inVarByVec;
      int              _outVar;
      double           _coefficient;
  };


 private:
  int                                point_dimension_;
  std::vector< std::vector<int> >    index_key_by_power_; // std::vector<int>[i_1] = j_1
  std::vector< std::vector<int> >    index_key_by_vector_; // std::vector<int>[j_1] = i_1
  Matrix<double>                     coefficient_matrix_;
  std::vector<PolynomialCoefficient> polynomial_vector_;
  static bool                        print_headers_;
};

std::ostream& operator<<(std::ostream&, const LeastSquaresTransferMapCalculator&);


// template functions
template <class TEMP_CLASS, class TEMP_ITER>
bool LeastSquaresTransferMapCalculator::IterableEquality
                                      (const TEMP_CLASS& a, const TEMP_CLASS& b,
                                       TEMP_ITER a_begin, TEMP_ITER a_end,
                                       TEMP_ITER b_begin, TEMP_ITER b_end) {
  TEMP_ITER a_it = a_begin;
  TEMP_ITER b_it = b_begin;
  while (a_it != a_end && b_it != b_end) {
    if ( *a_it != *b_it ) return false;
    ++a_it;
    ++b_it;
  }
  if ( a_it != a_end || b_it != b_end ) return false;
  return true;
}

size_t LeastSquaresTransferMapCalculator::NumberOfPolynomialCoefficients() {
  return NumberOfPolynomialCoefficients(point_dimension_, PolynomialOrder());
}


} // namespace MAUS

#endif




