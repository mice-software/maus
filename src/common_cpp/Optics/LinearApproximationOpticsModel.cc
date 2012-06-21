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
#include "src/common_cpp/Reconstruction/Global/Particle.hh"
#include "Maths/Vector.hh"

#include "Interface/Squeal.hh"

namespace MAUS {

const TransferMap * LinearApproximationOpticsModel::CalculateTransferMap(
    const std::vector<reconstruction::global::TrackPoint> & start_plane_hits,
    const std::vector<reconstruction::global::TrackPoint> & station_hits)
    const {
std::cout << "DEBUG CalculateTransferMap(): Start Plane Hits back() (1) PID = " << start_plane_hits.back().particle_id() << std::endl;
  std::vector<reconstruction::global::TrackPoint>::const_iterator hit;

  double hit_total = 0.0;
  for (hit = start_plane_hits.begin(); hit != start_plane_hits.end(); ++hit) {
    hit_total += hit->z();
  }
  double start_plane = hit_total / start_plane_hits.size();
  
  --hit;
  const int particle_id = hit->particle_id();
std::cout << "DEBUG CalculateTransferMap(): Start Plane Hit's PID = " << hit->particle_id() << std::endl;

  hit_total = 0.0;
  for (hit = station_hits.begin(); hit != station_hits.end(); ++hit) {
    hit_total += hit->z();
  }
  double end_plane = hit_total / station_hits.size();
  
  const double mass
    = reconstruction::global::Particle::GetInstance()->GetMass(particle_id);
  
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
fprintf(stdout, "CHECKPOINT Transport() 0\n"); fflush(stdout);
  // Use the energy and momentum to determine when and where the particle would
  // be if it were traveling in free space from start_plane_ to end_plane_.
  PhaseSpaceVector transported_vector(vector);
  double delta_z = end_plane_ - start_plane_;
std::cout << "Delta Z: " << delta_z << " mm" << std::endl;

  const double momentum = ::sqrt(vector.E()*vector.E() - mass_*mass_);
std::cout << "Momentum: " << momentum << " MeV/c" << std::endl;
  const double velocity = ::CLHEP::c_light * momentum / vector.E();
std::cout << "Velocity: " << velocity << " mm/ns" << std::endl;

  // delta_t = delta_z / v_z ~= delta_z / velocity
  double delta_t = delta_z / velocity;
std::cout << "Delta T: " << delta_t << " ns" << std::endl;

  // delta_x = v_x * delta_t
  double delta_x = vector.Px() * delta_t / mass_;
std::cout << "Delta X: " << delta_x << " mm" << std::endl;

  // delta_y = v_y * delta_t
  double delta_y = vector.Py() * delta_t / mass_;
std::cout << "Delta Y: " << delta_y << " mm" << std::endl;

  transported_vector[0] += delta_t;
  transported_vector[2] += delta_x;
  transported_vector[4] += delta_y;

fprintf(stdout, "CHECKPOINT Transport() 999\n"); fflush(stdout);
  return transported_vector;
}

}  // namespace MAUS
