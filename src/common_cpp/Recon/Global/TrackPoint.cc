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

#include "Recon/Global/TrackPoint.hh"

#include <cmath>
#include "CLHEP/Units/PhysicalConstants.h"

#include "DataStructure/ThreeVector.hh"
#include "src/common_cpp/Maths/Vector.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Interface/Squeak.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/ParticleOpticalVector.hh"

namespace MAUS {
namespace recon {
namespace global {

TrackPoint::TrackPoint()
    : PhaseSpaceVector(),
      detector_id_(Detector::kNone),
      particle_id_(Particle::kNone),
      z_(0.0)
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      detector_id_(original_instance.detector_id_),
      uncertainties_(original_instance.uncertainties_),
      particle_id_(original_instance.particle_id_),
      z_(original_instance.z_)
{ }

TrackPoint::TrackPoint(const PhaseSpaceVector & original_instance,
                       const double z, const Particle::ID pid)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone),
      particle_id_(pid), z_(z)
{ }

TrackPoint::TrackPoint(const Vector<double> & original_instance,
                       const double z, const Particle::ID pid)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone),
      particle_id_(pid), z_(z)
{ }

TrackPoint::TrackPoint(const ParticleOpticalVector & vector,
                       const double t0, const double E0, const double P0)
    : PhaseSpaceVector()
{
  const double beta0 = P0 / E0;
  const double gamma0 = 1. / ::sqrt(1 - beta0*beta0);
  const double k = beta0 * ::CLHEP::c_light * gamma0 / (1 + gamma0);

  set_t(vector.l() / k + t0);
  set_E(vector.delta() * E0 + E0);
  set_x(vector.x());
  set_Px(vector.a() * P0);
  set_y(vector.y());
  set_Py(vector.b() * P0);
}

TrackPoint::TrackPoint(const ParticleOpticalVector & original_instance)
    : PhaseSpaceVector(original_instance) { }

TrackPoint::TrackPoint(const double time, const double energy,
                       const double x, const double px,
                       const double y, const double py,
                       const CovarianceMatrix & uncertainties,
                       const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(Detector::kNone),
      uncertainties_(uncertainties),
      particle_id_(Particle::kNone), z_(z) { }

TrackPoint::TrackPoint(const double time, const double energy,
                       const double x, const double px,
                       const double y, const double py,
                       const Detector & detector)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(detector.id()),
      uncertainties_(detector.uncertainties()),
      particle_id_(Particle::kNone), z_(detector.plane()) { }

TrackPoint::TrackPoint(const double time, const double energy,
                       const double x, const double px,
                       const double y, const double py,
                       const Particle::ID particle_id,
                       const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(Detector::kNone),
      particle_id_(particle_id), z_(z) { }

TrackPoint::TrackPoint(double const * const array,
                       const double z)
    : PhaseSpaceVector(array),
      detector_id_(Detector::kNone),
      particle_id_(Particle::kNone), z_(z) {
}

TrackPoint::~TrackPoint() {
}

// *************************
//  Assignment Operators
// *************************

TrackPoint & TrackPoint::operator=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator=(rhs);
  particle_id_ = rhs.particle_id_;
  uncertainties_ = rhs.uncertainties_;
  detector_id_ = rhs.detector_id_;
  z_ = rhs.z_;

  return *this;
}

TrackPoint & TrackPoint::operator+=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator+=(rhs);
  particle_id_ = rhs.particle_id_;
  uncertainties_ = rhs.uncertainties_;
  detector_id_ = rhs.detector_id_;
  z_ = rhs.z_;

  return *this;
}

TrackPoint & TrackPoint::operator-=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator-=(rhs);
  particle_id_ = rhs.particle_id_;
  uncertainties_ = rhs.uncertainties_;
  detector_id_ = rhs.detector_id_;
  z_ = rhs.z_;

  return *this;
}

TrackPoint & TrackPoint::operator*=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator*=(rhs);
  particle_id_ = rhs.particle_id_;
  uncertainties_ = rhs.uncertainties_;
  detector_id_ = rhs.detector_id_;
  z_ = rhs.z_;

  return *this;
}

