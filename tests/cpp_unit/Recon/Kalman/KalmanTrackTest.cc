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

/* NOTE:
  The KalmanTrack class does most of the Kalman work. Here are the member functions:

  // Projection
  void calc_system_noise(KalmanSite *old_site, KalmanSite *new_site);
  double BetheBlochStoppingPower(double p);
  void subtract_energy_loss(KalmanSite *old_site, KalmanSite *new_site);

  // FILTERING
  void calc_filtered_state(KalmanSite *a_site);
  void update_misaligments(KalmanSite *a_site, KalmanSite *old_site);
  void update_V(KalmanSite *a_site);
  void update_covariance(KalmanSite *a_site);
  void update_H(KalmanSite *a_site);
  void update_W(KalmanSite *a_site);

  TMatrixD solve_measurement_equation(TMatrixD a, TMatrixD s);
  void set_residual(KalmanSite *a_site);


  // SMOOTHING
  void update_back_transportation_matrix(KalmanSite *optimum_site, KalmanSite *smoothing_site);
  void smooth_back(KalmanSite *optimum_site, KalmanSite *smoothing_site);
  void prepare_for_smoothing(std::vector<KalmanSite> &sites);
  void exclude_site(KalmanSite *site);


  TMatrixD get_propagator() { return _F; }
  TMatrixD get_pull(KalmanSite *a_site);
  TMatrixD get_system_noise() { return _Q; }
  TMatrixD get_kalman_gain(KalmanSite *a_site);
  double get_chi2() const { return _chi2; }
  double get_ndf() const { return _ndf; }
  double get_P_value() const { return _P_value; }
  double get_tracker() const { return _tracker; }
  double get_mass() const { return _mass; }
  double get_momentum() const { return _momentum; }

  TMatrixD get_Q() const { return _Q; }
  void set_Q(TMatrixD Q) { _Q = Q; }
  void set_mass(double mass) { _mass = mass; }
  void set_momentum(double momentum) { _momentum = momentum; }

  void compute_chi2(const std::vector<KalmanSite> &sites);
*/

namespace MAUS {

class KalmanTrackTest : public ::testing::Test {
 protected:
  KalmanTrackTest()  {}
  virtual ~KalmanTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  void set_up_sites();
  // MAUS::KalmanSite *a_site;
  MAUS::KalmanSite old_site;
  MAUS::KalmanSite new_site;
};

void KalmanTrackTest::set_up_sites() {
  double deltaZ = 1100.0;
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
}

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
TEST_F(KalmanTrackTest, test_error_methods) {
  // MAUS::KalmanTrack *track = new MAUS::StraightTrack();

  // double momentum = 200.0;
  // track->BetheBlochStoppingPower(momentum);


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

  // KalmanSite *a_site;
  // a_site->set_direction(direction_plane0_tracker0);
  // a_site->set_measurement(0.0);
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
