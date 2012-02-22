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

#include "src/common_cpp/Optics/ParticleTrajectory.hh"

#include "Interface/Squeak.hh"

namespace MAUS {

ParticleTrajectory::ParticleTrajectory()
    : std::vector<ParticleTrack>()
{ }

ParticleTrajectory::ParticleTrajectory(const std::vector<ParticleTrack>& tracks,
                                       const int particle_id)
    : std::vector<ParticleTrack>(tracks), particle_id_(particle_id)
{ }

ParticleTrajectory::~ParticleTrajectory()
{ }

int ParticleTrajectory::particle_id() const {
  return particle_id_;
}

void ParticleTrajectory::set_particle_id(const int particle_id) {
  particle_id_ = particle_id;
}
}  // namespace MAUS
