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
      uncertainties_(new CovarianceMatrix(),
      particle_id_(Particle::kNone))
{ }

TrackPoint::TrackPoint(const PhaseSpaceType type)
    : PhaseSpaceVector(type),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix(),
      particle_id_(Particle::kNone))
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(*original_instance.uncertainties_),
      particle_id_(Particle::kNone))
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance,
                       const double mass,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(original_instance, mass, type),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(*original_instance.uncertainties_),
      particle_id_(Particle::kNone))
{ }

TrackPoint::TrackPoint(const PhaseSpaceVector & original_instance,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(original_instance, type),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix(),
      particle_id_(Particle::kNone))
{ }

TrackPoint::TrackPoint(const Vector<double> & original_instance,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(original_instance, type),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()),
      particle_id_(Particle::kNone)
{ }

TrackPoint::TrackPoint(const double x1, const double p1,
                       const double x2, const double p2,
                       const double x3, const double p3,
                       const CovarianceMatrix & uncertainties,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(x1, p1, x2, p2, x3, p3, type),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix(uncertainties),
      particle_id_(Particle::kNone)) { }

TrackPoint::TrackPoint(const double x1, const double p1,
                       const double x2, const double p2,
                       const double x3, const double p3,
                       const Detector & detector,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(x1, p1, x2, p2, x3, p3, type),
      detector_id_(detector.id()),
      uncertainties_(new CovarianceMatrix(detector.uncertainties()),
      particle_id_(Particle::kNone)) { }

TrackPoint::TrackPoint(const double x1, const double p1,
                       const double x2, const double p2,
                       const double x3, const double p3,
                       const int particle_id,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(x1, p1, x2, p2, x3, p3, type),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix()),
      particle_id_(particle_id)) { }

TrackPoint::TrackPoint(double const * const array,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(array, type),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix(),
      particle_id_(Particle::kNone)) {
}

TrackPoint::~TrackPoint() {
  delete uncertainties_;
}

TrackPoint & TrackPoint::operator=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator=(rhs);
  detector_id_ = rhs.detector_id_;
  particle_id_ = rhs.particle_id_;

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
  if (type_ == PhaseSpaceVector::PhaseSpaceType::kPositional) {
    return z() < rhs.z();
  }

  return t() < rhs.t();
    
}

const bool TrackPoint::operator>(const TrackPoint& rhs) const {
  if (type_ == PhaseSpaceVector::PhaseSpaceType::kPositional) {
    return z() > rhs.z();
  }

  return t() > rhs.t();
    
}

void TrackPoint::set_particle_id(unsigned int id) {
  particle_id_ = id;
}

unsigned int TrackPoint::particle_id() const {
  return particle_id_;
}

void TrackPoint::set_detector_id(unsigned int id) {
  detector_id_ = id;
}

unsigned int TrackPoint::detector_id() const {
  return detector_id_;
}

void TrackPoint::set_uncertainties(const CovarianceMatrix & uncertainties) {
  delete uncertainties_;
  uncertainties_ = new CovarianceMatrix(uncertainties);
}

const CovarianceMatrix & TrackPoint::uncertainties() const {
  return *uncertainties_;
}

double TrackPoint::time() const {
  if (type_ == PhaseSpaceType::kPositional) {
    const double mass = Particle::GetInstance()->GetMass(particle_id_);
    const double energy = energy();
    const double x = (*this)[0];
    const double y = (*this)[2];
    const double z = (*this)[4];

    const double momentum = ::sqrt(energy*energy - mass*mass);
    const double velocity = ::CLHEP::c_light * momentum / energy;
    const double position = ::sqrt(x*x + y*y + z*z);
    const double time = position / velocity;

    if (z < 0) {
      time = -time;
    }

    return time;
  }
  
  return PhaseSpaceVector::time();
}

double TrackPoint::energy() const {
  if (type_ == PhaseSpaceType::kPositional) {
    const double mass = Particle::GetInstance()->GetMass(particle_id_);
    const double px = (*this)[1];
    const double py = (*this)[3];
    const double pz = (*this)[5];
    const double momentum = ::sqrt(px*px + py*py + pz*pz);

    return ::sqrt(mass*mass + momentum*momentum);
  }
  
  return PhaseSpaceVector::energy();
}

double TrackPoint::z() const {
  if (type_ == PhaseSpaceType::kTemporal) {
    const double time = (*this)[0];
    const double energy = (*this)[1];
    const double x = (*this)[2];
    const double y = (*this)[4];

    const double momentum = ::sqrt(energy*energy - mass*mass);
    double velocity = ::CLHEP::c_light * momentum / energy;
    double position = velocity * time;
    const double z = ::sqrt(position*position - x*x - y*y);

    if (time < 0) {
      z = -z;
    }

    return z;
  }
  
  return PhaseSpaceVector::time();
}

double TrackPoint::z_momentum() const {
  if (type_ == PhaseSpaceType::kTemporal) {
    const double mass = Particle::GetInstance()->GetMass(particle_id_);
    const double energy = (*this)[1];
    const double px = (*this)[3];
    const double py = (*this)[5];

    return ::sqrt(energy*energy - mass*mass - px*px - py*py);
  }
  
  return PhaseSpaceVector::energy();
}

void TrackPoint::set_time(double time) {
  if (type_ == PhaseSpaceType::kPositional) {
    // set z instead
    const double energy = (*this)[1];
    const double x = (*this)[2];
    const double y = (*this)[4];

    const double momentum = ::sqrt(energy*energy - mass*mass);
    double velocity = ::CLHEP::c_light * momentum / energy;
    double position = velocity * time;
    const double z = ::sqrt(position*position - x*x - y*y);

    if (time < 0) {
      z = -z;
    }

    set_z(z);
  } else {
    PhaseSpaceVector::set_time(time);
  }
}

void TrackPoint::set_energy(double energy) {
  if (type_ == PhaseSpaceType::kPositional) {
    // set z momentum instead
    const double mass = Particle::GetInstance()->GetMass(particle_id_);
    const double px = (*this)[3];
    const double py = (*this)[5];

    set_z_momentum(::sqrt(energy*energy - mass*mass - px*px - py*py));
  } else {
    PhaseSpaceVector::set_energy(energy);
  }
}

void TrackPoint::set_z(double z) {
  if (type_ == PhaseSpaceType::kTemporal) {
    // set time instead
    const double mass = Particle::GetInstance()->GetMass(particle_id_);
    const double energy = energy();
    const double x = (*this)[0];
    const double y = (*this)[2];

    const double momentum = ::sqrt(energy*energy - mass*mass);
    const double velocity = ::CLHEP::c_light * momentum / energy;
    const double position = ::sqrt(x*x + y*y + z*z);
    const double time = position / velocity;

    if (z < 0) {
      time = -time;
    }

    set_time(time);
  } else {
    PhaseSpaceVector::set_z(z);
  }
}

void TrackPoint::set_z_momentum(double z_momentum) {
  if (type_ == PhaseSpaceType::kTemporal) {
    // set energy instead
    const double mass = Particle::GetInstance()->GetMass(particle_id_);
    const double px = (*this)[1];
    const double py = (*this)[3];
    const double pz = z_momentum;
    const double momentum = ::sqrt(px*px + py*py + pz*pz);

    set_energy(::sqrt(mass*mass + momentum*momentum));
  } else {
    PhaseSpaceVector::set_z_momentum(z_momentum);
  }
}

MAUS::MAUSPrimaryGeneratorAction::PGParticle
PrimaryGeneratorParticle(const TrackPoint & point) {
  const double mass = Particle::GetInstance()->GetMass(pid);

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

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS
