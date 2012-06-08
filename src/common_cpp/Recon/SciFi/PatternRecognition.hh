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

/** @class PatternRecognition
 *  
 * Pattern Recognition algorithms encapsulated in a class
 *
 */

#ifndef PATTERNRECOGNITION_HH
#define PATTERNRECOGNITION_HH

// C++ headers
#include <vector>
#include <map>
#include <string>

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Recon/SciFi/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"
#include "src/common_cpp/Recon/SciFi/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/Recon/SciFi/SciFiPRTrack.hh"

// namespace MAUS {

class PatternRecognition {
  public:

    /** @brief Default constructor, does nothing
     */
    PatternRecognition();

    /** @brief Default destructor, does nothing
     */
    ~PatternRecognition();

    /** @brief Top level function to begin Pattern Recognition
      *
      * Top level function to begin Pattern Recognition
      *
      *  @param evt - The SciFi event
      */
    void process(SciFiEvent &evt);

    /** @brief Make Pattern Recognition tracks with 5 spacepoints
     *
     *  Make a Pattern Recognition track/s when there are spacepoints
     *  found in all 5 stations of a tracker.  Least squared fitting used,
     *  together with a chi^2 goodness-of-fit test.
     *
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_5tracks(std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                      std::vector<SciFiStraightPRTrack> &trks, std::vector< std::vector<int> >
&residuals );

    /** @brief Make Pattern Recognition tracks with 4 spacepoints
     *
     *  Make a Pattern Recognition track/s when there at least 4 stations
     *  with spacepoints in them. Least squared fitting used, together with a
     *  chi^2 goodness-of-fit test.
     *
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_4tracks(std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                      std::vector<SciFiStraightPRTrack> &trks, std::vector< std::vector<int> >
&residuals );

    /** @brief Make Pattern Recognition tracks with 3 spacepoints
     *
     *  Make a Pattern Recognition track/s when there at least 3 stations
     *  with spacepoints in them. Least squared fitting used, together with a
     *  chi^2 goodness-of-fit test.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_3tracks(std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                      std::vector<SciFiStraightPRTrack>& trks, std::vector< std::vector<int> >
&residuals );

    /** @brief Fits a straight track for a given set of stations
     * 
     *  @param ignore_stations int vector specifying which stations are not to be used for
     *                         the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *                         while -1 means use *all* the stations (ignore none of them). 
     *                         The size of the vector should be 0 for a 5pt track,
     *                         1 for a 4pt track, and 2 for a 3pt track.
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_straight_tracks(const int num_points, const std::vector<int> ignore_stations,
                     std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                     std::vector<SciFiStraightPRTrack> &trks, std::vector< std::vector<int> >
&residuals );

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

    /** @brief Form a helical track from spacepoints
     *
     *  Two part process. (1) Fit circle in x-y projection. (2) Fit a
     *  line in the s-z projection
     *
     *  @param num_points
     *  @param ignore_stations int vector specifying which stations are not to be used for
     *                         the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *                         while -1 means use *all* the stations (ignore none of them).
     *                         The size of the vector should be 0 for a 5pt track,
     *                         1 for a 4pt track, and 2 for a 3pt track.
     *  @param spnts_by_station 2D vector of spacepoints, sorted by station
     *  @param trks The track holding the initial helix parameters and spacepoints used
     *
     */
    void make_helix(const int num_points, const std::vector<int> ignore_stations,
                    std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                    std::vector<SciFiPRTrack> &trks);

    /** @brief Find points from intermediate stations which fit to the "trial track"
     *
     * Calculate the radial deviation of intermediate station spacepoints from an
     * initial circle formed with only 3 spacepoints.  Check if these delta_R's are
     * acceptable with a cut, and if they are, append them to the trial track.
     *
     */
    double delta_R(const SimpleCircle &circle, const CLHEP::Hep3Vector &pos);

    /** @brief Fit a circle to spacepoints in x-y projection
     *
     *  Fit a circle of the form A*(x^2 + y^2) + b*x + c*y = 1 with least squares fit 
     *  for input spacepoints. Output is a circle in the x-y projection.
     *
     *  @param spnts - A vector containing the input spacepoints
     *  @param circle - The output circle fit
     *
     */
    void circle_fit(const std::vector<SciFiSpacePoint*> &spnts, SimpleCircle &circle);

    /** @brief Determine the dip angle of the helix
     *
     * Calculate the dip angle of the helix.  Output is a line, the slope of which
     * is dsdz = 1/tanlambda.
     *
     * @param spnts - A vector of all the input spacepoints
     * @param circle - The circle fit of spacepoints from x-y projection
     * @param line_sz - The output fitted line in s-z projection.
     *
     */
    void calculate_dipangle(const std::vector<SciFiSpacePoint*> &spnts,
                            const SimpleCircle &circle, std::vector<double> &dphi,
                            SimpleLine &line_sz, double &Phi_0);

