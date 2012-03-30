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

#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

// namespace MAUS {

// Constructor
SimpleCircle::SimpleCircle() {}

// Destructor
SimpleCircle::~SimpleCircle() {}

void SimpleCircle::set_parameters(double x0, double x0_err, double y0, double y0_err
                                  double R, double R_err, double chisq) {
  _x0 = x0;
  _x0_err = x0_err;
  _y0 = y0;
  _y0_err = y0_err;
  _R = R;
  _R_err = R_err;

  _chisq = chisq;
}
// } // ~namespace MAUS
