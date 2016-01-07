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

#ifndef _SRC_COMMON_CPP_RECON_EMR_TRACKFITTER_HH
#define _SRC_COMMON_CPP_RECON_EMR_TRACKFITTER_HH

// C++ headers
#include <vector>
#include <algorithm>

// ROOT headers
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMath.h"
#include "TF1.h"

// MAUS headers
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/EMRTrack.hh"
#include "DataStructure/EMRSpacePoint.hh"

typedef std::vector<MAUS::EMRSpacePoint*>	EMRSpacePointArray;

namespace TrackFitter {

 /** @brief Least-squares polynomial curve fit
  *
  *  Fit polynomial curves in space, using linear least squares fitting,
  *  for input track. Output is the fitted track.
  *
  *  @param points	- Input space points
  *  @param track 	- Output fitted track
  *  @param n		- Order of the polynomial to fit
  */
  void polynomial_fit(EMRSpacePointArray	spacePointArray,
		      MAUS::EMRTrack& 		track,
		      unsigned int 		n);

 /** @brief Least-squares 2D polynomial curve fit
  *
  *  Fit polynomial curves in a plane, using linear least squares fitting,
  *  for input track. Output is the fitted track.
  *
  *  @param x		- Input vector of abscissae
  *  @param y		- Input vector of ordinates
  *  @param ey		- Input vector of uncertainties on the ordinates
  *  @param n		- Order of the polynomial to fit
  *  @param a		- Output vector of parameters of the polynomial
  *  @param ae		- Output vector of errors on the parameters of the polynomial
  *  @param chi2	- Output chi2 of the fitted track in the plane
  */
  void polynomial_fit_2D(std::vector<double> 	x,
			 std::vector<double> 	y,
			 std::vector<double> 	ey,
			 unsigned int 		n,
			 std::vector<double>& 	a,
			 std::vector<double>& 	ea,
			 double& 		chi2);

 /** @brief Theil-Sen estimator as a track fitter (line)
  *
  *  Fit aline to a set of points. Output is the fitted track.
  *
  *  @param x		- Input vector of abscissae
  *  @param y		- Input vector of ordinates
  *  @param ey		- Input vector of uncertainties on the ordinates
  *  @param n		- Order of the polynomial to fit
  *  @param a		- Output vector of parameters of the polynomial
  *  @param ae		- Output vector of errors on the parameters of the polynomial
  *  @param chi2	- Output chi2 of the fitted track in the plane
  */
  void theil_sen_fit(std::vector<double> 	x,
		     std::vector<double> 	y,
		     std::vector<double> 	ey,
		     std::vector<double>& 	a,
		     std::vector<double>& 	ea,
		     double& 			chi2);

 /** @brief Inclination calculator
  *
  *  Returns the value of the polar angle
  *
  *  @param ax		- Gradient of the curve in the xz projection
  *  @param ay		- Gradient of the curve in the yz projection
  */
  double polar(double ax, double ay);

 /** @brief Inclination error calculator
  *
  *  Returns the uncertainty on the value of the polar angle
  *
  *  @param ax		- Gradient of the curve in the xz projection
  *  @param ay		- Gradient of the curve in the yz projection
  *  @param eax		- Uncertainty on the gradient of the curve in the xz projection
  *  @param eay		- Uncertainty on the gradient of the curve in the yz projection
  */
  double polar_error(double ax, double ay, double eax, double eay);

 /** @brief Azimuth calculator
  *
  *  Returns the value of the azimuthal angle
  *
  *  @param ax		- Gradient of the curve in the xz projection
  *  @param ay		- Gradient of the curve in the yz projection
  */
  double azimuth(double ax, double ay);

 /** @brief Azimuth error calculator
  *
  *  Returns the uncertainty on the value of the azimuthal angle
  *
  *  @param ax		- Gradient of the curve in the xz projection
  *  @param ay		- Gradient of the curve in the yz projection
  *  @param eax		- Uncertainty on the gradient of the curve in the xz projection
  *  @param eay		- Uncertainty on the gradient of the curve in the yz projection
  */
  double azimuth_error(double ax, double ay, double eax, double eay);

