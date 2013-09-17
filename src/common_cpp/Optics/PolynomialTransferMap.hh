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

#ifndef COMMON_CPP_OPTICS_POLYNOMIAL_TRANSFER_MAP_HH
#define COMMON_CPP_OPTICS_POLYNOMIAL_TRANSFER_MAP_HH

#include <ostream>
#include <vector>

#include "Utils/Exception.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/TransferMap.hh"

// Forward declarations for OpticsModel.hh
class OpticsModel;

namespace MAUS {

// Forward declarations for src/legacy/Interface/PolynomialMap.hh
class PolynomialMap;

// Forward declarations for PhaseSpaceVector.hh
class PhaseSpaceVector;

/** @class PolynomialTransferMap is a polynomial mapping, M, of a phase space vector from
 *  a plane at Z1 to another plane at Z2 such that a PhaseSpaceVector with
 *  coordinates U transforms like (using Einstein's summation convention)
 *    U(Z2)_p = M_ip U(Z1)_i + M_ijp U(Z1)_i U(Z1)_j
 *            + M_ijkp U(Z1)_i U(Z1)_j U(Z1)_k + ...
 *  extending to arbitrary order. A reference trajectory is assumed, that is the
 *  transformation is applied about some phase space vector that is taken to be
 *  a zero point. In other words, U(Z) is actually the delta vector from some
 *  reference trajectory U_0:
 *     V(Z) = U_0(Z) + U(Z),
 *  where V(Z) is the phase space vector that the PolynomialTransferMap is applied to.
 */
class PolynomialTransferMap : public TransferMap {
 public:
  // ******************************
  //  Constructors
  // ******************************

  /* @brief constructor for different input and output reference trajectories.
   * @params polynomial_map           the actual polynomial map
   * @params reference_trajectory_in  input reference trajectory
   * @params reference_trajectory_out output reference trajectory
   */
  PolynomialTransferMap(const PolynomialMap& polynomial_map,
                        const PhaseSpaceVector& reference_trajectory_in,
                        const PhaseSpaceVector& reference_trajectory_out);

  /* @brief constructor for identical input and output reference trajectories.
   * @params polynomial_map        the actual polynomial map
   * @params reference_trajectory  input/output reference trajectory
   */
  PolynomialTransferMap(const PolynomialMap& polynomial_map,
                        const PhaseSpaceVector& reference_trajectory);

  /* @brief copy constructor
   */
  PolynomialTransferMap(const PolynomialTransferMap& original_instance);

  /* @brief destructor
   */
  ~PolynomialTransferMap();

  friend std::ostream& operator<<(std::ostream& out,
                                  const PolynomialTransferMap& tm);

  TransferMap * Inverse() const;

  CovarianceMatrix Transport(const CovarianceMatrix & covariances) const;

  PhaseSpaceVector Transport(const PhaseSpaceVector & vector) const;
 protected:

  PolynomialTransferMap();

  // ******************************
  //  First-order Map Functions
  // ******************************

  Matrix<double> CreateTransferMatrix() const;

  PolynomialMap const * const polynomial_map_;
  PhaseSpaceVector const * const reference_trajectory_in_;
  PhaseSpaceVector const * const reference_trajectory_out_;
};

std::ostream& operator<<(std::ostream& out, const PolynomialTransferMap& map);
}  // namespace MAUS

#endif
