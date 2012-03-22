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

#include "Optics/LinearApproximationOpticsModel.hh"

#include <cmath>

#include <vector>

#include "Interface/Squeal.hh"
#include "Maths/Vector.hh"

namespace MAUS {
  
void LinearApproximationOpticsModel::Build(Json::Value * configuration) {
  // Nothing to do here
}

TransferMap transfer_map(double z1, double z2, double mass) {
  return LinearApproximationTransferMap(z1, z2, mass);
}

CovarianceMatrix LinearApproximationTransferMap::Transport(
    CovarianceMatrix const * const covariances) {
  // This method of transport is too stupid to handle covariance matrices,
  // so just return what was given.
  return *covariances;
}
PhaseSpaceVector LinearApproximationTransferMap::Transport(
    PhaseSpaceVector const * const vector) {
  // Use the energy and momentum to determine when and where the particle would
  // be if it were traveling in free space from plane z1 to plane z2.
  PhaseSpaceVector transported_vector(*vector);
  double detla_z = z2_ - z1_;
  double delta_t = mass_ * delta_z
                 / ::sqrt(  transported_vector(2)*transported_vector(2)
                          - mass_*mass_
                          - transported_vector(4)*transported_vector(4)
                          - vector(6)*vector(6));
  double delta_x = transported_vector(4) * delta_t / mass_;
  double delta_y = transported_vector(6) * delta_t / mass_;

  transported_vector(1) += delta_t;
  transported_vector(3) += delta_x;
  transported_vector(5) += delta_y;
  
  return transported_vector;
}

}  // namespace MAUS
