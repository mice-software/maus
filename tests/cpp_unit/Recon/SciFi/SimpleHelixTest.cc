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



#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"

#include "TMatrixD.h"
#include "gtest/gtest.h"

namespace MAUS {

class SimpleHelixTestDS : public ::testing::Test {
  protected:
    SimpleHelixTestDS()  {}
    virtual ~SimpleHelixTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SimpleHelixTestDS, test_default_constructor) {
  SimpleHelix h1;
  EXPECT_EQ(h1.get_xc(), 0.0);
  EXPECT_EQ(h1.get_xc_err(), 0.0);
  EXPECT_EQ(h1.get_yc(), 0.0);
  EXPECT_EQ(h1.get_yc_err(), 0.0);
  EXPECT_EQ(h1.get_R(), 0.0);
  EXPECT_EQ(h1.get_R_err(), 0.0);
  EXPECT_EQ(h1.get_dsdz(), 0.0);
  EXPECT_EQ(h1.get_dsdz_err(), 0.0);
  EXPECT_EQ(h1.get_s0(), 0.0);
  EXPECT_EQ(h1.get_s0_err(), 0.0);
  EXPECT_EQ(h1.get_transverse_chisq(), -1.0);
  EXPECT_EQ(h1.get_longitudinal_chisq(), -1.0);
  EXPECT_EQ(h1.get_chisq(), -1.0);
  EXPECT_EQ(h1.get_chisq_dof(), -1.0);
  EXPECT_EQ(h1.get_pvalue(), -1.0);

}

TEST_F(SimpleHelixTestDS, test_parameter_constructor) {
  double xc = 1.0;
  double xc_err = 0.1;
  double yc = 2.0;
  double yc_err = 0.2;
  double R = 3.0;
  double R_err = 0.3;
  double dsdz = 4.0;
  double dsdz_err = 0.4;
  double s0 = 5.0;
  double s0_err = 0.5;
  double transverse_chisq = 7.0;
  double longitudinal_chisq = 8.0;
  double chisq = 6.0;
  double chisq_dof = 1.0;
  double pvalue = 0.5;
  TMatrixD cov;

  SimpleHelix h1(xc, xc_err, yc, yc_err, R, R_err, dsdz, dsdz_err, s0, s0_err,
                 transverse_chisq, longitudinal_chisq, chisq, chisq_dof,
                 pvalue, cov);

  EXPECT_EQ(h1.get_xc(), xc);
  EXPECT_EQ(h1.get_xc_err(), xc_err);
  EXPECT_EQ(h1.get_yc(), yc);
  EXPECT_EQ(h1.get_yc_err(), yc_err);
  EXPECT_EQ(h1.get_R(), R);
  EXPECT_EQ(h1.get_R_err(), R_err);
  EXPECT_EQ(h1.get_dsdz(), dsdz);
  EXPECT_EQ(h1.get_dsdz_err(), dsdz_err);
  EXPECT_EQ(h1.get_s0(), s0);
  EXPECT_EQ(h1.get_s0_err(), s0_err);
  EXPECT_EQ(h1.get_transverse_chisq(), transverse_chisq);
  EXPECT_EQ(h1.get_longitudinal_chisq(), longitudinal_chisq);
  EXPECT_EQ(h1.get_chisq(), chisq);
  EXPECT_EQ(h1.get_chisq_dof(), chisq_dof);
  EXPECT_EQ(h1.get_pvalue(), pvalue);
}

TEST_F(SimpleHelixTestDS, test_getters_setters_clear) {
  double xc = 1.0;
  double xc_err = 0.1;
  double yc = 2.0;
  double yc_err = 0.2;
  double R = 3.0;
  double R_err = 0.3;
  double dsdz = 4.0;
  double dsdz_err = 0.4;
  double s0 = 5.0;
  double s0_err = 0.5;
  double transverse_chisq = 7.0;
  double longitudinal_chisq = 8.0;
  double chisq = 6.0;
  double chisq_dof = 1.0;
  double pvalue = 0.5;
  TMatrixD cov;

  SimpleHelix h1;

  h1.set_xc(xc);
  h1.set_xc_err(xc_err);
  h1.set_yc(yc);
  h1.set_yc_err(yc_err);
  h1.set_R(R);
  h1.set_R_err(R_err);
  h1.set_dsdz(dsdz);
  h1.set_dsdz_err(dsdz_err);
  h1.set_s0(s0);
  h1.set_s0_err(s0_err);
  h1.set_transverse_chisq(transverse_chisq);
  h1.set_longitudinal_chisq(longitudinal_chisq);
  h1.set_chisq(chisq);
  h1.set_chisq_dof(chisq_dof);
  h1.set_pvalue(pvalue);
  h1.set_cov(cov);

  EXPECT_EQ(h1.get_xc(), xc);
  EXPECT_EQ(h1.get_xc_err(), xc_err);
  EXPECT_EQ(h1.get_yc(), yc);
  EXPECT_EQ(h1.get_yc_err(), yc_err);
  EXPECT_EQ(h1.get_R(), R);
  EXPECT_EQ(h1.get_R_err(), R_err);
  EXPECT_EQ(h1.get_dsdz(), dsdz);
  EXPECT_EQ(h1.get_dsdz_err(), dsdz_err);
  EXPECT_EQ(h1.get_s0(), s0);
  EXPECT_EQ(h1.get_s0_err(), s0_err);
  EXPECT_EQ(h1.get_transverse_chisq(), transverse_chisq);
  EXPECT_EQ(h1.get_longitudinal_chisq(), longitudinal_chisq);
  EXPECT_EQ(h1.get_chisq(), chisq);
  EXPECT_EQ(h1.get_chisq_dof(), chisq_dof);
  EXPECT_EQ(h1.get_pvalue(), pvalue);

  h1.clear();

  EXPECT_EQ(h1.get_xc(), 0.0);
  EXPECT_EQ(h1.get_xc_err(), 0.0);
  EXPECT_EQ(h1.get_yc(), 0.0);
  EXPECT_EQ(h1.get_yc_err(), 0.0);
  EXPECT_EQ(h1.get_R(), 0.0);
  EXPECT_EQ(h1.get_R_err(), 0.0);
  EXPECT_EQ(h1.get_dsdz(), 0.0);
  EXPECT_EQ(h1.get_dsdz_err(), 0.0);
  EXPECT_EQ(h1.get_s0(), 0.0);
  EXPECT_EQ(h1.get_s0_err(), 0.0);
  EXPECT_EQ(h1.get_transverse_chisq(), -1.0);
  EXPECT_EQ(h1.get_longitudinal_chisq(), -1.0);
  EXPECT_EQ(h1.get_chisq(), -1.0);
  EXPECT_EQ(h1.get_chisq_dof(), -1.0);
  EXPECT_EQ(h1.get_pvalue(), -1.0);


  h1.set_parameters(xc, xc_err, yc, yc_err, R, R_err, dsdz, dsdz_err,
                    s0, s0_err, transverse_chisq, longitudinal_chisq,
                    chisq, chisq_dof, pvalue, cov);

  EXPECT_EQ(h1.get_xc(), xc);
  EXPECT_EQ(h1.get_xc_err(), xc_err);
  EXPECT_EQ(h1.get_yc(), yc);
  EXPECT_EQ(h1.get_yc_err(), yc_err);
  EXPECT_EQ(h1.get_R(), R);
  EXPECT_EQ(h1.get_R_err(), R_err);
  EXPECT_EQ(h1.get_dsdz(), dsdz);
  EXPECT_EQ(h1.get_dsdz_err(), dsdz_err);
  EXPECT_EQ(h1.get_s0(), s0);
  EXPECT_EQ(h1.get_s0_err(), s0_err);
  EXPECT_EQ(h1.get_transverse_chisq(), transverse_chisq);
  EXPECT_EQ(h1.get_longitudinal_chisq(), longitudinal_chisq);
  EXPECT_EQ(h1.get_chisq(), chisq);
  EXPECT_EQ(h1.get_chisq_dof(), chisq_dof);
  EXPECT_EQ(h1.get_pvalue(), pvalue);
}

} // ~namespace MAUS
