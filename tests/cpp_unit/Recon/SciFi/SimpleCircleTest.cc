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

// C headers
#include <assert.h>

// Other headers
#include "Config/MiceModule.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

#include "gtest/gtest.h"

// namespace MAUS {
class SimpleCircleTest : public ::testing::Test {
  protected:
    SimpleCircleTest()  {}
    virtual ~SimpleCircleTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SimpleCircleTest, test_setters_getters) {
  double x0 = -4.625;
  double x0_err = .1;
  double y0 = 0.172;
  double y0_err = .1;
  double R = 12.833;
  double R_err = .1;
  double alpha = 2.;
  double alpha_err = .1;
  double beta = 3;
  double beta_err = .1;
  double gamma = 5;
  double gamma_err = .1;
  double kappa = 0.5;
  double kappa_err = .1;
  double delta_R = .3;
  double chisq = 1.;

  SimpleCircle circle;

  circle.set_x0(x0);
  circle.set_x0_err(x0_err);
  circle.set_y0(y0);
  circle.set_y0_err(y0_err);
  circle.set_R(R);
  circle.set_R_err(R_err);
  circle.set_alpha(alpha);
  circle.set_alpha_err(alpha_err);
  circle.set_beta(beta);
  circle.set_beta_err(beta_err);
  circle.set_gamma(gamma);
  circle.set_gamma_err(gamma_err);
  circle.set_kappa(kappa);
  circle.set_kappa_err(kapper_err);
  circle.set_delta_R(delta_R);
  circle.set_chisq(chisq);

  EXPECT_EQ(circle.get_x0(), x0);
  EXPECT_EQ(circle.get_x0_err(), x0_err);
  EXPECT_EQ(circle.get_y0(), y0);
  EXPECT_EQ(circle.get_y0_err(), y0_err);
  EXPECT_EQ(circle.get_R(), R);
  EXPECT_EQ(circle.get_R_err(), R_err);
  EXPECT_EQ(circle.get_alpha(), alpha);
  EXPECT_EQ(circle.get_alpha_err(), alpha_err);
  EXPECT_EQ(circle.get_beta(), beta);
  EXPECT_EQ(circle.get_beta_err(), beta_err);
  EXPECT_EQ(circle.get_gamma(), gamma);
  EXPECT_EQ(circle.get_gamma_err(), gamma_err);
  EXPECT_EQ(circle.get_kappa(), kappa);
  EXPECT_EQ(circle.get_kappa_err(), kappa_err);
  EXPECT_EQ(circle.get_delta_R(), delta_R);
  EXPECT_EQ(circle.get_chisq(), chisq);
}
// } // namespace
