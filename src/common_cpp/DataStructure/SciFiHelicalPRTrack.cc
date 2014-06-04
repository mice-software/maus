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
SciFiHelicalPRTrack::SciFiHelicalPRTrack() :  _tracker(-1),
                                              _num_points(-1),
                                              _charge(0),
                                              _R(-1.0),
                                              _phi0(-1.0),
                                              _psi0(-1.0),
                                              _dsdz(-1.0),
                                              _line_sz_c(-1.0),
                                              _line_sz_chisq(-1.0),
                                              _circle_x0(-1.0),
                                              _circle_y0(-1.0),
                                              _circle_chisq(-1.0),
                                              _chisq(-1.0),
                                              _chisq_dof(-1.0),
                                              _pos0(-1.0, -1.0, -1.0),
                                              _phi(0) {
  _spoints = new TRefArray();
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int num_points, int charge, ThreeVector pos0,
                                         double phi0, double psi0, SimpleCircle circle,
                                         SimpleLine line_sz, double chisq, double chisq_dof,
                                         DoubleArray phi, SciFiSpacePointPArray spoints) {
  _tracker = tracker;
  _num_points = num_points;
  _charge = charge;
  _R  = circle.get_R();
  _phi0 = phi0;
  _psi0 = psi0;
  _dsdz = line_sz.get_m();
  _line_sz_c = line_sz.get_c();
  _line_sz_chisq = line_sz.get_chisq();
  _circle_x0 = circle.get_x0();
  _circle_y0 = circle.get_y0();
  _circle_chisq = circle.get_chisq();
  _chisq = chisq;
  _chisq_dof = chisq_dof;
  _pos0 = pos0;
  _phi = phi;
}

// Destructor
SciFiHelicalPRTrack::~SciFiHelicalPRTrack() {
  // Do nothing
}

// Assignment operator
SciFiHelicalPRTrack &SciFiHelicalPRTrack::operator=(const SciFiHelicalPRTrack &htrk) {
  if (this == &htrk) {
      return *this;
  }
  _R = htrk.get_R();
  _phi0 = htrk.get_phi0();
  _psi0 = htrk.get_psi0();
  _dsdz = htrk.get_dsdz();
  _line_sz_c = htrk.get_line_sz_c();
  _line_sz_chisq = htrk.get_line_sz_chisq();
  _circle_chisq = htrk.get_circle_chisq();
  _circle_x0 = htrk.get_circle_x0();
  _circle_y0 = htrk.get_circle_y0();
  _chisq = htrk.get_chisq();
  _chisq_dof = htrk.get_chisq_dof();
  _num_points = htrk.get_num_points();
  _charge = htrk.get_charge();
  _tracker = htrk.get_tracker();
  _pos0 = htrk.get_pos0();
  _spoints = new TRefArray(*(htrk.get_spacepoints()));
  _phi = htrk.get_phi();
  return *this;
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(const SciFiHelicalPRTrack &htrk)
  : _tracker(htrk.get_tracker()),
    _num_points(htrk.get_num_points()),
    _charge(htrk.get_charge()),
    _R(htrk.get_R()),
    _phi0(htrk.get_phi0()),
    _psi0(htrk.get_psi0()),
    _dsdz(htrk.get_dsdz()),
    _line_sz_c(htrk.get_line_sz_c()),
    _line_sz_chisq(htrk.get_line_sz_chisq()),
    _circle_x0(htrk.get_circle_x0()),
    _circle_y0(htrk.get_circle_y0()),
    _circle_chisq(htrk.get_circle_chisq()),
    _chisq(htrk.get_chisq()),
    _chisq_dof(htrk.get_chisq_dof()),
    _pos0(htrk.get_pos0()),
    _phi(htrk.get_phi()) {
  _spoints = new TRefArray(*htrk.get_spacepoints());
}


} // ~namespace MAUS
