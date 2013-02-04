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
    // Fill Cpp GlobalEvent
    for(int i = 0; i < 4; ++i) {
      _trackpoint.push_back(new MAUS::DataStructure::Global::TrackPoint());
      _spacepoint.push_back(new MAUS::DataStructure::Global::SpacePoint());
      _spacepoint[i]->set_charge(1. * i);
      _trackpoint[i]->set_spacepoint(_spacepoint[i]);
    }
    
    for(int i = 0; i < 2; ++i) {
      _track.push_back(new MAUS::DataStructure::Global::Track());
      _track[i]->AddTrackPoint(_trackpoint[2*i] );
      _track[i]->AddTrackPoint(_trackpoint[2*i + 1]);
    }
    
    _chain = new MAUS::DataStructure::Global::PrimaryChain();
    _chain->AddPrimaryTrack(_track[0]);
    _chain->AddTrack(_track[1], _track[0]);
    
    _event = new MAUS::GlobalEvent();
    _event->add_primarychain_recursive(_chain);

    local_chain = NULL;
    local_track.resize(2);
    local_trackpoint.resize(4);
    local_spacepoint.resize(4);
  }
    
  MAUS::GlobalEvent *_event;
  MAUS::DataStructure::Global::PrimaryChain* _chain;
  std::vector<MAUS::DataStructure::Global::Track*> _track;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> _trackpoint;
  std::vector<MAUS::DataStructure::Global::SpacePoint*> _spacepoint;
  
  MAUS::DataStructure::Global::PrimaryChain *local_chain;
  std::vector<MAUS::DataStructure::Global::Track*> local_track;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> local_trackpoint;
  std::vector<MAUS::DataStructure::Global::SpacePoint*> local_spacepoint;
  
};

TEST_F(GlobalEventProcessorTestClass, TestSetup) {
  
}

TEST_F(GlobalEventProcessorTestClass, CheckInitialSetup) {
  ASSERT_TRUE(_event);
  ASSERT_EQ(1U, _event->get_primarychains()->size());
  ASSERT_EQ(2U, _event->get_tracks()->size());
  ASSERT_EQ(4U, _event->get_trackpoints()->size());
  ASSERT_EQ(4U, _event->get_spacepoints()->size());

  local_chain = _event->get_primarychains()->at(0);
  EXPECT_EQ(local_chain, _chain);

  for(int i = 0; i < 2; ++i){
    local_track[i] = _event->get_tracks()->at(i);
    EXPECT_EQ(local_track[i], _track[i]);
  }
  
  for(int i = 0; i < 4; ++i) {
    local_trackpoint[i] = _event->get_trackpoints()->at(i);
    local_spacepoint[i] = _event->get_spacepoints()->at(i);
    EXPECT_EQ(local_trackpoint[i], _trackpoint[i]);
    EXPECT_EQ(local_spacepoint[i], _spacepoint[i]);
    double charge = local_spacepoint[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
  }
}

TEST_F(GlobalEventProcessorTestClass, CheckConsistentChain) {
  local_chain = _event->get_primarychains()->at(0);
  EXPECT_EQ(local_chain, _chain);

  for(int i = 0; i < 2; ++i){
    local_track[i] = _chain->get_track_parent_pairs()->at(i)->GetTrack();
    EXPECT_EQ(local_track[i], _track[i]);

    local_trackpoint[2*i] = (MAUS::DataStructure::Global::TrackPoint*)
        _track[i]->get_trackpoints()->At(0);
    local_trackpoint[2*i+1] = (MAUS::DataStructure::Global::TrackPoint*)
        _track[i]->get_trackpoints()->At(1);
  }
  
  for(int i = 0; i < 4; ++i){
    EXPECT_EQ(local_trackpoint[i], _trackpoint[i]);

    local_spacepoint[i] = _trackpoint[i]->get_spacepoint();
    EXPECT_EQ(local_spacepoint[i], _spacepoint[i]);

    double charge = local_spacepoint[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
  }
}

TEST_F(GlobalEventProcessorTestClass, CppToJson) {

  GlobalEventProcessor* proc = new GlobalEventProcessor();

  Json::Value *json_out = NULL;
  
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_NO_THROW(json_out = proc->CppToJson(*_event, ""));
  ASSERT_TRUE(json_out);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::GetInstance().ResolveReferences(*json_out));
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

  ASSERT_EQ(1U, event_out->get_primarychains()->size());
  ASSERT_EQ(2U, event_out->get_tracks()->size());
  ASSERT_EQ(4U, event_out->get_trackpoints()->size());
  ASSERT_EQ(4U, event_out->get_spacepoints()->size());

  local_chain = event_out->get_primarychains()->at(0);
  EXPECT_NE(local_chain, _chain);

  for(int i = 0; i < 2; ++i){
    local_track[i] = event_out->get_tracks()->at(i);
    EXPECT_NE(local_track[i], _track[i]);
  }
  
  for(int i = 0; i < 4; ++i) {
    local_trackpoint[i] = event_out->get_trackpoints()->at(i);
    local_spacepoint[i] = event_out->get_spacepoints()->at(i);
    EXPECT_NE(local_trackpoint[i], _trackpoint[i]);
    EXPECT_NE(local_spacepoint[i], _spacepoint[i]);
    double charge = local_spacepoint[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
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

  ASSERT_EQ(1U, event_out->get_primarychains()->size());
  ASSERT_EQ(2U, event_out->get_tracks()->size());
  ASSERT_EQ(4U, event_out->get_trackpoints()->size());
  ASSERT_EQ(4U, event_out->get_spacepoints()->size());

  local_chain = event_out->get_primarychains()->at(0);
  EXPECT_TRUE(local_chain);

  for(int i = 0; i < 2; ++i){
    local_track[i] = event_out->get_tracks()->at(i);
    EXPECT_TRUE(local_track[i]);
  }
  
  for(int i = 0; i < 4; ++i) {
    local_trackpoint[i] = event_out->get_trackpoints()->at(i);
    local_spacepoint[i] = event_out->get_spacepoints()->at(i);
    EXPECT_TRUE(local_trackpoint[i]);
    EXPECT_TRUE(local_spacepoint[i]);
    double charge = local_spacepoint[i]->get_charge();
    EXPECT_TRUE(std::fabs(charge - (1.0*i)) < 0.00001);
  }
}

// TEST_F(GlobalEventProcessorTest, CreateCpp) {
//     ThreeVectorProcessor proc;
//     ProcessorTest::test_value(&proc, THREE_VEC);
// }

}
}
