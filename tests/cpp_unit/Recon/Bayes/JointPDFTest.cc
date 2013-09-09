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
    double shift_min = -10.;
    double shift_max = 10.;
    double bin_width = 0.2;
    _JointPDF = new JointPDF(lname, bin_width, shift_min, shift_max);
    double sigma = 1.5; // mm
    int number_of_tosses = 1000000;
    _JointPDF->Build("gaussian", sigma, number_of_tosses);
  }
  virtual void TearDown() {}
  JointPDF *_JointPDF;
  static const double err = 0.001;
};

TEST_F(JointPDFTest, test_mean) {
  double expected_mean = 1.2;
  TH1D *likelihood = reinterpret_cast<TH1D*>
                     ((&_JointPDF->GetLikelihood(expected_mean))->Clone("JointPDF"));

  double mean = likelihood->GetMean();
  EXPECT_NEAR(expected_mean, mean, err);
}

} // ~namespace MAUS
