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

// C++ headers
#include <vector>

// ROOT headers
#include "TMatrixD.h"
#include "TRefArray.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

namespace MAUS {

// Constructors
  SciFiStraightPRTrack::SciFiStraightPRTrack()
  : SciFiBasePRTrack(), _tracker(-1),
    _x0(-1.0), _mx(-1.0), _x_chisq(-1.0),
    _y0(-1.0), _my(-1.0), _y_chisq(-1.0),
    _global_x0(-1.0), _global_mx(-1.0),
    _global_y0(-1.0), _global_my(-1.0) {
    // Do nothing
  }

  SciFiStraightPRTrack::SciFiStraightPRTrack(int tracker, double x0, double mx, double x_chisq,
                                             double y0, double my, double y_chisq,
                                             const DoubleArray& covariance)
    : SciFiBasePRTrack(covariance),
      _tracker(tracker),
      _x0(x0),
      _mx(mx),
      _x_chisq(x_chisq),
      _y0(y0),
      _my(my),
      _y_chisq(y_chisq),
      _global_x0(-1.0),
      _global_mx(-1.0),
      _global_y0(-1.0),
      _global_my(-1.0) {
    this->set_chi_squared(_x_chisq + _y_chisq);
    this->set_ndf((2.0*this->get_num_points()) - 4);
  }

  SciFiStraightPRTrack::SciFiStraightPRTrack(int tracker, SimpleLine line_x, SimpleLine line_y,
                                             const DoubleArray& covariance)
    : SciFiBasePRTrack(covariance),
      _tracker(tracker),
      _x0(line_x.get_c()),
      _mx(line_x.get_m()),
      _x_chisq(line_x.get_chisq()),
      _y0(line_y.get_c()),
      _my(line_y.get_m()),
      _y_chisq(line_y.get_chisq()),
      _global_x0(-1.0),
      _global_mx(-1.0),
      _global_y0(-1.0),
      _global_my(-1.0) {
    this->set_chi_squared(_x_chisq + _y_chisq);
    this->set_ndf((2.0*this->get_num_points()) - 4);
  }

  SciFiStraightPRTrack::SciFiStraightPRTrack(const SciFiStraightPRTrack &strk)
    : SciFiBasePRTrack(strk),
      _tracker(strk.get_tracker()),
      _x0(strk.get_x0()),
      _mx(strk.get_mx()),
      _x_chisq(strk.get_x_chisq()),
      _y0(strk.get_y0()),
      _my(strk.get_my()),
      _y_chisq(strk.get_y_chisq()),
      _global_x0(strk.get_global_x0()),
      _global_mx(strk.get_global_mx()),
      _global_y0(strk.get_global_y0()),
      _global_my(strk.get_global_my()) {
    this->set_chi_squared(_x_chisq + _y_chisq);
    this->set_ndf((2.0*this->get_num_points()) - 4);
  }

  // Destructor
  SciFiStraightPRTrack::~SciFiStraightPRTrack() {}

  // Assignment operator
  SciFiStraightPRTrack &SciFiStraightPRTrack::operator=(const SciFiStraightPRTrack &strk) {
      if (this == &strk) {
          return *this;
      }
      SciFiBasePRTrack::operator=(strk);

      _tracker = strk.get_tracker();
      _x0 = strk.get_x0();
      _mx = strk.get_mx();
      _x_chisq = strk.get_x_chisq();
      _y0 = strk.get_y0();
      _my = strk.get_my();
      _y_chisq = strk.get_y_chisq();
      _global_x0 = strk.get_global_x0();
      _global_mx = strk.get_global_mx();
      _global_y0 = strk.get_global_y0();
      _global_my = strk.get_global_my();

      return *this;
  }

  void SciFiStraightPRTrack::form_total_chi_squared() {
    this->set_chi_squared(_x_chisq + _y_chisq);
  }

} // ~namespace MAUS
