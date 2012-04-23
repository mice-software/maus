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

#include "Reconstruction/TrackPoint.hh"

namespace MAUS {

class Track : public std::vector<TrackPoint> {
 public:
  /* @brief	Construct with all elements initialized to zero.
   */
  Track();

  /* @brief Create with particle ID only.
   */
  explicit Track(const int particle_id);

  /* @brief Create with tracks and particle ID.
   */
  Track(const std::vector<TrackPoint>& tracks,
                     const int particle_id);

  ~Track();
  
  int particle_id() const;
  void set_particle_id(const int particle_id);
 protected:
  int particle_id_;
};
}  // namespace MAUS

#endif
