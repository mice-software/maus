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
      uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const PhaseSpaceType type)
    : PhaseSpaceVector(type),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(*original_instance.uncertainties_))
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance,
                       const double mass,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(original_instance, mass, type),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(*original_instance.uncertainties_))
{ }

TrackPoint::TrackPoint(const PhaseSpaceVector & original_instance,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(original_instance, type),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const Vector<double> & original_instance,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(original_instance, type),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix())
{ }

TrackPoint::TrackPoint(const double x1, const double p1,
                       const double x2, const double p2,
                       const double x3, const double p3,
                       const CovarianceMatrix & uncertainties,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(x1, p1, x2, p2, x3, p3, type),
      detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix(uncertainties)) { }

TrackPoint::TrackPoint(const double x1, const double p1,
                       const double x2, const double p2,
                       const double x3, const double p3,
                       const Detector & detector,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(x1, p1, x2, p2, x3, p3, type),
      detector_id_(detector.id()),
      uncertainties_(new CovarianceMatrix(detector.uncertainties())) { }

TrackPoint::TrackPoint(double const * const array,
                       const PhaseSpaceType type)
    : PhaseSpaceVector(array, type),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()) {
}

TrackPoint::~TrackPoint() {
  delete uncertainties_;
}

TrackPoint & TrackPoint::operator=(const TrackPoint& rhs) {
  PhaseSpaceVector::operator=(rhs);
  detector_id_ = rhs.detector_id_;

  delete uncertainties_;
  uncertainties_ = new CovarianceMatrix(*rhs.uncertainties_);

  return *this;
}

const bool TrackPoint::operator==(const TrackPoint& rhs) const {
  if ((*this) != ((PhaseSpaceVector) rhs)) {
    return false;
  } else if (rhs.detector_id_ != detector_id_) {
    fprintf(stdout, "Detector IDs are not equal!\n");
    return false;
  } else if ((*rhs.uncertainties_) != (*uncertainties_)) {
    fprintf(stdout, "CovarianceMatrices are not equal!\n");
    return false;
  }

  return true;
}

const bool TrackPoint::operator<(const TrackPoint& rhs) const {
  return t() < rhs.t();
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

MAUS::MAUSPrimaryGeneratorAction::PGParticle
PrimaryGeneratorParticle(const TrackPoint & point, const int pid) {
  const double mass = Particle::GetInstance()->GetMass(pid);
  TrackPoint const * temporal_point = NULL;
  TrackPoint const * positional_point = NULL;
  if (point.type() == PhaseSpaceVector::PhaseSpaceType::kTemporal) {
    temporal_point = &point;
    positional_point
      = new TrackPoint(point, mass,
                        PhaseSpaceVector::PhaseSpaceType::kPositional);
  } else {
    temporal_point 
      = new TrackPoint(point, mass,
                        PhaseSpaceVector::PhaseSpaceType::kTemporal);
    positional_point = &point;
  }

  MAUSPrimaryGeneratorAction::PGParticle particle;
  particle.time = temporal_point->t();
  particle.energy = temporal_point->E();
  particle.x = point.x();
  particle.px = point.Px();
  particle.y = point.y();
  particle.py = point.Py();
  particle.z = positional_point->z();
  particle.pz = positional_point->Pz();
  particle.pid = pid;
  particle.seed = 0;
  
  delete temporal_point;
  delete positional_point;

  return particle;
}

}  // namespace global
}  // namespace reconstruction
}  // namespace MAUS
