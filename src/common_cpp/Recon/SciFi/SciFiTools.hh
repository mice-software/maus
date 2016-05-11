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

// C++ headers
#include <vector>

// ROOT headers
#include "TMatrixD.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

/** @namespace SciFiTools
 *
 * Various tools encapsulated in a class for use with SciFi data
 * (in particular during Pattern Recognition).
 */
namespace SciFiTools {

    /** @brief Calculates the turning angle of a spacepoint w.r.t. the x' axis
     *
     * Calculates the turning angle from the x' axis, returning (phi_i + phi_0). In the case that
     * x0 and y0 are used, it returns phi_0. Do not confuse the returned angle with phi_i itself,
     * the turning angle wrt the x' axis not the x axis.
     * 
     * @param[in] xpos - x position of spacepoint
     * @param[in] ypos - y position of  spacepoint
     * @param[out] circle - Contains the helix center
     * @return The turning angle
     */
    double calc_phi(double xpos, double ypos, const MAUS::SimpleCircle &circle);

    /** @brief Calculate the residuals of a straight line to a spacepoint
     *  @param[in] sp - The spacepoint
     *  @param[in] line_x - The x projection of the line
     *  @param[in] line_y - The y projection of the line
     *  @param[out] dx - x residual
     *  @param[out] dy - y residual
     */
    void calc_straight_residual(const MAUS::SciFiSpacePoint *sp, const MAUS::SimpleLine &line_x,
                                const MAUS::SimpleLine &line_y, double &dx, double &dy);

    /** @brief Calculate the residual of a circle to a spacepoint
     *  @param[in] sp - The spacepoint
     *  @param[in] c - The circle (x0, y0 and rho must be set)
     *  @return The residual
     */
    double calc_circle_residual(const MAUS::SciFiSpacePoint *sp, const MAUS::SimpleCircle &c);

    /** @brief Take two spoints and return 2 straight lines connecting them, 1 in x-z, 1 in y-z
     *  @param[in] sp1 - The first spacepoint
     *  @param[in] sp2 - The second spacepoint
     *  @param[out] line_x - The output line in x-z
     *  @param[out] line_y - The output line in y-z
     */
    void draw_line(const MAUS::SciFiSpacePoint *sp1, const MAUS::SciFiSpacePoint *sp2,
                   MAUS::SimpleLine &line_x, MAUS::SimpleLine &line_y);

    /** @brief Calculate the determinant for a 3*3 ROOT matrix
     *
     *  Calculate the determinant for a 3*3 ROOT matrix (the in-built ROOT method falls over
     *  in the case of a singular matrix)
     *
     *  @param[in] m - The 3*3 ROOT TMatrixD (a matrix of doubles)
     *  @return The determinant
     */
    double det3by3(const TMatrixD &m);

    /** @brief Create a circle from 3 spacepoints
     *
     *  Create a circle from 3 spacepoints. A circle is unambiguously defined by 3 points.
     *  For 3 spacepoints we can make a simple circle without requiring least squares fitting.
     *  See the tracker documentation for a description of algorithms.
     *
     *  @param[in] sp1 - The first spacepoint
     *  @param[in] sp2 - The second spacepoint
     *  @param[in] sp3 - The third spacepoint
     *  @return The output circle
     */
    MAUS::SimpleCircle make_3pt_circle(const MAUS::SciFiSpacePoint *sp1,
                                       const MAUS::SciFiSpacePoint *sp2,
                                       const MAUS::SciFiSpacePoint *sp3);

    /** @brief Short function to calculate the remainder of a division of num / denom
     *
     * Short function to calculate the remainder of a division of num / denom, slightly 
     * different to cmath's fod
     *
     * @param[in] num - the numerator
     * @param[in] denom - the denominator
     * @return The remainder
     */
    double my_mod(const double num, const double denom);

    /** @brief Count the number of stations that have unused spacepoints
     *  @param[in] spnts_by_station - Input 2D vector of spacepoints sorted by station
     *  @return Number of stations with unused spacepoints
     */
    int num_stations_with_unused_spnts(const MAUS::SpacePoint2dPArray &spnts_by_station);

    /** @brief Changes phi vector to s vector (scalar multiplication of each element phi by R).
     *  @param[in] R - radius of helix
     *  @param[in] phi - vector containing turning angle for each spacepoint
     *  @return The s values
     */
     std::vector<double> phi_to_s(const double R, const std::vector<double> &phi);

     /** @brief Print the x,y,z coordinates of each spacepoint in vector
      *  @param[in] spnts The spacepoints
      */
     void print_spacepoint_xyz(const std::vector<MAUS::SciFiSpacePoint*> &spnts);

    /** @brief Create a 2D vector of SciFi spacepoints sorted by tracker station
     *
     *  Take an input vector of spacepoints and output a 2D vector of spacepoints
     *  where the first index is the station the spacepoint is located in.
     *
     *  @param[in] spnts - A vector of all the input spacepoints
     *  @param[out] spnts_by_station - Output 2D vector of spacepoints sorted by station
     *
     */
    void sort_by_station(const MAUS::SciFiSpacePointPArray &spnts,
                         MAUS::SpacePoint2dPArray &spnts_by_station);

    /** @brief Create a 2D vector of SciFi spacepoints sorted by tracker
     *
     *  Take an input vector of spacepoints and output a 2D vector of spacepoints
     *  where the first index is the tracker the spacepoint is located in.
     *
     *  @param[in] spnts - A vector of all the input spacepoints
     *  @return The sorted spacepoints
     */
    MAUS::SpacePoint2dPArray sort_by_tracker(const std::vector<MAUS::SciFiSpacePoint*> &spnts);

    /** @brief Count and return how many tracker stations have at least 1 unused spacepoint
     *  @param[in] spnts_by_station Input 2D vector of spacepoints sorted by station
     *  @param[out] stations_hit Vector holding the numbers of each station with at
     *                           least 1 unused spacepoint
     *  @param[out] stations_not_hit Vector holding the numbers of each station with
     *                               no unused spacepoints
     */
    void stations_with_unused_spnts(const MAUS::SpacePoint2dPArray &spnts_by_station,
                                    std::vector<int> &stations_hit,
                                    std::vector<int> &stations_not_hit);

} // ~namespace SciFiTools
