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

#include <cmath>

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/SciFiChannelId.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"

namespace MAUS {

class SciFiAnalysisToolsTest : public ::testing::Test {
 protected:
  SciFiAnalysisToolsTest()  {}
  virtual ~SciFiAnalysisToolsTest() {}
};

TEST_F(SciFiAnalysisToolsTest, test_find_n_valid_mc_track) {
  // Set up, with the track causing hits in all 5 tracker stations in both trackers
  int track_id = 1;
  int n_stations = 5;

  std::vector<MAUS::SciFiHit>* hits = new std::vector<MAUS::SciFiHit>();
  for (int i = 0; i < n_stations; ++i) {
    SciFiHit hit;
    hit.SetTrackId(track_id);
    hit.SetChannelId(new SciFiChannelId());
    hit.GetChannelId()->SetStationNumber(i+1);
    hit.GetChannelId()->SetTrackerNumber(0);
    hits->push_back(hit);
  }
  for (int i = 0; i < n_stations; ++i) {
    SciFiHit hit;
    hit.SetTrackId(track_id);
    hit.SetChannelId(new SciFiChannelId());
    hit.GetChannelId()->SetStationNumber(i+1);
    hit.GetChannelId()->SetTrackerNumber(1);
    hits->push_back(hit);
  }

  // Set up the MC track
  Track mc_trk;
  mc_trk.SetTrackId(track_id);
  std::vector<Track>* tracks = new std::vector<Track>();
  tracks->push_back(mc_trk);

  // Set up the MC event
  MCEvent* mc_evt = new MCEvent();
  mc_evt->SetTracks(tracks);
  mc_evt->SetSciFiHits(hits);

  // Run the function
  int n_tracks_t1 = 0;
  int n_tracks_t2 = 0;
  SciFiAnalysisTools::find_n_valid_mc_track(0, mc_evt, n_tracks_t1, n_tracks_t2);

  EXPECT_EQ(1, n_tracks_t1);
  EXPECT_EQ(1, n_tracks_t2);
}

TEST_F(SciFiAnalysisToolsTest, test_is_valid_mc_track) {

  // Set up, with the track causing hits in all 5 tracker stations
  int tracker_num = 0;
  int track_id = 1;
  int n_stations = 5;

  std::vector<SciFiHit*> hits(n_stations);
  for (int i = 0; i < n_stations; ++i) {
    hits[i] = new SciFiHit();
    hits[i]->SetTrackId(track_id);
    hits[i]->SetChannelId(new SciFiChannelId());
    hits[i]->GetChannelId()->SetStationNumber(i+1);
    hits[i]->GetChannelId()->SetTrackerNumber(tracker_num);
  }

  Track* mc_trk = new Track();
  mc_trk->SetTrackId(track_id);

  EXPECT_TRUE(SciFiAnalysisTools::is_valid_mc_track(tracker_num, mc_trk, hits));

  // Now set so that hits occur only in stations 1 to 3
  hits[3]->GetChannelId()->SetStationNumber(3);
  hits[4]->GetChannelId()->SetStationNumber(3);

  EXPECT_FALSE(SciFiAnalysisTools::is_valid_mc_track(tracker_num, mc_trk, hits));

  // Clean up
  for (int i = 0; i < n_stations; ++i) {
    delete hits[i];
  }
}

} // ~namespace MAUS
