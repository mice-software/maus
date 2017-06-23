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

namespace MAUS {

// Constructor
SimpleCircle::SimpleCircle() {
  _x0 = 0.0;
  _x0_err = 0.0;
  _y0 = 0.0;
  _y0_err = 0.0;
  _R = 0.0;
  _R_err = 0.0;
  _alpha = 0.0;
  _alpha_err = 0.0;
  _beta = 0.0;
  _beta_err = 0.0;
  _gamma = 0.0;
  _gamma_err = 0.0;
  _kappa = 0.0;
  _kappa_err = 0.0;
  _delta_R = 0.0;
  _chisq = 0.0;
  _pvalue = 0.0;
}

SimpleCircle::SimpleCircle(double x0, double y0, double R) {
  _x0 = x0;
  _x0_err = 0.0;
  _y0 = y0;
  _y0_err = 0.0;
  _R = R;
  _R_err = 0.0;
  _alpha = 0.0;
  _alpha_err = 0.0;
  _beta = 0.0;
  _beta_err = 0.0;
  _gamma = 0.0;
  _gamma_err = 0.0;
  _kappa = 0.0;
  _kappa_err = 0.0;
  _delta_R = 0.0;
  _chisq = 0.0;
  _pvalue = 0.0;
}

SimpleCircle::SimpleCircle(double x0, double x0_err, double y0, double y0_err, double R,
                           double R_err, double alpha, double alpha_err, double beta,
                           double beta_err, double gamma, double gamma_err, double kappa,
                           double kappa_err, double delta_R, double chisq, double pvalue) {
  _x0 = x0;
  _x0_err = x0_err;
  _y0 = y0;
  _y0_err = y0_err;
  _R = R;
  _R_err = R_err;
  _alpha = alpha;
  _alpha_err = alpha_err;
  _beta = beta;
  _beta_err = beta_err;
  _gamma = gamma;
  _gamma_err = gamma_err;
  _kappa = kappa;
  _kappa_err = kappa_err;
  _delta_R = delta_R;
  _chisq = chisq;
  _pvalue = pvalue;
}

// Destructor
SimpleCircle::~SimpleCircle() {}

void SimpleCircle::clear() {
  _x0 = 0.0;
  _x0_err = 0.0;
  _y0 = 0.0;
  _y0_err = 0.0;
  _R = 0.0;
  _R_err = 0.0;
  _alpha = 0.0;
  _alpha_err = 0.0;
  _beta = 0.0;
  _beta_err = 0.0;
  _gamma = 0.0;
  _gamma_err = 0.0;
  _kappa = 0.0;
  _kappa_err = 0.0;
  _delta_R = 0.0;
  _chisq = 0.0;
  _pvalue = 0.0;
}

void SimpleCircle::set_parameters(double x0, double x0_err, double y0, double y0_err,
                                  double R, double R_err, double alpha, double alpha_err,
                                  double beta, double beta_err, double gamma, double gamma_err,
                                  double kappa, double kappa_err, double delta_R,
                                  double chisq, double pvalue) {
  _x0 = x0;
  _x0_err = x0_err;
  _y0 = y0;
  _y0_err = y0_err;
  _R = R;
  _R_err = R_err;
  _alpha = alpha;
  _alpha_err = alpha_err;
  _beta = beta;
  _beta_err = beta_err;
  _gamma = gamma;
  _gamma_err = gamma_err;
  _kappa = kappa;
  _kappa_err = kappa_err;
  _delta_R = delta_R;
  _chisq = chisq;
  _pvalue = pvalue;
}

} // ~namespace MAUS
