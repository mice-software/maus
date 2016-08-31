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

#include "gtest/gtest.h"

namespace MAUS {

class PrimaryChainTestDS : public ::testing::Test {
 protected:
  PrimaryChainTestDS()  {}
  virtual ~PrimaryChainTestDS() {}
  virtual void SetUp()    {
    _primary_chain0 = new MAUS::DataStructure::Global::PrimaryChain();

    _t0 = new MAUS::DataStructure::Global::Track();
    _t0->set_mapper_name("t0");
    _t1 = new MAUS::DataStructure::Global::Track();
    _t1->set_mapper_name("t1");
    _t2 = new MAUS::DataStructure::Global::Track();
    _t2->set_mapper_name("t2");

    track_parent_pairs =
        new std::vector<MAUS::DataStructure::Global::TRefTrackPair*>();
    track_parent_pairs->push_back(
        new MAUS::DataStructure::Global::TRefTrackPair(_t0, NULL));
    track_parent_pairs->push_back(
        new MAUS::DataStructure::Global::TRefTrackPair(_t1, _t0));
    track_parent_pairs->push_back(
        new MAUS::DataStructure::Global::TRefTrackPair(_t2, _t0));
    _primary_chain0->set_track_parent_pairs(track_parent_pairs);

    _goodness_of_fit = 5.;
    _primary_chain0->set_goodness_of_fit(_goodness_of_fit);

    _test_comment_key   = "TestError1";
    _test_comment_value = "Test errors are FUN!";
    _test_comments[_test_comment_key] = _test_comment_value;
    _primary_chain0->set_comments(_test_comments);
  }
  virtual void TearDown() {}
 protected:
  MAUS::DataStructure::Global::PrimaryChain* _primary_chain0;
  MAUS::DataStructure::Global::Track* _t0;
  MAUS::DataStructure::Global::Track* _t1;
  MAUS::DataStructure::Global::Track* _t2;
  double _goodness_of_fit;
  std::vector<MAUS::DataStructure::Global::TRefTrackPair*> *track_parent_pairs;
  std::string _test_comment_key;
  std::string _test_comment_value;
  std::map<std::string, std::string> _test_comments;
};

TEST_F(PrimaryChainTestDS, test_default_constructor) {
  MAUS::DataStructure::Global::PrimaryChain primary_chain;

  EXPECT_TRUE(primary_chain.get_track_parent_pairs()->empty());
  EXPECT_EQ(0., primary_chain.get_goodness_of_fit());
  EXPECT_EQ(0U, primary_chain.get_comments().size());
}

TEST_F(PrimaryChainTestDS, test_named_constructor) {
  MAUS::DataStructure::Global::PrimaryChain primary_chain("MapName");

  EXPECT_TRUE(primary_chain.get_track_parent_pairs()->empty());
  EXPECT_EQ(0., primary_chain.get_goodness_of_fit());
  EXPECT_EQ("MapName", primary_chain.get_mapper_name());
  EXPECT_EQ(0U, primary_chain.get_comments().size());
}

TEST_F(PrimaryChainTestDS, test_getters_setters) {
  ASSERT_EQ(3U, _primary_chain0->get_track_parent_pairs()->size());
  EXPECT_EQ(_t0, _primary_chain0->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_t1, _primary_chain0->get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_EQ(_t2, _primary_chain0->get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", _primary_chain0->get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", _primary_chain0->get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", _primary_chain0->get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == _primary_chain0->get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_EQ(_t0, _primary_chain0->get_track_parent_pairs()
            ->at(1)->GetParent());
  EXPECT_EQ(_t0, _primary_chain0->get_track_parent_pairs()
            ->at(2)->GetParent());
  EXPECT_EQ(_goodness_of_fit, _primary_chain0->get_goodness_of_fit());
  ASSERT_EQ(1U, _primary_chain0->get_comments().size());
  EXPECT_EQ(_test_comment_value,
            _primary_chain0->get_comments().at(_test_comment_key));
}

TEST_F(PrimaryChainTestDS, test_copy_constructor) {
  MAUS::DataStructure::Global::PrimaryChain primary_chain2(*_primary_chain0);

  ASSERT_NE(&primary_chain2, _primary_chain0);
  ASSERT_EQ(3U, primary_chain2.get_track_parent_pairs()->size());
  EXPECT_EQ(_t0, primary_chain2.get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_t1, primary_chain2.get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_EQ(_t2, primary_chain2.get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", primary_chain2.get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", primary_chain2.get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", primary_chain2.get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == primary_chain2.get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_EQ(_t0, primary_chain2.get_track_parent_pairs()
            ->at(1)->GetParent());
  EXPECT_EQ(_t0, primary_chain2.get_track_parent_pairs()
            ->at(2)->GetParent());
  EXPECT_EQ(_goodness_of_fit, primary_chain2.get_goodness_of_fit());
  ASSERT_EQ(1U, primary_chain2.get_comments().size());
  EXPECT_EQ(_test_comment_value,
            primary_chain2.get_comments().at(_test_comment_key));
}

TEST_F(PrimaryChainTestDS, test_assignment_operator) {
  MAUS::DataStructure::Global::PrimaryChain primary_chain2;
  primary_chain2 = *_primary_chain0;

  ASSERT_NE(&primary_chain2, _primary_chain0);
  ASSERT_EQ(3U, primary_chain2.get_track_parent_pairs()->size());
  EXPECT_EQ(_t0, primary_chain2.get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_t1, primary_chain2.get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_EQ(_t2, primary_chain2.get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", primary_chain2.get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", primary_chain2.get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", primary_chain2.get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == primary_chain2.get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_EQ(_t0, primary_chain2.get_track_parent_pairs()
            ->at(1)->GetParent());
  EXPECT_EQ(_t0, primary_chain2.get_track_parent_pairs()
            ->at(2)->GetParent());
  EXPECT_EQ(_goodness_of_fit, primary_chain2.get_goodness_of_fit());
  ASSERT_EQ(1U, primary_chain2.get_comments().size());
  EXPECT_EQ(_test_comment_value,
            primary_chain2.get_comments().at(_test_comment_key));
}

TEST_F(PrimaryChainTestDS, test_Clone) {
  MAUS::DataStructure::Global::PrimaryChain* primary_chain2 =
      _primary_chain0->Clone();

  ASSERT_NE(primary_chain2, _primary_chain0);
  ASSERT_EQ(3U, primary_chain2->get_track_parent_pairs()->size());
  EXPECT_NE(_t0, primary_chain2->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_NE(_t1, primary_chain2->get_track_parent_pairs()->at(1)->GetTrack());
  EXPECT_NE(_t2, primary_chain2->get_track_parent_pairs()->at(2)->GetTrack());
  EXPECT_EQ("t0", primary_chain2->get_track_parent_pairs()
            ->at(0)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t1", primary_chain2->get_track_parent_pairs()
            ->at(1)->GetTrack()->get_mapper_name());
  EXPECT_EQ("t2", primary_chain2->get_track_parent_pairs()
            ->at(2)->GetTrack()->get_mapper_name());
  EXPECT_TRUE(NULL == primary_chain2->get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_NE(_t0, primary_chain2->get_track_parent_pairs()->at(1)->GetParent());
  EXPECT_EQ(primary_chain2->get_track_parent_pairs()->at(1)->GetParent(),
            primary_chain2->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_NE(_t0, primary_chain2->get_track_parent_pairs()->at(2)->GetParent());
  EXPECT_EQ(primary_chain2->get_track_parent_pairs()->at(2)->GetParent(),
            primary_chain2->get_track_parent_pairs()->at(0)->GetTrack());
  EXPECT_EQ(_goodness_of_fit, primary_chain2->get_goodness_of_fit());
  ASSERT_EQ(1U, primary_chain2->get_comments().size());
  EXPECT_EQ(_test_comment_value,
            primary_chain2->get_comments().at(_test_comment_key));
}

TEST_F(PrimaryChainTestDS, test_TrackMethods) {
  MAUS::DataStructure::Global::PrimaryChain* primary_chain1 =
      new MAUS::DataStructure::Global::PrimaryChain();

  // AddTrack
  MAUS::DataStructure::Global::Track* t0 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t1 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t2 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t3 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t4 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t5 =
      new MAUS::DataStructure::Global::Track();

  // Add Primaries, t0, t1
  primary_chain1->AddPrimaryTrack(t0);
  primary_chain1->AddPrimaryTrack(t1);

  // Add t0 daughters
  primary_chain1->AddTrack(t2, t0);
  primary_chain1->AddTrack(t3, t0);

  // Add t1 daughters
  primary_chain1->AddTrack(t4, t1);

  ASSERT_EQ(5U, primary_chain1->get_track_parent_pairs()->size());
  EXPECT_TRUE(NULL == primary_chain1->get_track_parent_pairs()
              ->at(0)->GetParent());
  EXPECT_TRUE(NULL == primary_chain1->get_track_parent_pairs()
              ->at(1)->GetParent());
  EXPECT_EQ(t0, primary_chain1->get_track_parent_pairs()->at(2)->GetParent());
  EXPECT_EQ(t0, primary_chain1->get_track_parent_pairs()->at(3)->GetParent());
  EXPECT_EQ(t1, primary_chain1->get_track_parent_pairs()->at(4)->GetParent());

  // RemoveTrack
  // HasTrack & HasTrackAsParent
  EXPECT_TRUE(primary_chain1->HasTrack(t0));
  EXPECT_TRUE(primary_chain1->HasTrack(t1));
  EXPECT_TRUE(primary_chain1->HasTrack(t2));
  EXPECT_TRUE(primary_chain1->HasTrack(t3));
  EXPECT_TRUE(primary_chain1->HasTrack(t4));
  EXPECT_FALSE(primary_chain1->HasTrack(t5));

  EXPECT_TRUE(primary_chain1->HasTrackAsParent(t0));
  EXPECT_TRUE(primary_chain1->HasTrackAsParent(t1));
  EXPECT_FALSE(primary_chain1->HasTrackAsParent(t2));
  EXPECT_FALSE(primary_chain1->HasTrackAsParent(t3));
  EXPECT_FALSE(primary_chain1->HasTrackAsParent(t4));
  EXPECT_FALSE(primary_chain1->HasTrackAsParent(t5));

  // IsPrimaryTrack
  EXPECT_TRUE(primary_chain1->IsPrimaryTrack(t0));
  EXPECT_TRUE(primary_chain1->IsPrimaryTrack(t1));
  EXPECT_FALSE(primary_chain1->IsPrimaryTrack(t2));
  EXPECT_FALSE(primary_chain1->IsPrimaryTrack(t3));
  EXPECT_FALSE(primary_chain1->IsPrimaryTrack(t4));
  EXPECT_FALSE(primary_chain1->IsPrimaryTrack(t5));

  // GetTrackParent
  EXPECT_TRUE(NULL == primary_chain1->GetTrackParent(t0));
  EXPECT_TRUE(NULL == primary_chain1->GetTrackParent(t1));
  EXPECT_EQ(t0, primary_chain1->GetTrackParent(t2));
  EXPECT_EQ(t0, primary_chain1->GetTrackParent(t3));
  EXPECT_EQ(t1, primary_chain1->GetTrackParent(t4));
  EXPECT_TRUE(NULL == primary_chain1->GetTrackParent(t5));

  // GetTrackDaughters
  ASSERT_EQ(2U, primary_chain1->GetTrackDaughters(t0).size());
  EXPECT_EQ(t2, primary_chain1->GetTrackDaughters(t0)[0]);
  EXPECT_EQ(t3, primary_chain1->GetTrackDaughters(t0)[1]);
  ASSERT_EQ(1U, primary_chain1->GetTrackDaughters(t1).size());
  EXPECT_EQ(t4, primary_chain1->GetTrackDaughters(t1)[0]);
  EXPECT_EQ(0U, primary_chain1->GetTrackDaughters(t4).size());

  // GetTracks
  ASSERT_EQ(5U, primary_chain1->GetTracks().size());
  EXPECT_EQ(t0, primary_chain1->GetTracks().at(0));
  EXPECT_EQ(t1, primary_chain1->GetTracks().at(1));
  EXPECT_EQ(t2, primary_chain1->GetTracks().at(2));
  EXPECT_EQ(t3, primary_chain1->GetTracks().at(3));
  EXPECT_EQ(t4, primary_chain1->GetTracks().at(4));

  // GetPrimaryTracks
  ASSERT_EQ(2U, primary_chain1->GetPrimaryTracks().size());
  EXPECT_EQ(t0, primary_chain1->GetPrimaryTracks().at(0));
  EXPECT_EQ(t1, primary_chain1->GetPrimaryTracks().at(1));

  // ClearTracks
  primary_chain1->ClearTracks();
  EXPECT_TRUE(primary_chain1->get_track_parent_pairs()->empty());
}

TEST_F(PrimaryChainTestDS, test_ParentMethods) {
  // SetParentChains
  MAUS::DataStructure::Global::PrimaryChain* primary_chain1 =
      new MAUS::DataStructure::Global::PrimaryChain();
  MAUS::DataStructure::Global::PrimaryChain* primary_chain2 =
      new MAUS::DataStructure::Global::PrimaryChain();
  _primary_chain0->set_track_parent_pairs(track_parent_pairs);
  _primary_chain0->AddParentChain(primary_chain1);
  _primary_chain0->AddParentChain(primary_chain2);

  // GetParentChains
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> chains =
      _primary_chain0->GetParentChains();
  ASSERT_EQ(2U, chains.size());
  EXPECT_EQ(primary_chain1, chains.at(0));
  EXPECT_EQ(primary_chain2, chains.at(1));
}

TEST_F(PrimaryChainTestDS, test_Throws) {
  MAUS::DataStructure::Global::Track* t6 = NULL;
  MAUS::DataStructure::Global::Track* t7 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t8 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* t9 =
      new MAUS::DataStructure::Global::Track();

  ASSERT_THROW(_primary_chain0->AddTrack(t6, t7), MAUS::Exceptions::Exception);
  ASSERT_THROW(_primary_chain0->AddPrimaryTrack(t6), MAUS::Exceptions::Exception);

  ASSERT_NO_THROW(_primary_chain0->AddTrack(t7, t6));

  ASSERT_THROW(_primary_chain0->AddTrack(t9, t8), MAUS::Exceptions::Exception);
}
} // ~namespace MAUS
