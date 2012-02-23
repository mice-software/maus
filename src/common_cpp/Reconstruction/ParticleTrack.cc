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

#include "Reconstruction/ParticleTrack.hh"

#include <cmath>
#include "CLHEP/Units/PhysicalConstants.h"

#include "Interface/Squeak.hh"

namespace MAUS {

ParticleTrack::ParticleTrack()
    : PhaseSpaceVector(), z_(0.), z_momentum_(0.)
{ }

ParticleTrack::ParticleTrack(const ParticleTrack& original_instance)
    : PhaseSpaceVector(original_instance), z_(original_instance.z_),
      z_momentum_(original_instance.z_momentum_)
{ }

ParticleTrack::ParticleTrack(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py)
    : PhaseSpaceVector(t, E, x, Px, y, Py), z_(0.), z_momentum_(0.)
{ }

ParticleTrack::ParticleTrack(double const * const array)
    : PhaseSpaceVector(array)
{ }

ParticleTrack::~ParticleTrack() { }

void ParticleTrack::FillInCoordinates(const double mass) {
  double energy = (*this)[1]
  double c_squared = ::CLHEP::c_light * ::CLHEP::c_light;
  double px = (*this)[3];
  double py = (*this)[5];
  z_momentum_ = ::sqrt(energy*energy - mass*mass - px*px - py*py);

  double velocity = z_momentum_ / mass;
  double time = (*this)[0];
  z_ = velocity * time;
}

std::ostream& operator<<(std::ostream& out, const ParticleTrack& vector) {
  out << "t: " << vector[0]  << "E: "   << vector[1]
      << "x: " << vector[2]  << "Px: "  << vector[3]
      << "y: " << vector[4]  << "Py: "  << vector[5]
      << "z: " << vector.z() << "Pz: "  << vector.Pz();
  return out;
}
}  // namespace MAUS
