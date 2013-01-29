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

#ifndef COMMON_CPP_RECONSTRUCTION_TRACK_HH
#define COMMON_CPP_RECONSTRUCTION_TRACK_HH

#include <vector>

#include "Recon/Global/TrackPoint.hh"

namespace MAUS {
namespace recon {
namespace global {

class Track : public std::vector<TrackPoint> {
 public:
  /* @brief	Construct with all elements initialized to zero.
   */
  Track();

  /* @brief Create with particle ID only.
   */
  explicit Track(const Particle::ID particle_id);

  /* @brief Copy constructor;
   */  
  Track(const Track & original_instance);

  /* @brief Create with tracks and particle ID.
   */
  Track(const std::vector<TrackPoint>& tracks,
        const Particle::ID particle_id);

  ~Track();

  Particle::ID particle_id() const;
  void set_particle_id(const Particle::ID particle_id);
 protected:
  Particle::ID particle_id_;
};

std::ostream& operator<<(std::ostream & out, const Track & vector);

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
