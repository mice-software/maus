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
#include "src/common_cpp/Recon/SciFi/SciFiHelicalPRTrack.hh"

// namespace MAUS {

// Constructors
SciFiHelicalPRTrack::SciFiHelicalPRTrack() {
  _x0 = -1.0;
  _y0 = -1.0;
  _z0 = -1.0;
  _phi0 = -1.0;
  _psi0 = -1.0;
  _dzds = -1.0;
  _R = -1.0;
  _tracker = -1;
  // _station = -1;

  _vsl.resize(7);
  for ( int i = 0; i < static_cast<int>(_vsl.size()); ++i ) {
    _vsl[i] = -1.0;
  }
}


SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, double x0, double y0,
                                         double z0, double phi0, double psi0,
                                         double dzds, double R) {
  _x0 = x0;
  _y0 = y0;
  _z0 = z0;
  _phi0 = phi0;
  _psi0 = psi0;
  _dzds = dzds;
  _R = R;
  _tracker = tracker;
  // _station = station;

  _vsl.resize(7);
  _vsl[0] = _x0;
  _vsl[1] = _y0;
  _vsl[2] = _z0;
  _vsl[3] = _phi0;
  _vsl[4] = _psi0;
  _vsl[5] = _dzds;
  _vsl[6] = _R;
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0,
                                         SimpleHelix helix) {

  _x0        = pos0.x();
  _y0        = pos0.y();
  _z0        = pos0.z();
  _R         = helix.get_R();
  _phi0      = helix.get_Phi_0();
  _dzds      = helix.get_tan_lambda();
  _chisq     = helix.get_chisq();
  _chisq_dof = helix.get_chisq_dof(); // already reduced chisq

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

// Destructor
SciFiHelicalPRTrack::~SciFiHelicalPRTrack() {}

std::vector<double> SciFiHelicalPRTrack::get_vsl() {
  _vsl[0] = _x0;
  _vsl[1] = _y0;
  _vsl[2] = _z0;
  _vsl[3] = _phi0;
  _vsl[4] = _psi0;
  _vsl[5] = _dzds;
  _vsl[6] = _R;
  return _vsl;
}

void SciFiHelicalPRTrack::print_params() {
  std::cout << " x0 is " << _x0 << std::endl;
  std::cout << " y0 is " << _y0 << std::endl;
  std::cout << " z0 is " << _z0 << std::endl;
  std::cout << " phi0 is " << _phi0 << std::endl;
  std::cout << " psi0 is " << _psi0 << std::endl;
  std::cout << " dz/ds is " << _dzds << std::endl;
  std::cout << " R is " << _R << std::endl;
}
// } // ~namespace MAUS
