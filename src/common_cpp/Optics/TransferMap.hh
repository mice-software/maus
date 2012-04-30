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

namespace MAUS {
// Forward declarations for CovarianceMatrix.hh
class CovarianceMatrix;

// Forward declarations for PhaseSpaceVector.hh
class PhaseSpaceVector;

class TransferMap {
 public:
  TransferMap(const double start_plane, const double end_plane)
      : start_plane_(start_plane), end_plane_(end_plane) { }
  // ******************************
  //      Transport functions
  // ******************************

  /* @brief transports a beam envelope (covariance matrix).
   *
   * @params covariances the matrix of second moments of the phase space
   *                    variables {t, E, x, Px, y, Py} to be transported
   */
  virtual CovarianceMatrix Transport(const CovarianceMatrix & covariances)
      const = 0;

  /* @brief transports a phase space vector ({t, E, x, Px, y, Py}).
   *
   * @params aPhaseSpaceVector the phase space vector to be transported
   */
  virtual PhaseSpaceVector Transport(const PhaseSpaceVector & vector)
      const = 0;
      
  const double start_plane() {return start_plane_;}
  const double end_plane() {return end_plane_;}
 protected:
  const double start_plane_;
  const double end_plane_;
};
}  // namespace MAUS

#endif
