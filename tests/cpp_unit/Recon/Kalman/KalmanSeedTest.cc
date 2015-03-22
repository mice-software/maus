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
    int tracker = 0;

    charge = -1;
    field = 0.004;

    z_positions[0] = 0.0;
    z_positions[1] = 350.0;
    z_positions[2] = 650.0;
    z_positions[3] = 900.0;
    z_positions[4] = 1100.0;

    ThreeVector pos;
    double x, y, z;

    // SET UP STRSIGHT TRACK

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
    _straight_clusters.push_back(c0);
    _straight_clusters.push_back(c1);
    _straight_clusters.push_back(c3);
    _straight_clusters.push_back(c4);
    _straight_clusters.push_back(c6);
    _straight_clusters.push_back(c7);
    _straight_clusters.push_back(c9);
    _straight_clusters.push_back(c10);
    _straight_clusters.push_back(c12);
    _straight_clusters.push_back(c13);

    // Straight tracks
    x_0 = 10.0;
    y_0 = 0.0;
    mx = 20.0 / 1100.0;
    my = 0.0;

    x = x_0;
    y = 0.0;
    z = z_positions[0];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    SciFiSpacePoint *sp_1 = new SciFiSpacePoint();
    sp_1->set_tracker(tracker);
    sp_1->add_channel(c0);
    sp_1->add_channel(c1);
    sp_1->set_position(pos);
    sp_1->set_station(station_1);

    x = x_0 + mx * z_positions[1];
    y = 0.0;
    z = z_positions[1];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    SciFiSpacePoint *sp_2 = new SciFiSpacePoint();
    sp_2->set_tracker(tracker);
    sp_2->add_channel(c3);
    sp_2->add_channel(c4);
    sp_2->set_position(pos);
    sp_2->set_station(station_2);

    x = x_0 + mx * z_positions[2];
    y = 0.0;
    z = z_positions[2];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    SciFiSpacePoint *sp_3 = new SciFiSpacePoint();
    sp_3->set_tracker(tracker);
    sp_3->add_channel(c6);
    sp_3->add_channel(c7);
    sp_3->set_position(pos);
    sp_3->set_station(station_3);

    x = x_0 + mx * z_positions[3];
    y = 0.0;
    z = z_positions[3];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    SciFiSpacePoint *sp_4 = new SciFiSpacePoint();
    sp_4->set_tracker(tracker);
    sp_4->add_channel(c9);
    sp_4->add_channel(c10);
    sp_4->set_position(pos);
    sp_4->set_station(station_4);

    x = x_0 + mx * z_positions[4];
    y = 0.0;
    z = z_positions[4];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    SciFiSpacePoint *sp_5 = new SciFiSpacePoint();
    sp_5->set_tracker(tracker);
    sp_5->add_channel(c12);
    sp_5->add_channel(c13);
    sp_5->set_position(pos);
    sp_5->set_station(station_5);

    _straight_spacepoints.push_back(sp_1);
    _straight_spacepoints.push_back(sp_2);
    _straight_spacepoints.push_back(sp_3);
    _straight_spacepoints.push_back(sp_4);
    _straight_spacepoints.push_back(sp_5);

    // SET UP HELICAL TRACKS

    c0 = new SciFiCluster();
    c0->set_id(id_0);
    c1 = new SciFiCluster();
    c1->set_id(id_1);
    c3 = new SciFiCluster();
    c3->set_id(id_3);
    c4 = new SciFiCluster();
    c4->set_id(id_4);
    c6 = new SciFiCluster();
    c6->set_id(id_6);
    c7 = new SciFiCluster();
    c7->set_id(id_7);
    c9 = new SciFiCluster();
    c9->set_id(id_9);
    c10 = new SciFiCluster();
    c10->set_id(id_10);
    c12 = new SciFiCluster();
    c12->set_id(id_12);
    c13 = new SciFiCluster();
    c13->set_id(id_13);
    _helical_clusters.push_back(c0);
    _helical_clusters.push_back(c1);
    _helical_clusters.push_back(c3);
    _helical_clusters.push_back(c4);
    _helical_clusters.push_back(c6);
    _helical_clusters.push_back(c7);
    _helical_clusters.push_back(c9);
    _helical_clusters.push_back(c10);
    _helical_clusters.push_back(c12);
    _helical_clusters.push_back(c13);

    // Helix Parameters:
    x_c = 0.0;
    y_c = 0.0;
    r = 10.0;
    s_0 = 0.0;
    dsdz = 0.005;

    x = x_c + r*cos( ( s_0 - charge*z_positions[0]*dsdz ) / r );
    y = y_c + r*sin( ( s_0 - charge*z_positions[0]*dsdz ) / r );
    z = z_positions[0];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    sp_1 = new SciFiSpacePoint();
    sp_1->set_tracker(tracker);
    sp_1->add_channel(c0);
    sp_1->add_channel(c1);
    sp_1->set_position(pos);
    sp_1->set_station(station_1);

    x = x_c + r*cos( ( s_0 - charge*z_positions[1]*dsdz ) / r );
    y = y_c + r*sin( ( s_0 - charge*z_positions[1]*dsdz ) / r );
    z = z_positions[1];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    sp_2 = new SciFiSpacePoint();
    sp_2->set_tracker(tracker);
    sp_2->add_channel(c3);
    sp_2->add_channel(c4);
    sp_2->set_position(pos);
    sp_2->set_station(station_2);

    x = x_c + r*cos( ( s_0 - charge*z_positions[2]*dsdz ) / r );
    y = y_c + r*sin( ( s_0 - charge*z_positions[2]*dsdz ) / r );
    z = z_positions[2];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    sp_3 = new SciFiSpacePoint();
    sp_3->set_tracker(tracker);
    sp_3->add_channel(c6);
    sp_3->add_channel(c7);
    sp_3->set_position(pos);
    sp_3->set_station(station_3);

    x = x_c + r*cos( ( s_0 - charge*z_positions[3]*dsdz ) / r );
    y = y_c + r*sin( ( s_0 - charge*z_positions[3]*dsdz ) / r );
    z = z_positions[3];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    sp_4 = new SciFiSpacePoint();
    sp_4->set_tracker(tracker);
    sp_4->add_channel(c9);
    sp_4->add_channel(c10);
    sp_4->set_position(pos);
    sp_4->set_station(station_4);

    x = x_c + r*cos( ( s_0 - charge*z_positions[4]*dsdz ) / r );
    y = y_c + r*sin( ( s_0 - charge*z_positions[4]*dsdz ) / r );
    z = z_positions[4];
    pos.setX( x );
    pos.setY( y );
    pos.setZ( z );
    sp_5 = new SciFiSpacePoint();
    sp_5->set_tracker(tracker);
    sp_5->add_channel(c12);
    sp_5->add_channel(c13);
    sp_5->set_position(pos);
    sp_5->set_station(station_5);

    _helical_spacepoints.push_back(sp_1);
    _helical_spacepoints.push_back(sp_2);
    _helical_spacepoints.push_back(sp_3);
    _helical_spacepoints.push_back(sp_4);
    _helical_spacepoints.push_back(sp_5);
  }
  virtual void TearDown() {
    std::vector<SciFiSpacePoint*>::iterator spoint;
    std::vector<SciFiCluster*>::iterator cluster;
    // delete spacepoints ------------------------
    for (spoint = _helical_spacepoints.begin(); spoint!= _helical_spacepoints.end(); ++spoint) {
      delete (*spoint);
    }
    _helical_spacepoints.resize(0);
    // delete clusters ------------------------
    for (cluster = _helical_clusters.begin(); cluster!= _helical_clusters.end(); ++cluster) {
      delete (*cluster);
    }
    // delete spacepoints ------------------------
    _helical_clusters.resize(0);
    for (spoint = _straight_spacepoints.begin(); spoint!= _straight_spacepoints.end(); ++spoint) {
      delete (*spoint);
    }
    _straight_spacepoints.resize(0);
    // delete clusters ------------------------
    for (cluster = _straight_clusters.begin(); cluster!= _straight_clusters.end(); ++cluster) {
      delete (*cluster);
    }
    _straight_clusters.resize(0);
  }

  double z_positions[5];
  double charge;
  double field;
  // Straight track example
  double x_0;
  double y_0;
  double mx;
  double my;
  std::vector<SciFiCluster*>    _straight_clusters;
  std::vector<SciFiSpacePoint*> _straight_spacepoints;
  // Helical track example
  double dsdz;
  double x_c;
  double y_c;
  double r;
  double s_0;
  std::vector<SciFiCluster*>    _helical_clusters;
  std::vector<SciFiSpacePoint*> _helical_spacepoints;

  static const double err = 1.e-6;
};

