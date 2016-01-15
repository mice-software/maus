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

#ifndef _SRC_COMMON_CPP_RECON_EMR_TRACKRANGE_HH
#define _SRC_COMMON_CPP_RECON_EMR_TRACKRANGE_HH

// C++ headers
#include <vector>
#include <algorithm>

// ROOT headers
#include "TF1.h"

// MAUS headers
#include "DataStructure/ThreeVector.hh"

namespace TrackRange {

  /** @brief Piecewise range
    *
    *  Reconstructs the range by adding up the length of pieces of track
    *  between the reconstructed space points
    *
    *  @param points	- Array of ThreeVector of coordinates
    */
  double range_piecewise(std::vector<MAUS::ThreeVector> points);

  /** @brief Piecewise range error
    *
    *  Calculates the error on the piecewise range
    *
    *  @param points 	- Array of ThreeVector of coordinates
    *  @param errors	- Array of ThreeVector of errors on the coordinates
    */
  double range_piecewise_error(std::vector<MAUS::ThreeVector> points,
			       std::vector<MAUS::ThreeVector> errors);

  /** @brief Path length integral range
    *
    *  Does the path integral of the fitted track along z provided the 2*n
    *  parameters of the 3D track in the two projections and its end points
    *
    *  @param parx	- Parameters of the polynomial in the xz plane
    *  @param pary	- Parameters of the polynomial in the yz plane
    *  @param zstart	- Starting point of the track
    *  @param zend	- End point of the track
    */
  double range_integral(std::vector<double> parx,
			std::vector<double> pary,
		        double zstart,
			double zend);

  /** @brief Path length integral range error
    *
    *  Calculates the error on the integral range
    *
    *  @param parx 	- Parameters of the polynomial in the xz plane
    *  @param pary 	- Parameters of the polynomial in the yz plane
    *  @param eparx	- Parameters errors of the polynomial in the xz plane
    *  @param epary	- Parameters errors of the polynomial in the yz plane
    *  @param zstart	- Starting point of the track
    *  @param zend	- End point of the track
    *  @param zstart	- Uncertainty on the starting point of the track
    *  @param zend	- Uncertainty on the end point of the track
    */
  double range_integral_error(std::vector<double> parx,
			      std::vector<double> pary,
			      std::vector<double> eparx,
			      std::vector<double> epary,
		      	      double zstart,
			      double zend,
		      	      double ezstart,
			      double ezend);

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

  /** @brief 3D path function calculator
    *
    *  Returns the 3D path function in x
    *
    *  @param x		- Value at which to calculate the path
    *  @param par	- Array of parameters :
    *			-> par[0] = n, number of parameters
    *			-> par[i] = ax_(i), i = 1,...,n
    *			-> par[n+i] = ay_(i), i = 1,...,n
    */
  double fpath(double* x, double* par);

  /** @brief 3D path error function calculator
    *
    *  Returns the 3D path errror function at x
    *
    *  @param x		- Value at which to calculate the path error
    *  @param par	- Array of parameters :
    *			-> par[0] = n, number of parameters
    *			-> par[i] = ax_(i), i = 1,...,n
    *			-> par[n+i] = ay_(i), i = 1,...,n
    *			-> par[2*n+1] = k, order of the parameter in which to derivate
    */
  double fpath_error(double* x, double* par);
} // namespace TrackRange

#endif // #define _SRC_COMMON_CPP_RECON_EMR_TRACKRANGE_HH
