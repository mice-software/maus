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
#define SRC_COMMON_CPP_MATHS_POLYNOMIAL_MAP_HH

#include <map>
#include <vector>

#include "Maths/Vector.hh"

namespace MAUS {

/** PolynomialVector, an arbitrary order polynomial map class.
 *
 *  Maps a vector \f$\vec{x}\f$ onto a vector \f$\vec{y}\f$ with
 *  \f$\vec{y} = a_0 + sum(a_{j_1j_2...j_n} x_1^{j_1} x_2^{j_2}...x_n^{j_n})\f$.
 *  Also algorithms to map a single index J into \f$j_1...j_n\f$.\n
 *  \n
 *  IndexByPower gives an index like:\n
 *  \f$ w_i = x_1^{i_1} x_2^{i_2} ... x_n^{i_n} \f$ \n
 *  While IndexByVector gives an index like:\n
 *  \f$ w_i = x_{i_1}x_{i_2} \ldots x_{i_n} \f$ \n
 *  \n
 */

class PolynomialVector : public Vector<double> {
 public:
  // forward declaration of embedded class
  class PolynomialCoefficient;

  // *************************
  //  Constructors
  // *************************

  /** @brief  Make a vector like \f$(c, x, x^2, x^3...)\f$.
   *  @param[in] polyVector .
   *
   *  could be static but faster as member function (use lookup table for
   *  _polyKey).
   */
  PolynomialVector(const Vector<double>& point, size_t order) const;

  /** @brief  Make a vector like \f$(c, x, x^2, x^3...)\f$.
   *  @param[in] polyVector should be of size NumberOfPolynomialCoefficients().
   *
   *  Could be static but faster as member function (use lookup table for
   *  _polyKey).
   */
  PolynomialVector(const double* point, size_t order) const;

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

std::ostream& operator<<(std::ostream&, const PolynomialVector&);


} // namespace MAUS

#endif




