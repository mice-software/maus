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
#include <stdlib.h>

#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

#include "gtest/gtest.h"

/*
TODO: Update tests according to changes in the KalmanSeed class.
*/

namespace MAUS {

class KalmanSeedTest : public ::testing::Test {
 protected:
  KalmanSeedTest()  {}
  virtual ~KalmanSeedTest() {}
  virtual void SetUp()    {
    // Clusters need id,
    // spacepoints need station number.
    int id_0     = 1;
    int id_1     = 2;
    int id_3     = 3;
    int id_4     = 4;
    int id_6     = 5;
    int id_7     = 6;
    int id_9     = 7;
    int id_10    = 8;
    int id_12    = 9;
    int id_13    = 10;

    int station_1 = 1;
    int station_2 = 2;
    int station_3 = 3;
    int station_4 = 4;
    int station_5 = 5;
    int tracker = 1;

    SciFiCluster * c0 = new SciFiCluster();
    c0->set_id(id_0);
    SciFiCluster * c1 = new SciFiCluster();
    c1->set_id(id_1);
    SciFiCluster * c3 = new SciFiCluster();
    c3->set_id(id_3);
    SciFiCluster * c4 = new SciFiCluster();
    c4->set_id(id_4);
    SciFiCluster * c6 = new SciFiCluster();
    c6->set_id(id_6);
    SciFiCluster * c7 = new SciFiCluster();
    c7->set_id(id_7);
    SciFiCluster * c9 = new SciFiCluster();
    c9->set_id(id_9);
    SciFiCluster * c10 = new SciFiCluster();
    c10->set_id(id_10);
    SciFiCluster * c12 = new SciFiCluster();
    c12->set_id(id_12);
    SciFiCluster * c13 = new SciFiCluster();
    c13->set_id(id_13);
    _clusters.push_back(c0);
    _clusters.push_back(c1);
    _clusters.push_back(c3);
    _clusters.push_back(c4);
    _clusters.push_back(c6);
    _clusters.push_back(c7);
    _clusters.push_back(c9);
    _clusters.push_back(c10);
    _clusters.push_back(c12);
    _clusters.push_back(c13);

    x = 7.0;
    y = 8.0;
    z = 9.0;
    ThreeVector pos(x, y, z);

    SciFiSpacePoint *sp_1 = new SciFiSpacePoint();
    sp_1->set_tracker(tracker);
    sp_1->add_channel(c0);
    sp_1->add_channel(c1);
    sp_1->set_position(pos);
    sp_1->set_station(station_1);

    SciFiSpacePoint *sp_2 = new SciFiSpacePoint();
    sp_2->set_tracker(tracker);
    sp_2->add_channel(c3);
    sp_2->add_channel(c4);
    sp_2->set_position(pos);
    sp_2->set_station(station_2);

    SciFiSpacePoint *sp_3 = new SciFiSpacePoint();
    sp_3->set_tracker(tracker);
    sp_3->add_channel(c6);
    sp_3->add_channel(c7);
    sp_3->set_position(pos);
    sp_3->set_station(station_3);

    SciFiSpacePoint *sp_4 = new SciFiSpacePoint();
    sp_4->set_tracker(tracker);
    sp_4->add_channel(c9);
    sp_4->add_channel(c10);
    sp_4->set_position(pos);
    sp_4->set_station(station_4);

    SciFiSpacePoint *sp_5 = new SciFiSpacePoint();
    sp_5->set_tracker(tracker);
    sp_5->add_channel(c12);
    sp_5->add_channel(c13);
    sp_5->set_position(pos);
    sp_5->set_station(station_5);

    _spacepoints.push_back(sp_1);
    _spacepoints.push_back(sp_2);
    _spacepoints.push_back(sp_3);
    _spacepoints.push_back(sp_4);
    _spacepoints.push_back(sp_5);
  }
  virtual void TearDown() {
    // delete spacepoints ------------------------
    std::vector<SciFiSpacePoint*>::iterator spoint;
    for (spoint = _spacepoints.begin(); spoint!= _spacepoints.end(); ++spoint) {
      delete (*spoint);
    }
    _spacepoints.resize(0);
    // delete clusters ------------------------
    std::vector<SciFiCluster*>::iterator cluster;
    for (cluster = _clusters.begin(); cluster!= _clusters.end(); ++cluster) {
      delete (*cluster);
    }
    _clusters.resize(0);
  }
  double x;
  double y;
  double z;
  static const double err = 1.e-6;
  std::vector<SciFiSpacePoint*> _spacepoints;
  std::vector<SciFiCluster*>    _clusters;
};

TEST_F(KalmanSeedTest, test_ordering) {
  MAUS::KalmanSeed seed;
  // This will load the clusters stored in the _spacepoints
  seed.RetrieveClusters(_spacepoints);
  std::vector<SciFiCluster*> seed_clusters = seed.GetClusters();

  // Now, let's check that they are ordered.
  int temp = -1;
  for ( size_t i = 0; i < seed_clusters.size(); ++i ) {
    int id = seed_clusters[i]->get_id();
    EXPECT_TRUE(temp < id);
    temp = id;
  }
  // Check order of spacepoints.
  temp = -1;
  for ( size_t j = 0; j < _spacepoints.size(); ++j ) {
    int station_number = _spacepoints[j]->get_station();
    // Check that they are stored in increasing order.
    EXPECT_TRUE(temp < station_number);
    temp = station_number;
  }
  // Check if any clusters were lost.
  EXPECT_EQ(_clusters.size(), seed_clusters.size());
}

TEST_F(KalmanSeedTest, test_straight_state_vector) {
  //
  // Set up Seed object with spacepoints.
  //
  MAUS::KalmanSeed seed;
  //
  // Now set up a Straight Pattern Recognition Track
  //
  MAUS::SciFiStraightPRTrack straight_track;
  double mx = 1.;
  double my = 2.;
  straight_track.set_my(my);
  straight_track.set_mx(mx);
  straight_track.set_tracker(0);
  straight_track.set_spacepoints_pointers(_spacepoints);
  // Set up stuff for posterior use.
  seed.Build(&straight_track);
  //
  // Build an initial state vector.
  //
  TMatrixD a = seed.initial_state_vector();
  //
  // Check the result is the expected.
  //
  EXPECT_EQ(a.GetNrows(), 4);
  EXPECT_NEAR(a(0, 0), x,  err);
  EXPECT_NEAR(a(1, 0), mx, err);
  EXPECT_NEAR(a(2, 0), y,  err);
  EXPECT_NEAR(a(3, 0), my, err);
}

TEST_F(KalmanSeedTest, test_helical_state_vector) {
  //
  // Set up Seed object with spacepoints.
  //
  MAUS::KalmanSeed seed;
  seed.SetField(-0.004);
  //
  // Now set up a Helical Pattern Recognition Track
  //
  MAUS::SciFiHelicalPRTrack helical_track;
  double r     = 10.; // mm
  double tan_lambda = 1./200.;
  double dsdz  = 1./tan_lambda;
  // Is this Pi really necessary?
  double PI = acos(-1.);
  double phi_0 = 0.;
  int charge = -1.;
  double field = -0.004;
  double pt = charge*0.3*fabs(field)*r;
  double pz = pt*tan_lambda;

  DoubleArray phi;
  phi.push_back(phi_0);
  helical_track.set_charge(charge);
  helical_track.set_phi(phi);
  helical_track.set_tracker(0);
  helical_track.set_R(r);
  helical_track.set_dsdz(dsdz);
  helical_track.set_phi0(phi_0);
  helical_track.set_spacepoints_pointers(_spacepoints);
  // Set up stuff for posterior use.
  seed.Build(&helical_track);
  //
  // Build an initial state vector.
  //
  TMatrixD a = seed.initial_state_vector();
  //
  // Check the result is the expected.
  //
  EXPECT_EQ(a.GetNrows(), 5);
  EXPECT_NEAR(a(0, 0), x, err);
  EXPECT_NEAR(a(1, 0), pt*cos(phi_0+PI/2.), err);
  EXPECT_NEAR(a(2, 0), y, err);
  EXPECT_NEAR(a(3, 0), pt*sin(phi_0+PI/2.), err);
}

} // ~namespace MAUS
