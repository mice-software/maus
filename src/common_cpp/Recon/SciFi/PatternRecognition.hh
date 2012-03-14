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
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Recon/SciFi/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"
// Added by Summer
#include "src/common_cpp/Recon/SciFi/SciFiHelicalPRTrack.hh"

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
      * Top level function to begin Pattern Recognition, only used to
      * decide whether straight or helical fitting is used.
      *
      *  @param evt - The SciFi event
      */
    void process(SciFiEvent &evt);

  private:

    /** @brief Form an initial line and find intermediate spacepoints which best match
     * 
     *  @param spnts_by_station 2D vector of spacepoints, sorted by station
     *  @param outer_station_num outermost station being used
     *                           (e.g. station 5 in a 5pt track)
     *  @param inner_station_num innermost station being used
     *                           (e.g. station 1 in a 5pt track)
     *  @param ignore_stations int vector specifying which stations are not to be used for
     *                         the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *                         while -1 means use *all* the stations (ignore none of them). 
     *                         The size of the vector should be 0 for a 5pt track,
     *                         1 for a 4pt track, and 2 for a 3pt track.
     *  @param station_outer_sp index representing outer station spacepoint
     *  @param station_inner_sp index representing inner station spacepoint
     *  @param good_spnts map between station number and spacepoint number in station,
     *                    holding those spacepoints (if any) which best match the
     *                    initial line
     */
    void initial_line(const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                      const std::vector<int> ignore_stations,
                      const int outer_station_num, const int inner_station_num,
                      const int station_outer_sp, const int station_inner_sp,
                      std::map<int, SciFiSpacePoint*> &good_spnts);

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
                               std::vector<SciFiStraightPRTrack> &trks);

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
                      std::vector<SciFiStraightPRTrack> &trks);

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
         std::vector<SciFiStraightPRTrack>& trks);

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
                     std::vector<SciFiStraightPRTrack> &trks);

    /** @brief Fit a straight line in x and y to some spacepoints
     *
     *  Fit straight lines, x = f(z) and y = f(z), using gsl linear fitter,
     *  for input spacepoints. Output is the line in x and line in y.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param line_x - Output line in x - z plane
     *  @param line_y - Output line in y - z plane
     *
     */
    void linear_fit(const std::map<int, SciFiSpacePoint*> &spnts,
                    SimpleLine &line_x, SimpleLine &line_y);

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

    /** @brief Perform Pattern Recognition for straight tracks
     * 
     *  This takes one argument, the SciFi event, extracts the
     *  spacepoints, reconstructs the PR tracks, and pushes them
     *  back into the event.
     *
     *  @param evt - The SciFi event
     */
    void straight_track_recon(SciFiEvent &evt);

    // Summer added below member functions *************

    static const int _n_trackers = 2;
    static const int _n_stations = 5;
    static const double _sd_1to4 = 0.3844;
    static const double _sd_5 = 0.4298;
    static const double _res_cut = 10;
    static const double _chisq_cut = 15;
};
// } // ~namespace MAUS

#endif
