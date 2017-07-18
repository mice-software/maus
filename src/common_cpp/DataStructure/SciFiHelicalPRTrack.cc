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
    _R(-1.0),
    _dsdz(-1.0),
    _line_sz_c(-1.0),
    _line_sz_chisq(-1.0),
    _line_sz_ndf(-1),
    _circle_x0(-1.0),
    _circle_y0(-1.0),
    _circle_chisq(-1.0),
    _circle_ndf(-1),
    _alg_used_circle(0),
    _alg_used_longitudinal(0),
    _alg_used_full(-1)  {
  // Do nothing
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(const SimpleHelix& helix, SciFiSpacePointPArray spoints)
  : SciFiBasePRTrack(),
    _tracker(-1),
    _R(helix.get_R()),
    _dsdz(helix.get_dsdz()),
    _line_sz_c(helix.get_s0()),
    _line_sz_chisq(helix.get_longitudinal_chisq()),
    _line_sz_ndf(helix.get_longitudinal_ndf()),
    _circle_x0(helix.get_xc()),
    _circle_y0(helix.get_yc()),
    _circle_chisq(helix.get_transverse_chisq()),
    _circle_ndf(helix.get_transverse_ndf()),
    _alg_used_circle(0),
    _alg_used_longitudinal(0),
    _alg_used_full(-1) {
  this->set_spacepoints_pointers(spoints);
  this->set_chi_squared(helix.get_chisq());
  this->set_ndf((2*this->get_num_points()) - 5); // TODO: Should this be fit number of points?
}


SciFiHelicalPRTrack::SciFiHelicalPRTrack(int tracker, int charge,
                                         SimpleCircle circle, SimpleLine line_sz,
                                         SciFiSpacePointPArray spoints,
                                         const DoubleArray& covariance) :
    SciFiBasePRTrack(charge, covariance, spoints),
    _tracker(tracker),
    _R(circle.get_R()),
    _dsdz(line_sz.get_m()),
    _line_sz_c(line_sz.get_c()),
    _line_sz_chisq(line_sz.get_chisq()),
    _line_sz_ndf(-1),
    _circle_x0(circle.get_x0()),
    _circle_y0(circle.get_y0()),
    _circle_chisq(circle.get_chisq()),
    _circle_ndf(-1),
    _alg_used_circle(0),
    _alg_used_longitudinal(0),
    _alg_used_full(-1) {
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
  _dsdz = htrk.get_dsdz();
  _line_sz_c = htrk.get_line_sz_c();
  _line_sz_chisq = htrk.get_line_sz_chisq();
  _line_sz_ndf = htrk.get_line_sz_ndf();
  _circle_chisq = htrk.get_circle_chisq();
  _circle_x0 = htrk.get_circle_x0();
  _circle_y0 = htrk.get_circle_y0();
  _circle_ndf = htrk.get_circle_ndf();
  _tracker = htrk.get_tracker();
  _alg_used_circle = htrk.get_alg_used_circle();
  _alg_used_longitudinal = htrk.get_alg_used_longitudinal();
  _alg_used_full = htrk.get_alg_used_full();
  return *this;
}

void SciFiHelicalPRTrack::form_total_chi_squared() {
  this->set_chi_squared(_circle_chisq + _line_sz_chisq);
}

SciFiHelicalPRTrack::SciFiHelicalPRTrack(const SciFiHelicalPRTrack &htrk)
  : SciFiBasePRTrack(htrk),
    _tracker(htrk.get_tracker()),
    _R(htrk.get_R()),
    _dsdz(htrk.get_dsdz()),
    _line_sz_c(htrk.get_line_sz_c()),
    _line_sz_chisq(htrk.get_line_sz_chisq()),
    _line_sz_ndf(htrk.get_line_sz_ndf()),
    _circle_x0(htrk.get_circle_x0()),
    _circle_y0(htrk.get_circle_y0()),
    _circle_chisq(htrk.get_circle_chisq()),
    _circle_ndf(htrk.get_circle_ndf()),
    _alg_used_circle(htrk.get_alg_used_circle()),
    _alg_used_longitudinal(htrk.get_alg_used_longitudinal()),
    _alg_used_full(htrk.get_alg_used_full()) {
}
} // ~namespace MAUS
