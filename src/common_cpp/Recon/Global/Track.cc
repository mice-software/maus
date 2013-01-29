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

#include "Recon/Global/Track.hh"

#include "Interface/Squeak.hh"

namespace MAUS {
namespace recon {
namespace global {

Track::Track() : std::vector<TrackPoint>(), particle_id_(Particle::kNone)
{ }

Track::Track(const Particle::ID particle_id)
    : std::vector<TrackPoint>(), particle_id_(particle_id)
{ }

Track::Track(const std::vector<TrackPoint>& tracks,
             const Particle::ID particle_id)
    : std::vector<TrackPoint>(tracks), particle_id_(particle_id)
{ }

Track::Track(const Track & original_instance)
    : std::vector<TrackPoint>(original_instance),
      particle_id_(original_instance.particle_id_)
{ }

Track::~Track()
{ }

Particle::ID Track::particle_id() const {
  return particle_id_;
}

void Track::set_particle_id(const Particle::ID particle_id) {
  particle_id_ = particle_id;
}

std::ostream& operator<<(std::ostream& out, const Track& track) {
  out << track.size();
  for (size_t index = 0; index < track.size(); ++index) {
    out << std::endl << track[index];
  }
  return out;
}

}  // namespace global
}  // namespace recon
}  // namespace MAUS
