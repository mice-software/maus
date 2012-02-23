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

#include "Reconstruction/DetectorEvent.hh"

#include "Interface/Squeak.hh"

namespace MAUS {

DetectorEvent::DetectorEvent()
    : ParticleTrack(), detector_id_(-1)
{ }

DetectorEvent::DetectorEvent(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py,
                                   const int detector_id)
    : ParticleTrack(t, E, x, Px, y, Py), detector_id_(detector_id)
{ }

DetectorEvent::DetectorEvent(double const * const array)
    : ParticleTrack(array)
{ }

DetectorEvent::~DetectorEvent() { }

std::ostream& operator<<(std::ostream& out, const DetectorEvent& vector) {
  out << "t: "            << vector[0]  << "E: "  << vector[1]
      << "x: "            << vector[2]  << "Px: " << vector[3]
      << "y: "            << vector[4]  << "Py: " << vector[5]
      << "z: "            << vector.z() << "Pz: " << vector.Pz()
      << "Detector ID: "  << vector.detector_id();
  return out;
}
}  // namespace MAUS
