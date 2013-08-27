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

// C++ vector
#include <vector>

// ROOT headers
#include "TCanvas.h"

// Google test headers
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/TrackerData.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterSpoints.hh"

namespace MAUS {

class TrackerDataPlotterSpointsTest : public ::testing::Test {
 protected:
  TrackerDataPlotterSpointsTest()  {}
  virtual ~TrackerDataPlotterSpointsTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(TrackerDataPlotterSpointsTest, TestConstructor) {
  TrackerDataPlotterSpoints* plt = new TrackerDataPlotterSpoints();
  EXPECT_FALSE(plt->_gr_xy1);
  EXPECT_FALSE(plt->_gr_zx1);
  EXPECT_FALSE(plt->_gr_zy1);
  EXPECT_FALSE(plt->_gr_xy2);
  EXPECT_FALSE(plt->_gr_zx2);
  EXPECT_FALSE(plt->_gr_zy2);
  delete plt;
}

TEST_F(TrackerDataPlotterSpointsTest, TestBrackets) {
  TrackerDataPlotterSpoints* plt = new TrackerDataPlotterSpoints();

  // Set up some mock spacepoint data
  TrackerData t1;
  TrackerData t2;
  double x1 = 1.0;
  double y1 = 2.0;
  double z1 = 3.0;
  double x2 = 4.0;
  double y2 = 5.0;
  double z2 = 6.0;
  t1._spoints_x.push_back(x1);
  t1._spoints_y.push_back(y1);
  t1._spoints_z.push_back(z1);
  t2._spoints_x.push_back(x2);
  t2._spoints_y.push_back(y2);
  t2._spoints_z.push_back(z2);

  // Check Canvases assign properly
  TCanvas* c1 = new TCanvas();
  TCanvas* c2 = (*plt)(t1, t2, c1);
  EXPECT_TRUE(c1 == c2);
  TCanvas* c3 = (*plt)(t1, t2);
  EXPECT_FALSE(c1 == c3);

  delete c1;
  delete plt;

  // Check the internal state is correct after adding data
  plt = new TrackerDataPlotterSpoints();
  (*plt)(t1, t2);


  delete plt;
}

} // ~namespace MAUS
