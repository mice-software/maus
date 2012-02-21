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

#ifndef COMMON_CPP_OPTICS_DETECTOR_EVENT_HH
#define COMMON_CPP_OPTICS_DETECTOR_EVENT_HH

#include <iostream>

#include "src/common_cpp/Optics/ParticleTrack.hh"

namespace MAUS {

class DetectorEvent : public MAUS::ParticleTrack {
 public:
  /* @brief	Construct with all elements initialized to zero.
   */
  DetectorEvent();

  /* @brief Create with coordinates from an array. Order is t, E, x, Px, y, Py.
   */
  explicit DetectorEvent(double const * const array);

  /* @brief	Create with the given initial coordinates.
   */
  DetectorEvent(const double t, const double E,
                   const double x, const double Px,
                   const double y, const double Py,
                   const int detector_id);

  ~DetectorEvent();

  // accessors
  inline double detector_id() const {return detector_id_;}

  // mutators
  inline void set_detector_id(double detector_id) {detector_id_ = detector_id;}
  
 protected:
  int detector_id_;
};

std::ostream& operator<<(std::ostream& out, const DetectorEvent& vector);
}  // namespace MAUS

#endif
