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

#include "Recon/Global/WorkingTrackPoint.hh"

#include <cmath>
#include "CLHEP/Units/PhysicalConstants.h"

#include "DataStructure/ThreeVector.hh"
#include "src/common_cpp/Maths/Vector.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Interface/Squeak.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"

namespace MAUS {
namespace recon {
namespace global {

WorkingTrackPoint::WorkingTrackPoint()
    : PhaseSpaceVector(),
      detector_id_(Detector::kNone),
      particle_id_(Particle::kNone),
      z_(0.0)
{ }

WorkingTrackPoint::WorkingTrackPoint(const WorkingTrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      detector_id_(original_instance.detector_id_),
      uncertainties_(original_instance.uncertainties_),
      particle_id_(original_instance.particle_id_),
      z_(original_instance.z_)
{ }

WorkingTrackPoint::WorkingTrackPoint(const PhaseSpaceVector & original_instance,
                                     const double z, const Particle::ID pid)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone),
      particle_id_(pid), z_(z)
{ }

WorkingTrackPoint::WorkingTrackPoint(const Vector<double> & original_instance,
                                     const double z, const Particle::ID pid)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone),
      particle_id_(pid), z_(z)
{ }

WorkingTrackPoint::WorkingTrackPoint(const double time, const double energy,
                                     const double x, const double px,
                                     const double y, const double py,
                                     const CovarianceMatrix & uncertainties,
                                     const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(Detector::kNone),
      uncertainties_(uncertainties),
      particle_id_(Particle::kNone), z_(z) { }

WorkingTrackPoint::WorkingTrackPoint(const double time, const double energy,
                                     const double x, const double px,
                                     const double y, const double py,
                                     const Detector & detector,
                                     const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(detector.id()),
      uncertainties_(detector.uncertainties()),
      particle_id_(Particle::kNone), z_(z) { }

WorkingTrackPoint::WorkingTrackPoint(const double time, const double energy,
                                     const double x, const double px,
                                     const double y, const double py,
                                     const Particle::ID particle_id,
                                     const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(Detector::kNone),
      particle_id_(particle_id), z_(z) { }

WorkingTrackPoint::WorkingTrackPoint(double const * const array,
                                     const double z)
    : PhaseSpaceVector(array),
      detector_id_(Detector::kNone),
      particle_id_(Particle::kNone), z_(z) {
}

WorkingTrackPoint::~WorkingTrackPoint() {
}

WorkingTrackPoint & WorkingTrackPoint::operator=(const WorkingTrackPoint& rhs) {
  PhaseSpaceVector::operator=(rhs);
  particle_id_ = rhs.particle_id_;
  uncertainties_ = rhs.uncertainties_;
  detector_id_ = rhs.detector_id_;
  z_ = rhs.z_;

  return *this;
}

const bool WorkingTrackPoint::operator==(const WorkingTrackPoint& rhs) const {
  if (static_cast<PhaseSpaceVector const * const>(this)->operator!=(rhs)) {
    return false;
  } else if (rhs.detector_id_ != detector_id_) {
    return false;
  } else if (rhs.uncertainties_ != uncertainties_) {
    return false;
  } else if (rhs.particle_id_ != particle_id_) {
    return false;
  } else if (rhs.z_ != z_) {
    return false;
  }

  return true;
}

const bool WorkingTrackPoint::operator!=(const WorkingTrackPoint& rhs) const {
  return !operator==(rhs);
}

const bool WorkingTrackPoint::operator<(const WorkingTrackPoint& rhs) const {
  return z() < rhs.z();
}

const bool WorkingTrackPoint::operator>(const WorkingTrackPoint& rhs) const {
  return z() > rhs.z();
}

void WorkingTrackPoint::set_particle_id(const Particle::ID id) {
  particle_id_ = id;
}

Particle::ID WorkingTrackPoint::particle_id() const {
  return particle_id_;
}

void WorkingTrackPoint::set_detector_id(const Detector::ID id) {
  detector_id_ = id;
}

Detector::ID WorkingTrackPoint::detector_id() const {
  return detector_id_;
}

void WorkingTrackPoint::set_uncertainties(const CovarianceMatrix & uncertainties) {
  uncertainties_ = uncertainties;
}

const CovarianceMatrix & WorkingTrackPoint::uncertainties() const {
  return uncertainties_;
}

double WorkingTrackPoint::z_momentum() const {
  if (particle_id_ == Particle::kNone) {
    throw(Squeal(Squeal::recoverable,
                 "Attempting to calculate the momentum of a non-particle",
                 "MAUS::recon::global::WorkingTrackPoint::z_momentum()"));
  }
  const double mass = Particle::GetInstance()->GetMass(particle_id_);
  const double energy = (*this)[1];
  const double px = (*this)[3];
  const double py = (*this)[5];

  if (energy < mass) {
    throw(Squeal(Squeal::recoverable,
                 "Attempting to calculate the momentum of a "
                 "particle that is off mass shell.",
                 "MAUS::recon::global::WorkingTrackPoint::z_momentum()"));
  }

  return ::sqrt(energy*energy - mass*mass - px*px - py*py);
}

MAUS::MAUSPrimaryGeneratorAction::PGParticle
PrimaryGeneratorParticle(const WorkingTrackPoint & point) {
  MAUSPrimaryGeneratorAction::PGParticle particle;
  particle.time = point.t();
  particle.energy = point.E();
  particle.x = point.x();
  particle.px = point.Px();
  particle.y = point.y();
  particle.py = point.Py();
  particle.z = point.z();
  particle.pz = point.Pz();
  particle.pid = point.particle_id();
  particle.seed = 0;

  return particle;
}

std::ostream& operator<<(std::ostream& out, const WorkingTrackPoint& point) {
  out << "t: " << point.time() << "  E: "  << point.energy()
      << "  x: " << point.x()    << "  Px: " << point.Px()
      << "  y: " << point.y()    << "  Py: " << point.Py()
      << "  z: " << point.z()
      << "  PID: " << point.particle_id() << "  DID: " << point.detector_id()
      << "  Uncertainties: " << point.uncertainties();

  return out;
}

}  // namespace global
}  // namespace recon
}  // namespace MAUS
