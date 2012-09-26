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
#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace MAUS {

// Constructors
SciFiCluster::SciFiCluster(): _used(false),
                              _spill(0),
                              _event(0),
                              _tracker(0),
                              _station(0),
                              _plane(0),
                              _channel_w(0),
                              _npe(0.0),
                              _time(0.0) {
}

SciFiCluster::SciFiCluster(const SciFiCluster &_scificluster):_used(false),
                                                              _spill(0),
                                                              _event(0),
                                                              _tracker(0),
                                                              _station(0),
                                                              _plane(0),
                                                              _channel_w(0),
                                                              _npe(0.0),
                                                              _time(0.0) {
  *this = _scificluster;
}

SciFiCluster::SciFiCluster(SciFiDigit *_scifidigit):_used(false),
                                                    _spill(_scifidigit->get_spill()),
                                                    _event(_scifidigit->get_event()),
                                                    _tracker(_scifidigit->get_tracker()),
                                                    _station(_scifidigit->get_station()),
                                                    _plane(_scifidigit->get_plane()),
                                                    _channel_w(_scifidigit->get_channel()),
                                                    _npe(_scifidigit->get_npe()),
                                                    _time(_scifidigit->get_time()) {
  _scifidigit->set_used(true);
  _true_pos = _scifidigit->get_true_position();
  _true_p   = _scifidigit->get_true_momentum();
}

// Destructor
SciFiCluster::~SciFiCluster() {}

// Assignment operator
SciFiCluster& SciFiCluster::operator=(const SciFiCluster &_scificluster) {
  if (this == &_scificluster) {
    return *this;
  }
  _true_pos  = _scificluster.get_true_position();
  _true_p    = _scificluster.get_true_momentum();
  _used      = _scificluster.is_used();
  _spill     = _scificluster.get_spill();
  _event     = _scificluster.get_event();
  _tracker   = _scificluster.get_tracker();
  _station   = _scificluster.get_station();
  _plane     = _scificluster.get_plane();
  _channel_w = _scificluster.get_channel();
  _npe       = _scificluster.get_npe();
  _time      = _scificluster.get_time();
  _direction = _scificluster.get_direction();
  _position  = _scificluster.get_position();
  _relat_pos = _scificluster.get_relative_position();
  _alpha     = _scificluster.get_alpha();
  _id        = _scificluster.get_id();
  return *this;
}

// Merge digits
void SciFiCluster::add_digit(SciFiDigit* neigh) {
  neigh->set_used(true);

  _npe += neigh->get_npe();
  _channel_w /= 2.0;
  _channel_w += (neigh->get_channel())/2.0;
  _time /= 2.0;
  _time += (neigh->get_time())/2.0;
}

} // ~namespace MAUS
