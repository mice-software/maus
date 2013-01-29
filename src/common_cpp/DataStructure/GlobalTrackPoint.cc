/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// MAUS headers
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"

namespace MAUS {

// Constructors
GlobalTrackPoint::GlobalTrackPoint() : time_(0.), energy_(0.),
                                       position_(ThreeVector()),
                                       momentum_(ThreeVector()),
                                       detector_id_(0), particle_id_(0) { }

// Destructor
GlobalTrackPoint::~GlobalTrackPoint() {}

GlobalTrackPoint::GlobalTrackPoint(const GlobalTrackPoint & track_point)
    : time_(track_point.time_), energy_(track_point.energy_),
      position_(track_point.position_), momentum_(track_point.momentum_),
      detector_id_(track_point.detector_id_),
      particle_id_(track_point.particle_id_) { }

// Assignment operator
GlobalTrackPoint & GlobalTrackPoint::operator=(
    const GlobalTrackPoint & track_point) {
  if (this == &track_point) {
      return *this;
  }

  time_ = track_point.time_;
  energy_ = track_point.energy_;
  position_ = track_point.position_;
  momentum_ = track_point.momentum_;
  detector_id_ = track_point.detector_id_;
  particle_id_ = track_point.particle_id_;

  return *this;
}

} // ~namespace MAUS
