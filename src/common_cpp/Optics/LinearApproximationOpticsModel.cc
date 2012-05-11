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
#include <iostream>

#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Interface/Squeal.hh"
#include "Maths/Vector.hh"

namespace MAUS {
  
void LinearApproximationOpticsModel::Build(const Json::Value & configuration) {
  // Nothing to do here
}

TransferMap * LinearApproximationOpticsModel::GenerateTransferMap(
    double start_plane, double end_plane, double mass) const {
  return new LinearApproximationTransferMap(start_plane, end_plane, mass);
}

CovarianceMatrix LinearApproximationTransferMap::Transport(
    const CovarianceMatrix & covariances) const {
  // This method of transport is too stupid to handle covariance matrices,
  // so just return what was given.
  return covariances;
}

PhaseSpaceVector LinearApproximationTransferMap::Transport(
    const PhaseSpaceVector & vector) const {
  // Use the energy and momentum to determine when and where the particle would
  // be if it were traveling in free space from start_plane_ to end_plane_.
  PhaseSpaceVector transported_vector(vector);
  double delta_z = end_plane_ - start_plane_;
std::cout << "Delta Z: " << delta_z << std::endl;

  const double momentum = ::sqrt(vector.E()*vector.E() - mass_*mass_);
std::cout << "Momentum: " << momentum << std::endl;
  const double velocity = ::CLHEP::c_light * momentum / vector.E();
std::cout << "Velocity: " << velocity << std::endl;

  // delta_t = delta_z / v_z ~= delta_z / velocity
  double delta_t = delta_z / velocity;
std::cout << "Delta T: " << delta_t << std::endl;

  // delta_x = v_x * delta_t
  double delta_x = vector.Px() * delta_t / mass_;
std::cout << "Delta X: " << delta_x << std::endl;

  // delta_y = v_y * delta_t
  double delta_y = vector.Py() * delta_t / mass_;
std::cout << "Delta Y: " << delta_y << std::endl;

  transported_vector[0] += delta_t;
  transported_vector[2] += delta_x;
  transported_vector[4] += delta_y;
  
  return transported_vector;
}

}  // namespace MAUS
