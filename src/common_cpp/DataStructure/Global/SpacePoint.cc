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

#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
SpacePoint::SpacePoint()
    : _charge(0.),
      _position(0., 0., 0., 0.),
      _position_error(0., 0., 0., 0.),
      _detector(MAUS::DataStructure::Global::kUndefined),
      _geometry_path("") {}

// Copy contructor
SpacePoint::SpacePoint(const SpacePoint &space_point)
    : _charge(space_point.get_charge()),
      _position(space_point.get_position()),
      _position_error(space_point.get_position_error()),
      _detector(space_point.get_detector()),
      _geometry_path(space_point.get_geometry_path()) {}

// Destructor
SpacePoint::~SpacePoint() {}

// Assignment operator
SpacePoint& SpacePoint::operator=(const SpacePoint &space_point) {
  if (this == &space_point) {
    return *this;
  }
  _charge          = space_point.get_charge();
  _position        = space_point.get_position();
  _position_error  = space_point.get_position_error();
  _detector        = space_point.get_detector();
  _geometry_path   = space_point.get_geometry_path();

  return *this;
}

// Copy the member variables of 'this' into a new SpacePoint
SpacePoint* SpacePoint::Clone() const {

  // As SpacePoint has no ownership of other objects, we could do all
  // of this in the constructor, and save CPU time.  But, I prefer to
  // keep the methods similar between SpacePoint, TrackPoint and
  // Track.
  MAUS::DataStructure::Global::SpacePoint* spNew =
      new MAUS::DataStructure::Global::SpacePoint();

  spNew->set_charge(this->get_charge());
  spNew->set_position(this->get_position());
  spNew->set_position_error(this->get_position_error());
  spNew->set_detector(this->get_detector());
  spNew->set_geometry_path(this->get_geometry_path());

  return spNew;
}

void SpacePoint::set_charge(double charge) {
  _charge = charge;
}

double SpacePoint::get_charge() const {
  return _charge;
}

void SpacePoint::set_position(TLorentzVector position) {
  _position = position;
}

TLorentzVector SpacePoint::get_position() const {
  return _position;
}

void SpacePoint::set_position_error(TLorentzVector position_error) {
  _position_error = position_error;
}

TLorentzVector SpacePoint::get_position_error() const {
  return _position_error;
}

void SpacePoint::set_detector(DetectorPoint detector) {
  _detector = detector;
}

DetectorPoint SpacePoint::get_detector() const {
  return _detector;
}

void SpacePoint::set_geometry_path(std::string geometry_path) {
  _geometry_path = geometry_path;
}

std::string SpacePoint::get_geometry_path() const {
  return _geometry_path;
}

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
