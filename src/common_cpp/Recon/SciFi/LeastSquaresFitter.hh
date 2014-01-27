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
 *
 */

#ifndef LEASTSQUARESFITTER_HH
#define LEASTSQUARESFITTER_HH

// C++ headers
#include <vector>

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

namespace MAUS {

class LeastSquaresFitter {
  public:

    /** @brief Default constructor
     */
    LeastSquaresFitter(double sd_1to4, double sd_5, double R_res_cut);

    /** @brief Default destructor, does nothing
     */
    ~LeastSquaresFitter();

    /** @brief Set the member variables using the Global singleton class */
    bool LoadGlobals();

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
                    const std::vector<double> &_y_err, SimpleLine &line);

    /** @brief Fit a circle to spacepoints in x-y projection
     *
     *  Fit a circle of the form A*(x^2 + y^2) + b*x + c*y = 1 with least squares fit
     *  for input spacepoints. Output is a circle in the x-y projection.
     *
     *  @param spnts - A vector containing the input spacepoints
     *  @param circle - The output circle fit
     *
     */
    bool circle_fit(const std::vector<SciFiSpacePoint*> &spnts, SimpleCircle &circle);

    /** Set the resolution of stations 1 - 4 */
    void set_sd_1to4(double sd_1to4) { _sd_1to4 = sd_1to4; }

    /** Get the resolution of stations 1 - 4  */
    double get_sd_1to4() const { return _sd_1to4; }

    /** Set the resolution of station 5 */
    void set_sd_5(double sd_5) { _sd_5 = sd_5; }

    /** Get the resolution of station 5 */
    double get_sd_5() const { return _sd_5; }

    /** Set the helix radius cut */
    void set_R_res_cut(double R_res_cut) { _R_res_cut = R_res_cut; }

    /** Get the helix radius cut */
    double get_R_res_cut() const { return _R_res_cut; }


  private:
    double _sd_1to4;     /** Position error associated with stations 1 t0 4 */
    double _sd_5;        /** Position error associated with station 5 */
    double _R_res_cut;   /** Road cut for circle radius in mm */
};

} // ~namespace MAUS

#endif
