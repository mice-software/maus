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
// C headers

// C++ headers
#include <vector>
#include <string>

#include "src/common_cpp/Recon/SciFiEvent.hh"
#include "src/common_cpp/Recon/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFiStraightPRTrack.hh"

class PatternRecognition {
  public:

    PatternRecognition();

    ~PatternRecognition();

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

  /** @brief Make a straight Pattern Recognition track with 5 spacepoints
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

  void make_spr_4pt(const std::vector<SciFiSpacePoint*>& spnts,
                    std::vector<SciFiStraightPRTrack>& trks);

  void make_spr_3pt(const std::vector<SciFiSpacePoint*>& spnts,
                    std::vector<SciFiStraightPRTrack>& trks);

  void sort_by_station(const std::vector<SciFiSpacePoint*>& spnts,
                       std::vector< std::vector<SciFiSpacePoint*> >& spnts_stat);


  void run_analysis(std::vector<SciFiStraightPRTrack>& trks);
  void a_4_station_fit(std::vector<SciFiSpacePoint> space_points, Hep3Vector position, int st_i);

  static const int _n_trackers = 2;
  static const int _n_stations = 5;
  static const double _res_cut = 15.0;
  static const double _chisq_cut = 15.0;
};

#endif
