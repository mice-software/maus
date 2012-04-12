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

#ifndef SRC_COMMON_CPP_MATHS_POLYNOMIAL_MAP_HH
#define SRC_COMMON_CPP_MATHS_POLYNOMIAL_MAP_HH/

#include <map>
#include <vector>

#include "Maths/Vector.hh"

namespace MAUS {

/* @class PolynomialVector An arbitrary order polynomial vector class.
 *
 *  Maps a vector \f$\vec{x}\f$ onto a vector \f$\vec{y}\f$ with
 *  \f$\vec{y} = a_0 + sum(a_{j_1j_2...j_n} x_1^{j_1} x_2^{j_2}...x_n^{j_n})\f$.
 *
 *  Also includes algorithms to map a single index J into \f$j_1...j_n\f$.\n
 *  \n
 *  IndexByPower gives an index like:\n
 *  \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
 *  While IndexByVector gives an index like:\n
 *  \f$ w_i = x_{i_1}x_{i_2} \ldots x_{i_n} \f$ \n
 *  \n
 */

class PolynomialVector : public Vector<double> {
 public:
  // *************************
  //  Constructors
  // *************************

  /** @brief  Make a vector like \f$(c, x_1, x_1^2, x_1 x_2, x_2^2, ...)\f$ and
   *          fill each x_i with zero.
   *  @param[in] dimension The size of the point array (# of variables).
   *  @param[in] order The order of the polynomial.
   *
   *  The size of the PolynomialVector with point dimension n and polynomial
   *  order p will be the sum of C(i+n-1,n-1), where i goes from 0 to p and
   *  C(m, k) is the binomial coefficient function.
   */
  PolynomialVector(const size_t dimension, const size_t order);

  /** @brief  Make a vector like \f$(c, x_1, x_1^2, x_1 x_2, x_2^2, ...)\f$ and
   *          fill each x_i with point[i].
   *  @param[in] point The values of the multiplicands in the polynomial term
   *                   products (i.e. x_1, x_2, ..., x_n).
   *  @param[in] order The order of the polynomial.
   *
   *  The size of the PolynomialVector with point dimension n and polynomial
   *  order p will be the sum of C(i+n-1,n-1), where i goes from 0 to p and
   *  C(m, k) is the binomial coefficient function.
   */
  PolynomialVector(const Vector<double>& point, const size_t order);

  /** @brief  Make a vector like \f$(c, x_1, x_1^2, x_1 x_2, x_2^2, ...)\f$ and
   *          fill each x_i with point[i].
   *  @param[in] point The values of the multiplicands in the polynomial term
   *                   products (i.e. x_1, x_2, ..., x_n).
   *  @param[in] dimension The size of the point array (# of variables).
   *  @param[in] order The order of the polynomial.
   *
   *  The size of the PolynomialVector with point dimension n and polynomial
   *  order p will be the sum of C(i+n-1,n-1), where i goes from 0 to p and
   *  C(m, k) is the binomial coefficient function.
   */
  PolynomialVector(double const * const point,
                   const size_t dimension,
                   const size_t order);

  // *************************
  //  Fill Functions
  // *************************

  /** @brief  Fill each x_i with point[i].
   *  @param[in] point The values of the multiplicands in the polynomial term
   *                   products (i.e. x_1, x_2, ..., x_n). The vector must be of
   *                   size Pointdimension().
   */
  Fill(const Vector<double>& point);

  /** @brief  Fill each x_i with point[i].
   *  @param[in] point The values of the multiplicands in the polynomial term
   *                   products (i.e. x_1, x_2, ..., x_n). The array must be of
   *                   size Pointdimension().
   */
  Fill(double const * const point);

  // *************************
  //  Accessors
  // *************************

  /** @brief  Length of the input point vector.
   */
  unsigned int dimension() const;

  /** @brief Index of highest power - 0 is const, 1 is linear, 2 is quadratic...
   */
  unsigned int order() const;

  /** Transforms from a 1d index of polynomial coefficients to an nd index.
   *  This is slow - you should use it to build a lookup table.
   *  For polynomial term \f$x_1^i x_2^j ... x_d^n\f$ index like [i][j] ... [n]
   *  e.g. \f$x_1^4 x_2^3 = x_1^4 x_2^3 x_3^0 x_4^0\f$ = {4,3,0,0}.
   */
  static std::vector<int> IndexByPower(int index, int nInputVariables);

  /** Transforms from a 1d index of polynomial coefficients to an nd index.
   *  This is slow - you should use it to build a lookup table.
   *  For polynomial term \f$x_i x_j...x_n\f$ index like [i][j] ... [n]
   *  e.g. \f$x_1^4 x_2^3\f$ = {1,1,1,1,2,2,2}
   */
  static std::vector<int> IndexByVector(int index, int nInputVariables);
  /** Returns the number of coefficients required for an arbitrary dimension,
   *  order polynomial e.g.
   * \f$a_0 + a_1 x + a_2 y + a_3 z + a_4 x^2 + a_5 xy + a_6 y^2 + a_7 xz + a_8 yz + a_9 z^2\f$
   *  => NumberOfPolynomialCoefficients(3,2) = 9 (indexing starts from 0).
   */

  /** Write out the PolynomialVector (effectively just polyCoeffs).
   */
  friend std::ostream& operator<<(std::ostream&,  const PolynomialVector&);

 private:
  std::vector< std::vector<int> >    index_key_by_power_; // std::vector<int>[i_1] = j_1
  std::vector< std::vector<int> >    index_key_by_vector_; // std::vector<int>[j_1] = i_1
};

/* @class PolynomialVectorMap Implements the mapping from a point in n-space
 *                            to a point in the polynomial term space.
 */
class PolynomialVectorMap : public VectorMap {
 public:
  /* @brief Create a map from a vector \f$x = (x_1, x_2, ..., x_n)\f$ to a
   * vector \f$y = (c, x_1, x_1^2, x_1 x_2, x_2^2, ...)\f$.
   *  @param[in] dimension The size of the point array (# of variables).
   *  @param[in] order The order of any polynomial for which the mapped-to
   *             vector represents the variable products in it's terms.
   *
   *  The value dimension with point dimension n and polynomial order p will be
   *  the sum of C(i+n-1,n-1), where i goes from 0 to p and C(m, k) is the
   *  binomial coefficient function.
   */
  PolynomialVectorMap(const size_t dimension, const size_t order);

	unsigned int PointDimension() const;
	unsigned int ValueDimension() const;

  /** @brief Fill value with \f$ y_i \f$ at some set of \f$ x_i \f$ (point).
   *  @param[in] point an array of length PointDimension().
   *  @param[in] value an array of length ValueDimension().
   */
  void F(double const * point, double * value) const;

  /** @brief  Fill value with \f$ y_i \f$ at some set of \f$ x_i \f$ (point).
   *  @param[in] point a vector of length PointDimension().
   *  @param[in] value a vector of length ValueDimension().
   *
   *  Note that there is no bounds checking here.
   */
  void F(const Vector<double>& point, Vector<double>& value) const;

  /** @brief  Polymorphic copy constructor. This is a special copy constructor
   *          for inheritance structures, so that I can call vectorMap->Clone()
   *          and it will create a vectorMap of the appropriate child type
   *          without the caller needing to know what type vectorMap actually is.
   */
  PolynomialVector * Clone() const;
 private:
  const size_t point_dimension_;
  const size_t value_dimension_;
  const size_t polynomial_order_;
};

std::ostream& operator<<(std::ostream&, const PolynomialVector&);

} // namespace MAUS

#endif




