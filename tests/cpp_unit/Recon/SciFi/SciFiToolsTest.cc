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

#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"

namespace MAUS {

class SciFiToolsTest : public ::testing::Test {
 protected:
  SciFiToolsTest()  {}
  virtual ~SciFiToolsTest() {}
};

TEST_F(SciFiToolsTest, test_calc_circle_residual) {
  SciFiSpacePoint *sp = new SciFiSpacePoint();
  ThreeVector pos(1.0, 4.0, 0.0);
  sp->set_position(pos);
  double xc = 3.0;
  double yc = 2.0;
  double r = 2.0;
  double delta = SciFiTools::calc_circle_residual(sp, xc, yc, r);
  EXPECT_NEAR(0.8284, delta, 0.01);
}

TEST_F(SciFiToolsTest, test_calc_xy_pulls) {
  // Test the spacepoint pull calculation using a circle of rad = 5, centered on (0, 0)
  SciFiHelicalPRTrack* trk = new SciFiHelicalPRTrack();
  trk->set_circle_x0(0.0);
  trk->set_circle_y0(0.0);
  trk->set_R(5.0);

  // Set up spacepoints which lie exactly (to 4 sig fig) on this circle
  std::vector<SciFiSpacePoint*> spnts;
  for (int i = 0; i < 4; ++i) {
    spnts.push_back(new SciFiSpacePoint);
    spnts[i]->set_prxy_pull(-1.0);
  }
  spnts[0]->set_position(ThreeVector(1.0, 4.899, 0.0));
  spnts[1]->set_position(ThreeVector(2.0, 4.583, 0.0));
  spnts[2]->set_position(ThreeVector(3.0, 4.0, 0.0));
  spnts[3]->set_position(ThreeVector(4.0, 3.0, 0.0));

  trk->set_spacepoints_pointers(spnts);

  bool result = SciFiTools::calc_xy_pulls(trk);
  EXPECT_TRUE(result);

  // Check the pulls are all 0
  for (auto sp : spnts) {
    EXPECT_NEAR(0.0, sp->get_prxy_pull(), 0.1);
  }

  // Clean up
  for (auto sp : spnts) {
    delete sp;
  }
  delete trk;
}

TEST_F(SciFiToolsTest, test_make_3pt_circle) {
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  SciFiSpacePoint *sp3 = new SciFiSpacePoint();

  // Set up spacepoints corresponding to circle of radius 2 mm, centred at (1,2)
  ThreeVector pos(1.0, 4.0, 0.0);
  sp1->set_position(pos);
  pos.set(3.0, 2.0, 0.0);
  sp2->set_position(pos);
  pos.set(1.0, 0.0, 0.0);
  sp3->set_position(pos);

  SimpleCircle c = SciFiTools::make_3pt_circle(sp1, sp2, sp3);

  double x0 = 1.0;
  double y0 = 2.0;
  double R = 2.0;
  double epsilon = 0.01;

  EXPECT_NEAR(c.get_x0(), x0, epsilon);
  EXPECT_NEAR(c.get_y0(), y0, epsilon);
  EXPECT_NEAR(c.get_R(), R, epsilon);

  // Now check for a circle of radius 2mm, centred at (0,0) (involves singular matrices)
  pos.set(0.0, 2.0, 0.0);
  sp1->set_position(pos);
  pos.set(2.0, 0.0, 0.0);
  sp2->set_position(pos);
  pos.set(0.0, -2.0, 0.0);
  sp3->set_position(pos);

  c = SciFiTools::make_3pt_circle(sp1, sp2, sp3);

  x0 = 0.0;
  y0 = 0.0;
  R = 2.0;
  epsilon = 0.01;

  EXPECT_NEAR(c.get_x0(), x0, epsilon);
  EXPECT_NEAR(c.get_y0(), y0, epsilon);
  EXPECT_NEAR(c.get_R(), R, epsilon);

  delete sp1;
  delete sp2;
  delete sp3;
}

TEST_F(SciFiToolsTest, test_sort_by_station) {
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  sp1->set_station(1);
  sp2->set_station(2);
  sp3->set_station(3);
  sp4->set_station(4);
  sp5->set_station(5);

  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);

  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(5);
  SciFiTools::sort_by_station(spnts, spnts_by_station);
  EXPECT_EQ(sp1, spnts_by_station[0][0]);
  EXPECT_EQ(sp2, spnts_by_station[1][0]);
  EXPECT_EQ(sp3, spnts_by_station[2][0]);
  EXPECT_EQ(sp4, spnts_by_station[3][0]);
  EXPECT_EQ(sp5, spnts_by_station[4][0]);

  delete sp1;
  delete sp2;
  delete sp3;
  delete sp4;
  delete sp5;
}

TEST_F(SciFiToolsTest, test_stations_with_unused_sp) {

  // Set up spacepoints, leaving station 3 empty to check function copes with an empty station
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  // SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4_1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  sp1->set_station(1);
  sp2->set_station(2);
  // sp3->set_station(3);
  sp4->set_station(4);
  sp4_1->set_station(4);
  sp5->set_station(5);

  sp1->set_used(true);
  sp2->set_used(true);
  // sp3->set_used(true);
  sp4->set_used(false);
  sp4_1->set_used(true);
  sp5->set_used(false);

  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  // spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);
  spnts.push_back(sp4_1);

  SpacePoint2dPArray spnts_by_station(5);

  SciFiTools::sort_by_station(spnts, spnts_by_station);
  ASSERT_EQ(5u, spnts_by_station.size());
  ASSERT_EQ(1u, spnts_by_station[0].size());
  ASSERT_EQ(1u, spnts_by_station[1].size());
  ASSERT_EQ(2u, spnts_by_station[3].size());
  ASSERT_EQ(1u, spnts_by_station[4].size());

  std::vector<int> stations_hit, stations_not_hit;
  SciFiTools::stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

  ASSERT_EQ(2u, stations_hit.size());
  ASSERT_EQ(3u, stations_not_hit.size());
  EXPECT_EQ(3, stations_hit[0]);
  EXPECT_EQ(4, stations_hit[1]);
  EXPECT_EQ(0, stations_not_hit[0]);
  EXPECT_EQ(1, stations_not_hit[1]);
  EXPECT_EQ(2, stations_not_hit[2]);

  int stats_with_unused = SciFiTools::num_stations_with_unused_spnts(spnts_by_station);
  EXPECT_EQ(2, stats_with_unused);

  delete sp1;
  delete sp2;
  delete sp4;
  delete sp4_1;
  delete sp5;
}

} // ~namespace MAUS
