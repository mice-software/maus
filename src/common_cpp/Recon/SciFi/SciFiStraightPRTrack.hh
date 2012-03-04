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

/** @class SciFiStraightPRTrack
 *
 *
 */

#ifndef  SCIFISTRAIGHTPRTRACK_HH
#define SCIFISTRAIGHTPRTRACK_HH

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"

// namespace MAUS {

class SciFiStraightPRTrack {
  public:
    // Constructors
    SciFiStraightPRTrack();
    SciFiStraightPRTrack(int tracker, double x0, double mx, double y0, double my);

    // Destructors
    ~SciFiStraightPRTrack();  // Default destructor

    // Getters
    std::vector<SciFiSpacePoint> get_spacepoints() const { return _spoints; }

    double get_x0() const { return _x0; }
    double get_y0() const { return _y0; }
    double get_mx() const { return _mx; }
    double get_my() const { return _my; }
    int get_tracker() const { return _tracker; }

    std::vector<double> get_vsl();

    // Setters
    void set_spacepoints(std::vector<SciFiSpacePoint> spoints) { _spoints = spoints; }

    void set_x0(double x0) { _x0 = x0; }
    void set_y0(double y0) { _y0 = y0; }
    void set_mx(double mx) { _mx = mx; }
    void set_my(double my) { _my = my; }

    void set_tracker(int tracker) { _tracker = tracker; }
    // Print track parameters
    void print_params();

  private:
    std::vector<SciFiSpacePoint> _spoints;

    std::vector<double> _vsl;

    int _tracker;
    double _x0;
    double _y0;
    double _mx;
    double _my;
};
// } // ~namespace MAUS

#endif
