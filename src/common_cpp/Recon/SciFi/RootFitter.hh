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

#ifndef ROOTFITTER_HH
#define ROOTFITTER_HH

// C++ headers
#include <vector>

// ROOT headers
#include "TMatrixD.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

/** @namespace RootFitter
 *
 * ROOT fitting routines
 */
namespace RootFitter {

/** @brief Fit a straight line using the ROOT TLinearFit class
  * @param[in] x x coordinates (the indepepndent variable)
  * @param[in] y y coordinates (the depepndent variable)
  * @param[in] yerr Errors on the y coordinates
  * @param[out] line The fit result including errors
  * @param[out] cov_matrix The 2*2 covariance matrix of the returned parameters
  */
bool FitLineLinear(const std::vector<double>& x, const std::vector<double>& y,
                   const std::vector<double>& yerr, MAUS::SimpleLine& line, TMatrixD& cov_matrix);

/** @brief Fit a circle using the ROOT MINUIT minimiser class
  * @param[in] x x coordinates (a dependent variable)
  * @param[in] y y coordinates (a depepndent variable)
  * @param[out] circ The fit result
  * @param[out] cov_matrix The 3*3 covariance matrix of the returned parameters
  */
bool FitCircleMinuit(const std::vector<double>& x, const std::vector<double>& y,
                     MAUS::SimpleCircle& circ, TMatrixD& cov_matrix);
}

#endif
