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

#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

namespace MAUS {

// Constructors
SimpleHelix::SimpleHelix() : _xc(0.0),
                             _xc_err(0.0),
                             _yc(0.0),
                             _yc_err(0.0),
                             _R(0.0),
                             _R_err(0.0),
                             _dsdz(0.0),
                             _dsdz_err(0.0),
                             _s0(0.0),
                             _s0_err(0.0),
                             _transverse_chisq(-1.0),
                             _longitudinal_chisq(-1.0),
                             _chisq(-1.0),
                             _chisq_dof(-1.0),
                             _pvalue(-1.0) {
  _cov.ResizeTo(4, 4);
}

SimpleHelix::SimpleHelix(double xc, double xc_err, double yc, double yc_err, double R, double R_err,
                         double dsdz, double dsdz_err, double s0, double s0_err,
                         double transverse_chisq, double longitudinal_chisq,
                         double chisq, double chisq_dof, double pvalue, TMatrixD cov)
    : _xc(xc),
      _xc_err(xc_err),
      _yc(yc),
      _yc_err(yc_err),
      _R(R),
      _R_err(R_err),
      _dsdz(dsdz),
      _dsdz_err(dsdz_err),
      _s0(s0),
      _s0_err(s0_err),
      _transverse_chisq(transverse_chisq),
      _longitudinal_chisq(longitudinal_chisq),
      _chisq(chisq),
      _chisq_dof(chisq_dof),
      _pvalue(pvalue),
      _cov(cov) {
  // Do nothing
}

// Destructor
SimpleHelix::~SimpleHelix() {}

void SimpleHelix::clear() {
  _xc = 0.0;
  _xc_err = 0.0;
  _yc = 0.0;
  _yc_err = 0.0;
  _R = 0.0;
  _R_err = 0.0;
  _dsdz = 0.0;
  _dsdz_err = 0.0;
  _s0 = 0.0;
  _s0_err = 0.0;
  _transverse_chisq = -1.0;
  _longitudinal_chisq = -1.0;
  _chisq = -1.0;
  _chisq_dof = -1.0;
  _pvalue = -1.0;
  _cov = TMatrixD(4, 4);
}

void SimpleHelix::set_parameters(double xc, double xc_err, double yc, double yc_err,
                                 double R, double R_err, double dsdz, double dsdz_err,
                                 double s0, double s0_err, double transverse_chisq,
                                 double longitudinal_chisq, double chisq, double chisq_dof,
                                 double pvalue, TMatrixD& cov) {
  _xc = xc;
  _xc_err = xc_err;
  _yc = yc;
  _yc_err = yc_err;
  _R = R;
  _R_err = R_err;
  _dsdz = dsdz;
  _dsdz_err = dsdz_err;
  _s0 = s0;
  _s0_err = s0_err;
  _transverse_chisq = transverse_chisq;
  _longitudinal_chisq = longitudinal_chisq;
  _chisq = chisq;
  _chisq_dof = chisq_dof;
  _pvalue = pvalue;
  _cov = cov;
}

} // ~namespace MAUS
