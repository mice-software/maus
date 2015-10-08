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

TEST_F(SciFiSpacePointRecTest, test_default_constructor) {
  SciFiSpacePointRec test;
  double acceptable_radius = 160.0;
  double kuno_1_5 = 320.0;
  double kuno_else = 318.5;
  double kuno_tolerance = 3.0;
  EXPECT_EQ(acceptable_radius, test.get_acceptable_radius());
  EXPECT_EQ(kuno_1_5, test.get_kuno_1_5());
  EXPECT_EQ(kuno_else, test.get_kuno_else());
  EXPECT_EQ(kuno_tolerance, test.get_kuno_tolerance());
}

TEST_F(SciFiSpacePointRecTest, test_parameter_constructor) {
  double acceptable_radius = 160.0;
  double kuno_1_5 = 320.0;
  double kuno_else = 318.5;
  double kuno_tolerance = 3.0;
  SciFiSpacePointRec test(acceptable_radius, kuno_1_5, kuno_else, kuno_tolerance);
  EXPECT_EQ(acceptable_radius, test.get_acceptable_radius());
  EXPECT_EQ(kuno_1_5, test.get_kuno_1_5());
  EXPECT_EQ(kuno_else, test.get_kuno_else());
  EXPECT_EQ(kuno_tolerance, test.get_kuno_tolerance());
}

TEST_F(SciFiSpacePointRecTest, test_process) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  ThreeVector direction(0.0, 0.0, 1.0);
  ThreeVector position(1.0, 2.0, 3.0);

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
  int size = event.spacepoints().size();
  EXPECT_EQ(0., size);

  SciFiCluster *Cluster2 = new SciFiCluster();
  Cluster2->set_tracker(tracker);
  Cluster2->set_station(station);
  Cluster2->set_plane(1);
  Cluster2->set_channel(channel);
  Cluster->set_npe(npe);
  Cluster2->set_time(time);
  Cluster2->set_direction(direction);
  Cluster2->set_position(position);
}

TEST_F(SciFiSpacePointRecTest, test_duplet_radius) {
  SciFiSpacePointRec a_test;
  SciFiCluster* c1 = new SciFiCluster();
  SciFiCluster* c2 = new SciFiCluster();

  ThreeVector pos1(5.78222, -11.6053, -751.132);
  ThreeVector pos2(-50.6352, -0.604, -749.828);
  ThreeVector dir1(-0.866025, -0.5, 0);
  ThreeVector dir2(0.866025, -0.5, 0);

  c1->set_direction(dir1);

  c2->set_direction(dir2);

  c1->set_position(pos1);

  c2->set_position(pos2);

  bool test_1 = a_test.duplet_within_radius(c1, c2);
  EXPECT_TRUE(test_1);

  ThreeVector pos3(300.0, -11.6053, -751.132);
  ThreeVector pos4(300.0, -0.604, -749.828);
  c1->set_direction(pos3);
  c2->set_direction(pos4);
  bool test_2 = a_test.duplet_within_radius(c1, c2);
  EXPECT_FALSE(test_2);
}

TEST_F(SciFiSpacePointRecTest, test_clusters_arent_used) {
  SciFiCluster* c1 = new SciFiCluster();
  SciFiCluster* c2 = new SciFiCluster();
  SciFiCluster* c3 = new SciFiCluster();
  SciFiSpacePointRec a_test;

  bool expect_true_triplets =  a_test.clusters_are_not_used(c1, c2, c3);
  bool expect_true_duplets  =  a_test.clusters_are_not_used(c1, c2);
  EXPECT_TRUE(expect_true_triplets);
  EXPECT_TRUE(expect_true_duplets);

  c1->set_used(true);
  bool expect_false_triplets =  a_test.clusters_are_not_used(c1, c2, c3);
  bool expect_false_duplets  =  a_test.clusters_are_not_used(c1, c2);
  EXPECT_FALSE(expect_false_triplets);
  EXPECT_FALSE(expect_false_duplets);
}

