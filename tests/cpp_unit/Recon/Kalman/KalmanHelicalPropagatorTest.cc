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

#include "src/common_cpp/Recon/Kalman/KalmanHelicalPropagator.hh"
#include "gtest/gtest.h"

namespace {
class KalmanHelicalPropagatorTest : public ::testing::Test {
 protected:
  KalmanHelicalPropagatorTest() {}
  virtual ~KalmanHelicalPropagatorTest() {}
  virtual void SetUp()    {
    // if (!MAUS::Globals::HasInstance()) {
    //   MAUS::GlobalsManager::InitialiseGlobals("src/common_py/ConfigurationDefaults.py");
    // }

    old_site.Initialise(5);
    new_site.Initialise(5);
    kappa = -1./200.;
    x0 = 0.;
    y0 = 5.;
    z0 = 0.;
    mx0 = 15.*fabs(kappa);
    my0 = 15.*fabs(kappa);

    x1 = -8.0019;
    y1 = 34.614;
    z1 = 350.;
    mx1 = -20.513*fabs(kappa);
    my1 = 5.4042*fabs(kappa);

    old_site.set_z(z0);
    new_site.set_z(z1);
    new_site.set_id(15);

    a.ResizeTo(5, 1);
    a(0, 0) = x0;
    a(1, 0) = mx0;
    a(2, 0) = y0;
    a(3, 0) = my0;
    a(4, 0) = kappa;
    old_site.set_a(a, MAUS::KalmanState::Filtered);

    TMatrixD C(5, 5);
    C.UnitMatrix();
    old_site.set_covariance_matrix(C, MAUS::KalmanState::Projected);
  }
  virtual void TearDown() {
    // if ( MAUS::Globals::HasInstance() ) {
    //   MAUS::Globals::Instance().Death();
    // }
  }
  MAUS::KalmanState old_site;
  MAUS::KalmanState new_site;
  double z0, z1;
  double x0, y0, mx0, my0, kappa;
  double x1, y1, mx1, my1;
  TMatrixD a;
  static const double _Bz = -0.004;
  static const double err = 1.e-3;
};

TEST_F(KalmanHelicalPropagatorTest, test_propagation) {
  MAUS::KalmanPropagator *propagator = new MAUS::KalmanHelicalPropagator(_Bz);
  propagator->CalculatePredictedState(&old_site, &new_site);
  TMatrixD a_projected(5, 1);
  a_projected = new_site.a(MAUS::KalmanState::Projected);

  EXPECT_NEAR(x1,    a_projected(0, 0), err);
  EXPECT_NEAR(mx1,   a_projected(1, 0), err);
  EXPECT_NEAR(y1,    a_projected(2, 0), err);
  EXPECT_NEAR(my1,   a_projected(3, 0), err);
  EXPECT_NEAR(kappa, a_projected(4, 0), err);

  delete propagator;
}
// Removed for now: needs to call Globals::GetConfigurationCards()
/*
TEST_F(KalmanHelicalPropagatorTest, test_energy_loss) {
  MAUS::KalmanPropagator *propagator = new MAUS::KalmanHelicalPropagator(_Bz);
  propagator->CalculatePredictedState(&old_site, &new_site);

  TMatrixD a_old = old_site.a(MAUS::KalmanState::Projected);
  propagator->SubtractEnergyLoss(&old_site, &new_site);
  TMatrixD a     = new_site.a(MAUS::KalmanState::Projected);
  EXPECT_LT(a(4, 0), a_old(4, 0));
}
*/
TEST_F(KalmanHelicalPropagatorTest, test_covariance_extrapolation) {
  MAUS::KalmanPropagator *propagator = new MAUS::KalmanHelicalPropagator(_Bz);
  propagator->CalculatePredictedState(&old_site, &new_site);

  propagator->CalculateCovariance(&old_site, &new_site);

  // Verify that the covariance GROWS when we extrapolate.
  // Not much we can do with this.
  for ( int j = 0; j < 5; j++ ) {
    for ( int k = 0; k < 5; k++ ) {
      EXPECT_GE(new_site.covariance_matrix(MAUS::KalmanState::Projected)(j, k),
                old_site.covariance_matrix(MAUS::KalmanState::Filtered)(j, k));
    }
  }
  //
  // MCS increases matrix elements.
  //
  propagator->CalculateSystemNoise(&old_site, &new_site);
  propagator->CalculateCovariance(&old_site, &new_site);
  for ( int j = 0; j < 5; j++ ) {
    for ( int k = 0; k < 5; k++ ) {
      if ( new_site.covariance_matrix(MAUS::KalmanState::Projected)(j, k)>0 )
      EXPECT_GE(new_site.covariance_matrix(MAUS::KalmanState::Projected)(j, k),
                old_site.covariance_matrix(MAUS::KalmanState::Filtered)(j, k));
    }
  }
  delete propagator;
}

} // namespace
