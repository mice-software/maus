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
#include "src/common_cpp/Recon/SciFi/SciFiPRTrack.hh"

// namespace MAUS {

// Constructors
SciFiPRTrack::SciFiPRTrack() {

  _tracker = -1;
  _num_points = -1;

  // _vsl.resize(0);
}

SciFiPRTrack::SciFiPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0, SimpleHelix helix) {
  // This is a helical constructor
  _x0        = pos0.x();
  _y0        = pos0.y();
  _z0        = pos0.z();
  _R         = helix.get_R();
  _phi0      = helix.get_Phi_0();
  _dzds      = helix.get_tan_lambda();
  _chisq     = helix.get_chisq();
  _chisq_dof = helix.get_chisq_dof(); // already reduced chisq

  _isHelix = true;

  _num_points = num_points;
  _tracker = tracker;

  _vsl.resize(6);
  _vsl[0] = _x0;
  _vsl[1] = _y0;
  _vsl[2] = _z0;
  _vsl[3] = _R;
  _vsl[4] = _phi0;
  _vsl[5] = _dzds;
}

SciFiPRTrack::SciFiPRTrack(int tracker, int num_points, double x0, double mx, double x_chisq,
                           double y0, double my, double y_chisq) {
  // Linear constructor
  _x0 = x0;
  _mx = mx;
  _x_chisq = x_chisq;
  _y0 = y0;
  _my = my;
  _y_chisq = y_chisq;

  _num_points = num_points;
  _tracker = tracker;

  _isHelix = false;

  _vsl.resize(4);
  _vsl[0] = _x0;
  _vsl[1] = _mx;
  _vsl[2] = _y0;
  _vsl[3] = _my;
}

SciFiPRTrack::SciFiPRTrack(int tracker, int num_points, SimpleLine line_x, SimpleLine line_y) {
  // Linear constructor
  _x0 = line_x.get_c();
  _mx = line_x.get_m();
  _x_chisq = line_x.get_chisq();

  _y0 = line_y.get_c();
  _my = line_y.get_m();
  _y_chisq = line_y.get_chisq();

  _num_points = num_points;
  _tracker = tracker;

  _isHelix = false;

  _vsl.resize(4);
  _vsl[0] = _x0;
  _vsl[1] = _mx;
  _vsl[2] = _y0;
  _vsl[3] = _my;
}

// Destructor
SciFiPRTrack::~SciFiPRTrack() {}

std::vector<double> SciFiPRTrack::get_vsl() {
  if ( _isHelix ) {
    _vsl[0] = _x0;
    _vsl[1] = _y0;
    _vsl[2] = _z0;
    _vsl[3] = _phi0;
    _vsl[4] = _psi0;
    _vsl[5] = _dzds;
    _vsl[6] = _R;
    return _vsl;
  } else if ( !_isHelix ) {
    _vsl[0] = _x0;
    _vsl[1] = _mx;
    _vsl[2] = _y0;
    _vsl[3] = _my;
    return _vsl;
  }
}

void SciFiPRTrack::print_params() {
  if ( _isHelix ) {
    std::cout << " x0 is " << _x0 << std::endl;
    std::cout << " y0 is " << _y0 << std::endl;
    std::cout << " z0 is " << _z0 << std::endl;
    std::cout << " phi0 is " << _phi0 << std::endl;
    std::cout << " psi0 is " << _psi0 << std::endl;
    std::cout << " dz/ds is " << _dzds << std::endl;
    std::cout << " R is " << _R << std::endl;
  } else if ( !_isHelix ) {
    std::cout << " x0 is " << _x0 << std::endl;
    std::cout << " mx is " << _mx << std::endl;
    std::cout << " x_chisq is " << _x_chisq << std::endl;
    std::cout << " y0 is " << _y0 << std::endl;
    std::cout << " my is " << _my << std::endl;
    std::cout << " y_chisq is " << _y_chisq << std::endl;
  }
}
// } // ~namespace MAUS
