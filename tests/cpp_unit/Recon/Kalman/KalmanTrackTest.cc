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

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

#include "gtest/gtest.h"

namespace MAUS {

class KalmanTrackTest : public ::testing::Test {
 protected:
  KalmanTrackTest()  {}
  virtual ~KalmanTrackTest() {}
  virtual void SetUp()    {
    old_site.Initialise(5);
    new_site.Initialise(5);
    double deltaZ = 0.66;
    new_site.set_id(0);
    new_site.set_z(deltaZ);
    old_site.set_z(0.0);

    double mx = 2.0;
    double my = 3.0;
    TMatrixD a(5, 1);
    a(0, 0) = 0.0;
    a(1, 0) = mx;
    a(2, 0) = 0.0;
    a(3, 0) = my;
    a(4, 0) = 1./200.;
    old_site.set_a(a, MAUS::KalmanSite::Filtered);

    TMatrixD C(5, 5);
    C.Zero();
    for ( int i = 0; i < 5; ++i ) {
      C(i, i) = 1.; // dummy values
    }
    old_site.set_covariance_matrix(C, MAUS::KalmanSite::Projected);

    kalman_sites.push_back(old_site);
    kalman_sites.push_back(new_site);
  }
  virtual void TearDown() {}
  MAUS::KalmanSite old_site;
  MAUS::KalmanSite new_site;
  std::vector<MAUS::KalmanSite> kalman_sites;
};

TEST_F(KalmanTrackTest, test_constructor) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  track->Initialise();
  EXPECT_EQ(track->ndf(), 0.);
  EXPECT_EQ(track->tracker(), -1);
  delete track;
}

//
// ------- Projection ------------
//
TEST_F(KalmanTrackTest, test_energy_loss) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  track->Initialise();
  track->CalculatePredictedState(&old_site, &new_site);
  TMatrixD a_old = old_site.a(KalmanSite::Projected);
  a_old.Print();

  track->SubtractEnergyLoss(&old_site, &new_site);
  TMatrixD a     = new_site.a(KalmanSite::Projected);
  a.Print();
  EXPECT_LT(a(4, 0), a_old(4, 0));
}

//
// ------- Filtering ------------
//
TEST_F(KalmanTrackTest, test_update_H_for_misalignments) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  track->Initialise();
  CLHEP::Hep3Vector direction_plane0_tracker0(0., 1., 0.);
  CLHEP::Hep3Vector direction_plane1_tracker0(0.866, -0.5, 0.0);
  CLHEP::Hep3Vector direction_plane2_tracker0(-0.866, -0.5, 0.0);

  MAUS::KalmanSite *a_site= new MAUS::KalmanSite();
  a_site->Initialise(5);
  a_site->set_direction(direction_plane0_tracker0);

  // Say we have a measurement...
  TMatrixD measurement(2, 1);
  measurement(0, 0) = 10.;
  measurement(1, 0) = 0.;

  // and there's a x,y position corresponding to that measurement...
  TMatrixD a;
  a.ResizeTo(5, 1);
  a(0, 0) = -measurement(0, 0)*1.4945;
  a(1, 0) = 1.;
  a(2, 0) = 5.; // random number, y doesnt matter.
  a(3, 0) = 1.;
  a(4, 0) = 1.;
  a_site->set_a(a, MAUS::KalmanSite::Projected);

  TMatrixD s(3, 1);
  s.Zero();

  track->UpdateH(a_site);

  TMatrixD HA = track->SolveMeasurementEquation(a, s);
  measurement.Print();
  HA.Print();
  EXPECT_NEAR(measurement(0, 0), HA(0, 0), 1e-6);

  // now, we introduce a shift in x.
  // The state vector is the same, the measurement is slightly different
  double shift_x = 2.; // mm
  s(0, 0) = shift_x; // mm
  // the new measurement includes the misalignment shift
  double new_alpha = measurement(0, 0) + shift_x/1.4945;

  TMatrixD HA_new = track->SolveMeasurementEquation(a, s);

  EXPECT_NEAR(new_alpha, HA_new(0, 0), 1e-1);
  // Now, we introduce a shift in both x & y.
  double shift_y = 2.; // mm
  s(1, 0) = shift_y; // mm
  // So we need a plane that's not 0 (because this one is vertical, only measures x)
  a_site->set_direction(direction_plane1_tracker0);
  CLHEP::Hep3Vector perp = direction_plane1_tracker0.orthogonal();

  delete a_site;
  delete track;
}

