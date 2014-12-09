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


#include "TObject.h"
#include "TRef.h"

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanState.hh"

namespace MAUS {

class SciFiTrackPointTestDS : public ::testing::Test {
 protected:
  SciFiTrackPointTestDS()  {}
  virtual ~SciFiTrackPointTestDS() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiTrackPointTestDS, test_default_contructor) {
  SciFiTrackPoint* tp1 = new SciFiTrackPoint();
  EXPECT_EQ(-1, tp1->spill());
  EXPECT_EQ(-1, tp1->tracker());
  EXPECT_EQ(-1, tp1->station());
  EXPECT_EQ(-1, tp1->plane());
  EXPECT_EQ(666, tp1->channel());
  EXPECT_EQ(-1, tp1->chi2());
  EXPECT_EQ(ThreeVector(0, 0, 0), tp1->pos());
  EXPECT_EQ(ThreeVector(0, 0, 0), tp1->mom());
  EXPECT_EQ(-1, tp1->pull());
  EXPECT_EQ(-1, tp1->residual());
  EXPECT_EQ(-1, tp1->smoothed_residual());
  EXPECT_TRUE(tp1->get_cluster());
  delete tp1;
}

TEST_F(SciFiTrackPointTestDS, test_kalman_state_contructor) {
  // Build a Kalman Site
  KalmanState state1 = KalmanState();
  state1.Initialise(5);
  int id = 13;
  state1.set_id(id);

  // Define projected states...
  TMatrixD projected_a(5, 1);
  projected_a(0, 0) = 1.0;
  projected_a(0, 0) = 2.0;
  projected_a(0, 0) = 3.0;
  projected_a(0, 0) = 4.0;
  projected_a(0, 0) = 5.0;
  TMatrixD pull(2, 1);
  pull(0, 0) = 1.2;
  pull(1, 0) = 1.;

  // ... filtered states...
  TMatrixD a(5, 1);
  a(0, 0) = 1.1;
  a(0, 0) = 2.1;
  a(0, 0) = 3.1;
  a(0, 0) = 4.1;
  a(0, 0) = 5.1;
  TMatrixD residual(2, 1);
  residual(0, 0) = 0.8;
  residual(1, 0) = 2.;
  double f_chi2 = 2.;

  // ... and smoothed states.
  TMatrixD smoothed_a(5, 1);
  smoothed_a(0, 0) = 1.2;
  smoothed_a(0, 0) = 2.2;
  smoothed_a(0, 0) = 3.2;
  smoothed_a(0, 0) = 4.2;
  smoothed_a(0, 0) = 5.2;
  TMatrixD smoothed_residual(2, 1);
  smoothed_residual(0, 0) = 0.6;
  smoothed_residual(1, 0) = 3.;
  double s_chi2 = 1.;

  TMatrixD covariance_residual(2, 2);
  covariance_residual(0, 0) = 1.;
  covariance_residual(0, 1) = 2.;
  covariance_residual(1, 0) = 3.;
  covariance_residual(1, 1) = 4.;

  // Set them all.
  state1.set_a(projected_a, MAUS::KalmanState::Projected);
  state1.set_residual(pull, MAUS::KalmanState::Projected);

  state1.set_a(a, MAUS::KalmanState::Filtered);
  state1.set_residual(residual, MAUS::KalmanState::Filtered);
  state1.set_chi2(f_chi2);

  state1.set_a(smoothed_a, MAUS::KalmanState::Smoothed);
  state1.set_residual(smoothed_residual, MAUS::KalmanState::Smoothed);
  state1.set_covariance_residual(covariance_residual, MAUS::KalmanState::Smoothed);

  int tracker = 1;
  SciFiCluster* clus1 = new SciFiCluster();
  clus1->set_tracker(tracker);
  state1.set_cluster(clus1);

  // Construct a track point from the kalman state
  SciFiTrackPoint* tp1 = new SciFiTrackPoint(&state1);

  // Perform the tests
  EXPECT_EQ(clus1, tp1->get_cluster_pointer());
  EXPECT_EQ(tp1->get_cluster_pointer()->get_tracker(), tracker);

  delete tp1;
  delete clus1;
}

TEST_F(SciFiTrackPointTestDS, test_cluster_getters_and_setters) {
  SciFiTrackPoint *tp = new SciFiTrackPoint();
  SciFiCluster* cl = new SciFiCluster();
  int tracker = 1;
  cl->set_tracker(tracker);

  TRef *trf = new TRef(cl);
  tp->set_cluster(trf);
  ASSERT_TRUE(tp->get_cluster());
  EXPECT_EQ(cl, static_cast<SciFiCluster*>(tp->get_cluster()->GetObject()));
  EXPECT_EQ(cl, static_cast<SciFiCluster*>(tp->get_cluster_tobject()));
  EXPECT_EQ(cl, tp->get_cluster_pointer());

  delete tp;
  tp = new SciFiTrackPoint();
  TObject* tobj = cl;

  tp->set_cluster_tobject(tobj);
  ASSERT_TRUE(tp->get_cluster());
  EXPECT_EQ(cl, static_cast<SciFiCluster*>(tp->get_cluster()->GetObject()));
  EXPECT_EQ(cl, static_cast<SciFiCluster*>(tp->get_cluster_tobject()));
  EXPECT_EQ(cl, tp->get_cluster_pointer());

  delete tp;
  tp = new SciFiTrackPoint();

  tp->set_cluster_pointer(cl);
  ASSERT_TRUE(tp->get_cluster());
  EXPECT_EQ(cl, static_cast<SciFiCluster*>(tp->get_cluster()->GetObject()));
  EXPECT_EQ(cl, static_cast<SciFiCluster*>(tp->get_cluster_tobject()));
  EXPECT_EQ(cl, tp->get_cluster_pointer());

  delete cl;
  delete tp;
}

} // ~namespace MAUS
