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

/** @class SciFiPRTrack
 *
 *
 */

#ifndef  SCIFIPRTRACK_HH
#define SCIFIPRTRACK_HH

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"

// namespace MAUS {

class SciFiPRTrack {
  public:
    // Constructors
    SciFiPRTrack();

    // Destructors
    ~SciFiPRTrack();  // Default destructor

    // Getters
    std::vector<SciFiSpacePoint> get_spacepoints() const { return _spoints; }

    int get_tracker() const { return _tracker; }
    int get_num_points() const { return _num_points; }

    // std::vector<double> get_vsl();

    // Setters
    void set_spacepoints(std::vector<SciFiSpacePoint> spoints) { _spoints = spoints; }

    void set_tracker(int tracker) { _tracker = tracker; }
    void set_num_points(int num_points) { _num_points = num_points; }
    // Print track parameters
    // void print_params();

  private:
    std::vector<SciFiSpacePoint> _spoints;

    std::vector<double> _vsl;

    int _tracker;
    int _num_points;
};
// } // ~namespace MAUS

#endif