TEST_F(SciFiSpacePointRecTest, test_kuno_test) {
  SciFiCluster* c1 = new SciFiCluster();
  SciFiCluster* c2 = new SciFiCluster();
  SciFiCluster* c3 = new SciFiCluster();

  c1->set_tracker(0);
  c2->set_tracker(0);
  c3->set_tracker(0);
  c1->set_station(1);
  c2->set_station(1);
  c3->set_station(1);
  c1->set_plane(0);
  c2->set_plane(1);
  c3->set_plane(2);
  c1->set_channel(106);
  c2->set_channel(106);
  c3->set_channel(106);

  SciFiSpacePointRec a_test;
  bool expect_true = a_test.kuno_accepts(c1, c2, c3);
  EXPECT_TRUE(expect_true);

  c1->set_station(4);
  bool expect_false = a_test.kuno_accepts(c1, c2, c3);
  EXPECT_FALSE(expect_false);
}


TEST_F(SciFiSpacePointRecTest, test_make_cluster_container) {
  SciFiEvent event;

  // Create 12 clusters belonging to dif planes...
  int tracker = 0;
  int station = 1;
  int plane = 0;
  for ( int i = 0; i < 60; ++i ) {
    SciFiCluster* cluster = new SciFiCluster();
    cluster->set_tracker(tracker);
    cluster->set_station(station);
    cluster->set_plane(plane);
    event.add_cluster(cluster);

    tracker < 1 ? tracker += 1 : 0;
    station < 5 ? station += 1 : 1;
    plane   < 2 ? plane   += 1 : 0;
  }

  std::vector<SciFiCluster*> clusters[2][6][3];

  SciFiSpacePointRec a_test;
  a_test.make_cluster_container(event, clusters);
  for ( int tracker = 0; tracker < 1; ++tracker ) {
    for ( int station = 1; station < 6; ++station ) {
      for ( int plane = 0; plane < 3; ++plane ) {
        int numb_clusters = clusters[tracker][station][plane].size();
        for ( int clust_i = 0; clust_i < numb_clusters; ++clust_i ) {
          SciFiCluster* cluster = (clusters[tracker][station][plane])[clust_i];
          int cluster_tracker = cluster->get_tracker();
          int cluster_station = cluster->get_station();
          int cluster_plane   = cluster->get_plane();
          EXPECT_EQ(tracker, cluster_tracker);
          EXPECT_EQ(station, cluster_station);
          EXPECT_EQ(plane, cluster_plane);
        }
      }
    }
  }
}

TEST_F(SciFiSpacePointRecTest, test_crossing_position) {
  // Calculate the crossing position of two clusters.
  // The expected crossing coordinates are:
  double expected_x = 7.;
  double expected_y = 10.;
  double expected_z = 0.;

  SciFiCluster* view_X = new SciFiCluster();
  ThreeVector dir1(0., 1., 0.);
  ThreeVector pos1(expected_x, 0., expected_z);
  view_X->set_direction(dir1);
  view_X->set_position(pos1);

  SciFiCluster* view_W = new SciFiCluster();
  ThreeVector dir2(1., 0., 0.);
  ThreeVector pos2(0., expected_y, expected_z);
  view_W->set_direction(dir2);
  view_W->set_position(pos2);

  SciFiSpacePointRec a_test;
  ThreeVector intersection = a_test.crossing_pos(view_X, view_W);

  EXPECT_EQ(expected_x, intersection.x());
  EXPECT_EQ(expected_y, intersection.y());
  EXPECT_EQ(expected_z, intersection.z());
}

/*
TEST_F(SciFiSpacePointRecTest, test_builds) {
  SciFiCluster* c1 = new SciFiCluster();
  SciFiCluster* c2 = new SciFiCluster();
  SciFiCluster* c3 = new SciFiCluster();
  c1->set_spill(1);
  c1->set_event(1);
  c1->set_npe(3.);
  c2->set_npe(3.);
  c3->set_npe(3.);

  SciFiSpacePoint* triplet(c1, c2, c3);

  SciFiSpacePointRec a_test;
  a_test.build_triplet(triplet);


  _tracker = clust1->get_tracker();
  _station = clust1->get_station();
}
*/

} // ~namespace MAUS
