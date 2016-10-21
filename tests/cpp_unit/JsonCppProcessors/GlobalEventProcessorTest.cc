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

#include <string>

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppProcessors/GlobalEventProcessor.hh"

// Test conversion from cpp -> json -> cap

namespace MAUS {
namespace GlobalEventProcessorTest {

class GlobalEventProcessorTestClass : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Clear the reference resolvers, in case they were left in a strange
    // state by another test failing...
    if (ReferenceResolver::CppToJson::RefManager::HasInstance())
      ReferenceResolver::CppToJson::RefManager::Death();
    if (ReferenceResolver::JsonToCpp::RefManager::HasInstance())
      ReferenceResolver::JsonToCpp::RefManager::Death();

    // Fill Cpp GlobalEvent
    detector_array[0] = MAUS::DataStructure::Global::kUndefined;
    detector_array[1] = MAUS::DataStructure::Global::kVirtual;
    detector_array[2] = MAUS::DataStructure::Global::kTracker0_1;
    detector_array[3] = MAUS::DataStructure::Global::kTracker1_2;

    for (int i = 0; i < 4; ++i) {
      _track_point.push_back(new MAUS::DataStructure::Global::TrackPoint());
      _space_point.push_back(new MAUS::DataStructure::Global::SpacePoint());
      _space_point[i]->set_charge(1. * i);
      _space_point[i]->set_detector(detector_array[i]);
      _track_point[i]->set_charge(1. * i);
      _track_point[i]->set_detector(detector_array[i]);
      _track_point[i]->set_space_point(_space_point[i]);
    }

    for (int i = 0; i < 2; ++i) {
      _track.push_back(new MAUS::DataStructure::Global::Track());
      _track[i]->AddTrackPoint(_track_point[2*i] );
      _track[i]->AddTrackPoint(_track_point[2*i + 1]);
    }

    //~ _chain = new MAUS::DataStructure::Global::PrimaryChain();
    //~ _chain->AddPrimaryTrack(_track[0]);
    //~ _chain->AddTrack(_track[1], _track[0]);

    _event = new MAUS::GlobalEvent();
    //~ _event->add_primary_chain_recursive(_chain);
    _event->add_track_recursive(_track[0]);
    _event->add_track_recursive(_track[1]);

    //~ local_chain = NULL;
    local_track.resize(2);
    local_track_point.resize(4);
    local_space_point.resize(4);
  }

  MAUS::GlobalEvent *_event;
  //~ MAUS::DataStructure::Global::PrimaryChain* _chain;
  std::vector<MAUS::DataStructure::Global::Track*> _track;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> _track_point;
  std::vector<MAUS::DataStructure::Global::SpacePoint*> _space_point;
  MAUS::DataStructure::Global::DetectorPoint detector_array[4];

  //~ MAUS::DataStructure::Global::PrimaryChain *local_chain;
  std::vector<MAUS::DataStructure::Global::Track*> local_track;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> local_track_point;
  std::vector<MAUS::DataStructure::Global::SpacePoint*> local_space_point;
};

TEST_F(GlobalEventProcessorTestClass, TestSetup) {
  ASSERT_TRUE(_event);
}

TEST_F(GlobalEventProcessorTestClass, CheckInitialSetup) {
  ASSERT_TRUE(_event);
  //~ ASSERT_EQ(1U, _event->get_primary_chains()->size());
  ASSERT_EQ(2U, _event->get_tracks()->size());
  ASSERT_EQ(4U, _event->get_track_points()->size());
  ASSERT_EQ(4U, _event->get_space_points()->size());

  //~ local_chain = _event->get_primary_chains()->at(0);
  //~ EXPECT_EQ(local_chain, _chain);

  for (int i = 0; i < 2; ++i) {
    local_track[i] = _event->get_tracks()->at(i);
    EXPECT_EQ(local_track[i], _track[i]);
  }

  double charge;
  MAUS::DataStructure::Global::DetectorPoint dp;
  for (int i = 0; i < 4; ++i) {
    local_track_point[i] = _event->get_track_points()->at(i);
    local_space_point[i] = _event->get_space_points()->at(i);
    EXPECT_EQ(local_track_point[i], _track_point[i]);
    EXPECT_EQ(local_space_point[i], _space_point[i]);

    charge = local_space_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    dp = local_space_point[i]->get_detector();
    EXPECT_EQ(dp, detector_array[i]);

    charge = local_track_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    dp = local_track_point[i]->get_detector();
    EXPECT_EQ(dp, detector_array[i]);
  }
}

//~ TEST_F(GlobalEventProcessorTestClass, CheckConsistentChain) {
  //~ local_chain = _event->get_primary_chains()->at(0);
  //~ EXPECT_EQ(local_chain, _chain);

  //~ for (int i = 0; i < 2; ++i) {
    //~ local_track[i] = _chain->get_track_parent_pairs()->at(i)->GetTrack();
    //~ EXPECT_EQ(local_track[i], _track[i]);

    //~ local_track_point[2*i] = (MAUS::DataStructure::Global::TrackPoint*)
        //~ _track[i]->get_track_points()->At(0);
    //~ local_track_point[2*i+1] = (MAUS::DataStructure::Global::TrackPoint*)
        //~ _track[i]->get_track_points()->At(1);
  //~ }

  //~ double charge;
  //~ MAUS::DataStructure::Global::DetectorPoint dp;
  //~ for (int i = 0; i < 4; ++i) {
    //~ EXPECT_EQ(local_track_point[i], _track_point[i]);

    //~ local_space_point[i] = _track_point[i]->get_space_point();
    //~ EXPECT_EQ(local_space_point[i], _space_point[i]);

    //~ charge = local_space_point[i]->get_charge();
    //~ EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    //~ dp = local_space_point[i]->get_detector();
    //~ EXPECT_EQ(dp, detector_array[i]);

    //~ charge = local_track_point[i]->get_charge();
    //~ EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    //~ dp = local_track_point[i]->get_detector();
    //~ EXPECT_EQ(dp, detector_array[i]);
  //~ }
