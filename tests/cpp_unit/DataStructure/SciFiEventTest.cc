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

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"

namespace MAUS {

class SciFiEventTestDS : public ::testing::Test {
 protected:
  SciFiEventTestDS()  {}
  virtual ~SciFiEventTestDS() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiEventTestDS, test_default_contructor) {
  SciFiEvent* evt = new SciFiEvent();
  EXPECT_TRUE(evt->digits().empty());
  EXPECT_TRUE(evt->clusters().empty());
  EXPECT_TRUE(evt->spacepoints().empty());
  EXPECT_TRUE(evt->straightprtracks().empty());
  EXPECT_TRUE(evt->helicalprtracks().empty());
  EXPECT_TRUE(evt->scifiseeds().empty());
}

TEST_F(SciFiEventTestDS, test_copy_constructor) {

  // Create a first SciFiEvent
  SciFiEvent* evt1 = new SciFiEvent();

  // Set up a digit and give its parameters non-default values
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  SciFiDigit *digit = new SciFiDigit(spill, event, tracker, station, plane, channel, npe, time);

  // Set up clusters, spacepoints, seeds, straight and helical pr tracks using defaults only
  SciFiCluster *cluster = new SciFiCluster();
  SciFiSpacePoint *spoint = new SciFiSpacePoint();
  SciFiStraightPRTrack *strk = new SciFiStraightPRTrack();
  SciFiHelicalPRTrack *htrk =  new SciFiHelicalPRTrack();
  SciFiSeed *seed = new SciFiSeed();

  // Add the hits etc to the first event
  evt1->add_digit(digit);
  evt1->add_cluster(cluster);
  evt1->add_spacepoint(spoint);
  evt1->add_straightprtrack(strk);
  evt1->add_helicalprtrack(htrk);
  evt1->add_seed(seed);

  // Create a second event using the first
  SciFiEvent evt2(*evt1);

  // Delete the first event so that the tests will check all the pointers were deep copied
  delete evt1;

  // Run the tests, checking digits thoroughly, the rest with just one parameter
  ASSERT_EQ(evt2.digits().size(), 1u);
  EXPECT_EQ(evt2.digits()[0]->get_spill(), spill);
  EXPECT_EQ(evt2.digits()[0]->get_event(), event);
  EXPECT_EQ(evt2.digits()[0]->get_tracker(), tracker);
  EXPECT_EQ(evt2.digits()[0]->get_station(), station);
  EXPECT_EQ(evt2.digits()[0]->get_plane(), plane);
  EXPECT_EQ(evt2.digits()[0]->get_channel(), channel);
  EXPECT_EQ(evt2.digits()[0]->get_npe(), npe);
  EXPECT_EQ(evt2.digits()[0]->get_time(), time);
  EXPECT_FALSE(evt2.digits()[0]->is_used());

  ASSERT_EQ(evt2.clusters().size(), 1u);
  EXPECT_EQ(evt2.clusters()[0]->get_tracker(), -1);

  ASSERT_EQ(evt2.spacepoints().size(), 1u);
  EXPECT_EQ(evt2.spacepoints()[0]->get_tracker(), 0);

  ASSERT_EQ(evt2.straightprtracks().size(), 1u);
  EXPECT_EQ(evt2.straightprtracks()[0]->get_tracker(), -1);

  ASSERT_EQ(evt2.helicalprtracks().size(), 1u);
  EXPECT_EQ(evt2.helicalprtracks()[0]->get_tracker(), -1);

  ASSERT_EQ(evt2.scifiseeds().size(), 1u);
  EXPECT_EQ(evt2.scifiseeds()[0]->getTracker(), 0); 
}

TEST_F(SciFiEventTestDS, test_assignment_operator) {
  SciFiEvent* evt1 = new SciFiEvent();

  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;

  SciFiDigit *digit = new SciFiDigit(spill, event, tracker, station, plane, channel, npe, time);
  evt1->add_digit(digit);

  SciFiCluster *c1 = new SciFiCluster(digit);
  evt1->add_cluster(c1);

  SciFiHelicalPRTrack* hpr_track = new SciFiHelicalPRTrack();
  evt1->add_helicalprtrack(hpr_track);

  SciFiTrack *trk1 = new SciFiTrack();
  trk1->SetAlgorithmUsed(1);
  SciFiTrackPoint *tp1 = new SciFiTrackPoint();
  trk1->add_scifitrackpoint(tp1);
  trk1->set_pr_track_pointer(hpr_track);
  evt1->add_scifitrack(trk1);

  ASSERT_EQ(evt1->helicalprtracks()[0], evt1->scifitracks()[0]->pr_track_pointer());

  SciFiEvent* evt2 = new SciFiEvent();
  *evt2 = *evt1;

  ASSERT_EQ(evt2->helicalprtracks()[0], evt2->scifitracks()[0]->pr_track_pointer());
  ASSERT_NE(evt1->helicalprtracks()[0], evt2->scifitracks()[0]->pr_track_pointer());

  delete evt1;

  EXPECT_EQ(spill, evt2->digits()[0]->get_spill());
  EXPECT_EQ(event, evt2->digits()[0]->get_event());
  EXPECT_EQ(tracker, evt2->digits()[0]->get_tracker());
  EXPECT_EQ(station, evt2->digits()[0]->get_station());
  EXPECT_EQ(plane, evt2->digits()[0]->get_plane());
  EXPECT_EQ(channel, evt2->digits()[0]->get_channel());
  EXPECT_EQ(npe, evt2->digits()[0]->get_npe());
  EXPECT_EQ(time, evt2->digits()[0]->get_time());
  EXPECT_TRUE(evt2->digits()[0]->is_used());

  EXPECT_EQ(tracker, evt2->clusters()[0]->get_tracker());
  EXPECT_EQ(evt2->digits()[0], evt2->clusters()[0]->get_digits()->At(0));

  EXPECT_EQ(-1, evt2->scifitracks()[0]->scifitrackpoints()[0]->tracker());
  EXPECT_EQ(evt2->helicalprtracks()[0], evt2->scifitracks()[0]->pr_track_pointer());
}

TEST_F(SciFiEventTestDS, test_digit_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiDigit* digit = new SciFiDigit();
  evt->add_digit(digit);
  EXPECT_EQ(1u, evt->digits().size());
  EXPECT_EQ(digit, evt->digits()[0]);
}

TEST_F(SciFiEventTestDS, test_cluster_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiCluster* cluster = new SciFiCluster();
  evt->add_cluster(cluster);
  EXPECT_EQ(1u, evt->clusters().size());
  EXPECT_EQ(cluster, evt->clusters()[0]);
}

TEST_F(SciFiEventTestDS, test_spoint_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiSpacePoint* spacepoint = new SciFiSpacePoint();
  evt->add_spacepoint(spacepoint);
  EXPECT_EQ(1u, evt->spacepoints().size());
  EXPECT_EQ(spacepoint, evt->spacepoints()[0]);
}

TEST_F(SciFiEventTestDS, test_spoint_set_used_flag) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiSpacePoint* spacepoint1 = new SciFiSpacePoint();
  spacepoint1->set_used(false);
  evt->add_spacepoint(spacepoint1);
  SciFiSpacePoint* spacepoint2 = new SciFiSpacePoint();
  evt->add_spacepoint(spacepoint2);
  spacepoint2->set_used(false);
  EXPECT_FALSE(evt->spacepoints()[0]->get_used());
  EXPECT_FALSE(evt->spacepoints()[1]->get_used());
  evt->set_spacepoints_used_flag(true);
  EXPECT_TRUE(evt->spacepoints()[0]->get_used());
  EXPECT_TRUE(evt->spacepoints()[1]->get_used());
  evt->set_spacepoints_used_flag(false);
  EXPECT_FALSE(evt->spacepoints()[0]->get_used());
  EXPECT_FALSE(evt->spacepoints()[1]->get_used());
}

