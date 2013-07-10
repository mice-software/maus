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

#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"

namespace MAUS {

// Constructors
SimpleLine::SimpleLine() {
  _c = 0.0;
  _c_err = 0.0;
  _m = 0.0;
  _m_err = 0.0;
  _chisq = 0.0;
  _chisq_dof = 0.0;
}

SimpleLine::SimpleLine(double c, double m) {
  _c = c;
  _c_err = 0.0;
  _m = m;
  _m_err = 0.0;
  _chisq = 0.0;
  _chisq_dof = 0.0;
}

SimpleLine::SimpleLine(double c, double c_err, double m, double m_err,
                       double chisq, double chisq_dof) {
  _c = c;
  _c_err = c_err;
  _m = m;
  _m_err = m_err;
  _chisq = chisq;
  _chisq_dof = chisq_dof;
}

// Destructor
SimpleLine::~SimpleLine() {}

void SimpleLine::clear() {
  _c = 0.0;
  _c_err = 0.0;
  _m = 0.0;
  _m_err = 0.0;
  _chisq = 0.0;
  _chisq_dof = 0.0;
}

void SimpleLine::set_parameters(double c, double c_err, double m, double m_err,
                                double chisq, double chisq_dof) {
  _c = c;
  _c_err = c_err;
  _m = m;
  _m_err = m_err;
  _chisq = chisq;
  _chisq_dof = chisq_dof;
}
} // ~namespace MAUS
