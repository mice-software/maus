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

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerData.hh"


namespace MAUS {

TrackerData::TrackerData() {
  _spill_num = 0;
  _trker_num = 0;
  _num_events = 0;
  _num_digits = 0;
  _num_clusters = 0;
  _num_spoints = 0;
  _num_seeds = 0;
  _num_stracks_5pt = 0;
  _num_stracks_4pt = 0;
  _num_stracks_3pt = 0;
  _num_htracks_5pt = 0;
  _num_htracks_4pt = 0;
  _num_htracks_3pt = 0;
  _num_pos_tracks = 0;
  _num_neg_tracks = 0;
};

TrackerData::~TrackerData() {
  // Do nothing
};

void TrackerData::clear() {
  _spill_num = 0;
  _trker_num = 0;
  _num_events = 0;
  _num_digits = 0;
  _num_clusters = 0;
  _num_spoints = 0;
  _num_seeds = 0;
  _num_stracks_5pt = 0;
  _num_stracks_4pt = 0;
  _num_stracks_3pt = 0;
  _num_htracks_5pt = 0;
  _num_htracks_4pt = 0;
  _num_htracks_3pt = 0;
  _num_pos_tracks = 0;
  _num_neg_tracks = 0;
  _spoints_x.clear();
  _spoints_x.resize(0);
  _spoints_y.clear();
  _spoints_y.resize(0);
  _spoints_z.clear();
  _spoints_z.resize(0);
  _seeds_z.clear();
  _seeds_z.resize(0);
  _seeds_phi.clear();
  _seeds_phi.resize(0);
  _seeds_s.clear();
  _seeds_s.resize(0);
  _trks_str_xz.clear();
  _trks_str_xz.resize(0);
  _trks_str_yz.clear();
  _trks_str_yz.resize(0);
  _trks_xy.clear();
  _trks_xy.resize(0);
  _trks_xz.clear();
  _trks_xz.resize(0);
  _trks_yz.clear();
  _trks_yz.resize(0);
  _trks_sz.clear();
  _trks_sz.resize(0);
};

} // ~namespace MAUS
