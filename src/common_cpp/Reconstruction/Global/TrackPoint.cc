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

#include "Reconstruction/Global/TrackPoint.hh"

#include <cmath>
#include "CLHEP/Units/PhysicalConstants.h"

#include "DataStructure/ThreeVector.hh"
#include "src/common_cpp/Maths/Vector.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Interface/Squeak.hh"
#include "Reconstruction/Global/Detector.hh"
#include "Reconstruction/Global/Particle.hh"

namespace MAUS {
namespace reconstruction {
namespace global {

TrackPoint::TrackPoint()
    : PhaseSpaceVector(),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix()),
      particle_id_(Particle::kNone), z_(0.0)
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      Step(),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(*original_instance.uncertainties_)),
      particle_id_(original_instance.particle_id_),
      z_(original_instance.z_)
{
  double x = original_instance.x();
  double y = original_instance.y();
  double z = original_instance.z();
  SetPosition(ThreeVector(x, y, z));

  double px = original_instance.Px();
  double py = original_instance.Py();
  double pz = original_instance.Pz();
  SetMomentum(ThreeVector(px, py, pz));
  
  double energy = original_instance.energy();
  SetEnergy(energy);
  SetEnergyDeposited(energy);  // FIXME(plane1@hawk.iit.edu)

  double distance = ::sqrt(x*x + y*y + z*z);
  double momentum = ::sqrt(px*px + py*py + pz*pz);
  double time = distance * energy / momentum;
  SetTime(time);

  const double mass = Particle::GetInstance()->GetMass(particle_id_);
  double proper_time = time * mass / momentum;
  SetProperTime(proper_time);

  // SetPathLength();
}

TrackPoint::TrackPoint(const PhaseSpaceVector & original_instance,
                       const double z, const Particle::ID pid)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()),
      particle_id_(pid), z_(z)
{ }

TrackPoint::TrackPoint(const Vector<double> & original_instance,
                       const double z, const Particle::ID pid)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()),
      particle_id_(pid), z_(z)
{ }

TrackPoint::TrackPoint(const double time, const double energy,
                       const double x, const double px,
                       const double y, const double py,
                       const CovarianceMatrix & uncertainties,
                       const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix(uncertainties)),
      particle_id_(Particle::kNone), z_(z) { }

TrackPoint::TrackPoint(const double time, const double energy,
                       const double x, const double px,
                       const double y, const double py,
                       const Detector & detector,
                       const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(detector.id()),
      uncertainties_(new CovarianceMatrix(detector.uncertainties())),
      particle_id_(Particle::kNone), z_(z) { }

TrackPoint::TrackPoint(const double time, const double energy,
                       const double x, const double px,
                       const double y, const double py,
                       const Particle::ID particle_id,
                       const double z)
    : PhaseSpaceVector(time, energy, x, px, y, py),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix()),
      particle_id_(particle_id), z_(z) { }

TrackPoint::TrackPoint(double const * const array,
                       const double z)
    : PhaseSpaceVector(array),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()),
      particle_id_(Particle::kNone), z_(z) {
}

TrackPoint::~TrackPoint() {
  delete uncertainties_;
}

TrackPoint & TrackPoint::operator=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator=(rhs);
  detector_id_ = rhs.detector_id_;
  particle_id_ = rhs.particle_id_;
  z_ = rhs.z_;

  delete uncertainties_;
  uncertainties_ = new CovarianceMatrix(*rhs.uncertainties_);

  return *this;
}

const bool TrackPoint::operator==(const TrackPoint& rhs) const {
  if ((*this) != ((PhaseSpaceVector) rhs)) {
    return false;
  } else if (rhs.particle_id_ != particle_id_) {
    return false;
  } else if (rhs.detector_id_ != detector_id_) {
    return false;
  } else if ((*rhs.uncertainties_) != (*uncertainties_)) {
    return false;
  }

  return true;
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
  delete uncertainties_;
  uncertainties_ = new CovarianceMatrix(uncertainties);
}

const CovarianceMatrix & TrackPoint::uncertainties() const {
  return *uncertainties_;
}

double TrackPoint::z_momentum() const {
  const double mass = Particle::GetInstance()->GetMass(particle_id_);
  const double energy = (*this)[1];
  const double px = (*this)[3];
  const double py = (*this)[5];

  return ::sqrt(energy*energy - mass*mass - px*px - py*py);
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
  out << "t: " << point.time() << "E: "  << point.energy()
      << "x: " << point.x()    << "Px: " << point.Px()
      << "y: " << point.y()    << "Py: " << point.Py()
      << "z: " << point.z()    << "Pz: " << point.Pz();

  return out;
}

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS
