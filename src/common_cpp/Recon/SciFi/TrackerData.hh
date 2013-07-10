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

/** @class TrackerData
 *
 * A class to hold the data for one of the tracker detectors from multiple events,
 * in a format (vectors of doubles, ints, etc) which can be easily plotted using ROOT via
 * the SciFiTrackerDataManager class.
 *
 */

#ifndef _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATA_
#define _SRC_COMMON_CPP_RECON_SCIFI_TRACKERDATA_

// C++ headers
#include <vector>

// ROOT headers
#include "TArc.h"
#include "TF1.h"

#include "gtest/gtest_prod.h"

// MAUS headers


namespace MAUS {

class TrackerData {
  public:

    /** Make various classes a friend, far easier than writing all the getters and setters */
    friend class TrackerDataManager;
    friend class ReduceCppPatternRecognition;
    friend class TrackerDataPlotterBase;
    friend class TrackerDataPlotterXYZ;
    friend class TrackerDataPlotterTracks;
    friend class TrackerDataPlotterSpoints;
    friend class TrackerDataPlotterInfoBox;

    // Macros to allow friendship with the gtests
    FRIEND_TEST(TrackerDataTest, TestConstructor);
    FRIEND_TEST(TrackerDataTest, TestClear);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessDigits);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessClusters);
    FRIEND_TEST(TrackerDataManagerTest, TestProcessHtrks);

    /** Constructor */
    TrackerData();

    /** Destructor */
    ~TrackerData();

    /** Clear all data, and set all vectors to zero size */
    void clear();

  protected:
    int _spill_num;
    int _trker_num;
    int _num_events;
    int _num_digits;
    int _num_clusters;
    int _num_spoints;
    int _num_seeds;
    int _num_stracks_5pt;
    int _num_stracks_4pt;
    int _num_stracks_3pt;
    int _num_htracks_5pt;
    int _num_htracks_4pt;
    int _num_htracks_3pt;

    // Spacepoint data
    std::vector<double> _spoints_x;
    std::vector<double> _spoints_y;
    std::vector<double> _spoints_z;

    // Data associated specifically with spacepoints associated with tracks
    // (2D vectors as have multiple tracks in multiple events)
    std::vector< std::vector<double> > _seeds_x;
    std::vector< std::vector<double> > _seeds_y;
    std::vector< std::vector<double> > _seeds_z;
    std::vector< std::vector<double> > _seeds_phi;
    std::vector< std::vector<double> > _seeds_s;

    // Vectors of the functions representing the helical tracks in various projections
    std::vector<TF1> _trks_str_xz;
    std::vector<TF1> _trks_str_yz;
    std::vector<TArc> _trks_xy;
    std::vector<TF1> _trks_xz;
    std::vector<TF1> _trks_yz;
    std::vector<TF1> _trks_sz;
};

} // ~namespace MAUS

#endif
