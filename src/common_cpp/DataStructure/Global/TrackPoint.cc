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

#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"

#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
TrackPoint::TrackPoint()
    : _mapper_name(""),
      _charge(0.),
      _position(0., 0., 0., 0.),
      _position_error(0., 0., 0., 0.),
      _momentum(0., 0., 0., 0.),
      _momentum_error(0., 0., 0., 0.),
      _detector(MAUS::DataStructure::Global::kUndefined),
      _geometry_path(""),
      _space_point(NULL) {}

// Copy contructor
TrackPoint::TrackPoint(const TrackPoint &track_point)
    : _mapper_name(track_point.get_mapper_name()),
      _charge(track_point.get_charge()),
      _position(track_point.get_position()),
      _position_error(track_point.get_position_error()),
      _momentum(track_point.get_momentum()),
      _momentum_error(track_point.get_momentum_error()),
      _detector(track_point.get_detector()),
      _geometry_path(track_point.get_geometry_path()),
      _space_point(track_point.get_space_point()) {}

// Destructor
TrackPoint::~TrackPoint() {}

// Assignment operator
TrackPoint& TrackPoint::operator=(const TrackPoint &track_point) {
  if (this == &track_point) {
    return *this;
  }
  _mapper_name     = track_point.get_mapper_name();
  _charge          = track_point.get_charge();
  _position        = track_point.get_position();
  _position_error  = track_point.get_position_error();
  _momentum        = track_point.get_momentum();
  _momentum_error  = track_point.get_momentum_error();
  _detector        = track_point.get_detector();
  _geometry_path   = track_point.get_geometry_path();
  _space_point      = track_point.get_space_point();

  return *this;
}

// Create a new TrackPoint, identical to the original, but separate.
TrackPoint* TrackPoint::Clone() {
  MAUS::DataStructure::Global::TrackPoint* tpNew =
      new MAUS::DataStructure::Global::TrackPoint();

  tpNew->set_mapper_name(this->get_mapper_name());
  tpNew->set_charge(this->get_charge());
  tpNew->set_position(this->get_position());
  tpNew->set_position_error(this->get_position_error());
  tpNew->set_momentum(this->get_momentum());
  tpNew->set_momentum_error(this->get_momentum_error());
  tpNew->set_detector(this->get_detector());
  tpNew->set_geometry_path(this->get_geometry_path());

  // ToDo (Ian Taylor - 29/10/12): Do we want to Clone the Spacepoint,
  // or copy it?  I am cloning for now, but I don't think we are
  // expecting to change the space_point (it is just for book-keeping),
  // so a copy is probably better.

  // Changed my mind, not cloning...  Need to think about this A LOT MORE!
  tpNew->set_space_point(this->get_space_point());

  return tpNew;
}

void TrackPoint::set_mapper_name(std::string mapper_name) {
  _mapper_name = mapper_name;
}

std::string TrackPoint::get_mapper_name() const {
  return _mapper_name;
}

void TrackPoint::set_charge(double charge) {
  _charge = charge;
}

double TrackPoint::get_charge() const {
  return _charge;
}

void TrackPoint::set_position(TLorentzVector position) {
  _position = position;
}

TLorentzVector TrackPoint::get_position() const {
  return _position;
}

void TrackPoint::set_position_error(TLorentzVector position_error) {
  _position_error = position_error;
}

TLorentzVector TrackPoint::get_position_error() const {
  return _position_error;
}

void TrackPoint::set_momentum(TLorentzVector momentum) {
  _momentum = momentum;
}

TLorentzVector TrackPoint::get_momentum() const {
  return _momentum;
}

void TrackPoint::set_momentum_error(TLorentzVector momentum_error) {
  _momentum_error = momentum_error;
}

TLorentzVector TrackPoint::get_momentum_error() const {
  return _momentum_error;
}

void TrackPoint::set_detector(DetectorPoint detector) {
  _detector = detector;
}

DetectorPoint TrackPoint::get_detector() const {
  return _detector;
}

void TrackPoint::set_geometry_path(std::string geometry_path) {
  _geometry_path = geometry_path;
}

std::string TrackPoint::get_geometry_path() const {
  return _geometry_path;
}

void TrackPoint::set_space_point_tref(TRef space_point) {
  _space_point = space_point;
}

TRef TrackPoint::get_space_point_tref() const {
  return _space_point;
}

void TrackPoint::set_space_point(MAUS::DataStructure::Global::SpacePoint* space_point) {
  _space_point = space_point;
}

MAUS::DataStructure::Global::SpacePoint* TrackPoint::get_space_point() const {
  MAUS::DataStructure::Global::SpacePoint* space_point =
      (MAUS::DataStructure::Global::SpacePoint*) _space_point.GetObject();
  return space_point;
}
} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
