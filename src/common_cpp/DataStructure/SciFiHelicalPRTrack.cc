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

#include <cmath>

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

namespace MAUS {

// Constructors
SciFiHelicalPRTrack::SciFiHelicalPRTrack()
  : SciFiBasePRTrack(),
    _tracker(-1),
    _charge(0),
    _R(-1.0),
    _phi0(-1.0),
    _dsdz(-1.0),
    _line_sz_c(-1.0),
    _line_sz_chisq(-1.0),
    _circle_x0(-1.0),
    _circle_y0(-1.0),
    _circle_chisq(-1.0),
    _point_spread(-1.0),
    _pos0(-1.0, -1.0, -1.0),
    _phi(0) {
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int charge, ThreeVector pos0,
                                         double phi0, SimpleCircle circle, SimpleLine line_sz,
                                         double point_spread, DoubleArray phi,
                                         SciFiSpacePointPArray spoints,
                                         const DoubleArray& covariance) :
    SciFiBasePRTrack(covariance, spoints),
    _tracker(tracker),
    _charge(charge),
    _R(circle.get_R()),
    _phi0(phi0),
    _dsdz(line_sz.get_m()),
    _line_sz_c(line_sz.get_c()),
    _line_sz_chisq(line_sz.get_chisq()),
    _circle_x0(circle.get_x0()),
    _circle_y0(circle.get_y0()),
    _circle_chisq(circle.get_chisq()),
    _point_spread(point_spread),
    _pos0(pos0),
    _phi(phi) {
  this->set_chi_squared(_circle_chisq + _line_sz_chisq);
  this->set_ndf((2*this->get_num_points()) - 5);
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
  SciFiBasePRTrack::operator=(htrk);

  _R = htrk.get_R();
  _phi0 = htrk.get_phi0();
  _dsdz = htrk.get_dsdz();
  _line_sz_c = htrk.get_line_sz_c();
  _line_sz_chisq = htrk.get_line_sz_chisq();
  _circle_chisq = htrk.get_circle_chisq();
  _circle_x0 = htrk.get_circle_x0();
  _circle_y0 = htrk.get_circle_y0();
  _charge = htrk.get_charge();
  _tracker = htrk.get_tracker();
  _point_spread = htrk.get_point_spread();
  _pos0 = htrk.get_pos0();
  _phi = htrk.get_phi();
  return *this;
}

void SciFiHelicalPRTrack::form_total_chi_squared() {
  this->set_chi_squared(_circle_chisq + _line_sz_chisq);
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(const SciFiHelicalPRTrack &htrk)
  : SciFiBasePRTrack(htrk),
    _tracker(htrk.get_tracker()),
    _charge(htrk.get_charge()),
    _R(htrk.get_R()),
    _phi0(htrk.get_phi0()),
    _dsdz(htrk.get_dsdz()),
    _line_sz_c(htrk.get_line_sz_c()),
    _line_sz_chisq(htrk.get_line_sz_chisq()),
    _circle_x0(htrk.get_circle_x0()),
    _circle_y0(htrk.get_circle_y0()),
    _circle_chisq(htrk.get_circle_chisq()),
    _point_spread(htrk.get_point_spread()),
    _pos0(htrk.get_pos0()),
    _phi(htrk.get_phi()) {
}
} // ~namespace MAUS