    /** @brief Calculate the turning angle of a spacepoint w.r.t. helix center
     *
     * @param xpos - x position of spacepoint
     * @param ypos - y position of  spacepoint
     * @param circle - Contains the helix center
     *
     */
    double calculate_Phi(double xpos, double ypos, const SimpleCircle &circle);

    /** @brief Account for possible 2*pi rotations between stations
     *
     *  As the seperation along z increases between stations, the difference in phi should
     *  also increase. Inputs are vectors containing the seperation in z, from stations 1-5,
     *  and a vector containing the phi differences.  Returns true if the rotations are
     *  consistent with z spacing, and the vector dphi should contain new dphis corrected for
     *  2*pi rotations.
     *
     * @param dz - a vector containing z seperations, z_j - z_i
     * @param dphi - a vector containing the differences in phi b/w stations, phi_j - phi_i
     *
     */
    bool turns_between_stations(const std::vector<double> &dz, std::vector<double> &dphi);

    /** @brief Check that the ratio between the change in z and change in phi is appropriate
     *
     *  Returns true is the ABcut is satisfied.  Should be very nearly 0.
     *
     *  @param dphi_kj - dphi_k - dphi_j where k > j
     *  @param dphi_ji - dphi_j - dphi_i where j > i
     *  @param dz_kj - dz_k - dz_j where k > j
     *  @param dz_ji - dz_j - dz_i where j > i
     *
     */
    bool AB_ratio(double &dphi_ji, double &dphi_kj, double dz_ji, double dz_kj);

    /** @brief Changes dphi vector to ds vector
     *
     *  Just scalar multiplication of each element dphi_ji by R.
     *
     * @param R - radius of helix
     * @param dphi - vector containing dphi_ji for each station step
     * @param ds - vector containing ds_ji for each station step (corresponding to dz_ji's)
     *
     */
    void dphi_to_ds(double R, const std::vector<double> &dphi, std::vector<double> &ds);

    /** @brief Fit a full helix to the spacepoints with a nonlinear least squares fit
     *
     *  A non-linear least squares helix fit is performed on spacepoints which passed previous
     *  chisq circle and line tests in the x-y and s-z projections, respectively.  This is the last
     *  fit in the helical pattern recognition routine, and will provide the initial parameter
     *  values for the Kalman Filter. Returns true if a good fit has been made in a reasonable
     *  number of iterations.  Returns false otherwise.
     *
     *  @param spnts - Vector containing spacepoints being used for the fit, in order of
     *                 innermost to outermost stations
     *
     */
    bool full_helix_fit(const std::vector<SciFiSpacePoint*> &spnts, const SimpleCircle &circle,
                        const SimpleLine &line_sz, SimpleHelix &helix);

    /** @brief Calculates helix at a point i
     *
     *  Finds helix function at the ith spacepoint using the parameter values and the spacepoint
     *
     *  @param R - raidus of helix
     *  @param phi_0 - turning angle of initial spacepoint
     *  @param tan_lambda - helix dip angle
     *  @param xi - Helix value x at point i (cartesian)
     *  @param yi - Helix value y at point i
     *  @param zi - Helix value z at point i
     *
     */
    void helix_function_at_i(double R, double phi_0, double dsdz, double A, double B,
                             double C, double phi_i, double &xi, double &yi, double &zi);

    /** @brief Calculates helix chisq
     *
     *  Calculates chisq for given parameter values and spacepoints
     *
     *  @param spnts - Vector containing spacepoints being used for the fit, in order of
     *                 innermost to outermost stations
     *  @param turning_angles - vector containing turning angles for each spacepoint.
     *                  organized inner-> outermost stations for i = 0.... N
     *  @param Phi_0 - azimuthal angle of inital spacepoint in x-y plane
     *  @param tan_lambda - helix dip angle
     *  @param R - radius of helix
     *
     */
    double calculate_chisq(const std::vector<SciFiSpacePoint*> &spnts,
                           const std::vector<double> &turning_angles, double Phi_0,
                           double dsdz, double R);

    /** @brief Calculates the adjustments to the seed parameters
     *
     *  By taking first, second, and mixed derivatives of chi_sq for helix, we calculate the
     *  adjustments to the seed paramters R, Phi_0, and tan_lambda. i.e. R' = R + dR, where R' is
     *  the new parameter value, R is the seed value, and dR is the adjustment. Inputs are inital
     *  seed parameters and the outputs are the ajustments.
     *
     *  @param spnts - Vector containing spacepoints being used for the fit, in order of innermost
     *                 to outermost stations
     *  @param turning_angles - vector containing turning angles for each spacepoint.
     *                  organized inner-> outermost stations for i = 0.... N
     *  @param R - initial R value as calculated from circle fit
     *  @param phi_0 - turning angle of initial spacepoint as calculate from circle fit
     *  @param tan_lambda - helix dip angle calculated from the slope of line in the s-z projection
     *  @param dR - adjustment to R
     *  @param dphi_0 - adjumtment to phi_0
     *  @param dtan_lmabda - adjustment to tan_lmabda
     *
     */
    void calculate_adjustments(const std::vector<SciFiSpacePoint*> &spnts,
                               const std::vector<double> &turning_angles, double R, double phi_0,
                               double tan_lambda, double &dR, double &dphi_0, double &dtan_lambda);

