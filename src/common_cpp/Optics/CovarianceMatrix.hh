/* This file is part of MAUS: http://   micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#ifndef COMMON_CPP_OPTICS_COVARIANCE_MATRIX_HH
#define COMMON_CPP_OPTICS_COVARIANCE_MATRIX_HH

#include <vector>

#include "Maths/SymmetricMatrix.hh"
#include "Optics/PhaseSpaceVector.hh"

namespace CLHEP {
class HepSymMatrix;
}

namespace MAUS {
// *************************
//  Forward Declarations
// *************************

class Matrix<double>;
class CovarianceMatrix;

// ############################
//  Free Functions
// ############################

/* @brief Return a rotated covariance matrix.
 */
CovarianceMatrix rotate(const CovarianceMatrix& covariances, double angle);

std::ostream& operator<<(std::ostream& out, CovarianceMatrix cov);

// ############################
//  CovarianceMatrix
// ############################

/** @class CovarianceMatrix
 *  Extend SymmetricMatrix to represent a 6x6 matrix of covariances
 *  (second moments) of phase space coordinates {t, E, x, Px, y, Py}.
 *  For example, element [1][3] is <E x> and [5][4] is <Py Px>.
 */
class CovarianceMatrix : public SymmetricMatrix {
 public:
  /** @brief  Create a 6x6 matrix with all elements initialized to zero.
   */
  CovarianceMatrix();

  /** @brief  Copy constructor.
   */
  CovarianceMatrix(const CovarianceMatrix& original_instance);

  /** @brief  Matrix copy constructor. This copies only the first 6x6 block of
   *          elements of the Matrix<double> object.
   */
  explicit CovarianceMatrix(const Matrix<double>& matrix);

  /** @brief  Base class copy constructor. This copies only the first 6x6 block
   *          elements of the SymmetricMatrix object.
   */
  explicit CovarianceMatrix(const SymmetricMatrix& symmetric_matrix);

  /** @brief  HepSymMatrix copy constructor. This copies only the first 6x6
   *          block elements of the HepSymMatrix object.
   */
  explicit CovarianceMatrix(const ::CLHEP::HepSymMatrix& hep_sym_matrix);

  /** @brief  Construct from an array of doubles in row-major order.
   */
  explicit CovarianceMatrix(double const * const array_matrix);

  /** @brief  Set the matrix elements using the Penn parameterisation for a
   *          cylindrically symmetric matrix.
   *  @param[in] mass particle mass
   *  @param[in] momentum reference momentum
   *  @param[in] charge particle charge
   *  @param[in] emittance_t transverse emittance
   *  @param[in] beta_t transverse Twiss beta function
   *  @param[in] alpha_t transverse Twiss alpha function
   *  @param[in] Ltwiddle_t normalised, canonical, angular momentum
   *  @param[in] emittance_l longetudinal emittance
   *  @param[in] beta_l longetudinal Twiss beta function
   *  @param[in] alpha_l longetudinal Twiss alpha function
   *  @param[in] Bz magnetic field strength at r=0
   *  @param[in] dispersion_x x dispersion
   *  @param[in] dispersion_prime_x change in x dispersion w.r.t. s
   *  @param[in] dispersion_y y dispersion
   *  @param[in] dispersion_prime_y change in y dispersion w.r.t. s
   */
  CovarianceMatrix(double mass,
                   double momentum,
                   double charge,
                   double emittance_t,
                   double beta_t,
                   double alpha_t,
                   double Ltwiddle_t,
                   double emittance_l,
                   double beta_l,
                   double alpha_l,
                   double Bz,
                   double dispersion_x,
                   double dispersion_prime_x,
                   double dispersion_y,
                   double dispersion_prime_y);

  /** @brief  Set the matrix elements using the Twiss parameterisation.
   *
   *  @param[in] mass particle mass
   *  @param[in] momentum reference momentum
   *  @param[in] charge particle charge
   *  @param[in] emittance_x x emittance
   *  @param[in] beta_x x Twiss beta function
   *  @param[in] alpha_x x Twiss alpha function
   *  @param[in] emittance_y y emittance
   *  @param[in] beta_y y Twiss beta function
   *  @param[in] alpha_y y Twiss alpha function
   *  @param[in] emittance_l longetudinal emittance
   *  @param[in] beta_l longetudinal Twiss beta function
   *  @param[in] alpha_l longetudinal Twiss alpha function
   *  @param[in] dispersion_x x dispersion
   *  @param[in] dispersion_prime_x change in x dispersion w.r.t. s
   *  @param[in] dispersion_y y dispersion
   *  @param[in] dispersion_prime_y change in y dispersion w.r.t. s
   */
  CovarianceMatrix(double mass,
                   double momentum,
                   double energy,
                   double emittance_x,
                   double beta_x,
                   double alpha_x,
                   double emittance_y,
                   double beta_y,
                   double alpha_y,
                   double emittance_l,
                   double beta_l,
                   double alpha_l,
                   double dispersion_x,
                   double dispersion_prime_x,
                   double dispersion_y,
                   double dispersion_prime_y);

  // ********************************
  // *** Public member functions  ***
  // ********************************

  /* @brief Return true if the Covariance matrix is positive definite
   */
  bool IsPositiveDefinite();
};
}  // namespace MAUS

#endif
