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

/* Author: Peter Lane
 */

#ifndef COMMON_CPP_OPTICS_TRANSFER_MAP_HH
#define COMMON_CPP_OPTICS_TRANSFER_MAP_HH

#include <ostream>
#include <vector>

#include "Interface/Squeal.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"

// Forward declarations for OpticsModel.hh
class OpticsModel;

namespace MAUS {

// Forward declarations for src/legacy/Interface/PolynomialVector.hh
class PolynomialVector;

// Forward declarations for PhaseSpaceVector.hh
class PhaseSpaceVector;

/** @class TransferMap is a polynomial mapping, M, of a phase space vector from
 *  a plane at Z1 to another plane at Z2 such that a PhaseSpaceVector with
 *  coordinates U transforms like (using Einstein's summation convention)
 *    U(Z2)_p = M_ip U(Z1)_i + M_ijp U(Z1)_i U(Z1)_j
 *            + M_ijkp U(Z1)_i U(Z1)_j U(Z1)_k + ...
 *  extending to arbitrary order. A reference trajectory is assumed, that is the
 *  transformation is applied about some phase space vector that is taken to be
 *  a zero point. In other words, U(Z) is actually the delta vector from some
 *  reference trajectory U_0:
 *     V(Z) = U_0(Z) + U(Z),
 *  where V(Z) is the phase space vector that the TransferMap is applied to.
 */
class TransferMap {
 public:
  // ******************************
  //  Constructors
  // ******************************

  /* @brief constructor for different input and output reference trajectories.
   * @params polynomial                the mapping as a polynomial vector
   * @params reference_trajectory_in   input reference trajectory
   * @params reference_trajectory_out  output reference trajectory
   */
  TransferMap(const PolynomialVector& polynomial,
              const PhaseSpaceVector& reference_trajectory_in,
              const PhaseSpaceVector& reference_trajectory_out);

  /* @brief constructor for identical input and output reference trajectories.
   * @params polynomial            the mapping as a polynomial vector
   * @params reference_trajectory  input/output reference trajectory
   */
  TransferMap(const PolynomialVector& polynomial,
              const PhaseSpaceVector& reference_trajectory);

  /* @brief copy constructor
   */
  TransferMap(const TransferMap& original_instance);

  /* @brief destructor
   */
  ~TransferMap() {;}

  // ******************************
  //  First-order Map Functions
  // ******************************

  Matrix<double> CreateTransferMatrix() const;

  // ******************************
  //           operators
  // ******************************

  /* @brief transports a beam envelope (covariance matrix) using a first-order
   * transfer mapping (transfer matrix). The function performs a similarity
   * transform on the covariance matrix: M^T C M.
   *
   * @params aCovMatrix the matrix of second moments of the phase space
   *                    variables {t, E, x, Px, y, Py} to be transported
   */
  CovarianceMatrix operator*(const CovarianceMatrix & covariances) const;

  /* @brief transports a phase space vector ({t, E, x, Px, y, Py}) using a
   * polynomial vector mapping. The matrix of polynomial vector coefficients
   * operates on the delta of the initial vector and the initial
   * reference trajectory vector. The final reference trajectory is added onto
   * the transformed delta and returns the resulting vector.
   *
   * @params aPhaseSpaceVector the phase space vector to be transported
   */
  PhaseSpaceVector operator*(PhaseSpaceVector const & vector) const;

  friend std::ostream& operator<<(std::ostream& out, const TransferMap& tm);

 protected:

  TransferMap();

  const PolynomialVector& polynomial_;
  const PhaseSpaceVector& reference_trajectory_in_;
  const PhaseSpaceVector& reference_trajectory_out_;
};

std::ostream& operator<<(std::ostream& out, const TransferMap& map);
}  // namespace MAUS

#endif