TEST_F(KalmanSeedTest, test_ordering) {
  MAUS::KalmanSeed seed;
  // This will load the clusters stored in the _spacepoints
  seed.RetrieveClusters(_helical_spacepoints);
  std::vector<SciFiCluster*> seed_clusters = seed.GetClusters();

  // Now, let's check that they are ordered.
  int temp_id = -1;
  for ( size_t i = 0; i < seed_clusters.size(); ++i ) {
    int id = seed_clusters[i]->get_id();
    EXPECT_TRUE(temp_id < id);
    temp_id = id;
  }
  // Check order of spacepoints.
  double temp_z = 100000.0;
  for ( size_t j = 0; j < _helical_spacepoints.size(); ++j ) {
    double z_pos = _helical_spacepoints[j]->get_position().z();
    // Check that they are stored in increasing order.
    EXPECT_TRUE(temp_z > z_pos);
    temp_z = z_pos;
  }
  // Check if any clusters were lost.
  EXPECT_EQ(_helical_clusters.size(), seed_clusters.size());
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
  // Make a fake covariance matrix.
  // Should be makde by pattern recognition
  std::vector<double> seed_covariance( 16, 0.0 );
  seed_covariance[0] = 1000.0;
  seed_covariance[5] = 1000.0;
  seed_covariance[10] = 1000.0;
  seed_covariance[15] = 1000.0;
  straight_track.set_covariance( seed_covariance );

  straight_track.set_x0(x_0);
  straight_track.set_y0(y_0);
  straight_track.set_my(my);
  straight_track.set_mx(mx);
  straight_track.set_tracker(0);
  straight_track.set_spacepoints_pointers(_straight_spacepoints);
  // Set up stuff for posterior use.
  seed.Build(&straight_track);
  //
  // Build an initial state vector.
  //
  TMatrixD a = seed.initial_state_vector();
  //
  // Check the result is the expected.
  //

  double x = x_0 + 1100.0*mx;
  double y = y_0 + 1100.0*my;
  EXPECT_EQ(a.GetNrows(), 4);
  EXPECT_NEAR(a(0, 0), x,  err);
  EXPECT_NEAR(a(1, 0), mx, err);
  EXPECT_NEAR(a(2, 0), y,  err);
  EXPECT_NEAR(a(3, 0), my, err);
}

