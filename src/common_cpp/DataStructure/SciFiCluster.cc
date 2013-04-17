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
                              _time(0.0),
                              _digits(0) {
}

SciFiCluster::SciFiCluster(const SciFiCluster &scificluster): _used(false),
                                                              _spill(0),
                                                              _event(0),
                                                              _tracker(0),
                                                              _station(0),
                                                              _plane(0),
                                                              _channel_w(0),
                                                              _npe(0.0),
                                                              _time(0.0),
                                                              _digits(0) {
  *this = scificluster;
}

SciFiCluster::SciFiCluster(SciFiDigit *scifidigit): _used(false),
                                                    _spill(scifidigit->get_spill()),
                                                    _event(scifidigit->get_event()),
                                                    _tracker(scifidigit->get_tracker()),
                                                    _station(scifidigit->get_station()),
                                                    _plane(scifidigit->get_plane()),
                                                    _channel_w(scifidigit->get_channel()),
                                                    _npe(scifidigit->get_npe()),
                                                    _time(scifidigit->get_time()),
                                                    _digits(0) {
  scifidigit->set_used(true);
  _digits.push_back(scifidigit);
  _true_pos = scifidigit->get_true_position();
  _true_p   = scifidigit->get_true_momentum();
}

// Destructor
SciFiCluster::~SciFiCluster() {}

// Assignment operator
SciFiCluster& SciFiCluster::operator=(const SciFiCluster &scificluster) {
  if (this == &scificluster) {
    return *this;
  }
  _true_pos  = scificluster.get_true_position();
  _true_p    = scificluster.get_true_momentum();
  _used      = scificluster.is_used();
  _spill     = scificluster.get_spill();
  _event     = scificluster.get_event();
  _tracker   = scificluster.get_tracker();
  _station   = scificluster.get_station();
  _plane     = scificluster.get_plane();
  _channel_w = scificluster.get_channel();
  _npe       = scificluster.get_npe();
  _time      = scificluster.get_time();
  _direction = scificluster.get_direction();
  _position  = scificluster.get_position();
  _alpha     = scificluster.get_alpha();
  _id        = scificluster.get_id();
  _digits    = scificluster.get_digits();
  return *this;
}

// Merge digits
void SciFiCluster::add_digit(SciFiDigit* neigh) {
  neigh->set_used(true);
  _digits.push_back(neigh);

  _npe += neigh->get_npe();
  _channel_w /= 2.0;
  _channel_w += (neigh->get_channel())/2.0;
  _time /= 2.0;
  _time += (neigh->get_time())/2.0;
}

} // ~namespace MAUS