TEST_F(KalmanTrackTest, test_filtering_methods) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  track->Initialise();

  CLHEP::Hep3Vector direction_plane0_tracker0(0., 1., 0.);
  CLHEP::Hep3Vector direction_plane1_tracker0(0.866, -0.5, 0.0);
  CLHEP::Hep3Vector direction_plane2_tracker0(-0.866, -0.5, 0.0);

  MAUS::KalmanSite *a_site= new MAUS::KalmanSite();
  a_site->Initialise(5);

  // Plane 0. 1st case.
  a_site->set_direction(direction_plane2_tracker0);
  TMatrixD a;
  a.ResizeTo(5, 1);
  a(0, 0) = 50.;
  a(1, 0) = 1.;
  a(2, 0) = 5.;
  a(3, 0) = 1.;
  a(4, 0) = 1./200.;

  a_site->set_a(a, MAUS::KalmanSite::Projected);
  TMatrixD s(3, 1);
  s.Zero();
  track->UpdateH(a_site);
  TMatrixD HA = track->SolveMeasurementEquation(a, s);

  EXPECT_TRUE(HA(0, 0) > 0);
  // 2nd case.
  a(0, 0) = 2.;
  a(2, 0) = 60.;
  a_site->set_a(a, MAUS::KalmanSite::Projected);
  HA = track->SolveMeasurementEquation(a, s);

  EXPECT_TRUE(HA(0, 0) < 0);
  // 3rd case
  a(0, 0) = -30.;
  a(2, 0) = 30.;
  a_site->set_a(a, MAUS::KalmanSite::Projected);
  HA = track->SolveMeasurementEquation(a, s);

  EXPECT_TRUE(HA(0, 0) < 0);
  // 4th case
  a(0, 0) = -50.;
  a(2, 0) = -5.;
  a_site->set_a(a, MAUS::KalmanSite::Projected);
  HA = track->SolveMeasurementEquation(a, s);

  EXPECT_TRUE(HA(0, 0) < 0);
  // 5th case
  a(0, 0) = -2.;
  a(2, 0) = -60.;
  a_site->set_a(a, MAUS::KalmanSite::Projected);
  HA = track->SolveMeasurementEquation(a, s);
  HA.Print();
  EXPECT_TRUE(HA(0, 0) > 0);
  // 6th case
  a(0, 0) = 30.;
  a(2, 0) = -30.;
  a_site->set_a(a, MAUS::KalmanSite::Projected);
  HA = track->SolveMeasurementEquation(a, s);

  EXPECT_TRUE(HA(0, 0) > 0);

  a_site->set_measurement(HA(0, 0));
/*
  double x, px, y, py, kappa;
  x =
  TMatrixD projected_a(5, 1);
  projected_a(0, 0) = x;
  projected_a(0, 0) = px;
  projected_a(0, 0) = y;
  projected_a(0, 0) = py;
  projected_a(0, 0) = kappa;

  a_site->set_projected_a();
  a_site->set_projected_covariance_matrix();


  a_site->get_a(a_filt);
  a_site->get_projected_alpha(alpha_model);

  track->update_H(&a_site);
  track->calc_filtered_state
*/

  // So we have H ready. Let's test the built of W.
  // For that, we will need V.
  track->UpdateV(a_site);
  TMatrixD C(5, 5);
  TMatrixD C_S(3, 3);
  a_site->set_covariance_matrix(C, KalmanSite::Projected);
  a_site->set_input_shift_covariance(C_S);

  // this breaks
  // EXPECT_THROW(track->UpdateW(a_site), Squeal);
  delete a_site;
  delete track;
}

TEST_F(KalmanTrackTest, test_covariance_extrapolation) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  track->Initialise();
  track->CalculatePredictedState(&old_site, &new_site);

  // Build a covariance matrix. Elements are random in [0, 100].
  TMatrixD covariance = TMatrixD(5, 5);
  for ( int j = 0; j < 5; j++ ) {
    for ( int k = 0; k < 5; k++ ) {
      covariance(j, k) = 100.*rand()/RAND_MAX;
    }
  }

  old_site.set_covariance_matrix(covariance, MAUS::KalmanSite::Filtered);
  track->CalculateCovariance(&old_site, &new_site);

  // Verify that the covariance GROWS when we extrapolate.
  // Not much we can do with this.
  for ( int j = 0; j < 5; j++ ) {
    for ( int k = 0; k < 5; k++ ) {
      EXPECT_GE(new_site.covariance_matrix(MAUS::KalmanSite::Projected)(j, k),
                old_site.covariance_matrix(MAUS::KalmanSite::Filtered)(j, k));
    }
  }
  //
  // MCS increases matrix elements.
  //
  track->CalculateSystemNoise(&old_site, &new_site);
  track->CalculateCovariance(&old_site, &new_site);
  for ( int j = 0; j < 5; j++ ) {
    for ( int k = 0; k < 5; k++ ) {
      EXPECT_GE(new_site.covariance_matrix(MAUS::KalmanSite::Projected)(j, k),
                old_site.covariance_matrix(MAUS::KalmanSite::Filtered)(j, k));
    }
  }
  delete track;
}

TEST_F(KalmanTrackTest, test_exclusion_of_site) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  track->Initialise();
  track->Extrapolate(kalman_sites, 1);
  track->Filter(kalman_sites, 1);
  track->PrepareForSmoothing(&new_site);
  track->SmoothBack(&new_site, &old_site);
  track->ExcludeSite(&old_site);
  // The projected state becomes the filtered one.
}
} // ~namespace MAUS
