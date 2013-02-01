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

#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"

#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"

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
      _spacepoint(NULL) {}

// Copy contructor
TrackPoint::TrackPoint(const TrackPoint &trackpoint)
    : _mapper_name(trackpoint.get_mapper_name()),
      _charge(trackpoint.get_charge()),
      _position(trackpoint.get_position()),
      _position_error(trackpoint.get_position_error()),
      _momentum(trackpoint.get_momentum()),
      _momentum_error(trackpoint.get_momentum_error()),
      _detector(trackpoint.get_detector()),
      _geometry_path(trackpoint.get_geometry_path()),
      _spacepoint(trackpoint.get_spacepoint()) {}

// Destructor
TrackPoint::~TrackPoint() {}

// Assignment operator
TrackPoint& TrackPoint::operator=(const TrackPoint &trackpoint) {
  if (this == &trackpoint) {
    return *this;
  }
  _mapper_name     = trackpoint.get_mapper_name();
  _charge          = trackpoint.get_charge();
  _position        = trackpoint.get_position();
  _position_error  = trackpoint.get_position_error();
  _momentum        = trackpoint.get_momentum();
  _momentum_error  = trackpoint.get_momentum_error();
  _detector        = trackpoint.get_detector();
  _geometry_path   = trackpoint.get_geometry_path();
  _spacepoint      = trackpoint.get_spacepoint();

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
  // expecting to change the spacepoint (it is just for book-keeping),
  // so a copy is probably better.

  // Changed my mind, not cloning...  Need to think about this A LOT MORE!
  tpNew->set_spacepoint(this->get_spacepoint());

  return tpNew;
}

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
