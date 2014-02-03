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

// C headers
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Units/PhysicalConstants.h>

// MAUS headers
#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"

namespace MAUS {

LeastSquaresFitter::LeastSquaresFitter(double sd_1to4, double sd_5, double R_res_cut) {
  _sd_1to4 = sd_1to4;
  _sd_5 = sd_5;
  _R_res_cut = R_res_cut;
}

LeastSquaresFitter::~LeastSquaresFitter() {}

bool LeastSquaresFitter::LoadGlobals() {
  if (!Globals::HasInstance()) {
    Json::Value *json = Globals::GetConfigurationCards();
    _sd_1to4 = (*json)["SciFi_sigma_triplet"].asDouble();
    _sd_5 = (*json)["SciFi_sigma_tracker0_station5"].asDouble();
    _R_res_cut = (*json)["SciFiRadiusResCut"].asDouble();
    return true;
  } else {
    return false;
  }
}

void LeastSquaresFitter::linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                        const std::vector<double> &_y_err, SimpleLine &line) {

  int n_points = static_cast<int>(_x.size());

  CLHEP::HepMatrix A(n_points, 2); // rows, columns
  CLHEP::HepMatrix V(n_points, n_points); // error matrix
  CLHEP::HepMatrix Y(n_points, 1); // measurements

  for ( int i = 0; i < static_cast<int>(_x.size()); ++i ) {
    // std::cout <<"( " << _x[i] << "," << _y[i] << " )" << std::endl;
    A[i][0] = 1;
    A[i][1] = _x[i];
    V[i][i] = ( _y_err[i] * _y_err[i] );
    Y[i][0] = _y[i];
  }

  CLHEP::HepMatrix At, tmpy, yparams;

  int ierr;
  V.invert(ierr);
  At = A.T();

  tmpy = At * V * A;
  tmpy.invert(ierr);
  yparams = tmpy * At * V * Y;

  line.set_c(yparams[0][0]);
  line.set_m(yparams[1][0]);
  line.set_c_err(sqrt(tmpy[0][0]));
  line.set_m_err(sqrt(tmpy[1][1]));

  CLHEP::HepMatrix C, result;

  C = Y - (A * yparams);
  result = C.T() * V * C;
  line.set_chisq(result[0][0]);
  line.set_chisq_dof(result[0][0] / n_points);
} // ~linear_fit(...)

bool LeastSquaresFitter::circle_fit(const std::vector<SciFiSpacePoint*> &spnts,
                                    SimpleCircle &circle) {

  int n_points = static_cast<int>(spnts.size());
  CLHEP::HepMatrix A(n_points, 3); // rows, columns
  CLHEP::HepMatrix V(n_points, n_points); // error matrix
  CLHEP::HepMatrix K(n_points, 1);

  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    // This part will change once I figure out proper errors
    double sd = -1.0;
    if ( spnts[i]->get_station() == 5 )
      sd = _sd_5;
    else
      sd = _sd_1to4;

    double x_i = spnts[i]->get_position().x();
    double y_i = spnts[i]->get_position().y();

    A[i][0] = ( x_i * x_i ) + ( y_i * y_i );
    A[i][1] = x_i;
    A[i][2] = y_i;

    V[i][i] = ( sd * sd );
    K[i][0] = 1.;
  }

  CLHEP::HepMatrix At, tmpx, tmp_params;
  int ierr;
  V.invert(ierr);
  At = A.T();
  tmpx = At * V * A;
  tmpx.invert(ierr);
  tmp_params = tmpx * At * V * K;

  // These values will be used for delta_R calculation
  double alpha, beta, gamma;
  alpha = tmp_params[0][0];
  beta = tmp_params[1][0];
  gamma = tmp_params[2][0];

  // Convert the linear parameters into the circle center and radius
  double x0, y0, R;
  x0 = (-1*beta) / (2 * alpha);
  y0 = (-1*gamma) / (2 * alpha);
  if ( ((4 * alpha) + (beta * beta) + (gamma * gamma)) < 0 )
    R = 0;
  else
    R = sqrt((4 * alpha) + (beta * beta) + (gamma * gamma)) / (2 * alpha);

  // if ( R < 0. )
  //  std::cout << "R was < 0 but taking abs_val for physical correctness\n";
  R = fabs(R);

  if (R > _R_res_cut) return false; // Cannot be larger than 150mm or the track is not contained

  circle.set_x0(x0);
  circle.set_y0(y0);
  circle.set_R(R);
  circle.set_alpha(alpha);
  circle.set_beta(beta);
  circle.set_gamma(gamma);

  CLHEP::HepMatrix C, result;

  C = K - (A * tmp_params);
  result = C.T() * V * C;
  double chi2 = result[0][0];
  circle.set_chisq(chi2); // should I leave this un-reduced?
  return true;
} // ~circle_fit(...)

} // ~namespace MAUS
