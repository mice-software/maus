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

#include "src/common_cpp/Recon/Bayes/JointPDF.hh"
#include "src/common_cpp/Recon/Bayes/PDF.hh"

#include "gtest/gtest.h"

namespace MAUS {

class JointPDFTest : public ::testing::Test {
 protected:
  JointPDFTest()  {}
  virtual ~JointPDFTest() {}
  virtual void SetUp() {
    // Sets up a JointPDF object for testing.
    std::string lname("JointPDF");
    std::string pname("prob_station3");
    _shift_min = -8.;
    _shift_max = 8.;
    _bin_width = 0.01;
    _jointPDF = new JointPDF(lname, _bin_width, _shift_min, _shift_max);
    double sigma = 1.5; // mm
    int number_of_tosses = 100000;
    _jointPDF->Build("gaussian", sigma, number_of_tosses);
  }
  virtual void TearDown() {
    delete _jointPDF;
  }
  JointPDF *_jointPDF;
  // err can be made smaller by defining a smaller _bin_width
  // That will increase the test time, so I'm keeping things
  // large.
  static constexpr double err = 0.01;
  double _shift_min;
  double _shift_max;
  double _bin_width;
};

TEST_F(JointPDFTest, test_binning) {
  // Bin numbers run from 0 to nbins.
  // (bin 0 and nbins+1 are underflow and overflow)
  // Must assert that bin 0 and bin nbins have their centres
  // falling on the extremes we defined for our PDF.
  // Must also check that the middle bin corresponds to 0.
  int n_bins = _jointPDF->n_bins();
  // centre of bin 1
  double bin_1_centre = _jointPDF->GetJointPDF()->GetXaxis()->GetBinCenter(1);
  EXPECT_NEAR(_shift_min, bin_1_centre, 1e-6);
  // centre of middle bin (x=0).
  double middle_bin_centre = _jointPDF->GetJointPDF()->GetXaxis()->GetBinCenter(n_bins/2+1);
  EXPECT_NEAR(0., middle_bin_centre, 1e-6);
  // centre of bin nbins
  double bin_n_bins_centre = _jointPDF->GetJointPDF()->GetXaxis()->GetBinCenter(n_bins);
  EXPECT_NEAR(_shift_max, bin_n_bins_centre, 1e-6);
}

TEST_F(JointPDFTest, test_mean) {
  double expected_mean = 0;
  TH1D *likelihood = reinterpret_cast<TH1D*>
                     ((_jointPDF->GetLikelihood(expected_mean)).Clone("likelihood"));
  double mean = likelihood->GetMean();
  EXPECT_NEAR(expected_mean, mean, err);
}

TEST_F(JointPDFTest, test_posterior) {
  // Build a posterior.
  double new_shift = 1.2;
  TH1D *likelihood = reinterpret_cast<TH1D*>
                     (_jointPDF->GetLikelihood(new_shift).Clone("likelihood"));
  EXPECT_NEAR(new_shift, likelihood->GetMean(), err);

  std::string pname("prob_station");
  PDF *pdf = new PDF(pname, _bin_width, _shift_min, _shift_max);
  // Assert the prior is flat.
  EXPECT_NEAR(0., pdf->GetMean(), err);
  pdf->ComputeNewPosterior(*likelihood);
  // Check if the posterior is correct.
  EXPECT_NEAR(new_shift, pdf->GetMean(), err);

  delete pdf;
}

} // ~namespace MAUS
