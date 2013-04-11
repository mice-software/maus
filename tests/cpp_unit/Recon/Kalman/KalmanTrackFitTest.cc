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
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "gtest/gtest.h"

namespace {
class KalmanTrackFitTest : public ::testing::Test {
 protected:
  KalmanTrackFitTest()  {}
  virtual ~KalmanTrackFitTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}

  MAUS::KalmanSite old_site;
  MAUS::KalmanSite new_site;
  double z0, z1;
  double x0, y0, mx0, my0, kappa;
  double x1, y1, mx1, my1;
  void set_up_sites();
  TMatrixD a;
  static const double err = 1.e-4;
};

/*
void KalmanTrackFitTest::set_up_sites() {
  old_site.Initialise(5);
  new_site.Initialise(5);
  kappa = 1./200.;
  x0 = 0.;
  y0 = 5.;
  z0 = 0.;
  mx0 = 15.*kappa;
  my0 = 15.*kappa;

  x1 = -8.0019;
  y1 = 34.614;
  z1 = 350.;
  mx1 = -20.513*kappa;
  my1 = 5.4042*kappa;

  old_site.set_z(z0);
  new_site.set_z(z1);
  new_site.set_id(16);

  a.ResizeTo(5, 1);
  a(0, 0) = x0;
  a(1, 0) = mx0;
  a(2, 0) = y0;
  a(3, 0) = my0;
  a(4, 0) = kappa;
  old_site.set_a(a, MAUS::KalmanSite::Filtered);

  TMatrixD C(5, 5);
  C.Zero();
  for ( int i = 0; i < 5; ++i ) {
     C(i, i) = 1.; // dummy values
  }
  old_site.set_covariance_matrix(C, MAUS::KalmanSite::Projected);
}
*/

TEST_F(KalmanTrackFitTest, test_filter_without_station) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  std::vector<MAUS::KalmanSite> sites;

  int ignore_station = 6;

  MAUS::KalmanTrackFit track_fit;
  EXPECT_THROW(track_fit.RunFilter(track, sites, ignore_station), Squeal);
  delete track;
}

} // namespace
