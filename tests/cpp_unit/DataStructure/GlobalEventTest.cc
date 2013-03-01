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
      _track_point.push_back(new MAUS::DataStructure::Global::TrackPoint());
      _space_point.push_back(new MAUS::DataStructure::Global::SpacePoint());
      _space_point[i]->set_charge(1. * i);
      _track_point[i]->set_space_point(_space_point[i]);
    }
    
    for(int i = 0; i < 2; ++i) {
      _track.push_back(new MAUS::DataStructure::Global::Track());
      _track[i]->AddTrackPoint(_track_point[2*i] );
      _track[i]->AddTrackPoint(_track_point[2*i + 1]);
    }
    
    _chain = new MAUS::DataStructure::Global::PrimaryChain();
    _chain->AddPrimaryTrack(_track[0]);
    _chain->AddTrack(_track[1], _track[0]);
    
    _event = new MAUS::GlobalEvent();
    _event->add_primarychain_recursive(_chain);

    local_chain = NULL;
    local_track.resize(2);
    local_track_point.resize(4);
    local_space_point.resize(4);    
  }
  virtual void TearDown() {}

  MAUS::GlobalEvent *_event;
  MAUS::DataStructure::Global::PrimaryChain* _chain;
  std::vector<MAUS::DataStructure::Global::Track*> _track;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> _track_point;
  std::vector<MAUS::DataStructure::Global::SpacePoint*> _space_point;
  
  MAUS::DataStructure::Global::PrimaryChain *local_chain;
  std::vector<MAUS::DataStructure::Global::Track*> local_track;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> local_track_point;
  std::vector<MAUS::DataStructure::Global::SpacePoint*> local_space_point;
  
};

TEST_F(GlobalEventTestDS, test_all_constructors) {
  EXPECT_TRUE(_event);
  EXPECT_TRUE(_chain);

  for(int i = 0; i < 2; ++i){
    EXPECT_TRUE(_track[i]);
  }
  
  for(int i = 0; i < 4; ++i) {
    EXPECT_TRUE(_track_point[i]);
    EXPECT_TRUE(_space_point[i]);
    double charge = _space_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
  }
}

TEST_F(GlobalEventTestDS, test_all_allocators) {
  GlobalEvent event;

  MAUS::DataStructure::Global::PrimaryChain* global_chain =
      new MAUS::DataStructure::Global::PrimaryChain();
  MAUS::DataStructure::Global::Track*        global_track =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::TrackPoint*   global_track_point =
      new MAUS::DataStructure::Global::TrackPoint();
  MAUS::DataStructure::Global::SpacePoint*   global_space_point =
      new MAUS::DataStructure::Global::SpacePoint();

  std::vector<MAUS::DataStructure::Global::PrimaryChain*>* chains =
      new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  std::vector<MAUS::DataStructure::Global::Track*>* tracks =
      new std::vector<MAUS::DataStructure::Global::Track*>();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  std::vector<MAUS::DataStructure::Global::TrackPoint*>* track_points
      = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
  track_points->push_back(global_track_point);
  event.set_track_points(track_points);

  std::vector<MAUS::DataStructure::Global::SpacePoint*>* space_points
      = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
  space_points->push_back(global_space_point);
  event.set_space_points(space_points);

  EXPECT_EQ(event.get_primarychains()->size(), chains->size());
  EXPECT_EQ(event.get_primarychains()->at(0), chains->at(0));

  EXPECT_EQ(event.get_tracks()->size(), tracks->size());
  EXPECT_EQ(event.get_tracks()->at(0), tracks->at(0));

  EXPECT_EQ(event.get_track_points()->size(), track_points->size());
  EXPECT_EQ(event.get_track_points()->at(0), track_points->at(0));

  EXPECT_EQ(event.get_space_points()->size(), space_points->size());
  EXPECT_EQ(event.get_space_points()->at(0), space_points->at(0));
}

