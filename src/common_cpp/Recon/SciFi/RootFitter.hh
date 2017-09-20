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
#include <functional>

// ROOT headers
#include "TMatrixD.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

/** @namespace RootFitter
 *
 * ROOT fitting routines written for the scifi pattern recognition.
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
                     const std::vector<double>& err,
                     MAUS::SimpleCircle& circ, TMatrixD& cov_matrix);

/** @brief Fit a helix using the ROOT MINUIT minimiser class.
  *
  * Fit a helix using the ROOT MINUIT minimiser class.
  * pStart holds xc (circle centre in x), yc (circle centre in y), the radius and dsdz, which
  * seed MINUIT. xc, yc and radius are fixed, that is MINUIT will not improve them, just use them
  * to help fit dsdz. The dsdz seed is not currently used by the algorithm, as a scan of the dsdz
  * chisq function is performed to find the approximate global minima which then seeds MINUIT. If
  * the track handedness (rotation direction) is supplied, this will be used by the scan function
  * to exclude unphysical minima (those where dsdz has the wrong sign).
  *
  * The MINUIT algorithm used for the minimisation in MIGRAD.
  *
  * @param[in] x x coordinates (a dependent variable)
  * @param[in] y y coordinates (a depepndent variable)
  * @param[in] z z coordinates (the independent variable)
  * @param[in] pStart double array of xc, yc, R, dsdz which seeds the parameters for MINUIT
  * @param[out] helix The fit result
  * @param[in] handedness Track rotation direction, if set to 1 or -1 used to help dsdz scan func
  * @param[in] cut The pattern recongition longitundal chisq cut, used by the dsdz scan function
  */
bool FitHelixMinuit(const std::vector<double>& x, const std::vector<double>& y,
                    const std::vector<double>& z, const std::vector<double>& err,
                    const double* pStart, MAUS::SimpleHelix& helix, int handedness = 0,
                    double cut = -1.0);

/** Scan the chisq function to find the rough location of the global minimum to seed MINUIT
  * (otherwise it gets stuck in local minima).
  * It will take minimum chisq value, unless we have an expected handedness (i.e. dsdz sign)
  * supplied in which case we will use that to exclude wrong sign candidates.
  *
  * @param[in] pStart 4D double array holding xc, yx, radius, dsdz_initial_seed
  * @param[in] handedness Particle rotation direction
  * @param[in] cut The chisq per dof cut from pattern recognition
  * @param[in] Chi2Function std::function used to calculate the chisq
  * @return The optimised dsdz_seed (hopefully for the rough global minimum of the chisq func)
  */
double LocateGlobalChiSqMinimum(const double* pStart, int handedness, double cut,
                                std::function<double(const double*)> Chi2Function); //NOLINT(*)
}

#endif
