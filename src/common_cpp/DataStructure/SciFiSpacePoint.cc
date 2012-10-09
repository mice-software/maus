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

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"

namespace MAUS {

// Default constructor
SciFiSpacePoint::SciFiSpacePoint(): _used(false),
                                    _spill(0),
                                    _event(0),
                                    _tracker(0),
                                    _station(0),
                                    _time(0.0),
                                    _npe(0.0),
                                    _chi2(0.0),
                                    _type(""),
                                    _position(0, 0, 0) {
}

// Copy contructor
SciFiSpacePoint::SciFiSpacePoint(const SciFiSpacePoint &_scifispacepoint):_used(false),
                                                                          _spill(0),
                                                                          _event(0),
                                                                          _tracker(0),
                                                                          _station(0),
                                                                          _time(0),
                                                                          _npe(0.0),
                                                                          _chi2(0.0),
                                                                          _type(""),
                                                                          _position(0, 0, 0) {
  _used      = _scifispacepoint.is_used();
  _spill     = _scifispacepoint.get_spill();
  _event     = _scifispacepoint.get_event();
  _tracker   = _scifispacepoint.get_tracker();
  _station   = _scifispacepoint.get_station();
  _time      = _scifispacepoint.get_time();
  _npe       = _scifispacepoint.get_npe();
  _chi2      = _scifispacepoint.get_chi2();
  _type      = _scifispacepoint.get_type();
  _position  = _scifispacepoint.get_position();

  _channels.resize(_scifispacepoint._channels.size());
  for (unsigned int i = 0; i < _scifispacepoint._channels.size(); ++i) {
    _channels[i] = new SciFiCluster(*_scifispacepoint._channels[i]);
  }

  *this = _scifispacepoint;
}

// Three cluster constructor
SciFiSpacePoint::SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2, SciFiCluster *clust3)
                : _time(0.0), _chi2(0.0), _position(0, 0, 0) {
  _used = false;
  _type = "triplet";
  clust1->set_used(true);
  clust2->set_used(true);
  clust3->set_used(true);
  _channels.push_back(clust1);
  _channels.push_back(clust2);
  _channels.push_back(clust3);

  _spill   = clust1->get_spill();
  _event   = clust1->get_event();
  _npe = clust1->get_npe()+clust2->get_npe()+clust3->get_npe();
  _tracker = clust1->get_tracker();
  _station = clust1->get_station();

  // _time_error = 0;
  // _time_res   = 0;
}

// Two cluster constructor
SciFiSpacePoint::SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2)
                : _time(0.0), _chi2(0.0), _position(0, 0, 0) {

  _used = false;
  _type = "duplet";
  clust1->set_used(true);
  clust2->set_used(true);
  _channels.push_back(clust1);
  _channels.push_back(clust2);

  _spill   = clust1->get_spill();
  _event   = clust1->get_event();
  _tracker = clust1->get_tracker();
  _station = clust1->get_station();
  _npe = clust1->get_npe()+clust2->get_npe();

  // _time_error = 0;
  // _time_res   = 0;
}

// Destructor
SciFiSpacePoint::~SciFiSpacePoint() {}

// Assignment operator
SciFiSpacePoint& SciFiSpacePoint::operator=(const SciFiSpacePoint &_scifispacepoint) {
  if (this == &_scifispacepoint) {
    return *this;
  }
  _used      = _scifispacepoint.is_used();
  _spill     = _scifispacepoint.get_spill();
  _event     = _scifispacepoint.get_event();
  _tracker   = _scifispacepoint.get_tracker();
  _station   = _scifispacepoint.get_station();
  _time      = _scifispacepoint.get_time();
  _npe       = _scifispacepoint.get_npe();
  _chi2      = _scifispacepoint.get_chi2();
  _type      = _scifispacepoint.get_type();
  _position  = _scifispacepoint.get_position();

  _channels.resize(_scifispacepoint._channels.size());
  for (unsigned int i = 0; i < _scifispacepoint._channels.size(); ++i) {
    _channels[i] = new SciFiCluster(*_scifispacepoint._channels[i]);
  }

  return *this;
}

} // ~namespace MAUS