TEST_F(SciFiEventTestDS, test_seed_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiSeed* seed = new SciFiSeed();
  evt->add_seed(seed);
  EXPECT_EQ(1u, evt->scifiseeds().size());
  EXPECT_EQ(seed, evt->scifiseeds()[0]);
}

TEST_F(SciFiEventTestDS, test_add_straightprtrack_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiStraightPRTrack* trk = new SciFiStraightPRTrack();
  trk->set_x0(5.0);
  evt->add_straightprtrack(trk);
  EXPECT_EQ(1u, evt->straightprtracks().size());
  EXPECT_EQ(5.0, evt->straightprtracks()[0]->get_x0());
}

TEST_F(SciFiEventTestDS, test_straightprtrack_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiStraightPRTrack* trk = new SciFiStraightPRTrack();
  trk->set_x0(5.0);
  SciFiStraightPRTrack* trk2 = new SciFiStraightPRTrack();
  trk2->set_x0(10.0);
  std::vector<SciFiStraightPRTrack*> trks;
  trks.push_back(trk);
  trks.push_back(trk2);
  evt->set_straightprtrack(trks);
  EXPECT_EQ(2u, evt->straightprtracks().size());
  EXPECT_EQ(5.0, evt->straightprtracks()[0]->get_x0());
  EXPECT_EQ(10.0, evt->straightprtracks()[1]->get_x0());
}

TEST_F(SciFiEventTestDS, test_add_helicalprtrack) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiHelicalPRTrack* trk = new SciFiHelicalPRTrack();
  trk->set_pos0(ThreeVector(1.0, 2.0, 3.0));
  evt->add_helicalprtrack(trk);
  EXPECT_EQ(1u, evt->helicalprtracks().size());
  EXPECT_EQ(1.0, evt->helicalprtracks()[0]->get_pos0().x());
}

TEST_F(SciFiEventTestDS, test_helicalprtrack_getters_setters) {
  SciFiEvent* evt = new SciFiEvent();
  SciFiHelicalPRTrack* trk = new SciFiHelicalPRTrack();
  trk->set_pos0(ThreeVector(1.0, 2.0, 3.0));
  SciFiHelicalPRTrack* trk2 = new SciFiHelicalPRTrack();
  trk2->set_pos0(ThreeVector(-1.0, -2.0, -3.0));
  std::vector<SciFiHelicalPRTrack*> trks;
  trks.push_back(trk);
  trks.push_back(trk2);
  evt->set_helicalprtrack(trks);
  EXPECT_EQ(2u, evt->helicalprtracks().size());
  EXPECT_EQ(1.0, evt->helicalprtracks()[0]->get_pos0().x());
  EXPECT_EQ(2.0, evt->helicalprtracks()[0]->get_pos0().y());
  EXPECT_EQ(3.0, evt->helicalprtracks()[0]->get_pos0().z());
  EXPECT_EQ(-1.0, evt->helicalprtracks()[1]->get_pos0().x());
  EXPECT_EQ(-2.0, evt->helicalprtracks()[1]->get_pos0().y());
  EXPECT_EQ(-3.0, evt->helicalprtracks()[1]->get_pos0().z());
}

} // ~namespace MAUS
