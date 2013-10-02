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

// ROOT headers
#include "TCanvas.h"

// Google test headers
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterInfoBox.hh"

namespace MAUS {

class TrackerDataPlotterInfoBoxTest : public ::testing::Test {
 protected:
  TrackerDataPlotterInfoBoxTest()  {}
  virtual ~TrackerDataPlotterInfoBoxTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(TrackerDataPlotterInfoBoxTest, TestConstructor) {

  // Test the default contructor
  TrackerDataPlotterInfoBox* infoBox = new TrackerDataPlotterInfoBox();
  EXPECT_FALSE(infoBox->_setup_true);
  EXPECT_EQ(0, infoBox->_tot_digits_t1);
  EXPECT_EQ(0, infoBox->_tot_clusters_t1);
  EXPECT_EQ(0, infoBox->_tot_spoints_t1);
  EXPECT_EQ(0, infoBox->_tot_5htracks_t1);
  EXPECT_EQ(0, infoBox->_tot_4htracks_t1);
  EXPECT_EQ(0, infoBox->_tot_3htracks_t1);
  EXPECT_EQ(0, infoBox->_tot_5stracks_t1);
  EXPECT_EQ(0, infoBox->_tot_4stracks_t1);
  EXPECT_EQ(0, infoBox->_tot_3stracks_t1);
  EXPECT_EQ(0, infoBox->_tot_digits_t2);
  EXPECT_EQ(0, infoBox->_tot_clusters_t2);
  EXPECT_EQ(0, infoBox->_tot_spoints_t2);
  EXPECT_EQ(0, infoBox->_tot_5htracks_t2);
  EXPECT_EQ(0, infoBox->_tot_4htracks_t2);
  EXPECT_EQ(0, infoBox->_tot_3htracks_t2);
  EXPECT_EQ(0, infoBox->_tot_5stracks_t2);
  EXPECT_EQ(0, infoBox->_tot_4stracks_t2);
  EXPECT_EQ(0, infoBox->_tot_3stracks_t2);
  EXPECT_EQ(300, infoBox->_canvas_width);
  EXPECT_EQ(750, infoBox->_canvas_height);
  delete infoBox;

  // Test the copy constructor
  int canvas_width = 1;
  int canvas_height = 2;
  double line1_pos = 0.1;
  double line2_pos = 0.2;
  infoBox = new TrackerDataPlotterInfoBox(canvas_width, canvas_height, line1_pos, line2_pos);
  EXPECT_FALSE(infoBox->_setup_true);
  EXPECT_EQ(0, infoBox->_tot_digits_t1);
  EXPECT_EQ(0, infoBox->_tot_clusters_t1);
  EXPECT_EQ(0, infoBox->_tot_spoints_t1);
  EXPECT_EQ(0, infoBox->_tot_5htracks_t1);
  EXPECT_EQ(0, infoBox->_tot_4htracks_t1);
  EXPECT_EQ(0, infoBox->_tot_3htracks_t1);
  EXPECT_EQ(0, infoBox->_tot_5stracks_t1);
  EXPECT_EQ(0, infoBox->_tot_4stracks_t1);
  EXPECT_EQ(0, infoBox->_tot_3stracks_t1);
  EXPECT_EQ(0, infoBox->_tot_digits_t2);
  EXPECT_EQ(0, infoBox->_tot_clusters_t2);
  EXPECT_EQ(0, infoBox->_tot_spoints_t2);
  EXPECT_EQ(0, infoBox->_tot_5htracks_t2);
  EXPECT_EQ(0, infoBox->_tot_4htracks_t2);
  EXPECT_EQ(0, infoBox->_tot_3htracks_t2);
  EXPECT_EQ(0, infoBox->_tot_5stracks_t2);
  EXPECT_EQ(0, infoBox->_tot_4stracks_t2);
  EXPECT_EQ(0, infoBox->_tot_3stracks_t2);
  EXPECT_EQ(canvas_width, infoBox->_canvas_width);
  EXPECT_EQ(canvas_height, infoBox->_canvas_height);
  EXPECT_EQ(line1_pos, infoBox->_line1_pos);
  EXPECT_EQ(line2_pos, infoBox->_line2_pos);
  delete infoBox;
}

TEST_F(TrackerDataPlotterInfoBoxTest, TestBrackets) {
  TrackerDataPlotterInfoBox* infoBox = new TrackerDataPlotterInfoBox();

  // Set up some mock data
  TrackerData t1;
  TrackerData t2;
  t1._num_digits = 1;
  t1._num_clusters = 1;
  t1._num_spoints = 1;
  t1._num_seeds = 1;
  t1._num_stracks_5pt = 1;
  t1._num_stracks_4pt = 1;
  t1._num_stracks_3pt = 1;
  t1._num_htracks_5pt = 1;
  t1._num_htracks_4pt = 1;
  t1._num_htracks_3pt = 1;
  t2._num_digits = 2;
  t2._num_clusters = 2;
  t2._num_spoints = 2;
  t2._num_seeds = 2;
  t2._num_stracks_5pt = 2;
  t2._num_stracks_4pt = 2;
  t2._num_stracks_3pt = 2;
  t2._num_htracks_5pt = 2;
  t2._num_htracks_4pt = 2;
  t2._num_htracks_3pt = 2;

  // Send data to infoBox twice to check accumulates, and check Canvases assigned properly
  TCanvas* c1 = new TCanvas();
  TCanvas* c2 = (*infoBox)(t1, t2, c1);
  EXPECT_TRUE(c1 == c2);
  TCanvas* c3 = (*infoBox)(t1, t2); // No canvas passed so should switch to internal canvas
  EXPECT_TRUE(c3 == infoBox->_Canvas);
  delete c1;
  c1 = NULL;
  c2 = NULL;

  // Check infoBox internal state (not bothering to check internal state of root TPaveTexts etc)
  EXPECT_EQ(2, infoBox->_tot_digits_t1);
  EXPECT_EQ(2, infoBox->_tot_clusters_t1);
  EXPECT_EQ(2, infoBox->_tot_spoints_t1);
  EXPECT_EQ(2, infoBox->_tot_5stracks_t1);
  EXPECT_EQ(2, infoBox->_tot_4stracks_t1);
  EXPECT_EQ(2, infoBox->_tot_3stracks_t1);
  EXPECT_EQ(2, infoBox->_tot_5htracks_t1);
  EXPECT_EQ(2, infoBox->_tot_4htracks_t1);
  EXPECT_EQ(2, infoBox->_tot_3htracks_t1);
  EXPECT_EQ(4, infoBox->_tot_digits_t2);
  EXPECT_EQ(4, infoBox->_tot_clusters_t2);
  EXPECT_EQ(4, infoBox->_tot_spoints_t2);
  EXPECT_EQ(4, infoBox->_tot_5stracks_t2);
  EXPECT_EQ(4, infoBox->_tot_4stracks_t2);
  EXPECT_EQ(4, infoBox->_tot_3stracks_t2);
  EXPECT_EQ(4, infoBox->_tot_5htracks_t2);
  EXPECT_EQ(4, infoBox->_tot_4htracks_t2);
  EXPECT_EQ(4, infoBox->_tot_3htracks_t2);
  delete infoBox; // Deletes the current canvas too
}

} // ~namespace MAUS
