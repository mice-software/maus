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

#include "src/common_cpp/Recon/SingleStation/SESpacePoint.hh"

// namespace MAUS {

SESpacePoint::SESpacePoint() { _used = false; }

SESpacePoint::~SESpacePoint() {}

SESpacePoint::SESpacePoint(SECluster *clust1, SECluster *clust2, SECluster *clust3) {
  _used = false;
  _type = "triplet";
  clust1->set_used();
  clust2->set_used();
  clust3->set_used();
  _channels.push_back(clust1);
  _channels.push_back(clust2);
  _channels.push_back(clust3);

  _spill = clust1->get_spill();
  _event = clust1->get_event();

  _npe = clust1->get_npe()+clust2->get_npe()+clust3->get_npe();

  _chi2 = 0;
  // _position(0, 0, 0);
  // _time = 0;
  // _time_error = 0;
  // _time_res   = 0;
}

SESpacePoint::SESpacePoint(SECluster *clust1, SECluster *clust2) {
  _used = false;
  _type = "duplet";
  _spill = clust1->get_spill();
  _event = clust1->get_event();
  clust1->set_used();
  clust2->set_used();
  _channels.push_back(clust1);
  _channels.push_back(clust2);

  _npe = clust1->get_npe()+clust2->get_npe();
  _chi2 = -10.0;

  // _position(0, 0, 0);
  // _time = 0;
  // _time_error = 0;
  // _time_res   = 0;
}
// } // ~namespace MAUS
