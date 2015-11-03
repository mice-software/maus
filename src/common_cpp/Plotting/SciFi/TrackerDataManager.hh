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

/** @class TrackerDataManager
 *
 * A class used to draw tracker data on a spill-by-spill basis using ROOT.
 * TrackerData considers TrackerDataManager a friend, but not vice-versa.
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAMANAGER_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATAMANAGER_

// C++ headers
#include <vector>

// ROOT headers
#include "TArc.h"
#include "TF1.h"

// Google test headers
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerData.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"


namespace MAUS {

class TrackerDataManager {
  public:

    /** Make the Pattern Recognition reducer a friend for testing purposes */
    friend class ReduceCppPatternRecognition;

    // Macros to allow friendship with the gtests
    FRIEND_TEST(TrackerDataManagerTest, TestConstructor);
    FRIEND_TEST(TrackerDataManagerTest, TestClear);
    FRIEND_TEST(TrackerDataManagerTest, TestDraw);
    FRIEND_TEST(TrackerDataManagerTest, TestProcess);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessDigits);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessClusters);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessSpoints);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessStrks);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessHtrks);

    /** Constructor */
    TrackerDataManager();

    /** Destructor */
    ~TrackerDataManager();

    /** Clear data for this spill only */
    void clear_spill();

    /**  Draw data using TrackerDataPlotter classes */
    void draw(std::vector<TrackerDataPlotterBase*> plotters);

    /** Get whether to print track info to screen */
    bool get_print_tracks() { return _print_tracks; }

    /** Get whether to print seed info to screen */
    bool get_print_seeds() { return _print_seeds; }

    /** Return tracker 1 data container */
    TrackerData get_t1_data() { return _t1; }

    /** Return tracker 2 data container */
    TrackerData get_t2_data() { return _t2; }

    /** Make a circle using ROOT TArc function */
    TArc make_circle(double x0, double y0, double rad);

    /** Make a straight line using ROOT TF1 */
    TF1 make_str_track(double c, double m, double zmin, double zmax);

    /** Make a function for the z-x projection of the helix */
    TF1 make_xz(int handness, double circle_x0, double rad, double dsdz,
                double sz_c, double zmin, double zmax);

    /** Make a function for the z-y projection of the helix */
    TF1 make_yz(double circle_y0, double rad, double dsdz, double sz_c, double zmin, double zmax);

    /** Main control function, processes a spill */
    void process(const Spill *spill);

    /** Process the scifi digits in the current event.  Need to pass the spill as well
        as we use the digits to pull out the number of recon events in each tracker.
     */
    void process_digits(const Spill *spill, const std::vector<SciFiDigit*> digits);

    /** Process the scifi clusters in the current event */
    void process_clusters(const std::vector<SciFiCluster*> clusters);

    /** Process the scifi spacepoints in the current event */
    void process_spoints(const std::vector<SciFiSpacePoint*> spoints);

    /** Process the scifi pattern recognition straight tracks in the current event */
    void process_strks(const std::vector<SciFiStraightPRTrack*> strks);

    /** Process the scifi pattern recognition helical tracks in the current event */
    void process_htrks(const std::vector<SciFiHelicalPRTrack*> htrks);

    /** Print info about the track */
    void print_track_info(const SciFiHelicalPRTrack * const trk, int trk_num);

    // Print info about the seeds in a track */
    // void print_seed_info(const SciFiHelicalPRTrack * const trk, int seed_num);

    /** Set whether to print track info to screen */
    void set_print_tracks(bool b) { _print_tracks = b; }

    /** Set whether to print seed info to screen */
    void set_print_seeds(bool b) { _print_seeds = b; }

  protected:
    bool _print_tracks;
    bool _print_seeds;

    SciFiLookup _lookup;

    // The data associated with each tracker for one spill
    TrackerData _t1;
    TrackerData _t2;

  private:
    // Bit of physics, factor translating radius to transverse momentum, pt = _RtoPt * rad
    static constexpr double _RtoPt = 1.2;

    // Some plotting parameters
    static constexpr double _zmin = 0.0;
    static constexpr double _zmax = 1200.0;
};

} // ~namespace MAUS

#endif
