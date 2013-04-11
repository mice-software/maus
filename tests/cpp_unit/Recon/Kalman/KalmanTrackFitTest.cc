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

TEST_F(KalmanTrackFitTest, test_filter_without_station) {
  MAUS::KalmanTrack *track = new MAUS::HelicalTrack(false, false);
  std::vector<MAUS::KalmanSite> sites;

  int ignore_station = 6;

  MAUS::KalmanTrackFit track_fit;
  EXPECT_THROW(track_fit.RunFilter(track, sites, ignore_station), Squeal);
  delete track;
}
} // namespace
