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

// Author: Peter Lane

#ifndef SRC_COMMON_CPP_OPTICS_LEAST_SQUARES_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_LEAST_SQUARES_MODEL_HH

#include <vector>

namespace MAUS {

class TransferMap;

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

class LeastSquaresOpticsModel : public OpticsModel {
 public:
  // *************************
  //  Constructors
  // *************************

  LeastSquaresOpticsModel()
      : algorithm_(&Algorithm.kUnconstrainedPolynomial), transfer_map_(NULL) { }

  LeastSquaresOpticsModel(Algorithm& algorithm)
      : algorithm_(&algorithm), transfer_map_(NULL) { }

  /** @brief no memory allocated so doesn't do anything.
   */
  ~LeastSquaresOpticsModel() { }
  
  virtual Build(BTField * electromagnetic_field,
                std::vector<Material> * materials) {
    /* TODO:
     *  1) Generate a multivariate Gaussian distribution of particles
     *  2) 
    int algorithm_id = algorithm_->id();
    switch {
      case 5: {  // sweeping chi squared with variable walls
        // Chi2SweepingLeastSquaresFitVariableWalls(...);
        break;
      }
      case 4: {  // sweeping chi squared
        // Chi2SweepingLeastSquaresFit(...);
        break;
      }
      case 3: {  // constrained chi squared
        // Chi2ConstrainedLeastSquaresFit(...);
        break;
      }
      case 2: {  // constrained polynomial
        // ConstrainedPolynomialLeastSquaresFit(...);
        break;
      }
      case 1:    // unconstrained polynomial
      default: {
        // PolynomialLeastSquaresFit(...);
      }
    }
  }
  
  TransferMap const * const transfer_map() {
    return transfer_map_;
  }
  
 private:
  Algorithm * algorithm_;
  TransferMap * transfer_map_;

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
  static LeastSquaresOpticsModel* PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                                        polynomialOrder,
    const VectorMap*                          weightFunction);

  static LeastSquaresOpticsModel* PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                                        polynomialOrder,
    const std::vector<double>& weights = std::vector<double>());

  /** Find a polynomial least squares fit given that I know certain coefficients
   *  already (stored in coeffs). For example, say I know the polynomial to 
   *  2nd order, I want to extend to 3rd order using a least squares fit.
   */
  static LeastSquaresOpticsModel* ConstrainedPolynomialLeastSquaresFit
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
  static LeastSquaresOpticsModel* Chi2ConstrainedLeastSquaresFit
      (const std::vector< std::vector<double> >&  xin,
       const std::vector< std::vector<double> >& xout,
       unsigned int polynomialOrder,
       std::vector< LeastSquaresOpticsModel::PolynomialCoefficient > coeffs,
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
  static LeastSquaresOpticsModel* Chi2SweepingLeastSquaresFit
      (const VectorMap& vec, unsigned int polynomialOrder,
       std::vector< LeastSquaresOpticsModel::PolynomialCoefficient > coeffs,
       double chi2Max, std::vector<double>& delta, double deltaFactor,
       int maxNumberOfSteps);

  /** Sweep out from centre of LLS trying to keep chi2 small
   *  Here I let the validity region expand independently in each direction
   *  (which is slower as I need to test many more cases) First do
   *  Chi2SweepingLeastSquaresFit; then expand the validity region sequentially
   *  in each direction. Idea is to make a good fit even in the case that delta
   *  doesn't scale appropriately between different directions
   */
  static LeastSquaresOpticsModel* Chi2SweepingLeastSquaresFitVariableWalls
      (const VectorMap& vec, unsigned int polynomialOrder,
       std::vector< LeastSquaresOpticsModel::PolynomialCoefficient > coeffs,
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
  static std::vector< std::vector<double> > PointShell(
    Matrix<double> scale_matrix, int i_order);

  /** Return chi2 of residuals of some set of output variables compared with
   *  the polynomial vector of some set of input variables Here Chi2 Avg is
   *  defined as
   *  \f$sum( \vec{v}^T \mathbf{M}^{-1} \vec{v})\f$ where v is the vector of
   *  residuals and M is covariance matrix of v
   */
  double GetAvgChi2OfDifference(std::vector< std::vector<double> > in,
                                std::vector< std::vector<double> > out);

  /** @brief An algorithm identification class.
   * Use LeastSquaresOpticsModel.Algorithm.kWhateverAlgorithm in the constructor
   * and the constructor calls Algorithm.id() to identify the algorithm.
   */
  class Algorithm {
   public:
    Algorithm(int id) : id_(id) {}
    const int id() const {
      return id_;
    }

    static const Algorithm kUnconstrainedPolynomial;
    static const Algorithm kConstrainedPolynomial;
    static const Algorithm kConstrainedChiSquared;
    static const Algorithm kSweepingChiSquared;
    static const Algorithm kSweepingChiSquaredWithVariableWalls;
   private:
    Algorithm() : id_(-1) {}
    int id_;
  };
  const Algorithm kUnconstrainedPolynomial              = Algorithm(1);
  const Algorithm kConstrainedPolynomial                = Algorithm(2);
  const Algorithm kConstrainedChiSquared                = Algorithm(3);
  const Algorithm kSweepingChiSquared                   = Algorithm(4);
  const Algorithm kSweepingChiSquaredWithVariableWalls  = Algorithm(5);
};


} // namespace MAUS

#endif




