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

#include "gtest/gtest.h"

namespace MAUS {

class KalmanTrackTest : public ::testing::Test {
 protected:
  KalmanTrackTest()  {}
  virtual ~KalmanTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  KalmanSite *a_site;
};

TEST_F(KalmanTrackTest, test_constructor) {
  a_site->set_measurement(0);
  KalmanTrack *track = new HelicalTrack();
  EXPECT_EQ(track->get_chi2(), 0.0);
  EXPECT_EQ(track->get_ndf(), 0.0);
  EXPECT_EQ(track->get_tracker(), 0.0);
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
}


} // ~namespace MAUS