TrackPoint & TrackPoint::operator/=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator/=(rhs);
  particle_id_ = rhs.particle_id_;
  uncertainties_ = rhs.uncertainties_;
  detector_id_ = rhs.detector_id_;
  z_ = rhs.z_;

  return *this;
}

TrackPoint & TrackPoint::operator+=(const double& rhs) {
  PhaseSpaceVector::operator+=(rhs);

  return *this;
}

TrackPoint & TrackPoint::operator-=(const double& rhs) {
  PhaseSpaceVector::operator-=(rhs);

  return *this;
}

TrackPoint & TrackPoint::operator*=(const double& rhs) {
  PhaseSpaceVector::operator*=(rhs);

  return *this;
}

TrackPoint & TrackPoint::operator/=(const double& rhs) {
  PhaseSpaceVector::operator/=(rhs);

  return *this;
}

// *************************
//  Algebraic Operators
// *************************

const TrackPoint TrackPoint::operator+(const TrackPoint& rhs) const {
  return TrackPoint(*this) += rhs;
}

const TrackPoint TrackPoint::operator-(const TrackPoint& rhs) const {
  return TrackPoint(*this) -= rhs;
}

const TrackPoint TrackPoint::operator*(const TrackPoint& rhs) const {
  return TrackPoint(*this) *= rhs;
}

const TrackPoint TrackPoint::operator/(const TrackPoint& rhs) const {
  return TrackPoint(*this) /= rhs;
}

const TrackPoint TrackPoint::operator+(const double& rhs) const {
  return TrackPoint(*this) += rhs;
}

const TrackPoint TrackPoint::operator-(const double& rhs) const {
  return TrackPoint(*this) -= rhs;
}

const TrackPoint TrackPoint::operator*(const double& rhs) const {
  return TrackPoint(*this) *= rhs;
}

const TrackPoint TrackPoint::operator/(const double& rhs) const {
  return TrackPoint(*this) /= rhs;
}

// *************************
//  Comparison Operators
// *************************

const bool TrackPoint::operator==(const TrackPoint& rhs) const {
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

const bool TrackPoint::operator!=(const TrackPoint& rhs) const {
  return !operator==(rhs);
}

const bool TrackPoint::operator<(const TrackPoint& rhs) const {
  return z() < rhs.z();
}

const bool TrackPoint::operator>(const TrackPoint& rhs) const {
  return z() > rhs.z();
}

void TrackPoint::set_particle_id(const Particle::ID id) {
  particle_id_ = id;
}

Particle::ID TrackPoint::particle_id() const {
  return particle_id_;
}

void TrackPoint::set_detector_id(const Detector::ID id) {
  detector_id_ = id;
}

Detector::ID TrackPoint::detector_id() const {
  return detector_id_;
}

void TrackPoint::set_uncertainties(const CovarianceMatrix & uncertainties) {
  uncertainties_ = uncertainties;
}

const CovarianceMatrix & TrackPoint::uncertainties() const {
  return uncertainties_;
}

double TrackPoint::z_momentum() const {
  if (particle_id_ == Particle::kNone) {
    throw(Squeal(Squeal::recoverable,
                 "Attempting to calculate the momentum of a non-particle",
                 "MAUS::recon::global::TrackPoint::z_momentum()"));
  }
  const double mass = Particle::GetInstance()->GetMass(particle_id_);
  const double energy = (*this)[1];
  const double px = (*this)[3];
  const double py = (*this)[5];

  const double pz_squared = energy*energy - mass*mass - px*px - py*py;
  if (pz_squared < 0) {
    throw(Squeal(Squeal::recoverable,
                 "Attempting to calculate the longitudinal momentum of a "
                 "particle that is off mass shell.",
                 "MAUS::recon::global::TrackPoint::z_momentum()"));
  }

  return ::sqrt(pz_squared);
}

MAUS::MAUSPrimaryGeneratorAction::PGParticle
PrimaryGeneratorParticle(const TrackPoint & point) {
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

std::ostream& operator<<(std::ostream& out, const TrackPoint& point) {
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
