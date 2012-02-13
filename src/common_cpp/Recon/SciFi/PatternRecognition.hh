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

    /** @brief Perform Pattern Recognition for straight tracks
     *
     *  This takes one argument, the SciFi event, extracts the
     *  spacepoints, reconstructs the PR tracks, and pushes them
     *  back into the event.
     *
     *  @param evt - The SciFi event
     */
    void straightprtrack_recon(SciFiEvent &evt);

    /** @brief Make straight Pattern Recognition tracks with 5 spacepoints
     *
     *  Make a straight Pattern Recognition track/s when there are spacepoints
     *  found in all 5 stations of a tracker.  Least squared fitting used,
     *  together with a chi^2 goodness-of-fit test.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_spr_5pt(const std::vector<SciFiSpacePoint*>& spnts,
                      std::vector<SciFiStraightPRTrack>& trks);

    /** @brief Make straight Pattern Recognition tracks with 4 spacepoints
     *
     *  Make a straight Pattern Recognition track/s when there at least 4 stations
     *  with spacepoints in them. Least squared fitting used, together with a
     *  chi^2 goodness-of-fit test.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_spr_4pt(const std::vector<SciFiSpacePoint*>& spnts,
                      std::vector<SciFiStraightPRTrack>& trks);

    /** @brief Make straight Pattern Recognition tracks with 3 spacepoints
     *
     *  Make a straight Pattern Recognition track/s when there at least 3 stations
     *  with spacepoints in them. Least squared fitting used, together with a
     *  chi^2 goodness-of-fit test.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param trks - A vector of the output Pattern Recognition tracks
     */
    void make_spr_3pt(const std::vector<SciFiSpacePoint*>& spnts,
                      std::vector<SciFiStraightPRTrack>& trks);

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
    void linear_fit(std::map<int, SciFiSpacePoint*> &spnts,
                    SimpleLine &line_x, SimpleLine &line_y);

    /** @brief Create a 2D vector of SciFi spacepoints sorted by tracker station
     *
     *  Take an input vector of spacepoints and output a 2D vector of spacepoints
     *  where the first index is the station the spacepoint is located.
     *
     *  @param spnts - A vector of all the input spacepoints
     *  @param spnts_stat - Output 2D vector of spacepoints sorted by station
     *
     */
    void sort_by_station(const std::vector<SciFiSpacePoint*>& spnts,
                        std::vector< std::vector<SciFiSpacePoint*> >& spnts_stat);

    static const int _n_trackers = 2;
    static const int _n_stations = 5;
    static const double _res_cut = 15.0;
    static const double _chisq_cut = 15.0;
};
// }// ~namespace MAUS

#endif
