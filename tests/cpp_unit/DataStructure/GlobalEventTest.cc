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

#include <cmath>
#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/GlobalEvent.hh"

namespace MAUS {

/** Tests everything in the data structure from spill up to GlobalEvent
 */
class GlobalEventTestDS : public ::testing::Test {
 protected:
  
  GlobalEventTestDS() {}
  virtual ~GlobalEventTestDS() {}
  virtual void SetUp() {
    // Fill Cpp GlobalEvent
    for(int i = 0; i < 4; ++i) {
      _trackpoint.push_back(new MAUS::recon::global::TrackPoint());
      _spacepoint.push_back(new MAUS::recon::global::SpacePoint());
      _spacepoint[i]->set_charge(1. * i);
      _trackpoint[i]->set_spacepoint(_spacepoint[i]);
    }
    
    for(int i = 0; i < 2; ++i) {
      _track.push_back(new MAUS::recon::global::Track());
      _track[i]->AddTrackPoint(_trackpoint[2*i] );
      _track[i]->AddTrackPoint(_trackpoint[2*i + 1]);
    }
    
    _chain = new MAUS::recon::global::PrimaryChain();
    _chain->AddPrimaryTrack(_track[0]);
    _chain->AddTrack(_track[1], _track[0]);
    
    _event = new MAUS::GlobalEvent();
    _event->add_primarychain_recursive(_chain);

    local_chain = NULL;
    local_track.resize(2);
    local_trackpoint.resize(4);
    local_spacepoint.resize(4);    
  }
  virtual void TearDown() {}

  MAUS::GlobalEvent *_event;
  MAUS::recon::global::PrimaryChain* _chain;
  std::vector<MAUS::recon::global::Track*> _track;
  std::vector<MAUS::recon::global::TrackPoint*> _trackpoint;
  std::vector<MAUS::recon::global::SpacePoint*> _spacepoint;
  
  MAUS::recon::global::PrimaryChain *local_chain;
  std::vector<MAUS::recon::global::Track*> local_track;
  std::vector<MAUS::recon::global::TrackPoint*> local_trackpoint;
  std::vector<MAUS::recon::global::SpacePoint*> local_spacepoint;
  
};

TEST_F(GlobalEventTestDS, test_all_constructors) {
  EXPECT_TRUE(_event);
  EXPECT_TRUE(_chain);

  for(int i = 0; i < 2; ++i){
    EXPECT_TRUE(_track[i]);
  }
  
  for(int i = 0; i < 4; ++i) {
    EXPECT_TRUE(_trackpoint[i]);
    EXPECT_TRUE(_spacepoint[i]);
    double charge = _spacepoint[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
  }
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

TEST_F(GlobalEventTestDS, test_recursive_add) {
  GlobalEvent event;

  GlobalPrimaryChain* global_chain = new GlobalPrimaryChain();
  GlobalTrack*        global_track = new GlobalTrack();
  GlobalTrackPoint*   global_trackpoint = new GlobalTrackPoint();
  GlobalSpacePoint*   global_spacepoint = new GlobalSpacePoint();

  global_trackpoint->set_spacepoint(global_spacepoint);
  global_track->AddTrackPoint(global_trackpoint);
  global_chain->AddPrimaryTrack(global_track);

  event.add_primarychain_recursive(global_chain);
  
  ASSERT_TRUE(event.get_primarychains());
  EXPECT_EQ(event.get_primarychains()->size(), 1U);
  EXPECT_EQ(event.get_primarychains()->at(0), global_chain);

  ASSERT_TRUE(event.get_tracks());
  EXPECT_EQ(event.get_tracks()->size(), 1U);
  EXPECT_EQ(event.get_tracks()->at(0), global_track);

  ASSERT_TRUE(event.get_trackpoints());
  EXPECT_EQ(event.get_trackpoints()->size(), 1U);
  EXPECT_EQ(event.get_trackpoints()->at(0), global_trackpoint);

  ASSERT_TRUE(event.get_spacepoints());
  EXPECT_EQ(event.get_spacepoints()->size(), 1U);
  EXPECT_EQ(event.get_spacepoints()->at(0), global_spacepoint);
}

}

