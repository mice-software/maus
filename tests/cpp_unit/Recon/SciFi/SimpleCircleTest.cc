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

#include <vector>

#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "gtest/gtest.h"

namespace MAUS {

class SimpleCircleTestDS : public ::testing::Test {
  protected:
    SimpleCircleTestDS()  {}
    virtual ~SimpleCircleTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SimpleCircleTestDS, test_default_constructor) {
  SimpleCircle circ1;
  EXPECT_EQ(circ1.get_x0(), 0.0);
  EXPECT_EQ(circ1.get_x0_err(), 0.0);
  EXPECT_EQ(circ1.get_y0(), 0.0);
  EXPECT_EQ(circ1.get_y0_err(), 0.0);
  EXPECT_EQ(circ1.get_R(), 0.0);
  EXPECT_EQ(circ1.get_R_err(), 0.0);
  EXPECT_EQ(circ1.get_alpha(), 0.0);
  EXPECT_EQ(circ1.get_alpha_err(), 0.0);
  EXPECT_EQ(circ1.get_beta(), 0.0);
  EXPECT_EQ(circ1.get_beta_err(), 0.0);
  EXPECT_EQ(circ1.get_gamma(), 0.0);
  EXPECT_EQ(circ1.get_gamma_err(), 0.0);
  EXPECT_EQ(circ1.get_kappa(), 0.0);
  EXPECT_EQ(circ1.get_kappa_err(), 0.0);
  EXPECT_EQ(circ1.get_delta_R(), 0.0);
  EXPECT_EQ(circ1.get_chisq(), 0.0);
}

TEST_F(SimpleCircleTestDS, test_first_parameter_constructor) {
  double x0 = 1.0;
  double y0 = 2.0;
  double R = 3.0;

  SimpleCircle circ1(x0, y0, R);

  EXPECT_EQ(circ1.get_x0(), x0);
  EXPECT_EQ(circ1.get_x0_err(), 0.0);
  EXPECT_EQ(circ1.get_y0(), y0);
  EXPECT_EQ(circ1.get_y0_err(), 0.0);
  EXPECT_EQ(circ1.get_R(), R);
  EXPECT_EQ(circ1.get_R_err(), 0.0);
  EXPECT_EQ(circ1.get_alpha(), 0.0);
  EXPECT_EQ(circ1.get_alpha_err(), 0.0);
  EXPECT_EQ(circ1.get_beta(), 0.0);
  EXPECT_EQ(circ1.get_beta_err(), 0.0);
  EXPECT_EQ(circ1.get_gamma(), 0.0);
  EXPECT_EQ(circ1.get_gamma_err(), 0.0);
  EXPECT_EQ(circ1.get_kappa(), 0.0);
  EXPECT_EQ(circ1.get_kappa_err(), 0.0);
  EXPECT_EQ(circ1.get_delta_R(), 0.0);
  EXPECT_EQ(circ1.get_chisq(), 0.0);
}

TEST_F(SimpleCircleTestDS, test_second_parameter_constructor) {
  double x0 = 1.0;
  double x0_err = 0.1;
  double y0 = 2.0;
  double y0_err = 0.2;
  double R = 3.0;
  double R_err = 0.3;
  double alpha = 4.0;
  double alpha_err = 0.4;
  double beta = 5.0;
  double beta_err = 0.5;
  double gamma = 6.0;
  double gamma_err = 7.0;
  double kappa = 8.0;
  double kappa_err = 0.8;
  double delta_R = 9;
  double chisq = 10;
  double pvalue = 11.0;

  SimpleCircle circ1(x0, x0_err, y0, y0_err, R, R_err, alpha, alpha_err, beta, beta_err,
                     gamma, gamma_err, kappa, kappa_err, delta_R, chisq, pvalue);

  EXPECT_EQ(circ1.get_x0(), x0);
  EXPECT_EQ(circ1.get_x0_err(), x0_err);
  EXPECT_EQ(circ1.get_y0(), y0);
  EXPECT_EQ(circ1.get_y0_err(), y0_err);
  EXPECT_EQ(circ1.get_R(), R);
  EXPECT_EQ(circ1.get_R_err(), R_err);
  EXPECT_EQ(circ1.get_alpha(), alpha);
  EXPECT_EQ(circ1.get_alpha_err(), alpha_err);
  EXPECT_EQ(circ1.get_beta(), beta);
  EXPECT_EQ(circ1.get_beta_err(), beta_err);
  EXPECT_EQ(circ1.get_gamma(), gamma);
  EXPECT_EQ(circ1.get_gamma_err(), gamma_err);
  EXPECT_EQ(circ1.get_kappa(), kappa);
  EXPECT_EQ(circ1.get_kappa_err(), kappa_err);
  EXPECT_EQ(circ1.get_delta_R(), delta_R);
  EXPECT_EQ(circ1.get_chisq(), chisq);
  EXPECT_EQ(circ1.get_pvalue(), pvalue);
}

TEST_F(SimpleCircleTestDS, test_getters_setters_clear) {
  double x0 = 1.0;
  double x0_err = 0.1;
  double y0 = 2.0;
  double y0_err = 0.2;
  double R = 3.0;
  double R_err = 0.3;
  double alpha = 4.0;
  double alpha_err = 0.4;
  double beta = 5.0;
  double beta_err = 0.5;
  double gamma = 6.0;
  double gamma_err = 7.0;
  double kappa = 8.0;
  double kappa_err = 0.8;
  double delta_R = 9.0;
  double chisq = 10.0;
  double pvalue = 11.0;

  SimpleCircle circ1;

  circ1.set_x0(x0);
  circ1.set_x0_err(x0_err);
  circ1.set_y0(y0);
  circ1.set_y0_err(y0_err);
  circ1.set_R(R);
  circ1.set_R_err(R_err);
  circ1.set_alpha(alpha);
  circ1.set_alpha_err(alpha_err);
  circ1.set_beta(beta);
  circ1.set_beta_err(beta_err);
  circ1.set_gamma(gamma);
  circ1.set_gamma_err(gamma_err);
  circ1.set_kappa(kappa);
  circ1.set_kappa_err(kappa_err);
  circ1.set_delta_R(delta_R);
  circ1.set_chisq(chisq);
  circ1.set_pvalue(pvalue);

  EXPECT_EQ(circ1.get_x0(), x0);
  EXPECT_EQ(circ1.get_x0_err(), x0_err);
  EXPECT_EQ(circ1.get_y0(), y0);
  EXPECT_EQ(circ1.get_y0_err(), y0_err);
  EXPECT_EQ(circ1.get_R(), R);
  EXPECT_EQ(circ1.get_R_err(), R_err);
  EXPECT_EQ(circ1.get_alpha(), alpha);
  EXPECT_EQ(circ1.get_alpha_err(), alpha_err);
  EXPECT_EQ(circ1.get_beta(), beta);
  EXPECT_EQ(circ1.get_beta_err(), beta_err);
  EXPECT_EQ(circ1.get_gamma(), gamma);
  EXPECT_EQ(circ1.get_gamma_err(), gamma_err);
  EXPECT_EQ(circ1.get_kappa(), kappa);
  EXPECT_EQ(circ1.get_kappa_err(), kappa_err);
  EXPECT_EQ(circ1.get_delta_R(), delta_R);
  EXPECT_EQ(circ1.get_chisq(), chisq);
  EXPECT_EQ(circ1.get_pvalue(), pvalue);

  circ1.clear();

  EXPECT_EQ(circ1.get_x0(), 0.0);
  EXPECT_EQ(circ1.get_x0_err(), 0.0);
  EXPECT_EQ(circ1.get_y0(), 0.0);
  EXPECT_EQ(circ1.get_y0_err(), 0.0);
  EXPECT_EQ(circ1.get_R(), 0.0);
  EXPECT_EQ(circ1.get_R_err(), 0.0);
  EXPECT_EQ(circ1.get_alpha(), 0.0);
  EXPECT_EQ(circ1.get_alpha_err(), 0.0);
  EXPECT_EQ(circ1.get_beta(), 0.0);
  EXPECT_EQ(circ1.get_beta_err(), 0.0);
  EXPECT_EQ(circ1.get_gamma(), 0.0);
  EXPECT_EQ(circ1.get_gamma_err(), 0.0);
  EXPECT_EQ(circ1.get_kappa(), 0.0);
  EXPECT_EQ(circ1.get_kappa_err(), 0.0);
  EXPECT_EQ(circ1.get_delta_R(), 0.0);
  EXPECT_EQ(circ1.get_chisq(), 0.0);
  EXPECT_EQ(circ1.get_pvalue(), 0.0);

  circ1.set_parameters(x0, x0_err, y0, y0_err, R, R_err, alpha, alpha_err, beta, beta_err,
                       gamma, gamma_err, kappa, kappa_err, delta_R, chisq, pvalue);

  EXPECT_EQ(circ1.get_x0(), x0);
  EXPECT_EQ(circ1.get_x0_err(), x0_err);
  EXPECT_EQ(circ1.get_y0(), y0);
  EXPECT_EQ(circ1.get_y0_err(), y0_err);
  EXPECT_EQ(circ1.get_R(), R);
  EXPECT_EQ(circ1.get_R_err(), R_err);
  EXPECT_EQ(circ1.get_alpha(), alpha);
  EXPECT_EQ(circ1.get_alpha_err(), alpha_err);
  EXPECT_EQ(circ1.get_beta(), beta);
  EXPECT_EQ(circ1.get_beta_err(), beta_err);
  EXPECT_EQ(circ1.get_gamma(), gamma);
  EXPECT_EQ(circ1.get_gamma_err(), gamma_err);
  EXPECT_EQ(circ1.get_kappa(), kappa);
  EXPECT_EQ(circ1.get_kappa_err(), kappa_err);
  EXPECT_EQ(circ1.get_delta_R(), delta_R);
  EXPECT_EQ(circ1.get_chisq(), chisq);
  EXPECT_EQ(circ1.get_pvalue(), pvalue);
}

} // ~namespace MAUS
