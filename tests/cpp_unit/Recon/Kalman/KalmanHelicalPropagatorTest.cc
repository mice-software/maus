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
    double kappa = -1./200.;
    double x0 = 0.;
    double y0 = 5.;
    double z0 = 0.;
    double mx0 = 15.*fabs(kappa);
    double my0 = 15.*fabs(kappa);

    double x1 = 29.614;
    double y1 = -3.0019;
    double z1 = 350.;
    double mx1 = 0.027021;
    double my1 = -0.10257;

    old_site.set_z(z0);
    new_site.set_z(z1);
    new_site.set_id(15);

    TMatrixD a(5, 1);
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
  // TMatrixD a;
  static const double _Bz = -0.004;
  static const double _tracker0_Bz = -0.0039815;
  static const double _tracker1_Bz = -0.00400066;
  static const double err = 1.e-3;
};

TEST_F(KalmanHelicalPropagatorTest, test_propagation_using_MC_tracker1) {
  MAUS::KalmanPropagator *propagator = new MAUS::KalmanHelicalPropagator(_tracker1_Bz);
  MAUS::KalmanState state_0;
  MAUS::KalmanState state_15;

  state_0.Initialise(5);
  state_15.Initialise(5);

  double x0  = 19.4509;
  double y0  = 6.86145;
  double z0  = 16021.6;
  double px0 = 5.04953;
  double py0 = -50.4535;
  double pz0 = 212.641;
  double mx0 = px0/pz0;
  double my0 = py0/pz0;
  double charge = 1.;
  double kappa  = charge/pz0;

  double x15  = 98.65861;
  double y15  = -9.12456;
  double z15  = 16472.6;
  double px15 = 24.22770;
  double py15 = 44.5266;
  double pz15 = 212.686;
  double mx15 = px15/pz15;
  double my15 = py15/pz15;

  state_0.set_z(z0);
  state_15.set_z(z15);
  state_15.set_id(15);

  TMatrixD a(5, 1);
  a(0, 0) = x0;
  a(1, 0) = mx0;
  a(2, 0) = y0;
  a(3, 0) = my0;
  a(4, 0) = kappa;
  state_0.set_a(a, MAUS::KalmanState::Filtered);

  propagator->CalculatePredictedState(&state_0, &state_15);
  TMatrixD a_projected(5, 1);
  a_projected = state_15.a(MAUS::KalmanState::Projected);

  EXPECT_NEAR(x15,    a_projected(0, 0), err);
  EXPECT_NEAR(mx15,   a_projected(1, 0), err);
  EXPECT_NEAR(y15,    a_projected(2, 0), err);
  EXPECT_NEAR(my15,   a_projected(3, 0), err);
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
