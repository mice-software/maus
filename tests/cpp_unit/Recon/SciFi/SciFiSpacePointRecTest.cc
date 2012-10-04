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

// C headers
#include <assert.h>

// Other headers
#include "Config/MiceModule.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"

#include "gtest/gtest.h"

namespace MAUS {

  class SciFiSpacePointRecTest : public ::testing::Test {
 protected:
  SciFiSpacePointRecTest()  {}
  virtual ~SciFiSpacePointRecTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

// TEST_F(SciFiSpacePointRecTest, test_process) {
// }


TEST_F(SciFiSpacePointRecTest, test_process) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  Hep3Vector direction(0.0, 0.0, 1.0);
  Hep3Vector position(1.0, 2.0, 3.0);

  SciFiCluster *Cluster = new SciFiCluster();

  Cluster->set_tracker(tracker);
  Cluster->set_station(station);
  Cluster->set_plane(plane);
  Cluster->set_channel(channel);
  Cluster->set_npe(npe);
  Cluster->set_time(time);
  Cluster->set_direction(direction);
  Cluster->set_position(position);

  SciFiEvent event;
  event.add_cluster(Cluster);

  SciFiSpacePointRec test;
  test.process(event);
  int size = 0;
  // EXPECT_EQ(event.spacepoints().size(), size);
}

TEST_F(SciFiSpacePointRecTest, test_duplet_radius) {
  SciFiSpacePointRec a_test;
  SciFiCluster* c1 = new SciFiCluster();
  SciFiCluster* c2 = new SciFiCluster();

  Hep3Vector pos1(5.78222, -11.6053, -751.132);
  Hep3Vector pos2(-50.6352, -0.604, -749.828);
  Hep3Vector dir1(-0.866025, -0.5, 0);
  Hep3Vector dir2(0.866025, -0.5, 0);

  c1->set_direction(dir1);

  c2->set_direction(dir2);

  c1->set_position(pos1);

  c2->set_position(pos2);

  bool test_1 = a_test.duplet_within_radius(c1, c2);
  EXPECT_TRUE(test_1);

  Hep3Vector pos3(300.0, -11.6053, -751.132);
  Hep3Vector pos4(300.0, -0.604, -749.828);
  c1->set_direction(pos3);
  c2->set_direction(pos4);
  bool test_2 = a_test.duplet_within_radius(c1, c2);
  EXPECT_FALSE(test_2);
}

/*
  void process(SciFiEvent &evt);

  void look_for_triplets(SciFiEvent &evt, std::vector<SciFiCluster*> (&clusters)[2][6][3]);

  void look_for_duplets(SciFiEvent &evt, std::vector<SciFiCluster*> (&clusters)[2][6][3]);

  void build_duplet(SciFiSpacePoint* duplet);

  void build_triplet(SciFiSpacePoint* triplet);

  ThreeVector crossing_pos(SciFiCluster* c1, SciFiCluster* c2);

  bool kuno_accepts(SciFiCluster* cluster1,
                    SciFiCluster* cluster2,
                    SciFiCluster* cluster3);


  bool clusters_are_not_used(SciFiCluster* candidate_A,
                             SciFiCluster* candidate_B);

  bool clusters_are_not_used(SciFiCluster* candidate_A,
                             SciFiCluster* candidate_B,
                             SciFiCluster* candidate_C);

  bool duplet_within_radius(SciFiCluster* candidate_A,
                            SciFiCluster* candidate_B);
*/

} // ~namespace MAUS
