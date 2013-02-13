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
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// MAUS headers
#include "src/common_cpp/DataStructure/SimpleLine.hh"
#include "src/common_cpp/DataStructure/SimpleCircle.hh"
#include "src/common_cpp/DataStructure/SimpleHelix.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

namespace MAUS {

typedef std::vector< std::vector<SciFiSpacePoint*> > SpacePoint2dPArray;

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
    void process(const bool helical_pr_on, const bool straight_pr_on, SciFiEvent &evt);

     /** @brief Small function to easily add trks to a SciFiEvent
      *
      *  Small utility function to easily add  both straight and helical tracks to a SciFiEvent,
      *  and to set the tracker number of all the tracks added
      *
      *  @param strks - The straight tracks vector
      *  @param htrks - The helical tracks vector
      *  @param trker_no - The tracker number
      *  @param evt - The SciFi event
      */
    void add_tracks(const int trker_no, std::vector<SciFiStraightPRTrack> &strks,
                    std::vector<SciFiHelicalPRTrack> &htrks, SciFiEvent &evt);

     /** @brief Small function to easily add straight trks to a SciFiEvent
      *
      *  Small utility function to easily add  both straight tracks to a SciFiEvent,
      *  and to set the tracker number of all the tracks added
      *
      *  @param strks - The straight tracks vector
      *  @param trker_no - The tracker number
      *  @param evt - The SciFi event
      */

    void make_all_tracks(const bool track_type, const int trker_no,
                         SpacePoint2dPArray &spnts_by_station, SciFiEvent &evt);


    /** @brief Make Pattern Recognition tracks with 5 spacepoints
     *
     *  Make a Pattern Recognition track/s when there are spacepoints
     *  found in all 5 stations of a tracker.  Least squared fitting used,
     *  together with a chi^2 goodness-of-fit test.
     *
     *  @param track_type - boolean, 0 for straight tracks, 1 for helical tracks
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param strks - A vector of the output Pattern Recognition straight tracks
     *  @param htrks - A vector of the output Pattern Recognition helical tracks
     */
    void make_5tracks(const bool track_type, SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack> &strks,
                      std::vector<SciFiHelicalPRTrack> &htrks);

    /** @brief Make Pattern Recognition tracks with 4 spacepoints
     *
     *  Make a Pattern Recognition track/s when there at least 4 stations
     *  with spacepoints in them. Least squared fitting used, together with a
     *  chi^2 goodness-of-fit test.
     *
     *  @param track_type - boolean, 0 for straight tracks, 1 for helical tracks
     *  @param spnts_by_station - A 2D vector of all the input spacepoints ordered by station
     *  @param strks - A vector of the output Pattern Recognition straight tracks
     *  @param htrks - A vector of the output Pattern Recognition helical tracks
     */
    void make_4tracks(const bool track_type, SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack> &strks,
                      std::vector<SciFiHelicalPRTrack> &htrks);