 /** @brief Polynomial value calculator par0+par1*x+...+parn*x^n
  *
  *  Returns the value of the polynomial in x
  *
  *  @param x		- Value in which to calculate the polynomial
  *  @param par		- Array of parameters of the polynomial
  *			  par[i] = a_i, ith parameter
  */
  double pol(double x, std::vector<double> par);

 /** @brief Polynomial error calculator
  *
  *  Returns the value of the error on the value of the polynomial in x
  *
  *  @param x		- Value in which to calculate the error
  *  @param ex		- Uncertainty on x
  *  @param par		- Array of parameters of the polynomial
  *			  par[i] = a_i, ith parameter
  *  @param epar	- Array of uncertainties on the parameters
  *			  par[i] = sigma_a_i, ith uncertainty
  */
  double pol_error(double x,
		   double ex,
		   std::vector<double> par,
		   std::vector<double> epar);

 /** @brief kth derivative of a polynomial of the form par0+par1*x+...+parn*x^n
  *
  *  Returns the value of the kth derivative of the polynomial in x
  *
  *  @param x		- Value in which to calculate the derivative
  *  @param par		- Array of parameters of the polynomial
  *			  par[i] = a_i, ith parameter
  *  @param k		- Order of the derivative
  */
  double dnpol(double x, std::vector<double> par, unsigned int k);

 /** @brief Uncertainty on the kth derivative of a polynomial
  *
  *  Returns the value of the uncertainty on the error on the value of the polynomial in x
  *
  *  @param x		- Value in which to calculate the error
  *  @param ex		- Uncertainty on x
  *  @param par		- Array of parameters of the polynomial
  *			  par[i] = a_i, ith parameter
  *  @param epar	- Array of uncertainties on the parameters
  *			  par[i] = sigma_a_i, ith uncertainty
  *  @param k		- Order of the derivative
  */
  double dnpol_error(double x,
		     double ex,
		     std::vector<double> par,
		     std::vector<double> epar,
		     unsigned int k);

 /** @brief Returns the point of the polynomial closest to a point P(xp, yp)
  *
  *  Returns the ordinate of the polynomial point closest to P
  *
  *  @param par		- Array of parameters of the polynomial
  *			  par[i] = a_i, ith parameter
  *  @param xp		- Abscissae of the point P
  *  @param yp		- Ordinate of the point P
  *  @param xmin	- Lower bound of where to look for a minimum
  *  @param xmax	- Upper bound of where to look for a minimum
  */
  double pol_closest(std::vector<double> par, double xp, double yp, double xmin, double xmax);

 /** @brief Factorial value calculator n*(n-1)*...*2*1
  *
  *  Returns the value of the factorial of n
  *
  *  @param n		- Value in which to calculate the factorial
  */
  unsigned int factorial(unsigned int n);

  /** @brief Polynomial value calculator par0+par1*x+...+parn*x^n
    *
    *  Returns the value of the polynomial in x
    *
    *  @param x		- Value in which to calculate the polynomial
    *  @param par	- Array of parameters of the polynomial
    *			-> par[0] = n, number of parameters
    *			-> par[i] = a_(i-1), array of polynomial parameters
    */
  double fpol(double* x, double* par);

 /** @brief Distance between a point (x_i, y_i) and a polynomial
  *
  *  Returns the value of the distance between the point and the function in x
  *
  *  @param x		- Value in which to calculate the distance
  *  @param par	- Array of parameters of the distance
  *			  par[0] = n, number of parameters
  *			  par[i] = a_(i-1), array of polynomial parameters
  *			  par[n+2] = x_i, first coordinate of the point
  *			  par[n+3] = y_i, second coordinate of the point
  */
  double fdistance(double *x, double *par);
} // namespace TrackFitter

#endif // #define _SRC_COMMON_CPP_RECON_EMR_TRACKFITTER_HH
