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

#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/PrimaryChain.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

#include "gtest/gtest.h"

namespace MAUS {

class PrimaryChainTestDS : public ::testing::Test {
 protected:
  PrimaryChainTestDS()  {}
  virtual ~PrimaryChainTestDS() {}
  virtual void SetUp()    {
    _us_chain = new DataStructure::Global::PrimaryChain("USChain", DataStructure::Global::kUS);
    _ds_chain = new DataStructure::Global::PrimaryChain("DSChain", DataStructure::Global::kDS);
    _through_chain = new DataStructure::Global::PrimaryChain("ThroughChain",
        DataStructure::Global::kThrough);
    _us_track = new MAUS::DataStructure::Global::Track();
    _us_track->set_pid(DataStructure::Global::kEPlus);
    _ds_track = new MAUS::DataStructure::Global::Track();
    _ds_track->set_pid(DataStructure::Global::kEMinus);
    _through_track = new MAUS::DataStructure::Global::Track();
    _through_track->set_pid(DataStructure::Global::kMuPlus);
    _through_track->AddTrack(_us_track);
    _through_track->AddTrack(_ds_track);
    _pid_track = new MAUS::DataStructure::Global::Track();
    _pid_track->set_pid(DataStructure::Global::kMuMinus);
    _fitted_track = new MAUS::DataStructure::Global::Track();
    _fitted_track->set_pid(DataStructure::Global::kPiPlus);
    _us_chain->AddMatchedTrack(_us_track);
    _ds_chain->AddMatchedTrack(_ds_track);
    _through_chain->AddMatchedTrack(_through_track);
    _through_chain->SetUSDaughter(_us_chain);
    _through_chain->SetDSDaughter(_ds_chain);
    _through_chain->SetPIDTrack(_pid_track);
    _through_chain->SetFittedTrack(_fitted_track);
  }
  virtual void TearDown() {
    delete _us_chain;
    delete _ds_chain;
    delete _through_chain;
    delete _us_track;
    delete _ds_track;
    delete _through_track;
    delete _pid_track;
    delete _fitted_track;
  }
 protected:
  DataStructure::Global::PrimaryChain* _us_chain;
  DataStructure::Global::PrimaryChain* _ds_chain;
  DataStructure::Global::PrimaryChain* _through_chain;
  DataStructure::Global::Track* _us_track;
  DataStructure::Global::Track* _ds_track;
  DataStructure::Global::Track* _through_track;
  DataStructure::Global::Track* _pid_track;
  DataStructure::Global::Track* _fitted_track;
};

TEST_F(PrimaryChainTestDS, test_default_constructor) {
  DataStructure::Global::PrimaryChain primary_chain;
  EXPECT_EQ(primary_chain.get_chain_type(), DataStructure::Global::kNoChainType);
  EXPECT_EQ(primary_chain.get_multiplicity(), DataStructure::Global::kUnique);
}

TEST_F(PrimaryChainTestDS, test_constructor_mappername) {
  DataStructure::Global::PrimaryChain primary_chain("MapperName");
  EXPECT_EQ(primary_chain.get_chain_type(), DataStructure::Global::kNoChainType);
  EXPECT_EQ(primary_chain.get_multiplicity(), DataStructure::Global::kUnique);
  EXPECT_EQ(primary_chain.get_mapper_name(), "MapperName");
}

TEST_F(PrimaryChainTestDS, test_constructor_mappername_type) {
  DataStructure::Global::PrimaryChain primary_chain("MapperName", DataStructure::Global::kUS);
  EXPECT_EQ(primary_chain.get_chain_type(), DataStructure::Global::kUS);
  EXPECT_EQ(primary_chain.get_multiplicity(), DataStructure::Global::kUnique);
  EXPECT_EQ(primary_chain.get_mapper_name(), "MapperName");
}

TEST_F(PrimaryChainTestDS, test_copy_constructor) {
  DataStructure::Global::PrimaryChain copied_chain(*_through_chain);
  EXPECT_EQ(copied_chain.get_chain_type(), DataStructure::Global::kThrough);
  EXPECT_EQ(copied_chain.get_multiplicity(), DataStructure::Global::kUnique);
  EXPECT_EQ(copied_chain.get_mapper_name(), "ThroughChain");
  EXPECT_NE(&copied_chain, _through_chain);
  // Child chains are not deep copied
  EXPECT_EQ(copied_chain.GetUSDaughter(), _us_chain);
  EXPECT_EQ(copied_chain.GetDSDaughter(), _ds_chain);
  DataStructure::Global::Track* pid_track = copied_chain.GetPIDTrack();
  EXPECT_EQ(pid_track, _pid_track);
}

TEST_F(PrimaryChainTestDS, test_assignment_operator) {
  DataStructure::Global::PrimaryChain assigned_chain = *_through_chain;
  EXPECT_EQ(assigned_chain.get_chain_type(), DataStructure::Global::kThrough);
  EXPECT_EQ(assigned_chain.get_multiplicity(), DataStructure::Global::kUnique);
  EXPECT_EQ(assigned_chain.get_mapper_name(), "ThroughChain");
  EXPECT_NE(&assigned_chain, _through_chain);
  // Child chains are not deep copied
  EXPECT_EQ(assigned_chain.GetUSDaughter(), _us_chain);
  EXPECT_EQ(assigned_chain.GetDSDaughter(), _ds_chain);
  DataStructure::Global::Track* pid_track = assigned_chain.GetPIDTrack();
  EXPECT_EQ(pid_track, _pid_track);
}

TEST_F(PrimaryChainTestDS, test_getters_setters) {
  EXPECT_EQ(_through_chain->GetUSDaughter(), _us_chain);
  EXPECT_EQ(_through_chain->GetDSDaughter(), _ds_chain);
  DataStructure::Global::Track* matched_track =
      _through_chain->GetMatchedTrack(DataStructure::Global::kMuPlus);
  EXPECT_EQ(matched_track, _through_track);
  EXPECT_EQ(matched_track->get_mapper_name(), "MapCppGlobalTrackMatching");
  std::vector<DataStructure::Global::Track*> matched_tracks = _through_chain->GetMatchedTracks();
  EXPECT_EQ(matched_tracks.size(), 1);
  EXPECT_EQ(matched_tracks.at(0), _through_track);
  DataStructure::Global::Track* pid_track = _through_chain->GetPIDTrack();
  EXPECT_EQ(pid_track, _pid_track);
  EXPECT_EQ(pid_track->get_mapper_name(), "MapCppGlobalPID");
  DataStructure::Global::Track* fitted_track = _through_chain->GetFittedTrack();
  EXPECT_EQ(fitted_track, _fitted_track);
  EXPECT_EQ(fitted_track->get_mapper_name(), "MapCppGlobalTrackFitting");
  EXPECT_EQ(_through_chain->get_multiplicity(), DataStructure::Global::kUnique);
  _through_chain->set_multiplicity(DataStructure::Global::kMultipleDS);
  EXPECT_EQ(_through_chain->get_multiplicity(), DataStructure::Global::kMultipleDS);
}


} // ~namespace MAUS