TEST_F(GlobalEventTestDS, test_copy_constructor) {
  GlobalEvent event;

  MAUS::DataStructure::Global::PrimaryChain* global_chain =
      new MAUS::DataStructure::Global::PrimaryChain();
  MAUS::DataStructure::Global::Track*        global_track =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::TrackPoint*   global_track_point =
      new MAUS::DataStructure::Global::TrackPoint();
  MAUS::DataStructure::Global::SpacePoint*   global_space_point =
      new MAUS::DataStructure::Global::SpacePoint();

  std::vector<MAUS::DataStructure::Global::PrimaryChain*>* chains =
      new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  std::vector<MAUS::DataStructure::Global::Track*>* tracks =
      new std::vector<MAUS::DataStructure::Global::Track*>();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  std::vector<MAUS::DataStructure::Global::TrackPoint*>* track_points
      = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
  track_points->push_back(global_track_point);
  event.set_track_points(track_points);

  std::vector<MAUS::DataStructure::Global::SpacePoint*>* space_points
      = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
  space_points->push_back(global_space_point);
  event.set_space_points(space_points);

  GlobalEvent event_copy(event);

  ASSERT_TRUE(event_copy.get_primarychains());
  EXPECT_NE(event_copy.get_primarychains(), event.get_primarychains());
  EXPECT_EQ(event_copy.get_primarychains()->size(), chains->size());
  EXPECT_NE(event_copy.get_primarychains()->at(0), chains->at(0));

  ASSERT_TRUE(event_copy.get_tracks());
  EXPECT_NE(event_copy.get_tracks(), event.get_tracks());
  EXPECT_EQ(event_copy.get_tracks()->size(), tracks->size());
  EXPECT_NE(event_copy.get_tracks()->at(0), tracks->at(0));

  ASSERT_TRUE(event_copy.get_track_points());
  EXPECT_NE(event_copy.get_track_points(), event.get_track_points());
  EXPECT_EQ(event_copy.get_track_points()->size(), track_points->size());
  EXPECT_NE(event_copy.get_track_points()->at(0), track_points->at(0));

  ASSERT_TRUE(event_copy.get_space_points());
  EXPECT_NE(event_copy.get_space_points(), event.get_space_points());
  EXPECT_EQ(event_copy.get_space_points()->size(), space_points->size());
  EXPECT_NE(event_copy.get_space_points()->at(0), space_points->at(0));
}

TEST_F(GlobalEventTestDS, test_null_copy_constructor) {
  GlobalEvent event;

  std::vector<MAUS::DataStructure::Global::PrimaryChain*>* chains = NULL;
  event.set_primarychains(chains);

  std::vector<MAUS::DataStructure::Global::Track*>* tracks = NULL;
  event.set_tracks(tracks);

  std::vector<MAUS::DataStructure::Global::TrackPoint*>* track_points = NULL;
  event.set_track_points(track_points);

  std::vector<MAUS::DataStructure::Global::SpacePoint*>* space_points = NULL;
  event.set_space_points(space_points);

  GlobalEvent event_copy1(event);
  GlobalEvent event_copy2 = event;

  ASSERT_FALSE(event_copy1.get_primarychains());
  ASSERT_FALSE(event_copy1.get_tracks());
  ASSERT_FALSE(event_copy1.get_track_points());
  ASSERT_FALSE(event_copy1.get_space_points());

  ASSERT_FALSE(event_copy2.get_primarychains());
  ASSERT_FALSE(event_copy2.get_tracks());
  ASSERT_FALSE(event_copy2.get_track_points());
  ASSERT_FALSE(event_copy2.get_space_points());
}

