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
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"

#include "gtest/gtest.h"

namespace {
class KalmanTrackTest : public ::testing::Test {
 protected:
  KalmanTrackTest()  {}
  virtual ~KalmanTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  KalmanSite *a_site;
};
}

/*
TEST_F(KalmanTrackTest, test_updateG) {
  a_site->set_measurement(0);
  KalmanTrack *track = new StraightTrack();
  track->update_G(a_site);
}

TEST_F(KalmanTrackTest, test_propagator) {
}

} // namespace

void KalmanTrack::update_G(KalmanSite *a_site) {
  double alpha = (a_site->get_measurement())(0, 0);
  double l = pow(ACTIVE_RADIUS*ACTIVE_RADIUS -
                 (alpha*CHAN_WIDTH)*(alpha*CHAN_WIDTH), 0.5);
  double sig_beta = l/CHAN_WIDTH;
  double SIG_ALPHA = 1.0;
  _G.Zero();
  _G(0, 0) = SIG_ALPHA*SIG_ALPHA;
  _G(1, 1) = sig_beta*sig_beta;
  _G.Invert();
}
*/
