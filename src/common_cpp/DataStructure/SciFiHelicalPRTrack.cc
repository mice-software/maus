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
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

namespace MAUS {

// Constructors
SciFiHelicalPRTrack::SciFiHelicalPRTrack() {
  _x0 = -1.0;
  _y0 = -1.0;
  _z0 = -1.0;
  _phi0 = -1.0;
  _psi0 = -1.0;
  _dsdz = -1.0;
  _R = -1.0;
  _tracker = -1;
  _num_points = -1;
}


SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int num_points, double x0, double y0,
                                         double z0, double phi0, double psi0, double dsdz,
                                         double R, double chisq) {
  _x0 = x0;
  _y0 = y0;
  _z0 = z0;
  _phi0 = phi0;
  _psi0 = psi0;
  _dsdz = dsdz;
  _R = R;
  _tracker = tracker;
  _num_points = num_points;
  _chisq = chisq;
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0,
                                           SimpleCircle circle, SimpleLine line_sz) {

  _tracker = tracker;
  _num_points = num_points;
  _x0 = pos0.x();
  _y0 = pos0.y();
  _z0 = pos0.z();
  _phi0 = circle.get_turning_angle()[0];
  _R    = circle.get_R();
  _psi0 = -1.0;
  _dsdz = line_sz.get_m();
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int num_points, CLHEP::Hep3Vector pos0,
                                         SimpleHelix helix) {
  _tracker = tracker;
  _num_points = num_points;
  _x0        = pos0.x();
  _y0        = pos0.y();
  _z0        = pos0.z();
  _R         = helix.get_R();
  _phi0      = helix.get_Phi_0();
  _dsdz      = helix.get_dsdz();
  _chisq     = helix.get_chisq();
  _chisq_dof = helix.get_chisq_dof(); // already reduced chisq
}

// Destructor
SciFiHelicalPRTrack::~SciFiHelicalPRTrack() {}

// Assignment operator
SciFiHelicalPRTrack &SciFiHelicalPRTrack::operator=(const SciFiHelicalPRTrack &_htrk) {
    if (this == &_htrk) {
        return *this;
    }
    _x0 = _htrk.get_x0();
    _y0 = _htrk.get_y0();
    _z0 = _htrk.get_z0();
    _R = _htrk.get_R();
    _phi0 = _htrk.get_phi0();
    _psi0 = _htrk.get_psi0();
    _dsdz = _htrk.get_dsdz();
    _chisq = _htrk.get_chisq();
    _num_points = _htrk.get_num_points();
    _tracker = _htrk.get_tracker();
    _spoints = _htrk.get_spacepoints();
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(const SciFiHelicalPRTrack &_htrk)
  :  _x0(-1.0), _y0(-1.0), _z0(-1.0), _phi0(-1.0), _psi0(-1.0), _dsdz(-1.0), _R(-1.0),
     _tracker(-1), _num_points(-1), _chisq(-1.0), _spoints(0) {
  *this = _htrk;
}

void SciFiHelicalPRTrack::print_params() {
  std::cout << " x0 is " << _x0 << std::endl;
  std::cout << " y0 is " << _y0 << std::endl;
  std::cout << " z0 is " << _z0 << std::endl;
  std::cout << " phi0 is " << _phi0 << std::endl;
  std::cout << " psi0 is " << _psi0 << std::endl;
  std::cout << " dz/ds is " << _dsdz << std::endl;
  std::cout << " R is " << _R << std::endl;
}

} // ~namespace MAUS