TEST_F(GlobalEventTestDS, test_equality_operator) {
  GlobalEvent event;

  MAUS::DataStructure::Global::PrimaryChain* global_chain =
      new MAUS::DataStructure::Global::PrimaryChain();
  MAUS::DataStructure::Global::Track*        global_track =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::TrackPoint*   global_track_point =
      new MAUS::DataStructure::Global::TrackPoint();
  MAUS::DataStructure::Global::SpacePoint*   global_space_point =
      new MAUS::DataStructure::Global::SpacePoint();

  std::vector<MAUS::DataStructure::Global::PrimaryChain*>* chains =
      new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  std::vector<MAUS::DataStructure::Global::Track*>* tracks =
      new std::vector<MAUS::DataStructure::Global::Track*>();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  std::vector<MAUS::DataStructure::Global::TrackPoint*>* track_points
      = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
  track_points->push_back(global_track_point);
  event.set_track_points(track_points);

  std::vector<MAUS::DataStructure::Global::SpacePoint*>* space_points
      = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
  space_points->push_back(global_space_point);
  event.set_space_points(space_points);

  GlobalEvent event_equal = event;

  ASSERT_TRUE(event_equal.get_primarychains());
  EXPECT_NE(event_equal.get_primarychains(), event.get_primarychains());
  EXPECT_EQ(event_equal.get_primarychains()->size(), chains->size());
  EXPECT_NE(event_equal.get_primarychains()->at(0), chains->at(0));

  ASSERT_TRUE(event_equal.get_tracks());
  EXPECT_NE(event_equal.get_tracks(), event.get_tracks());
  EXPECT_EQ(event_equal.get_tracks()->size(), tracks->size());
  EXPECT_NE(event_equal.get_tracks()->at(0), tracks->at(0));

  ASSERT_TRUE(event_equal.get_track_points());
  EXPECT_NE(event_equal.get_track_points(), event.get_track_points());
  EXPECT_EQ(event_equal.get_track_points()->size(), track_points->size());
  EXPECT_NE(event_equal.get_track_points()->at(0), track_points->at(0));

  ASSERT_TRUE(event_equal.get_space_points());
  EXPECT_NE(event_equal.get_space_points(), event.get_space_points());
  EXPECT_EQ(event_equal.get_space_points()->size(), space_points->size());
  EXPECT_NE(event_equal.get_space_points()->at(0), space_points->at(0));  
}

TEST_F(GlobalEventTestDS, test_null_copy) {
  GlobalEvent event;

  MAUS::DataStructure::Global::PrimaryChain* global_chain =
      new MAUS::DataStructure::Global::PrimaryChain();
  MAUS::DataStructure::Global::Track*        global_track =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::TrackPoint*   global_track_point =
      new MAUS::DataStructure::Global::TrackPoint();
  MAUS::DataStructure::Global::SpacePoint*   global_space_point =
      new MAUS::DataStructure::Global::SpacePoint();

  std::vector<MAUS::DataStructure::Global::PrimaryChain*>* chains =
      new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
  chains->push_back(global_chain);
  event.set_primarychains(chains);

  std::vector<MAUS::DataStructure::Global::Track*>* tracks =
      new std::vector<MAUS::DataStructure::Global::Track*>();
  tracks->push_back(global_track);
  event.set_tracks(tracks);

  std::vector<MAUS::DataStructure::Global::TrackPoint*>* track_points
      = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
  track_points->push_back(global_track_point);
  event.set_track_points(track_points);

  std::vector<MAUS::DataStructure::Global::SpacePoint*>* space_points
      = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
  space_points->push_back(global_space_point);
  event.set_space_points(space_points);

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

  ASSERT_TRUE(event_equal.get_track_points());
  EXPECT_EQ(event_equal.get_track_points()->size(), 0U);

  ASSERT_TRUE(event_equal.get_space_points());
  EXPECT_EQ(event_equal.get_space_points()->size(), 0U);
}

TEST_F(GlobalEventTestDS, test_recursive_add) {
  GlobalEvent event;

  MAUS::DataStructure::Global::PrimaryChain* global_chain =
      new MAUS::DataStructure::Global::PrimaryChain();
  MAUS::DataStructure::Global::Track*        global_track =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::TrackPoint*   global_track_point =
      new MAUS::DataStructure::Global::TrackPoint();
  MAUS::DataStructure::Global::SpacePoint*   global_space_point =
      new MAUS::DataStructure::Global::SpacePoint();

  global_track_point->set_space_point(global_space_point);
  global_track->AddTrackPoint(global_track_point);
  global_chain->AddPrimaryTrack(global_track);

  event.add_primarychain_recursive(global_chain);
  
  ASSERT_TRUE(event.get_primarychains());
  EXPECT_EQ(event.get_primarychains()->size(), 1U);
  EXPECT_EQ(event.get_primarychains()->at(0), global_chain);

  ASSERT_TRUE(event.get_tracks());
  EXPECT_EQ(event.get_tracks()->size(), 1U);
  EXPECT_EQ(event.get_tracks()->at(0), global_track);

  ASSERT_TRUE(event.get_track_points());
  EXPECT_EQ(event.get_track_points()->size(), 1U);
  EXPECT_EQ(event.get_track_points()->at(0), global_track_point);

  ASSERT_TRUE(event.get_space_points());
  EXPECT_EQ(event.get_space_points()->size(), 1U);
  EXPECT_EQ(event.get_space_points()->at(0), global_space_point);
}

}