//~ }

TEST_F(GlobalEventProcessorTestClass, CppToJson) {

  GlobalEventProcessor* proc = new GlobalEventProcessor();

  Json::Value *json_out = NULL;

  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_NO_THROW(json_out = proc->CppToJson(*_event, ""));
  ASSERT_TRUE(json_out);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::GetInstance().
                  ResolveReferences(*json_out));
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Death());
}

TEST_F(GlobalEventProcessorTestClass, JsonToCpp) {
  GlobalEventProcessor* proc = new GlobalEventProcessor();

  Json::Value *json_out = NULL;
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_NO_THROW(json_out = proc->CppToJson(*_event, ""));
  ASSERT_TRUE(json_out);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::GetInstance().
                  ResolveReferences(*json_out));
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Death());

  MAUS::GlobalEvent *event_out = NULL;
  ASSERT_NO_THROW(ReferenceResolver::JsonToCpp::RefManager::Birth());
  ASSERT_NO_THROW(event_out = proc->JsonToCpp(*json_out));
  ASSERT_TRUE(event_out);
  ASSERT_NO_THROW(ReferenceResolver::JsonToCpp::RefManager::GetInstance().
                  ResolveReferences());
  ASSERT_NO_THROW(ReferenceResolver::JsonToCpp::RefManager::Death());

  //~ ASSERT_EQ(1U, event_out->get_primary_chains()->size());
  ASSERT_EQ(2U, event_out->get_tracks()->size());
  ASSERT_EQ(4U, event_out->get_track_points()->size());
  ASSERT_EQ(4U, event_out->get_space_points()->size());

  //~ local_chain = event_out->get_primary_chains()->at(0);
  //~ EXPECT_NE(local_chain, _chain);

  for (int i = 0; i < 2; ++i) {
    local_track[i] = event_out->get_tracks()->at(i);
    EXPECT_NE(local_track[i], _track[i]);
  }

  double charge;
  MAUS::DataStructure::Global::DetectorPoint dp;
  for (int i = 0; i < 4; ++i) {
    local_track_point[i] = event_out->get_track_points()->at(i);
    local_space_point[i] = event_out->get_space_points()->at(i);
    EXPECT_NE(local_track_point[i], _track_point[i]);
    EXPECT_NE(local_space_point[i], _space_point[i]);

    charge = local_space_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    dp = local_space_point[i]->get_detector();
    EXPECT_EQ(dp, detector_array[i]);

    charge = local_track_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    dp = local_track_point[i]->get_detector();
    EXPECT_EQ(dp, detector_array[i]);
  }
}

TEST_F(GlobalEventProcessorTestClass, JsonToCppWithDelete) {
  GlobalEventProcessor* proc = new GlobalEventProcessor();

  Json::Value *json_out = NULL;
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_NO_THROW(json_out = proc->CppToJson(*_event, ""));
  ASSERT_TRUE(json_out);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::GetInstance().
                  ResolveReferences(*json_out));
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Death());

  // Delete the original event
  delete _event;

  MAUS::GlobalEvent *event_out = NULL;
  ASSERT_NO_THROW(ReferenceResolver::JsonToCpp::RefManager::Birth());
  ASSERT_NO_THROW(event_out = proc->JsonToCpp(*json_out));
  ASSERT_TRUE(event_out);
  ASSERT_NO_THROW(ReferenceResolver::JsonToCpp::RefManager::GetInstance().
                  ResolveReferences());
  ASSERT_NO_THROW(ReferenceResolver::JsonToCpp::RefManager::Death());

  //~ ASSERT_EQ(1U, event_out->get_primary_chains()->size());
  ASSERT_EQ(2U, event_out->get_tracks()->size());
  ASSERT_EQ(4U, event_out->get_track_points()->size());
  ASSERT_EQ(4U, event_out->get_space_points()->size());

  //~ local_chain = event_out->get_primary_chains()->at(0);
  //~ EXPECT_TRUE(local_chain);

  for (int i = 0; i < 2; ++i) {
    local_track[i] = event_out->get_tracks()->at(i);
    EXPECT_TRUE(local_track[i]);
  }

  double charge;
  MAUS::DataStructure::Global::DetectorPoint dp;
  for (int i = 0; i < 4; ++i) {
    local_track_point[i] = event_out->get_track_points()->at(i);
    local_space_point[i] = event_out->get_space_points()->at(i);
    EXPECT_TRUE(local_track_point[i]);
    EXPECT_TRUE(local_space_point[i]);

    charge = local_space_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    dp = local_space_point[i]->get_detector();
    EXPECT_EQ(dp, detector_array[i]);

    charge = local_track_point[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
    dp = local_track_point[i]->get_detector();
    EXPECT_EQ(dp, detector_array[i]);
  }
}
}
}
