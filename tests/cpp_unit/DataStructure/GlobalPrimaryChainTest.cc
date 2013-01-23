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

#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/DataStructure/GlobalTrack.hh"
#include "src/common_cpp/DataStructure/GlobalPrimaryChain.hh"

#include "gtest/gtest.h"

namespace MAUS {

class GlobalPrimaryChainTestDS : public ::testing::Test {
 protected:
  GlobalPrimaryChainTestDS()  {}
  virtual ~GlobalPrimaryChainTestDS() {}
  virtual void SetUp()    {
    _primarychain1 = new GlobalPrimaryChain();

    _t0 = new MAUS::GlobalTrack();
    _t0->set_mapper_name("t0");
    _t1 = new MAUS::GlobalTrack();
    _t1->set_mapper_name("t1");
    _t2 = new MAUS::GlobalTrack();
    _t2->set_mapper_name("t2");

    track_parent_pairs = 
        new std::vector<MAUS::recon::global::TRefTrackPair*>();
    track_parent_pairs->push_back(
        new MAUS::recon::global::TRefTrackPair(_t0, NULL));
    track_parent_pairs->push_back(
        new MAUS::recon::global::TRefTrackPair(_t1, _t0));
    track_parent_pairs->push_back(
        new MAUS::recon::global::TRefTrackPair(_t2, _t0));

    _primarychain1->set_track_parent_pairs(track_parent_pairs);

    _goodness_of_fit = 5.;
    _primarychain1->set_goodness_of_fit(_goodness_of_fit);      
  }
  virtual void TearDown() {}
 protected:
  GlobalPrimaryChain* _primarychain1;
  MAUS::GlobalTrack* _t0;
  MAUS::GlobalTrack* _t1;
  MAUS::GlobalTrack* _t2;
  double _goodness_of_fit;
  std::vector<MAUS::recon::global::TRefTrackPair*> *track_parent_pairs;
};

TEST_F(GlobalPrimaryChainTestDS, test_default_constructor) {
  GlobalPrimaryChain primarychain;

  EXPECT_TRUE(primarychain.get_track_parent_pairs()->empty());
  EXPECT_EQ(0., primarychain.get_goodness_of_fit());
}

TEST_F(GlobalPrimaryChainTestDS, test_getters_setters) {
  ASSERT_EQ(3U, _primarychain1->get_track_parent_pairs()->size());
  EXPECT_EQ(_t0, _primarychain1->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_t1, _primarychain1->get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_EQ(_t2, _primarychain1->get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", _primarychain1->get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", _primarychain1->get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", _primarychain1->get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == _primarychain1->get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_EQ(_t0, _primarychain1->get_track_parent_pairs()
            ->at(1)->GetParent());
  EXPECT_EQ(_t0, _primarychain1->get_track_parent_pairs()
            ->at(2)->GetParent());
  EXPECT_EQ(_goodness_of_fit, _primarychain1->get_goodness_of_fit());
}

TEST_F(GlobalPrimaryChainTestDS, test_copy_constructor) {
  GlobalPrimaryChain primarychain2(*_primarychain1);

  ASSERT_NE(&primarychain2, _primarychain1);
  ASSERT_EQ(3U, primarychain2.get_track_parent_pairs()->size());
  EXPECT_EQ(_t0, primarychain2.get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_t1, primarychain2.get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_EQ(_t2, primarychain2.get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", primarychain2.get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", primarychain2.get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", primarychain2.get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == primarychain2.get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_EQ(_t0, primarychain2.get_track_parent_pairs()
            ->at(1)->GetParent());
  EXPECT_EQ(_t0, primarychain2.get_track_parent_pairs()
            ->at(2)->GetParent());
  EXPECT_EQ(_goodness_of_fit, primarychain2.get_goodness_of_fit());
}

TEST_F(GlobalPrimaryChainTestDS, test_assignment_operator) {
  GlobalPrimaryChain primarychain2 = *_primarychain1;

  ASSERT_NE(&primarychain2, _primarychain1);
  ASSERT_EQ(3U, primarychain2.get_track_parent_pairs()->size());
  EXPECT_EQ(_t0, primarychain2.get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_t1, primarychain2.get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_EQ(_t2, primarychain2.get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", primarychain2.get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", primarychain2.get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", primarychain2.get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == primarychain2.get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_EQ(_t0, primarychain2.get_track_parent_pairs()
            ->at(1)->GetParent());
  EXPECT_EQ(_t0, primarychain2.get_track_parent_pairs()
            ->at(2)->GetParent());
  EXPECT_EQ(_goodness_of_fit, primarychain2.get_goodness_of_fit());
}

TEST_F(GlobalPrimaryChainTestDS, test_Clone) {
  GlobalPrimaryChain* primarychain2 = _primarychain1->Clone();

  ASSERT_NE(primarychain2, _primarychain1);
  ASSERT_EQ(3U, primarychain2->get_track_parent_pairs()->size());
  EXPECT_NE(_t0, primarychain2->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_NE(_t1, primarychain2->get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_NE(_t2, primarychain2->get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", primarychain2->get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", primarychain2->get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", primarychain2->get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == primarychain2->get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_NE(_t0, primarychain2->get_track_parent_pairs()->at(1)->GetParent());
  EXPECT_EQ(primarychain2->get_track_parent_pairs()->at(1)->GetParent(),
            primarychain2->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_NE(_t0, primarychain2->get_track_parent_pairs()->at(2)->GetParent());
  EXPECT_EQ(primarychain2->get_track_parent_pairs()->at(2)->GetParent(),
            primarychain2->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_goodness_of_fit, primarychain2->get_goodness_of_fit());
}

TEST_F(GlobalPrimaryChainTestDS, test_TrackMethods) {
  GlobalPrimaryChain* primarychain1 =
      new GlobalPrimaryChain();

  // AddTrack
  MAUS::GlobalTrack* t0 = new MAUS::GlobalTrack();
  MAUS::GlobalTrack* t1 = new MAUS::GlobalTrack();
  MAUS::GlobalTrack* t2 = new MAUS::GlobalTrack();
  MAUS::GlobalTrack* t3 = new MAUS::GlobalTrack();
  MAUS::GlobalTrack* t4 = new MAUS::GlobalTrack();
  MAUS::GlobalTrack* t5 = new MAUS::GlobalTrack();
  MAUS::GlobalTrack* t6 = new MAUS::GlobalTrack();

  // Add Primaries, t0, t1
  primarychain1->AddPrimaryTrack(t0);
  primarychain1->AddPrimaryTrack(t1);

  // Add t0 daughters
  primarychain1->AddTrack(t2, t0);
  primarychain1->AddTrack(t3, t0);
  
  // Add t1 daughters
  primarychain1->AddTrack(t4, t1);
  primarychain1->AddTrack(t5, t1);

  ASSERT_EQ(6U, primarychain1->get_track_parent_pairs()->size());
  EXPECT_TRUE(NULL == primarychain1->get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_TRUE(NULL == primarychain1->get_track_parent_pairs()
              ->at(1)->GetParent());
  EXPECT_EQ(t0, primarychain1->get_track_parent_pairs()->at(2)->GetParent());
  EXPECT_EQ(t0, primarychain1->get_track_parent_pairs()->at(3)->GetParent());
  EXPECT_EQ(t1, primarychain1->get_track_parent_pairs()->at(4)->GetParent());
  EXPECT_EQ(t1, primarychain1->get_track_parent_pairs()->at(5)->GetParent());

  // RemoveTrack
  // Try to remove a track that isn't in primary chain, t6
  EXPECT_FALSE(primarychain1->RemoveTrack(t6));

  // Try to remove a track with daughters, t1
  EXPECT_FALSE(primarychain1->RemoveTrack(t1));
  
  // Remove t4, t5 and t1
  EXPECT_TRUE(primarychain1->RemoveTrack(t4));
  EXPECT_TRUE(primarychain1->RemoveTrack(t5));
  EXPECT_TRUE(primarychain1->RemoveTrack(t1));
  
  // HasTrack & HasTrackAsParent
  EXPECT_TRUE(primarychain1->HasTrack(t0));
  EXPECT_FALSE(primarychain1->HasTrack(t1));
  EXPECT_TRUE(primarychain1->HasTrack(t2));
  EXPECT_TRUE(primarychain1->HasTrack(t3));
  EXPECT_FALSE(primarychain1->HasTrack(t4));
  EXPECT_FALSE(primarychain1->HasTrack(t5));

  EXPECT_TRUE(primarychain1->HasTrackAsParent(t0));
  EXPECT_FALSE(primarychain1->HasTrackAsParent(t1));
  EXPECT_FALSE(primarychain1->HasTrackAsParent(t2));
  EXPECT_FALSE(primarychain1->HasTrackAsParent(t3));
  EXPECT_FALSE(primarychain1->HasTrackAsParent(t4));
  EXPECT_FALSE(primarychain1->HasTrackAsParent(t5));

  // IsPrimaryTrack
  EXPECT_TRUE(primarychain1->IsPrimaryTrack(t0));
  EXPECT_FALSE(primarychain1->IsPrimaryTrack(t2));
  EXPECT_FALSE(primarychain1->IsPrimaryTrack(t3));

  // GetTrackParent
  EXPECT_TRUE(NULL == primarychain1->GetTrackParent(t0));
  EXPECT_EQ(t0, primarychain1->GetTrackParent(t2));
  EXPECT_EQ(t0, primarychain1->GetTrackParent(t3));

  // GetTrackDaughters
  ASSERT_EQ(2U, primarychain1->GetTrackDaughters(t0).size());
  EXPECT_EQ(t2, primarychain1->GetTrackDaughters(t0)[0]);
  EXPECT_EQ(t3, primarychain1->GetTrackDaughters(t0)[1]);
  EXPECT_EQ(0U, primarychain1->GetTrackDaughters(t2).size());
  EXPECT_EQ(0U, primarychain1->GetTrackDaughters(t3).size());

  // ClearTracks
  primarychain1->ClearTracks();
  EXPECT_TRUE(primarychain1->get_track_parent_pairs()->empty());
}
  
} // ~namespace MAUS
