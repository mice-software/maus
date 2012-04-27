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

// namespace MAUS {

// Constructors
SimpleHelix::SimpleHelix() {
  _Phi_0 = 0.0;
  _Phi_0_err = 0.0;
  _R = 0.0;
  _R_err = 0.0;
  _tan_lambda = 0.0;
  _tan_lambda_err = 0.0;
  _Psi_0 = 0.0;
  _Psi_0_err = 0.0;
  _chisq = 0.0;
  _chisq_dof = 0.0;
}

SimpleHelix::SimpleHelix(double Phi_0, double Phi_0_err, double R, double R_err, double tan_lambda,
                         double tan_lambda_err, double Psi_0, double Psi_0_err, double chisq,
                         double chisq_dof) {
  _Phi_0 = Phi_0;
  _Phi_0_err = Phi_0_err;
  _R = R;
  _R_err = R_err;
  _tan_lambda = tan_lambda;
  _tan_lambda_err = tan_lambda_err;
  _Psi_0 = Psi_0;
  _Psi_0_err = Psi_0_err;
  _chisq = chisq;
  _chisq_dof = chisq_dof;
}

// Destructor
SimpleHelix::~SimpleHelix() {}

void SimpleHelix::clear() {
  _Phi_0 = 0.0;
  _Phi_0_err = 0.0;
  _R = 0.0;
  _R_err = 0.0;
  _tan_lambda = 0.0;
  _tan_lambda_err = 0.0;
  _Psi_0 = 0.0;
  _Psi_0_err = 0.0;
  _chisq = 0.0;
  _chisq_dof = 0.0;
}

void SimpleHelix::set_parameters(double Phi_0, double Phi_0_err, double R, double R_err,
                                 double tan_lambda, double tan_lambda_err, double Psi_0,
                                 double Psi_0_err, double chisq, double chisq_dof) {
  _Phi_0 = Phi_0;
  _Phi_0_err = Phi_0_err;
  _R = R;
  _R_err = R_err;
  _tan_lambda = tan_lambda;
  _tan_lambda_err = tan_lambda_err;
  _Psi_0 = Psi_0;
  _Psi_0_err = Psi_0_err;
  _chisq = chisq;
  _chisq_dof = chisq_dof;
}
// } // ~namespace MAUS
