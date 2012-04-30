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

#include "Reconstruction/Track.hh"

#include "Interface/Squeak.hh"

namespace MAUS {

Track::Track() : std::vector<TrackPoint>(), particle_id_(0)
{ }

Track::Track(const std::vector<TrackPoint>& tracks,
             const int particle_id)
    : std::vector<TrackPoint>(tracks), particle_id_(particle_id)
{ }

Track::~Track()
{ }

Track::Track(const Track & original_instance)
    : std::vector<TrackPoint>(original_instance),
      particle_id_(original_instance.particle_id_)
{ }

int Track::particle_id() const {
  return particle_id_;
}

void Track::set_particle_id(const int particle_id) {
  particle_id_ = particle_id;
}

}  // namespace MAUS
