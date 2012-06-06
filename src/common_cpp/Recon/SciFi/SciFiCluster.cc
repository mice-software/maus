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
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"

// namespace MAUS {

SciFiCluster::SciFiCluster(): _used(false) {}

SciFiCluster::SciFiCluster(SciFiDigit *digit):_used(false),
                                              _spill(digit->get_spill()),
                                              _event(digit->get_event()),
                                              _tracker(digit->get_tracker()),
                                              _station(digit->get_station()),
                                              _plane(digit->get_plane()),
                                              _channel_w(digit->get_channel()),
                                              _npe(digit->get_npe()),
                                              _time(digit->get_time()) {
  digit->set_used();
}

SciFiCluster::~SciFiCluster() {}

void SciFiCluster::add_digit(SciFiDigit* neigh) {
  neigh->set_used();

  _npe += neigh->get_npe();
  _channel_w /= 2.0;
  _channel_w += (neigh->get_channel())/2.0;
}
// } // ~namespace MAUS
