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
  const int particle_id = start_plane_hits[0].particle_id();

  double hit_total = 0.0;
  std::vector<reconstruction::global::TrackPoint>::const_iterator hit;
  for (hit = station_hits.begin(); hit != station_hits.end(); ++hit) {
    hit_total += hit->z();
  }
  double end_plane = hit_total / station_hits.size();
  
  const double mass
    = reconstruction::global::Particle::GetInstance()->GetMass(particle_id);
  
  return new LinearApproximationTransferMap(start_plane_, end_plane, mass);
    
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

  const double px = vector.Px();
  const double py = vector.Py();

  double time;
  double energy;
  double z;
  double pz;
  if (vector.type() == PhaseSpaceVector::PhaseSpaceType::kTemporal) {
std::cout << "DEBUG Transport(): vector is temporal" << std::endl;
    time = vector.t();
    energy = vector.E();
    const PhaseSpaceVector pos_vector(
      vector,
      mass_,
      PhaseSpaceVector::PhaseSpaceType::kPositional);
    z = pos_vector.z();
    pz = pos_vector.Pz();
  } else {
std::cout << "DEBUG Transport(): vector is positional" << std::endl;
    z = vector.z();
    pz = vector.Pz();
    const PhaseSpaceVector temp_vector(
      vector,
      mass_,
      PhaseSpaceVector::PhaseSpaceType::kTemporal);
    time = temp_vector.t();
    energy = temp_vector.E();
  }
std::cout << "Time: " << time << " ns" << std::endl;
std::cout << "Energy: " << energy << " MeV" << std::endl;
std::cout << "Pz: " << pz << " MeV/c" << std::endl;
  const double momentum = ::sqrt(energy*energy - mass_*mass_);
std::cout << "Momentum: " << momentum << " MeV/c" << std::endl;
std::cout << "c: " << ::CLHEP::c_light << " mm/ns" << std::endl;
  const double beta = momentum / energy;
std::cout << "Beta: " << beta << std::endl;
std::cout << "Velocity: " << beta * ::CLHEP::c_light << " mm/ns" << std::endl;

  // delta_t = delta_z / v_z ~= delta_z / velocity
  double delta_t = delta_z / beta;
std::cout << "Delta T: " << delta_t << " ns" << std::endl;

  const double gamma = energy / mass_;
std::cout << "Gamma: " << gamma << std::endl;

  if (vector.type() == PhaseSpaceVector::PhaseSpaceType::kTemporal) {
    // delta_x = v_x * delta_t, px = gamma * m * v_x
    double delta_x = px * delta_t / gamma / mass_;
std::cout << "Delta X: " << delta_x << " mm" << std::endl;

    // delta_y = v_y * delta_t, py = gamma * m v_y
    double delta_y = py * delta_t / gamma / mass_;
std::cout << "Delta Y: " << delta_y << " mm" << std::endl;

    transported_vector[0] += delta_t;
    transported_vector[2] += delta_x;
    transported_vector[4] += delta_y;
  } else {
    // delta_x = v_x * delta_t, pz = gamma * m v_z
    double delta_x = px * delta_t / gamma / mass_;
std::cout << "Delta X: " << delta_x << " mm" << std::endl;

    // delta_y = v_y * delta_t
    double delta_y = py * delta_t / gamma/ mass_;
std::cout << "Delta Y: " << delta_y << " mm" << std::endl;

    // delta_z = v_z * delta_t
    double delta_z = pz * delta_t / gamma / mass_;
std::cout << "Delta Z: " << delta_z << " mm" << std::endl;

    transported_vector[0] += delta_x;
    transported_vector[2] += delta_y;
    transported_vector[4] += delta_z;
  }

fprintf(stdout, "CHECKPOINT Transport() 999\n"); fflush(stdout);
  return transported_vector;
}

}  // namespace MAUS
