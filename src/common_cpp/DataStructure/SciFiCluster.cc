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
                              _spill(-1),
                              _event(-1),
                              _tracker(-1),
                              _station(-1),
                              _plane(-1),
                              _channel_w(-1),
                              _npe(-1),
                              _time(-1) {
  _digits = new TRefArray();
}

SciFiCluster::SciFiCluster(const SciFiCluster &clus): _used(clus.is_used()),
                                                      _spill(clus.get_spill()),
                                                      _event(clus.get_event()),
                                                      _tracker(clus.get_tracker()),
                                                      _station(clus.get_station()),
                                                      _plane(clus.get_plane()),
                                                      _id(clus.get_id()),
                                                      _channel_w(clus.get_channel()),
                                                      _npe(clus.get_npe()),
                                                      _time(clus.get_time()),
                                                      _alpha(clus.get_alpha()) {
  _direction = clus.get_direction();
  _position = clus.get_position();
  _true_position = clus.get_true_position();
  _true_momentum = clus.get_true_momentum();
  _digits = new TRefArray(*(clus.get_digits()));
}

SciFiCluster::SciFiCluster(SciFiDigit *scifidigit): _used(false),
                                                    _spill(scifidigit->get_spill()),
                                                    _event(scifidigit->get_event()),
                                                    _tracker(scifidigit->get_tracker()),
                                                    _station(scifidigit->get_station()),
                                                    _plane(scifidigit->get_plane()),
                                                    _channel_w(scifidigit->get_channel()),
                                                    _npe(scifidigit->get_npe()),
                                                    _time(scifidigit->get_time()) {
  scifidigit->set_used(true);
  _digits = new TRefArray();
  _digits->Add(scifidigit);
}

// Destructor
SciFiCluster::~SciFiCluster() {
  delete _digits;
}

// Assignment operator
SciFiCluster& SciFiCluster::operator=(const SciFiCluster &scificluster) {
  if (this == &scificluster) {
    return *this;
  }
  _used      = scificluster.is_used();
  _spill     = scificluster.get_spill();
  _event     = scificluster.get_event();
  _tracker   = scificluster.get_tracker();
  _station   = scificluster.get_station();
  _plane     = scificluster.get_plane();
  _id        = scificluster.get_id();
  _channel_w = scificluster.get_channel();
  _npe       = scificluster.get_npe();
  _time      = scificluster.get_time();
  _alpha     = scificluster.get_alpha();
  _direction = scificluster.get_direction();
  _position  = scificluster.get_position();
  _true_position = scificluster.get_true_position();
  _true_momentum = scificluster.get_true_momentum();
  if (_digits) delete _digits;
  _digits    = new TRefArray(*(scificluster.get_digits()));
  return *this;
}

// Merge digits
void SciFiCluster::add_digit(SciFiDigit* neigh) {
  neigh->set_used(true);
  _digits->Add(neigh);

  _npe += neigh->get_npe();
  _channel_w /= 2.0;
  _channel_w += (neigh->get_channel())/2.0;
  _time /= 2.0;
  _time += (neigh->get_time())/2.0;
}

SciFiDigitPArray SciFiCluster::get_digits_pointers() const {
  SciFiDigitPArray dig_pointers;

  // Check the _spoints container is not initialised
  if (!_digits) {
    std::cerr << "Digit TRefArray not initialised" << std::endl;
    return dig_pointers;
  }

  for (int i = 0; i < (_digits->GetLast()+1); ++i) {
    dig_pointers.push_back(static_cast<SciFiDigit*>(_digits->At(i)));
  }
  return dig_pointers;
}

void SciFiCluster::set_digits_pointers(const SciFiDigitPArray &digits) {
  if (_digits) delete _digits;
  _digits = new TRefArray();
  for (
    std::vector<SciFiDigit*>::const_iterator dig = digits.begin(); dig != digits.end(); ++dig) {
    _digits->Add(*dig);
  }
}

} // ~namespace MAUS