TEST_F(KalmanSeedTest, test_helical_state_vector) {
  int charge = -1.;
  double c  = CLHEP::c_light;
  //
  // Set up Seed object with spacepoints.
  //
  MAUS::KalmanSeed seed;
  seed.SetField(0.004);
  //
  // Now set up a Helical Pattern Recognition Track
  //
  MAUS::SciFiHelicalPRTrack helical_track;
  double pt = fabs(charge*c*field*r);

  helical_track.set_dsdz(dsdz);
  helical_track.set_charge(charge);
  helical_track.set_R(r);
  helical_track.set_tracker(0);
  helical_track.set_circle_x0( x_c );
  helical_track.set_circle_y0( y_c );
  helical_track.set_line_sz_c( s_0 );
  helical_track.set_spacepoints_pointers(_helical_spacepoints);
  // Make a fake covariance matrix.
  // Should be makde by pattern recognition
  std::vector<double> seed_covariance( 25, 0.0 );
  seed_covariance[0] = 1000.0;
  seed_covariance[6] = 1000.0;
  seed_covariance[12] = 1000.0;
  seed_covariance[18] = 1000.0;
  seed_covariance[24] = 1000.0;
  helical_track.set_covariance( seed_covariance );
  // Set up stuff for posterior use.
  seed.Build(&helical_track);
  //
  // Build an initial state vector.
  //
  TMatrixD a = seed.initial_state_vector();
  //
  // Check the result is the expected.
  //
  double phi = ( s_0 - charge*z_positions[4]*dsdz ) / r;
  double x = x_c + r*cos( phi );
  double y = y_c + r*sin( phi );
  double px = pt*cos( phi + TMath::PiOver2() );
  double py = pt*sin( phi + TMath::PiOver2() );
  double kappa = ( dsdz*charge ) / pt;

  // Allow less stringent coonstraints on momenutm. Should be tighter!
  EXPECT_EQ(a.GetNrows(), 5);
  EXPECT_NEAR(a(0, 0), x, err);
  EXPECT_NEAR(a(1, 0), px, 1.0);
  EXPECT_NEAR(a(2, 0), y, err);
  EXPECT_NEAR(a(3, 0), py, 1.0);
  EXPECT_NEAR(a(4, 0), kappa, 2.0);
}

} // ~namespace MAUS
