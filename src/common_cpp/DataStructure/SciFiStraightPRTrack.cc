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
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

namespace MAUS {

// Constructors
SciFiStraightPRTrack::SciFiStraightPRTrack() : _x0(-1.0), _mx(-1.0), _x_chisq(-1.0),
                                               _y0(-1.0), _my(-1.0), _y_chisq(-1.0),
                                               _tracker(-1), _num_points(-1), _spoints(0) {
}

SciFiStraightPRTrack::SciFiStraightPRTrack(int tracker, int num_points,
                                           double x0, double mx, double x_chisq,
                                           double y0, double my, double y_chisq)
                                          : _x0(-1.0), _mx(-1.0), _x_chisq(-1.0),
                                            _y0(-1.0), _my(-1.0), _y_chisq(-1.0),
                                            _tracker(-1), _num_points(-1), _spoints(0) {
  _x0 = x0;
  _mx = mx;
  _x_chisq = x_chisq;
  _y0 = y0;
  _my = my;
  _y_chisq = y_chisq;
  _tracker = tracker;
  _num_points = num_points;
}

SciFiStraightPRTrack::SciFiStraightPRTrack(int tracker, int num_points,
                                           SimpleLine line_x, SimpleLine line_y)
                                          : _x0(-1.0), _mx(-1.0), _x_chisq(-1.0),
                                            _y0(-1.0), _my(-1.0), _y_chisq(-1.0),
                                            _tracker(-1), _num_points(-1), _spoints(0) {

  _x0 = line_x.get_c();
  _mx = line_x.get_m();
  _x_chisq = line_x.get_chisq();

  _y0 = line_y.get_c();
  _my = line_y.get_m();
  _y_chisq = line_y.get_chisq();

  _tracker = tracker;
  _num_points = num_points;
}

SciFiStraightPRTrack::SciFiStraightPRTrack(const SciFiStraightPRTrack &_strk)
                                          : _x0(-1.0), _mx(-1.0), _x_chisq(-1.0),
                                            _y0(-1.0), _my(-1.0), _y_chisq(-1.0),
                                            _tracker(-1), _num_points(-1), _spoints(0) {
  *this = _strk;
}

// Destructor
SciFiStraightPRTrack::~SciFiStraightPRTrack() {}

// Assignment operator
SciFiStraightPRTrack &SciFiStraightPRTrack::operator=(const SciFiStraightPRTrack &_strk) {
    if (this == &_strk) {
        return *this;
    }
    _x0 = _strk.get_x0();
    _mx = _strk.get_mx();
    _x_chisq = _strk.get_x_chisq();
    _y0 = _strk.get_y0();
    _my = _strk.get_my();
    _y_chisq = _strk.get_y_chisq();
    _tracker = _strk.get_tracker();
    _num_points = _strk.get_num_points();
    _spoints = _strk.get_spacepoints();
    return *this;
}

} // ~namespace MAUS
