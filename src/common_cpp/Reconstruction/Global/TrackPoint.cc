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

namespace MAUS {
namespace reconstruction {
namespace global {

TrackPoint::TrackPoint()
    : PhaseSpaceVector(),
      z_(0.), z_momentum_(0.), detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      z_(original_instance.z_), z_momentum_(original_instance.z_momentum_),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(*original_instance.uncertainties_))
{ }

TrackPoint::TrackPoint(const PhaseSpaceVector & original_instance)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const Vector<double> & original_instance)
    : PhaseSpaceVector(original_instance),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py,
                                   const double z, const double Pz,
                                   const CovarianceMatrix & uncertainties)
    : PhaseSpaceVector(t, E, x, Px, y, Py),
      z_(z), z_momentum_(Pz), detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix(uncertainties)) { }

TrackPoint::TrackPoint(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py,
                                   const double z, const double Pz,
                                   const Detector & detector)
    : PhaseSpaceVector(t, E, x, Px, y, Py), z_(z), z_momentum_(Pz),
      detector_id_(detector.id()),
      uncertainties_(new CovarianceMatrix(detector.uncertainties())) { }

TrackPoint::TrackPoint(double const * const array)
    : PhaseSpaceVector(array), z_(array[6]), z_momentum_(array[7]),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()) {
}

TrackPoint::~TrackPoint() {
  delete uncertainties_;
}

TrackPoint & TrackPoint::operator=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator=(rhs);
  z_ = rhs.z_;
  z_momentum_ = rhs.z_momentum_;
  detector_id_ = rhs.detector_id_;

  delete uncertainties_;
  uncertainties_ = new CovarianceMatrix(*rhs.uncertainties_);
}

const bool TrackPoint::operator==(const TrackPoint& rhs) const {
  if ((*this) != ((PhaseSpaceVector) rhs)) {
    return false;
  } else if (rhs.z_ != z_) {
    fprintf(stdout, "z coordinates are not equal!\n");
    return false;
  } else if (rhs.z_momentum_ != z_momentum_) {
    fprintf(stdout, "z momenta are not equal!\n");
    return false;
  } else if (rhs.detector_id_ != detector_id_) {
    fprintf(stdout, "Detector IDs are not equal!\n");
    return false;
  } else if ((*rhs.uncertainties_) != (*uncertainties_)) {
    fprintf(stdout, "CovarianceMatrices are not equal!\n");
    return false;
  }
/*
  if (operator!=((PhaseSpaceVector) rhs) ||
      (rhs.z_ != z_) || (rhs.z_momentum_ != z_momentum_) ||
      (rhs.detector_id_ != detector_id_) ||
      (rhs.uncertainties_ != uncertainties_)) {
    return false;
  }
*/

  return true;
}

double TrackPoint::z() const {
  return z_;
}

void TrackPoint::set_z(const double z) {
  z_ = z;
}

double TrackPoint::z_momentum() const {
  return z_momentum_;
}

void TrackPoint::set_z_momentum(const double z_momentum) {
  z_momentum_ = z_momentum;
}

double TrackPoint::Pz() const {
  return z_momentum_;
}

void TrackPoint::set_Pz(const double Pz) {
  z_momentum_ = Pz;
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

/* Fills in z and Pz from t, E, and the mass.
 */
void TrackPoint::FillInAxialCoordinates(const double mass) {
  double energy = (*this)[1];
  const double px = (*this)[3];
  const double py = (*this)[5];

  const double momentum = ::sqrt(energy*energy - mass*mass);

  // fill in the Pz coordinate
  z_momentum_ = ::sqrt(momentum*momentum - px*px - py*py);

  const double time = (*this)[0];
  const double x = (*this)[2];
  const double y = (*this)[4];

  double velocity = ::CLHEP::c_light * momentum / energy;
  double position = velocity * time;

  // fill in the z coordinate
  z_ = ::sqrt(position*position - x*x - y*y);
}

/* Fills in t and E from z, Pz, and the given mass parameter.
 */
void TrackPoint::FillInTemporalCoordinates(const double mass) {
  double & energy_ = (*this)[1];
  const double px = (*this)[3];
  const double py = (*this)[5];
  const double pz = z_momentum_;

  const double momentum = ::sqrt(px*px + py*py + pz*pz);

  // fill in the energy coordinate
  energy_ = ::sqrt(mass*mass + momentum*momentum);

  double & time_ = (*this)[0];
  const double x = (*this)[2];
  const double y = (*this)[4];
  const double z = z_;

  const double position = ::sqrt(x*x + y*y + z*z);
  const double velocity = ::CLHEP::c_light * momentum / energy_;

  // fill in the time coordinate
  time_ = position / velocity;
}

MAUS::MAUSPrimaryGeneratorAction::PGParticle
PrimaryGeneratorParticle(const TrackPoint & point, const int pid) {
  MAUSPrimaryGeneratorAction::PGParticle particle;
  particle.time = point.t();
  particle.energy = point.E();
  particle.x = point.x();
  particle.px = point.Px();
  particle.y = point.y();
  particle.py = point.Py();
  particle.z = point.z();
  particle.pz = point.Pz();
  particle.pid = pid;
  particle.seed = 0;

  return particle;
}

std::ostream& operator<<(std::ostream& out, const TrackPoint& vector) {
  out << "t: " << vector[0]  << " E: "   << vector[1]
      << " x: " << vector[2]  << " Px: "  << vector[3]
      << " y: " << vector[4]  << " Py: "  << vector[5]
      << " z: " << vector.z() << " Pz: "  << vector.Pz();
  return out;
}

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS
