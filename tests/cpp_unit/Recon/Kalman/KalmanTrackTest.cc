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

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

#include "gtest/gtest.h"

namespace MAUS {

class KalmanTrackTest : public ::testing::Test {
 protected:
  KalmanTrackTest()  {}
  virtual ~KalmanTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  MAUS::KalmanSite *a_site;
};

TEST_F(KalmanTrackTest, test_constructor) {
  // a_site->set_measurement(0);
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack();
  EXPECT_EQ(track->get_chi2(), 0.0);
  EXPECT_EQ(track->get_ndf(), 0.0);
  EXPECT_EQ(track->get_tracker(), -1);
}

//
// ------- Projection ------------
//
TEST_F(KalmanTrackTest, test_projection_methods) {
}

//
// ------- Filtering ------------
//
TEST_F(KalmanTrackTest, test_filtering_methods) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack();
  CLHEP::Hep3Vector direction_plane0_tracker0 = (0., 1., 0.);
  CLHEP::Hep3Vector direction_plane1_tracker0 = (-0.866, -0.5, 0.0);
  CLHEP::Hep3Vector direction_plane2_tracker0 = (0.866, -0.5, 0.0);

  CLHEP::Hep3Vector direction_plane0_tracker1 = (0., 1., 0.);
  CLHEP::Hep3Vector direction_plane1_tracker1 = (0.866, -0.5, 0.0);
  CLHEP::Hep3Vector direction_plane2_tracker1 = (-0.866, -0.5, 0.0);

  //KalmanSite *a_site;
  //a_site->set_direction(direction_plane0_tracker0);
  //a_site->set_measurement(0.0);
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
}


} // ~namespace MAUS
