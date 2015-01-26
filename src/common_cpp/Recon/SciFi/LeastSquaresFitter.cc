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
#include "TMatrixD.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"

namespace LeastSquaresFitter {

void linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                const std::vector<double> &_y_err, MAUS::SimpleLine &line, TMatrixD& covariance) {

  // Set up the matrices
  int n_points = static_cast<int>(_x.size());  // Number of measurements
  TMatrixD A(n_points, 2);                     // Represents the functional form; rows, columns
  TMatrixD V_m(n_points, n_points);            // Covariance matrix of measurements
  TMatrixD Y(n_points, 1);                     // Measurements

  for ( int i = 0; i < static_cast<int>(_x.size()); ++i ) {
    A[i][0] = 1;
    A[i][1] = _x[i];
    V_m[i][i] = ( _y_err[i] * _y_err[i] );
    Y[i][0] = _y[i];
  }

  // Perform the inversions and multiplications which make up the least squares fit
  double* det = NULL;                   // To hold the determinant
  V_m.Invert(det);                      // Invert in place
  TMatrixD At(A);                       // Copy A to At
  At.T();                               // Transpose At (leaving A unchanged)
  TMatrixD V_p(At * V_m * A);           // The covariance matrix of the parameters of model (inv)
  V_p.Invert(det);                      // Invert in place
  covariance = V_p;
  TMatrixD P(V_p * At * V_m * Y);       // The least sqaures estimate of the parameters

  // Extract the fit parameters
  line.set_c(P[0][0]);
  line.set_m(P[1][0]);
  line.set_c_err(sqrt(V_p[0][0]));
  line.set_m_err(sqrt(V_p[1][1]));

  // Calculate the fit chisq
  TMatrixD C(Y - (A * P));
  TMatrixD Ct(C);
  Ct.T();
  TMatrixD result(Ct * V_m * C);
  line.set_chisq(result[0][0]);
  line.set_chisq_dof(result[0][0] / n_points);
} // ~linear_fit(...)

bool circle_fit(const double sd_1to4, const double sd_5, const double R_res_cut,
                const std::vector<MAUS::SciFiSpacePoint*> &spnts, MAUS::SimpleCircle &circle,
                TMatrixD& covariance) {

  // Set up the matrices
  int n_points = static_cast<int>(spnts.size()); // Number of measurements
  TMatrixD A(n_points, 3);                       // Represents the functional form; rows, columns
  TMatrixD V_m(n_points, n_points);              // Covariance matrix of measurements
  TMatrixD K(n_points, 1);                       // Vector of 1s, represents kappa in circle formula

  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
    // This part will change once I figure out proper errors
    double sd = -1.0;
    if ( spnts[i]->get_station() == 5 )
      sd = sd_5;
    else
      sd = sd_1to4;

    double x_i = spnts[i]->get_position().x();
    double y_i = spnts[i]->get_position().y();

    A[i][0] = ( x_i * x_i ) + ( y_i * y_i );
    A[i][1] = x_i;
    A[i][2] = y_i;

    V_m[i][i] = ( sd * sd );
    K[i][0] = 1.;
  }

  // Perform the inversions and multiplications which make up the least squares fit
  double* det = NULL;              // To hold the determinant after inversions
  V_m.Invert(det);                 // Invert the measurement covariance matrix in place
  TMatrixD At(A);                  // Create a copy of A
  At.T();                          // Transpose At (leaving A unchanged)
  TMatrixD V_p(At * V_m * A);      // The covariance matrix of the parameters of model (inv)
  V_p.Invert(det);                 // Invert in place
  TMatrixD P(V_p * At * V_m * K);  // The least sqaures estimate of the parameters

  // Extract the fit parameters
  double alpha, beta, gamma;
  alpha = P[0][0];
  beta = P[1][0];
  gamma = P[2][0];

  // Convert the linear parameters into the circle center and radius
  double x0, y0, R;
  x0 = (-1*beta) / (2 * alpha);
  y0 = (-1*gamma) / (2 * alpha);
  if ( ((4 * alpha) + (beta * beta) + (gamma * gamma)) < 0 )
    R = 0;
  else
    R = sqrt((4 * alpha) + (beta * beta) + (gamma * gamma)) / (2 * alpha);

  // Transform the covariance matrix to the same basis
  TMatrixD jacobian( 3, 3 );
  jacobian(0,0) = beta / (2.0*alpha*alpha);
  jacobian(0,1) = -1.0 / (2.0*alpha);
  jacobian(1,0) = gamma / (2.0*alpha*alpha);
  jacobian(1,2) = -1.0 / (2.0*alpha);
  jacobian(2,0) = ( -1.0/(2.0*alpha) ) * ( ( (beta*beta + gamma*gamma) / (2.0*alpha) ) + 1 ) /
                                                sqrt( ( (beta*beta + gamma*gamma) / 4.0 ) + alpha );
  jacobian(2,1) = ( beta/(4.0*alpha*alpha) ) /
                            sqrt( ( (beta*beta + gamma*gamma)/(4.0*alpha*alpha) ) + ( 1.0/alpha ) );
  jacobian(2,2) = ( gamma/(4.0*alpha*alpha) ) /
                            sqrt( ( (beta*beta + gamma*gamma)/(4.0*alpha*alpha) ) + ( 1.0/alpha ) );
  TMatrixD jacobianT(3,3);
  jacobianT.Transpose( jacobian );

  covariance = jacobian * V_p * jacobianT;

  // if ( R < 0. )
  //  std::cout << "R was < 0 but taking abs_val for physical correctness\n";
  R = fabs(R);

  if (R > R_res_cut) return false; // Cannot be larger than 150mm or the track is not contained

  circle.set_x0(x0);
  circle.set_y0(y0);
  circle.set_R(R);
  circle.set_alpha(alpha);
  circle.set_beta(beta);
  circle.set_gamma(gamma);

  // Calculate the fit chisq
  TMatrixD C(K - (A * P));
  TMatrixD Ct(C);
  Ct.T();
  TMatrixD result(Ct * V_m * C);
  double chi2 = result[0][0];
  circle.set_chisq(chi2);       // Left unreduced (not dividing by NDF)
  return true;
} // ~circle_fit(...)

} // ~namespace MAUS
