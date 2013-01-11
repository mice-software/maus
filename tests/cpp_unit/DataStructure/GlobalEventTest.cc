/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/GlobalEvent.hh"

namespace MAUS {

/** Tests everything in the data structure from spill up to GlobalEvent
 */
class GlobalEventTestDS : public ::testing::Test {
 protected:
  
  GlobalEventTestDS() {}
  virtual ~GlobalEventTestDS() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// all one function - urk
TEST_F(GlobalEventTestDS, test_all_constructors) {
  GlobalEvent*        global_event = new GlobalEvent();
  GlobalPrimaryChain* global_chain = new GlobalPrimaryChain();
  GlobalTrack*        global_track = new GlobalTrack();
  GlobalTrackPoint*   global_trackpoint = new GlobalTrackPoint();
  GlobalSpacePoint*   global_spacepoint = new GlobalSpacePoint();
    
  EXPECT_TRUE(global_event != NULL);
  EXPECT_TRUE(global_chain != NULL);
  EXPECT_TRUE(global_track != NULL);
  EXPECT_TRUE(global_trackpoint != NULL);
  EXPECT_TRUE(global_spacepoint != NULL);
}

TEST_F(GlobalEventTestDS, test_all_allocators) {
  GlobalEvent event;

  GlobalPrimaryChain* global_chain = new GlobalPrimaryChain();
  GlobalTrack*        global_track = new GlobalTrack();
  GlobalTrackPoint*   global_trackpoint = new GlobalTrackPoint();
  GlobalSpacePoint*   global_spacepoint = new GlobalSpacePoint();

  GlobalPrimaryChainPArray* chains = new GlobalPrimaryChainPArray();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  GlobalTrackPArray* tracks = new GlobalTrackPArray();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  GlobalTrackPointPArray* trackpoints
      = new GlobalTrackPointPArray();
  trackpoints->push_back(global_trackpoint);
  event.set_trackpoints(trackpoints);

  GlobalSpacePointPArray* spacepoints
      = new GlobalSpacePointPArray();
  spacepoints->push_back(global_spacepoint);
  event.set_spacepoints(spacepoints);

  EXPECT_EQ(event.get_primarychains()->size(), chains->size());
  EXPECT_EQ(event.get_primarychains()->at(0), chains->at(0));

  EXPECT_EQ(event.get_tracks()->size(), tracks->size());
  EXPECT_EQ(event.get_tracks()->at(0), tracks->at(0));

  EXPECT_EQ(event.get_trackpoints()->size(), trackpoints->size());
  EXPECT_EQ(event.get_trackpoints()->at(0), trackpoints->at(0));

  EXPECT_EQ(event.get_spacepoints()->size(), spacepoints->size());
  EXPECT_EQ(event.get_spacepoints()->at(0), spacepoints->at(0));
}

TEST_F(GlobalEventTestDS, test_copy_constructor) {
  GlobalEvent event;

  GlobalPrimaryChain* global_chain = new GlobalPrimaryChain();
  GlobalTrack*        global_track = new GlobalTrack();
  GlobalTrackPoint*   global_trackpoint = new GlobalTrackPoint();
  GlobalSpacePoint*   global_spacepoint = new GlobalSpacePoint();

  GlobalPrimaryChainPArray* chains = new GlobalPrimaryChainPArray();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  GlobalTrackPArray* tracks = new GlobalTrackPArray();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  GlobalTrackPointPArray* trackpoints
      = new GlobalTrackPointPArray();
  trackpoints->push_back(global_trackpoint);
  event.set_trackpoints(trackpoints);

  GlobalSpacePointPArray* spacepoints
      = new GlobalSpacePointPArray();
  spacepoints->push_back(global_spacepoint);
  event.set_spacepoints(spacepoints);

  GlobalEvent event_copy(event);

  ASSERT_TRUE(event_copy.get_primarychains());
  EXPECT_NE(event_copy.get_primarychains(), event.get_primarychains());
  EXPECT_EQ(event_copy.get_primarychains()->size(), chains->size());
  EXPECT_NE(event_copy.get_primarychains()->at(0), chains->at(0));

  ASSERT_TRUE(event_copy.get_tracks());
  EXPECT_NE(event_copy.get_tracks(), event.get_tracks());
  EXPECT_EQ(event_copy.get_tracks()->size(), tracks->size());
  EXPECT_NE(event_copy.get_tracks()->at(0), tracks->at(0));

  ASSERT_TRUE(event_copy.get_trackpoints());
  EXPECT_NE(event_copy.get_trackpoints(), event.get_trackpoints());
  EXPECT_EQ(event_copy.get_trackpoints()->size(), trackpoints->size());
  EXPECT_NE(event_copy.get_trackpoints()->at(0), trackpoints->at(0));

  ASSERT_TRUE(event_copy.get_spacepoints());
  EXPECT_NE(event_copy.get_spacepoints(), event.get_spacepoints());
  EXPECT_EQ(event_copy.get_spacepoints()->size(), spacepoints->size());
  EXPECT_NE(event_copy.get_spacepoints()->at(0), spacepoints->at(0));
}

TEST_F(GlobalEventTestDS, test_equality_operator) {
  GlobalEvent event;

  GlobalPrimaryChain* global_chain = new GlobalPrimaryChain();
  GlobalTrack*        global_track = new GlobalTrack();
  GlobalTrackPoint*   global_trackpoint = new GlobalTrackPoint();
  GlobalSpacePoint*   global_spacepoint = new GlobalSpacePoint();

  GlobalPrimaryChainPArray* chains = new GlobalPrimaryChainPArray();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  GlobalTrackPArray* tracks = new GlobalTrackPArray();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  GlobalTrackPointPArray* trackpoints
      = new GlobalTrackPointPArray();
  trackpoints->push_back(global_trackpoint);
  event.set_trackpoints(trackpoints);

  GlobalSpacePointPArray* spacepoints
      = new GlobalSpacePointPArray();
  spacepoints->push_back(global_spacepoint);
  event.set_spacepoints(spacepoints);

  GlobalEvent event_equal = event;

  ASSERT_TRUE(event_equal.get_primarychains());
  EXPECT_NE(event_equal.get_primarychains(), event.get_primarychains());
  EXPECT_EQ(event_equal.get_primarychains()->size(), chains->size());
  EXPECT_NE(event_equal.get_primarychains()->at(0), chains->at(0));

  ASSERT_TRUE(event_equal.get_tracks());
  EXPECT_NE(event_equal.get_tracks(), event.get_tracks());
  EXPECT_EQ(event_equal.get_tracks()->size(), tracks->size());
  EXPECT_NE(event_equal.get_tracks()->at(0), tracks->at(0));

  ASSERT_TRUE(event_equal.get_trackpoints());
  EXPECT_NE(event_equal.get_trackpoints(), event.get_trackpoints());
  EXPECT_EQ(event_equal.get_trackpoints()->size(), trackpoints->size());
  EXPECT_NE(event_equal.get_trackpoints()->at(0), trackpoints->at(0));

  ASSERT_TRUE(event_equal.get_spacepoints());
  EXPECT_NE(event_equal.get_spacepoints(), event.get_spacepoints());
  EXPECT_EQ(event_equal.get_spacepoints()->size(), spacepoints->size());
  EXPECT_NE(event_equal.get_spacepoints()->at(0), spacepoints->at(0));  
}

TEST_F(GlobalEventTestDS, test_null_copy) {
  GlobalEvent event;

  GlobalPrimaryChain* global_chain = new GlobalPrimaryChain();
  GlobalTrack*        global_track = new GlobalTrack();
  GlobalTrackPoint*   global_trackpoint = new GlobalTrackPoint();
  GlobalSpacePoint*   global_spacepoint = new GlobalSpacePoint();

  GlobalPrimaryChainPArray* chains = new GlobalPrimaryChainPArray();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  GlobalTrackPArray* tracks = new GlobalTrackPArray();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  GlobalTrackPointPArray* trackpoints
      = new GlobalTrackPointPArray();
  trackpoints->push_back(global_trackpoint);
  event.set_trackpoints(trackpoints);

  GlobalSpacePointPArray* spacepoints
      = new GlobalSpacePointPArray();
  spacepoints->push_back(global_spacepoint);
  event.set_spacepoints(spacepoints);

  // Test that we free okay on reallocation
  GlobalEvent event_equal = event;

  // Test that equality on myself doesn't cause trouble
  event_equal = event_equal;

  // Check that we handle copy of NULL data okay
  GlobalEvent default_event;
  event_equal = default_event;
  ASSERT_TRUE(event_equal.get_primarychains());
  EXPECT_EQ(event_equal.get_primarychains()->size(), 0U);

  ASSERT_TRUE(event_equal.get_tracks());
  EXPECT_EQ(event_equal.get_tracks()->size(), 0U);

  ASSERT_TRUE(event_equal.get_trackpoints());
  EXPECT_EQ(event_equal.get_trackpoints()->size(), 0U);

  ASSERT_TRUE(event_equal.get_spacepoints());
  EXPECT_EQ(event_equal.get_spacepoints()->size(), 0U);
}

}

