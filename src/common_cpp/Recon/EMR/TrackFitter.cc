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

#include "src/common_cpp/Recon/EMR/TrackFitter.hh"

namespace TrackFitter {

void polynomial_fit(EMRSpacePointArray spacePointArray,
		    MAUS::EMRTrack& track,
		    unsigned int n) {

  // Fill the arrays of spacepoints to fit
  std::vector<double> zx, zy, x, y, ex, ey;
  for (size_t iSP = 0; iSP < spacePointArray.size(); iSP++) {
    int xOri = (spacePointArray[iSP]->GetChannel()/60)%2;
    if ( !xOri ) {
      zx.push_back(spacePointArray[iSP]->GetPosition().z());
      x.push_back(spacePointArray[iSP]->GetPosition().x());
      ex.push_back(spacePointArray[iSP]->GetPositionErrors().x());
    } else {
      zy.push_back(spacePointArray[iSP]->GetPosition().z());
      y.push_back(spacePointArray[iSP]->GetPosition().y());
      ey.push_back(spacePointArray[iSP]->GetPositionErrors().y());
    }
  }

  // Fit the track in each projection with an order n polynomial
  std::vector<double> ax, ay; 		// Vector of parameters in the two projections
  std::vector<double> eax, eay; 	// Vector of parameters error in the two projections
  double chi2x(-1.), chi2y(-1.);	// Normalised chi squared in the two projections

  if ( !x.size() ) {
    for (size_t p = 0; p < n+1; p++)
        ax.push_back(0.);
    for (size_t p = 0; p < 2*(n+1); p++)
        eax.push_back(0.);
    chi2x = 0.;
  } else if ( x.size() == 1 ) {
    ax.push_back(x[0]);
    eax.push_back(ex[0]*ex[0]);
    for (size_t p = 1; p < n+1; p++)
        ax.push_back(0.);
    for (size_t p = 1; p < 2*(n+1); p++)
        eax.push_back(0.);
    chi2x = 0.;
  } else {
//      theil_sen_fit(zx, x, ex, ax, eax, chi2x);
      polynomial_fit_2D(zx, x, ex, n, ax, eax, chi2x);
  }

  if ( !y.size() ) {
    for (size_t p = 0; p < n+1; p++)
        ay.push_back(0.);
    for (size_t p = 0; p < 2*(n+1); p++)
        eay.push_back(0.);
    chi2y = 0.;
  } else if ( y.size() == 1 ) {
    ay.push_back(y[0]);
    eay.push_back(ey[0]*ey[0]);
    for (size_t p = 1; p < n+1; p++)
        ay.push_back(0.);
    for (size_t p = 1; p < 2*(n+1); p++)
        eay.push_back(0.);
    chi2y = 0.;
  } else {
//      theil_sen_fit(zy, y, ey, ay, eay, chi2y);
      polynomial_fit_2D(zy, y, ey, n, ay, eay, chi2y);
  }

  // Set the track parameters
  track.SetParametersX(ax);
  track.SetParametersErrorsX(eax);
  track.SetParametersY(ay);
  track.SetParametersErrorsY(eay);
  track.SetChi2(chi2x/x.size() + chi2y/y.size());
}

void polynomial_fit_2D(std::vector<double> x,
		       std::vector<double> y,
		       std::vector<double> ey,
		       unsigned int n,
		       std::vector<double>& a,
		       std::vector<double>& ea,
		       double& chi2) {

  // Set up the matrices
  size_t N = x.size();	// Number of measurements
  TMatrixD A(N, n+1);	// Represents the powers of x: (x_i^0,x_i^1,...,x_i^n)
  TMatrixD V_m(N, N);	// Covariance matrix of measurements
  TMatrixD Y(N, 1);	// Measurements

  for (size_t i = 0; i < N; ++i) {
    for (size_t p = 0; p < n+1; p++)
      A[i][p] = pow(x[i], p);

    V_m[i][i] = (ey[i] * ey[i]);
    Y[i][0] = y[i];
  }

  // Perform the inversions and multiplications which make up the least squares fit
  double* det = NULL;                   // To hold the determinant
  V_m.Invert(det);                      // Invert in place
  TMatrixD At(A);                       // Copy A to At
  At.T();                               // Transpose At (leaving A unchanged)
  TMatrixD V_p(At * V_m * A);           // The covariance matrix of the parameters of model (inv)
  V_p.Invert(det);                      // Invert in place
  TMatrixD P(V_p * At * V_m * Y);	// The least sqaures estimate of the parameters

  // Extract the fit parameters and their covariance matrix
  for (size_t p = 0; p < n+1; p++) {
    a.push_back(P[p][0]);
    for (size_t q = 0; q < n+1; q++)
      ea.push_back(V_p[p][q]);
  }

  // Calculate the fit chisq
  TMatrixD C(Y - (A * P));		// Residual vector
  TMatrixD Ct(C);			// Copy C to Ct
  Ct.T();				// Transpose Ct (leaving C unchanged)
  TMatrixD C2(Ct * V_m * C);		// Total chi squared
  chi2 = C2[0][0];
}

void theil_sen_fit(std::vector<double> x,
		   std::vector<double> y,
		   std::vector<double> ey,
		   std::vector<double>& a,
		   std::vector<double>& ea,
		   double& chi2) {

  // The gradient of the slope is the median of the distribution of gradients
  // of the lines joining all the possible combinations of points
  std::vector<double> gradient;
  double gmean(0.), g2mean(0.);
  for (size_t i = 0; i < x.size(); i++) {
    for (size_t j = 0; j < x.size(); j++) {
      if ( !(x[j]-x[i]) )
	continue;
      gradient.push_back((y[j]-y[i])/(x[j]-x[i]));
      gmean += (y[j]-y[i])/(x[j]-x[i]);
      g2mean += pow((y[j]-y[i])/(x[j]-x[i]), 2);
    }
  }
  if ( !gradient.size() ) { // TODO handle it properly (infinite gradient)
    a.push_back(0);
    a.push_back(0);
    ea.push_back(0);
    ea.push_back(0);
    chi2 = 0;
    return;
  }

  std::sort(gradient.begin(), gradient.end());
  double m = gradient.at(gradient.size()/2);

  // The y intecept of the slope is the median of the distribution of
  // y intercepts assuming m as a gradient for all points: y_i-m*x_i
  std::vector<double> intercept;
  double imean(0.), i2mean(0.);
  for (size_t i = 0; i < x.size(); i++) {
    for (size_t j = 0; j < x.size(); j++) {
      if ( i == j )
	continue;
      intercept.push_back(y[i] - m*x[i]);
      imean += y[i] - m*x[i];
      i2mean += pow(y[i] - m*x[i], 2);
    }
  }

  std::sort(intercept.begin(), intercept.end());
  double b = intercept.at(intercept.size()/2);

  // The y intercept is the first parameter, the gradient the second
  a.push_back(b);
  a.push_back(m);

  // Error propagation (uses the mean here, no such thing for the median)
  ea.push_back(sqrt(i2mean/intercept.size()-pow(imean/intercept.size(), 2)));
  ea.push_back(sqrt(g2mean/gradient.size()-pow(gmean/gradient.size(), 2)));

  // Chi squared calculation
  size_t N = x.size();	// Number of measurements
  TMatrixD A(N, 2);	// Represents the powers of x: (x_i^0, x_i^1)
  TMatrixD V_m(N, N);	// Covariance matrix of measurements
  TMatrixD Y(N, 1);	// Measurements
  TMatrixD P(2, 1);	// parameters

  for (size_t i = 0; i < N; ++i) {
    for (size_t p = 0; p < 2; p++)
      A[i][p] = pow(x[i], p);

    V_m[i][i] = (ey[i] * ey[i]);
    Y[i][0] = y[i];
  }

  for (size_t p = 0; p < 2; p++)
    P[p][0] = a[p];

  TMatrixD C(Y - (A * P));		// Residual vector
  TMatrixD Ct(C);			// Copy C to Ct
  Ct.T();				// Transpose Ct (leaving C unchanged)
  TMatrixD C2(Ct * V_m * C);		// Total chi squared
  chi2 = C2[0][0];
}

/*void polynomial_fit_2D(double* x, double* y,
			 double* ey, unsigned int n) {

  // Set up the necessary averages
  std::vector<double> xp;	// pth weighted moment of x, up to the 2*nth
  std::vector<double> xpy; 	// weighted average of x^p*y, up to x^n*y
  for (size_t p = 0; p < n; p++) {
    xp.push_back(0.);
    xp.push_back(0.);
    xpy.push_back(0.);
    for (size_t i = 0; i < x.size(); i++) {
      xp[p] += pow(x[i], p)/pow(ey[i], 2);
      xp[p+1] += pow(x[i], p+1)/pow(ey[i], 2);
      xpy[p] += pow(x[i], p)*y[i]/pow(ey[i], 2);
    }
  }

  // Set up the matrices to find the least square fit
  TMatrixD X(n+1, n+1);	// Matrix of moments
  TMatrixD XY[n+1];	// Matrix of moments with its pth column replaced by the x^p*y vector
  for (size_t p = 0; p < n+1; p++) {
    XY[p]->ResizeTo(n+1, n+1);
    for (size_t q = 0; q < n+1; q++) {
      X[p][q] = xp[p+q]; // p+qth moment
      for (size_t r = 0; r < n+1; r++) {
	if ( r != p ) {
          XY[p][q][r] = xp[q+r];
	} else {
          XY[p][q][r] = xpy[p];
	}
      }
    }
  }

  // If the determinant of X is zero, then the points are degenerated to one abscissa
  if ( !X.Determinant() ) {
    std::vector<double> a;
    for (size_t p = 0; p < n+1; p++) {
      a.push_back(0.);
    }
    return a;
  }

  // Calculate the ai parameters of a polynomial of the form: a0+a1*x+...+an*x^n
  std::vector a;
  for (size_t p = 0; p < n+1; p++)
    a.push_back(XY[p].Determinant()/X.Determinant());

  return a;
}*/

double polar(double ax, double ay) {

  return atan(sqrt(pow(ax, 2)+pow(ay, 2)));
}

double polar_error(double ax, double ay, double eax, double eay) {

  if ( ax || ay )
      return sqrt((pow(ax*eax, 2)+pow(ay*eay, 2))/((ax*ax+ay*ay)*(1+ax*ax+ay*ay)));

  return 0;
}

double azimuth(double ax, double ay) {

  if ( ay )
      return (.5+(ay < 0))*M_PI - atan(ax/ay);

  return (ax < 0)*M_PI;
}

double azimuth_error(double ax, double ay, double eax, double eay) {

  if ( ax || ay )
      return sqrt((pow(ay*eax, 2)+pow(ax*eay, 2))/(ax*ax+ay*ay));

  return 0;
}

double pol(double x, std::vector<double> par) {

  double y(0.);
  for (size_t p = 0; p < par.size(); p++)
    y += par[p]*pow(x, p);

  return y;
}

double pol_error(double x, double ex,
		 std::vector<double> par, std::vector<double> epar) {

  // Build the parameters covariance matrix
  TMatrixD V_p(par.size(), par.size());
  for (size_t p = 0; p < par.size(); p++)
      for (size_t q = 0; q < par.size(); q++)
          V_p[p][q] = epar[p*par.size()+q];

  // Calculate the variance on the measurement from the functional matrix
  TMatrixD A(par.size(), 1);
  for (size_t p = 0; p < par.size(); p++)
      A[p][0] = pow(x, p);
  TMatrixD At(A);
  At.T();
  TMatrix V_m(At*V_p*A);
  double error2 = V_m[0][0];

  // Error from the uncertainty on x, uncorrelated from the paramters
  double dpol = dnpol(x, par, 1);
  error2 += pow(dpol*ex, 2);

  return sqrt(error2);
}

double dnpol(double x, std::vector<double> par, unsigned int k) {

  double y(0.);
  for (size_t p = k; p < par.size(); p++)
    y += (factorial(p)/factorial(p-k))*par[p]*pow(x, p-k);

  return y;
}

double dnpol_error(double x, double ex,
		   std::vector<double> par, std::vector<double> epar, unsigned int k) {

  // Build the parameters covariance matrix
  TMatrixD V_p(par.size()-k, par.size()-k);
  for (size_t p = k; p < par.size(); p++)
      for (size_t q = k; q < par.size(); q++)
          V_p[p-k][q-k] = epar[p*par.size()+q];

  // Calculate the variance on the measurement from the functional matrix
  TMatrixD A(par.size()-k, 1);
  for (size_t p = k; p < par.size(); p++)
      A[p-k][0] = (factorial(p)/factorial(p-k))*pow(x, p-k);
  TMatrixD At(A);
  At.T();
  TMatrix V_m(At*V_p*A);
  double error2 = V_m[0][0];

  // Error from the uncertainty on x, uncorrelated from the parameters
  double dpol(0.);
  for (size_t p = k+1; p < par.size(); p++)
    dpol += (factorial(p)/factorial(p-k-1))*pow(x, p-k-1);
  error2 += pow(dpol*ex, 2);

  return sqrt(error2);
}

double pol_closest(std::vector<double> par, double xp, double yp, double xmin, double xmax) {

  double p[par.size()+3];
  p[0] = par.size();
  for (size_t k = 0; k < par.size(); k++)
      p[1+k] = par[k];
  p[par.size()+1] = xp;
  p[par.size()+2] = yp;

  TF1 fdist("fdist", fdistance, xmin, xmax, par.size()+3);
  fdist.SetParameters(p);
  double xopt = fdist.GetMinimumX(xmin, xmax, pow(10, -3));

  return xopt;
}

unsigned int factorial(unsigned int n) {

  unsigned int f(1);
  for (unsigned int i = n; i > 0; i--)
      f *= i;

  return f;
}

double fpol(double* x, double* par) {

  double y(0.);
  for (size_t p = 0; p < par[0]; p++)
    y += par[p+1]*pow(x[0], p);

  return y;
}

double fdistance(double *x, double *par) {

  size_t n = par[0];			// Number of parameters
  double pol = fpol(x, par);		// Value of the polynomial
  double xi(par[n+1]), yi(par[n+2]);	// Coordinates of the point

  return sqrt(pow(x[0]-xi, 2)+pow(pol-yi, 2));
}
} // namespace TrackFitter
