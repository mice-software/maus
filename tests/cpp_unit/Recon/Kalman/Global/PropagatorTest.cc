/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * Copyright Chris Rogers, 2016
 */

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/Squeak.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/Global/Propagator.hh"

namespace {

TEST(PropagatorTest, TestConstructorDestructor) {
    MAUS::Kalman::Global::Propagator* propagator =
                                        new MAUS::Kalman::Global::Propagator();
    delete propagator;
}

MAUS::Kalman::TrackPoint get_tp(double position, double energy) {
    MAUS::Kalman::TrackPoint tp(6, 0, position);
    TMatrixD vec(6, 1);
    TMatrixD cov(6, 6);
    for (size_t i = 0; i < 6; ++i) {
        vec[i] = i;
        for (size_t j = 0; j < 6; ++j) {
            cov[i][j] = (i+1.)*(j+1.);
        }
        cov[i][i] *= 10.;
    }
    vec[3] = energy;
    tp.SetPredicted(MAUS::Kalman::State(vec, cov));
    return tp;
}

std::vector<double> tp_to_vector(MAUS::Kalman::TrackPoint tp, double mass) {
    std::vector<double> x(29, 0.);
    for (size_t i = 0; i < 3; ++i) {
        x[i] = tp.GetPredicted().GetVector()[i][0];
    }
    x[3] = tp.GetPosition();
    for (size_t i = 3; i < 6; ++i) {
        x[i+1] = tp.GetPredicted().GetVector()[i][0];
    }
    double pz = std::sqrt(x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - mass*mass);
    x[7] = pz;
    int index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = i; j < 6; ++j) {
            x[index] = tp.GetPredicted().GetCovariance()[i][j];
            index++;
        }
    }
    return x;
}

TEST(PropagatorTest, TestPropagate) {
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField.dat";
    MAUS::GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));

    double mass = 105.658;
    double energy = 226.;
    double z = 100.;

    MAUS::Kalman::Global::Propagator propagator;
    MAUS::Kalman::TrackPoint start_tp = get_tp(0., energy);
    MAUS::Kalman::TrackPoint end_tp = get_tp(z, energy);
    propagator.SetMass(mass);


    // I just check that the propagator applies to the Predicted vector
    propagator.Propagate(start_tp, end_tp);
    MAUS::Squeak::mout(MAUS::Squeak::debug) << "Start\n" << start_tp << std::endl;
    MAUS::Squeak::mout(MAUS::Squeak::debug) << "End\n" << end_tp << std::endl;

    std::vector<double> ref_vec = tp_to_vector(start_tp, mass);
    propagator.GetTracking()->Propagate(&ref_vec[0], z);
    std::vector<double> test_vec = tp_to_vector(end_tp, mass);
    for (size_t i = 0; i < 29; ++i) {
        EXPECT_NEAR(ref_vec[i], test_vec[i], 1e-9);
    }

    MAUS::Kalman::TrackPoint end_tp_2 = get_tp(z, energy);
    propagator.Propagate(start_tp, end_tp_2);
    test_vec = tp_to_vector(end_tp_2, mass);
    for (size_t i = 0; i < 29; ++i) {
        EXPECT_NEAR(ref_vec[i], test_vec[i], 1e-9);
    }

    MAUS::Kalman::TrackPoint start_tp_2 = get_tp(z, 100.); // energy < mass
    propagator.Propagate(start_tp, end_tp_2);
}

TEST(PropagatorTest, TestCalculatePropagator) {
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField.dat";
    MAUS::GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));

    double mass = 105.658;
    double energy = 226.;
    double z = 100.;

    MAUS::Kalman::Global::Propagator propagator;
    MAUS::Kalman::TrackPoint start_tp = get_tp(0., energy);
    MAUS::Kalman::TrackPoint end_tp = get_tp(100., energy);
    propagator.SetMass(mass);

    // I just check that the propagator applies to the Predicted vector
    TMatrixD test = propagator.CalculatePropagator(start_tp, end_tp);
    MAUS::Squeak::mout(MAUS::Squeak::debug) << "Start\n" << start_tp << std::endl;
    MAUS::Squeak::mout(MAUS::Squeak::debug) << "End\n" << end_tp << std::endl;
}
}

