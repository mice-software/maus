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
    : BasePoint(),
      _mapper_name(""), _charge(0.) {}

// Copy contructor
SpacePoint::SpacePoint(const SpacePoint &space_point)
    : BasePoint(space_point),
      _mapper_name(space_point.get_mapper_name()), _charge(space_point.get_charge()) {}

// Destructor
SpacePoint::~SpacePoint() {}

// Assignment operator
SpacePoint& SpacePoint::operator=(const SpacePoint &space_point) {
  if (this == &space_point) {
    return *this;
  }
  BasePoint::operator=(space_point);

  _mapper_name     = space_point.get_mapper_name();
  _charge          = space_point.get_charge();

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

  // Clone the BasePoint elements
  this->BasePoint::Clone(spNew);

  spNew->set_mapper_name(this->get_mapper_name());
  spNew->set_charge(this->get_charge());

  return spNew;
}

void SpacePoint::set_charge(double charge) {
  _charge = charge;
}

double SpacePoint::get_charge() const {
  return _charge;
}

void SpacePoint::set_mapper_name(std::string mapper_name) {
  _mapper_name = mapper_name;
}

std::string SpacePoint::get_mapper_name() const {
  return _mapper_name;
}

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
