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

/** @class LeastSquaresFitter
 *
 * Least Squares Fitting algorithms encapsulated in a class
 * NOTE: It advisable to always call LoadGlobals() before performing circle fit
 *
 */

#ifndef LEASTSQUARESFITTER_HH
#define LEASTSQUARESFITTER_HH

// C++ headers
#include <vector>

// ROOT headers
#include "TMatrixD.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

namespace LeastSquaresFitter {

  /** @brief Least-squares straight line fit
    *
    *  Fit straight lines, using linear least squares fitting,
    *  for input spacepoints. Output is a line.
    *
    *  @param spnts - A vector of all the input spacepoints
    *  @param line_x - Output line in x - z plane
    *  @param line_y - Output line in y - z plane
    *
    */
  void linear_fit(const std::vector<double> &_x, const std::vector<double> &_y,
                  const std::vector<double> &_y_err, MAUS::SimpleLine &line,
                  TMatrixD& covariance);

  /** @brief Fit a circle to spacepoints in x-y projection
    *
    *  Fit a circle of the form A*(x^2 + y^2) + b*x + c*y = 1 with least squares fit
    *  for input spacepoints. Output is a circle in the x-y projection.
    *
    *  @param spnts - A vector containing the input spacepoints
    *  @param circle - The output circle fit
    *  @param sd_1to4 - Position error associated with stations 1 to 4
    *  @param sd_5 - Position error associated with stations 5
    *  @param R_res_cut - Road cut for circle radius in mm
    *
    */
  bool circle_fit(const double sd_1to4, const double sd_5, const double R_res_cut,
                  const std::vector<MAUS::SciFiSpacePoint*> &spnts, MAUS::SimpleCircle &circle,
                  TMatrixD& covariance);

} // ~namespace LeastSquaresFitter

#endif
