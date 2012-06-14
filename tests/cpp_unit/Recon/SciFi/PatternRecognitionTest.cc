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

#include "gtest/gtest.h"

#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"

namespace {
class PatternRecognitionTest : public ::testing::Test {
 protected:

  PatternRecognitionTest()  {}
  virtual ~PatternRecognitionTest() {}

  virtual void SetUp()    {
    // Set some spacepoints representing a straight line track.
    // We do not call a matching delete in TearDown() as the SciFiEvent destructor does this.
    sp1 = new SciFiSpacePoint();
    sp2 = new SciFiSpacePoint();
    sp3 = new SciFiSpacePoint();
    sp4 = new SciFiSpacePoint();
    sp5 = new SciFiSpacePoint();

    sp1->get_position().set(-68.24883333333334, -57.810948479361, -0.652299999999741);
    sp1->set_tracker(0);
    sp1->set_station(1);
    sp1->set_type("triplet");

    sp2->get_position().set(-62.84173333333334, -67.17694825239995, -200.6168999999991);
    sp2->set_tracker(0);
    sp2->set_station(2);
    sp2->set_type("triplet");

    sp3->get_position().set(-56.99676666666667, -76.0964980027428, -450.4798999999994);
    sp3->set_tracker(0);
    sp3->set_station(3);
    sp3->set_type("triplet");

    sp4->get_position().set(-47.89523333333333, -87.75184770769343, -750.4801999999991);
    sp4->set_tracker(0);
    sp4->set_station(4);
    sp4->set_type("triplet");

    sp5->get_position().set(-35.86799999999999, -99.22774738994798, -1100.410099999999);
    sp5->set_tracker(0);
    sp5->set_station(5);
    sp5->set_type("triplet");

    evt.add_spacepoint(sp1);
    evt.add_spacepoint(sp2);
    evt.add_spacepoint(sp3);
    evt.add_spacepoint(sp4);
    evt.add_spacepoint(sp5);
  }

  virtual void TearDown() {}

  PatternRecognition pr;
  SciFiEvent evt;
  SciFiSpacePoint *sp1;
  SciFiSpacePoint *sp2;
  SciFiSpacePoint *sp3;
  SciFiSpacePoint *sp4;
  SciFiSpacePoint *sp5;
};

TEST_F(PatternRecognitionTest, test_sort_by_station) {

  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);

  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(5);
  pr.sort_by_station(spnts, spnts_by_station);
  EXPECT_EQ(sp1, spnts_by_station[0][0]);
  EXPECT_EQ(sp2, spnts_by_station[1][0]);
  EXPECT_EQ(sp3, spnts_by_station[2][0]);
  EXPECT_EQ(sp4, spnts_by_station[3][0]);
  EXPECT_EQ(sp5, spnts_by_station[4][0]);
}

TEST_F(PatternRecognitionTest, test_linear_fit) {

  // Test with a simple line, c = 2, m = 1, with three points, small errors
  std::vector<double> x, y, y_err;
  x.push_back(1);
  x.push_back(2);
  x.push_back(3);
  y.push_back(3);
  y.push_back(4);
  y.push_back(5);
  y_err.push_back(0.05);
  y_err.push_back(0.05);
  y_err.push_back(0.05);

  SimpleLine line;
  pr.linear_fit(x, y, y_err, line);

  EXPECT_EQ(2.0, line.get_c());
  EXPECT_EQ(1.0, line.get_m());
}

TEST_F(PatternRecognitionTest, test_process) {

  PatternRecognition pr;

  EXPECT_EQ(5, evt.spacepoints().size());

  // The track parameters that should be reconstructed from the spacepoints
  int num_points = 5;
  double x_chisq = 22.87148204566524;
  double y_chisq = 20.99052559576519;
  int tracker = 0;
  double y0 = -58.8520138913516;
  double x0 = -68.94108927256372;
  double my = 0.03755825500617989;
  double mx = -0.02902014417006935;

  pr.process(evt);

  EXPECT_EQ(5, evt.spacepoints().size());
  /*EXPECT_EQ(1, evt.straightprtracks().size());

  SciFiStraightPRTrack trk = evt.straightprtracks()[0];

  EXPECT_EQ(num_points, trk.get_num_points());
  */
}

}// namespace
