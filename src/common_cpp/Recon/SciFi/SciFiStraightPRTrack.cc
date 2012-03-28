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

#include <vector>
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"

// namespace MAUS {

// Constructors
SciFiStraightPRTrack::SciFiStraightPRTrack() {
  _x0 = -1.0;
  _mx = -1.0;
  _y0 = -1.0;
  _my = -1.0;
  _x_chisq = -1.0;
  _y_chisq = -1.0;

  _tracker = -1;
  _num_points = -1;

  _vsl.resize(4);
  for ( int i = 0; i < static_cast<int>(_vsl.size()); ++i ) {
    _vsl[i] = -1.0;
  }
}


SciFiStraightPRTrack::SciFiStraightPRTrack(int tracker, int num_points,
                                           double x0, double mx, double x_chisq,
                                           double y0, double my, double y_chisq) {
  _x0 = x0;
  _mx = mx;
  _x_chisq = x_chisq;
  _y0 = y0;
  _my = my;
  _y_chisq = y_chisq;

  _num_points = num_points;
  _tracker = tracker;

  _vsl.resize(4);
  _vsl[0] = _x0;
  _vsl[1] = _mx;
  _vsl[2] = _y0;
  _vsl[3] = _my;
}
// Destructor
SciFiStraightPRTrack::~SciFiStraightPRTrack() {}

std::vector<double> SciFiStraightPRTrack::get_vsl() {
  _vsl[0] = _x0;
  _vsl[1] = _mx;
  _vsl[2] = _y0;
  _vsl[3] = _my;
  return _vsl;
}

void SciFiStraightPRTrack::print_params() {
  std::cout << " x0 is " << _x0 << std::endl;
  std::cout << " mx is " << _mx << std::endl;
  std::cout << " x_chisq is " << _x_chisq << std::endl;
  std::cout << " y0 is " << _y0 << std::endl;
  std::cout << " my is " << _my << std::endl;
  std::cout << " y_chisq is " << _y_chisq << std::endl;
}
// } // ~namespace MAUS