    /** @brief Determine which two stations the initial line should be drawn between
     * 
     *  The initial line is to be drawn between the two outermost stations being used.
     *  This in turn depends on which stations are presently being ignored
     *  e.g. for a 5 pt track, station 5 and station 1 are always  the outer and inner
     *  stations respectively.  This function returns the correct outer and inner
     *  station numbers, given which stations are presently being ignored.
     * 
     *  NB Stations are number 0 - 4 in the code, not 1 - 5 as in the outside world
     *
     *  @param ignore_stations - Vector of ints, holding which stations should be ignored
     *  @param outer_station_num - The outermost station number used for a given track fit
     *  @param inner_station_num - The innermost station number used for a given track fit
     *
     */
    void set_end_stations(const std::vector<int> ignore_stations,
                          int &outer_station_num, int &inner_station_num);

    /** @brief Determine which three stations the initial circle should be fit to
     *
     *  The initial circle is to be fit between the two outermost stations being used, and a middle
     *  station needs to be picked as well (need three points for a circle fit).
     *  This in turn depends on which stations are presently being ignored
     *  e.g. for a 5 pt track, station 5 and station 1 are always  the outer and inner
     *  stations respectively, and station 3 is the middle station.
     *  This function returns the correct outer, inner, and middle
     *  station numbers, given which stations are presently being ignored.
     *
     *  NB Stations are number 0 - 4 in the code, not 1 - 5 as in the outside world
     *
     *  @param ignore_stations - Vector of ints, holding which stations should be ignored
     *  @param outer_station_num - The outermost station number used for a given track fit
     *  @param inner_station_num - The innermost station number used for a given track fit
     *  @param middle_station_num - the middle station number used for a given track fit
     *
     */
    void set_seed_stations(const std::vector<int> ignore_stations, int &outer_station_num,
                           int &inner_station_num, int &middle_station_num);

    /** @brief Create a 2D vector of SciFi spacepoints sorted by tracker station
     *
     *  Take an input vector of spacepoints and output a 2D vector of spacepoints
     *  where the first index is the station the spacepoint is located.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param spnts_by_station - Output 2D vector of spacepoints sorted by station
     *
     */
    void sort_by_station(const std::vector<SciFiSpacePoint*> &spnts,
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station);

    /** @brief Count the number of stations that have unused spacepoint
     *
     *  @param spnts_by_station - Input 2D vector of spacepoints sorted by station
     * 
     */
    int num_stations_with_unused_spnts(
        const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station);

    /** @brief Count and return how many tracker stations have at least 1 unused spacepoint
     *
     *  @param spnts_by_station - Input 2D vector of spacepoints sorted by station
     *  @param stations_hit - Output vector holding the numbers of each station with at 
     *                        least 1 unused spacepoint
     *  @param stations_not_hit - Output vector holding the numbers of each station with
     *                            no unused spacepoints
     */
    void stations_with_unused_spnts(
           const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
           std::vector<int> &stations_hit, std::vector<int> &stations_not_hit);

    void set_ignore_stations(const std::vector<int> &ignore_stations,
                             int &ignore_station_1, int &ignore_station_2);

    void draw_line(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2,
                   SimpleLine &line_x, SimpleLine &line_y);

    bool add_residuals(const bool passed, const double dx, const double dy,
                       std::vector< std::vector<int> > &residuals);

    double parabola_fit(const std::vector<double> chisqs, const std::vector<double> Dparams);

  private:
    static const int _n_trackers = 2;
    static const int _n_stations = 5;
    static const int _n_bins = 100;         // Number of bins in each residuals histogram
    static const double _sd_1to4 = 0.3844;  // Position error associated with stations 1 through 4
    static const double _sd_5 = 0.4298;     // Position error associated with station 5
    static const double _res_cut = 10;      // Road cut for linear fit in mm
    static const double _R_res_cut = 20.;    // Road cut for circle radius in mm
    static const double _chisq_cut = 15;    // Cut on the chi^2 of the least squares fit in mm
    static const double _AB_cut = .1;       // Need to calculate appropriate cut here!!!
    static const double _sd_phi_1to4 = 1.;  // Still needs to be calculated!!!!
    static const double _sd_phi_5 = 1.;     // Still needs to be calculated!!!!
    static const double _active_diameter = 300.0;  // Active volume diameter a tracker in mm
};
// } // ~namespace MAUS

#endif