    /** @brief Make Pattern Recognition tracks with 3 spacepoints
     *
     *  Make a Pattern Recognition track/s when there at least 3 stations
     *  with spacepoints in them. Least squared fitting used, together with a
     *  chi^2 goodness-of-fit test.
     *
     *  @param track_type - boolean, 0 for straight tracks, 1 for helical tracks
     *  @param spnts - A vector of all the input spacepoints
     *  @param strks - A vector of the output Pattern Recognition straight tracks
     *  @param htrks - A vector of the output Pattern Recognition helical tracks
     */
    void make_3tracks(const bool track_type, SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack> &strks,
                      std::vector<SciFiHelicalPRTrack> &htrks);

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
                     SpacePoint2dPArray &spnts_by_station,
                     std::vector<SciFiStraightPRTrack> &strks);

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
                    SpacePoint2dPArray &spnts_by_station,
                    std::vector<SciFiHelicalPRTrack> &htrks);

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

    /** @brief Determine the dip angle of the helix
     *
     * Calculate the dip angle of the helix.  Output is a line, the slope of which
     * is dsdz = 1/tanlambda. Note: the function is sensitive to the order the
     * spacepoints appear in the input vector (should be sp1 in index 0, sp2 in index 1, ...)
     *
     * @param spnts - A vector of all the input spacepoints
     * @param circle - The circle fit of spacepoints from x-y projection
     * @param line_sz - The output fitted line in s-z projection.
     *
     */
    void calculate_dipangle(const std::vector<SciFiSpacePoint*> &spnts,
                            const SimpleCircle &circle, std::vector<double> &dphi,
                            SimpleLine &line_sz, double &phi_0);

    /** @brief Calculates the turning angle of a spacepoint w.r.t. the x axis
     *
     * Calculates the turning angle from the x axis, returning (phi_i + phi_0). In the case that
     * x0 and y0 are used, it returns phi_0. Do not confuse the returned angle with phi_i itself,
     * the turning angle wrt the x' axis.
     * 
     * @param xpos - x position of spacepoint
     * @param ypos - y position of  spacepoint
     * @param circle - Contains the helix center
     *
     */
    double calc_turning_angle(double xpos, double ypos, const SimpleCircle &circle);

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

    /** @brief Checks that the spacepoints in trial track fall within longest acceptable time range
     *
     *  Tracker timing resolution cable of ~2ns. Longest acceptable time of flight through tracker
     *  was calculated offline for straight and helical tracks
     *
     */
    bool check_time_consistency(const std::vector<SciFiSpacePoint*>);

    /** @brief Create a 2D vector of SciFi spacepoints sorted by tracker
     *
     *  Take an input vector of spacepoints and output a 2D vector of spacepoints
     *  where the first index is the tracker the spacepoint is located in.
     *
     *  @param spnts - A vector of all the input spacepoints
     *
     */
    SpacePoint2dPArray sort_by_tracker(const std::vector<SciFiSpacePoint*> &spnts);

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
     *  Returns true if successful, false if fails (due to a bad argument being passed)
     *
     *  @param ignore_stations - Vector of ints, holding which stations should be ignored
     *  @param outer_station_num - The outermost station number used for a given track fit
     *  @param inner_station_num - The innermost station number used for a given track fit
     *
     */
    bool set_end_stations(const std::vector<int> ignore_stations,
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
    bool set_seed_stations(const std::vector<int> ignore_stations, int &outer_station_num,
                           int &inner_station_num, int &middle_station_num);

    /** @brief Create a 2D vector of SciFi spacepoints sorted by tracker station
     *
     *  Take an input vector of spacepoints and output a 2D vector of spacepoints
     *  where the first index is the station the spacepoint is located in.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param spnts_by_station - Output 2D vector of spacepoints sorted by station
     *
     */
    void sort_by_station(const std::vector<SciFiSpacePoint*> &spnts,
                         SpacePoint2dPArray &spnts_by_station);

    /** @brief Count the number of stations that have unused spacepoint
     *
     *  @param spnts_by_station - Input 2D vector of spacepoints sorted by station
     * 
     */
    int num_stations_with_unused_spnts(const SpacePoint2dPArray &spnts_by_station);

    /** @brief Count and return how many tracker stations have at least 1 unused spacepoint
     *
     *  @param spnts_by_station - Input 2D vector of spacepoints sorted by station
     *  @param stations_hit - Output vector holding the numbers of each station with at 
     *                        least 1 unused spacepoint
     *  @param stations_not_hit - Output vector holding the numbers of each station with
     *                            no unused spacepoints
     */
    void stations_with_unused_spnts(const SpacePoint2dPArray &spnts_by_station,
                                    std::vector<int> &stations_hit,
                                    std::vector<int> &stations_not_hit);

    bool set_ignore_stations(const std::vector<int> &ignore_stations,
                             int &ignore_station_1, int &ignore_station_2);

    /** @brief Take two spoints and return 2 straight lines connecting them, 1 in x-z, 1 in y-z
     *
     *  @param sp1 - The first spacepoint
     *  @param sp2 - The second spacepoint
     *  @param line_x - The output line in x-z
     *  @param line_y - The output line in y-z
     *
     */
    void draw_line(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2,
                   SimpleLine &line_x, SimpleLine &line_y);

    void calc_residual(const SciFiSpacePoint *sp, const SimpleLine &line_x,
                       const SimpleLine &line_y, double &dx, double &dy);

    bool get_helical_pr_on() { return _helical_pr_on; }
    bool get_straight_pr_on() { return _straight_pr_on; }
    void set_helical_pr_on(const bool helical_pr_on) { _helical_pr_on = helical_pr_on; }
    void set_straight_pr_on(const bool straight_pr_on) { _straight_pr_on = straight_pr_on; }

  private:
    static const int debug = 1; // Set output level, 0 = little, 1 = more couts, 2 = files as well
    static const int _n_trackers = 2;
    static const int _n_stations = 5;
    static const int _n_bins = 100;         // Number of bins in each residuals histogram
    static const double _sd_1to4 = 0.3844;  // Position error associated with stations 1 through 4
    static const double _sd_5 = 0.4298;     // Position error associated with station 5
    static const double _sd_phi_5 = 1.0;
    static const double _sd_phi_1to4 = 1.0;
    static const double _res_cut = 2;      // Road cut for linear fit in mm
    static const double _R_res_cut = 50.0;    // Road cut for circle radius in mm
    static const double _chisq_cut = 15;    // Cut on the chi^2 of the least squares fit in mm
    static const double _sz_chisq_cut = 30.0; // Cut on the sz chi^2 from least squares fit in mm
    static const double _helix_chisq_cut = 100;
    static const double _chisq_diff = 3.;
    static const double _AB_cut = .7;       // Need to decide on appropriate cut here!!!
    static const double _active_diameter = 300.0;  // Active volume diameter a tracker in mm
    bool _helical_pr_on;   // Flag to turn on helical pr (0 off, 1 on)
    bool _straight_pr_on;  // Flag to turn on straight pr (0 off, 1 on)

    static const double _Pt_max = 180.; // MeV/c max Pt for helical tracks (given by R_max = 150mm)
    static const double _Pz_min = 50.; // MeV/c min Pz for helical tracks (this is a guess)

    // Some output files - only to be kept when in development stages
    std::ofstream * _f_res;
    std::ofstream * _f_res_good;
    std::ofstream * _f_res_chosen;
    std::ofstream * _f_trks;
};

} // ~namespace MAUS

#endif
