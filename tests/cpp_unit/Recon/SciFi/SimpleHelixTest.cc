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
  EXPECT_EQ(h1.get_Phi_0(), 0.0);
  EXPECT_EQ(h1.get_Phi_0_err(), 0.0);
  EXPECT_EQ(h1.get_R(), 0.0);
  EXPECT_EQ(h1.get_R_err(), 0.0);
  EXPECT_EQ(h1.get_tan_lambda(), 0.0);
  EXPECT_EQ(h1.get_tan_lambda_err(), 0.0);
  EXPECT_EQ(h1.get_Psi_0(), 0.0);
  EXPECT_EQ(h1.get_Psi_0_err(), 0.0);
  EXPECT_EQ(h1.get_chisq(), 0.0);
  EXPECT_EQ(h1.get_chisq_dof(), 0.0);
  EXPECT_EQ(h1.get_dsdz(), 0.0);
  EXPECT_EQ(h1.get_dsdz_err(), 0.0);
}

TEST_F(SimpleHelixTestDS, test_parameter_constructor) {

  double Phi_0 = 1.0;
  double Phi_0_err = 0.1;
  double R = 2.0;
  double R_err = 0.2;
  double tan_lambda = 3.0;
  double tan_lambda_err = 0.3;
  double Psi_0 = 4.0;
  double Psi_0_err = 0.4;
  double chisq = 5.0;
  double chisq_dof = 0.5;

  SimpleHelix h1(Phi_0, Phi_0_err, R, R_err, tan_lambda, tan_lambda_err,
                 Psi_0, Psi_0_err, chisq, chisq_dof);

  EXPECT_EQ(h1.get_Phi_0(), Phi_0);
  EXPECT_EQ(h1.get_Phi_0_err(), Phi_0_err);
  EXPECT_EQ(h1.get_R(), R);
  EXPECT_EQ(h1.get_R_err(), R_err);
  EXPECT_EQ(h1.get_tan_lambda(), tan_lambda);
  EXPECT_EQ(h1.get_tan_lambda_err(), tan_lambda_err);
  EXPECT_EQ(h1.get_Psi_0(), Psi_0);
  EXPECT_EQ(h1.get_Psi_0_err(), Psi_0_err);
  EXPECT_EQ(h1.get_chisq(), chisq);
  EXPECT_EQ(h1.get_chisq_dof(), chisq_dof);
  EXPECT_EQ(h1.get_dsdz(), 0.0);
  EXPECT_EQ(h1.get_dsdz_err(), 0.0);
}

TEST_F(SimpleHelixTestDS, test_getters_setters_clear) {
  double Phi_0 = 1.0;
  double Phi_0_err = 0.1;
  double R = 2.0;
  double R_err = 0.2;
  double tan_lambda = 3.0;
  double tan_lambda_err = 0.3;
  double Psi_0 = 4.0;
  double Psi_0_err = 0.4;
  double chisq = 5.0;
  double chisq_dof = 0.5;
  double dsdz = 6.0;
  double dsdz_err = 0.6;

  SimpleHelix h1;

  h1.set_Phi_0(Phi_0);
  h1.set_Phi_0_err(Phi_0_err);
  h1.set_R(R);
  h1.set_R_err(R_err);
  h1.set_tan_lambda(tan_lambda);
  h1.set_tan_lambda_err(tan_lambda_err);
  h1.set_Psi_0(Psi_0);
  h1.set_Psi_0_err(Psi_0_err);
  h1.set_chisq(chisq);
  h1.set_chisq_dof(chisq_dof);
  h1.set_dsdz(dsdz);
  h1.set_dsdz_err(dsdz_err);

  EXPECT_EQ(h1.get_Phi_0(), Phi_0);
  EXPECT_EQ(h1.get_Phi_0_err(), Phi_0_err);
  EXPECT_EQ(h1.get_R(), R);
  EXPECT_EQ(h1.get_R_err(), R_err);
  EXPECT_EQ(h1.get_tan_lambda(), tan_lambda);
  EXPECT_EQ(h1.get_tan_lambda_err(), tan_lambda_err);
  EXPECT_EQ(h1.get_Psi_0(), Psi_0);
  EXPECT_EQ(h1.get_Psi_0_err(), Psi_0_err);
  EXPECT_EQ(h1.get_chisq(), chisq);
  EXPECT_EQ(h1.get_chisq_dof(), chisq_dof);
  EXPECT_EQ(h1.get_dsdz(), dsdz);
  EXPECT_EQ(h1.get_dsdz_err(), dsdz_err);

  h1.clear();

  EXPECT_EQ(h1.get_Phi_0(), 0.0);
  EXPECT_EQ(h1.get_Phi_0_err(), 0.0);
  EXPECT_EQ(h1.get_R(), 0.0);
  EXPECT_EQ(h1.get_R_err(), 0.0);
  EXPECT_EQ(h1.get_tan_lambda(), 0.0);
  EXPECT_EQ(h1.get_tan_lambda_err(), 0.0);
  EXPECT_EQ(h1.get_Psi_0(), 0.0);
  EXPECT_EQ(h1.get_Psi_0_err(), 0.0);
  EXPECT_EQ(h1.get_chisq(), 0.0);
  EXPECT_EQ(h1.get_chisq_dof(), 0.0);
  EXPECT_EQ(h1.get_dsdz(), 0.0);
  EXPECT_EQ(h1.get_dsdz_err(), 0.0);

  h1.set_parameters(Phi_0, Phi_0_err, R, R_err, tan_lambda, tan_lambda_err,
                    Psi_0, Psi_0_err, chisq, chisq_dof);

  EXPECT_EQ(h1.get_Phi_0(), Phi_0);
  EXPECT_EQ(h1.get_Phi_0_err(), Phi_0_err);
  EXPECT_EQ(h1.get_R(), R);
  EXPECT_EQ(h1.get_R_err(), R_err);
  EXPECT_EQ(h1.get_tan_lambda(), tan_lambda);
  EXPECT_EQ(h1.get_tan_lambda_err(), tan_lambda_err);
  EXPECT_EQ(h1.get_Psi_0(), Psi_0);
  EXPECT_EQ(h1.get_Psi_0_err(), Psi_0_err);
  EXPECT_EQ(h1.get_chisq(), chisq);
  EXPECT_EQ(h1.get_chisq_dof(), chisq_dof);
  // EXPECT_EQ(h1.get_dsdz(), dsdz);
  // EXPECT_EQ(h1.get_dsdz_err(), dsdz_err);
}

} // ~namespace MAUS
