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

#include <stdlib.h>
#include <iomanip>

#include "TRandom.h"
#include "gtest/gtest.h"

#include "Geant4/G4Navigator.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4NistManager.hh"

#include "src/legacy/BeamTools/BTMultipole.hh"
#include "src/legacy/BeamTools/BTConstantField.hh"
#include "src/legacy/BeamTools/BTTracker.hh"
#include "src/common_cpp/Utils/Squeak.hh"
#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Maths/Matrix.hh"
#include "src/common_cpp/FieldTools/DerivativesSolenoid.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
Squeak::errorLevel verbose = Squeak::debug;

// Overall comment
//
// I think I have tested the error propagation pretty much to death. It is
// pretty good, but... there is something weird in some of the tests, some of
// the errors are too large. For example, some of the drift propagation (where
// derivatives are constant) result in non-zero errors. Really the errors here
// should be zero.
//
// My current theory is
// that GSL integration cocks up the first or last step - or something. I need
// to do some digging to really understand what is going on.
//

TEST(ErrorTrackingTest, GetSetDeviationsTest) {
    ErrorTracking propagator;
    propagator.SetDeviations(1., 2., 3., 4.);
    std::vector<double> dev = propagator.GetDeviations();
    ASSERT_EQ(dev.size(), 4);
    for (size_t i = 0; i < dev.size(); ++i) {
        EXPECT_NEAR(dev[i], i+1, 1e-12);
    }
}

TEST(ErrorTrackingTest, GetSetFieldTest) {
    ErrorTracking propagator;
    ::CLHEP::Hep3Vector bvec(1., 1., 1.);
    BTConstantField test_field(1000., 1000., 1000., bvec);
    propagator.SetField(&test_field);
    ASSERT_EQ(propagator.GetField(), &test_field);
}

TEST(ErrorTrackingTest, GetSetStepSizesTest) {
    ErrorTracking propagator;
    propagator.SetMaxStepSize(99.);
    EXPECT_NEAR(propagator.GetMaxStepSize(), 99., 1e-12);
    propagator.SetMinStepSize(9.);
    EXPECT_NEAR(propagator.GetMinStepSize(), 9., 1e-12);
    try {
        propagator.SetMaxStepSize(8.); // max < min
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
    try {
        propagator.SetMinStepSize(100.); // min > max
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
}

TEST(ErrorTrackingTest, GetSetELossModelTest) {
    ErrorTracking::ELossModel loss_enum[] = {ErrorTracking::bethe_bloch_forwards,
                                             ErrorTracking::bethe_bloch_backwards,
                                             ErrorTracking::no_eloss};
    std::string loss_str[] =
                  {"bethe_bloch_forwards", "bethe_bloch_backwards", "no_eloss"};
    ErrorTracking propagator;
    for (size_t i = 0; i < 3; ++i) {
        propagator.SetEnergyLossModel(loss_str[i]);
        EXPECT_EQ(propagator.GetEnergyLossModel(), loss_enum[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
        propagator.SetEnergyLossModel(loss_enum[i]);
        EXPECT_EQ(propagator.GetEnergyLossModel(), loss_enum[i]);
    }
    try {
        propagator.SetEnergyLossModel("error no such model");
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
}

TEST(ErrorTrackingTest, GetSetMCSTest) {
    ErrorTracking::MCSModel mcs_enum[] = {ErrorTracking::moliere_forwards,
                                          ErrorTracking::moliere_backwards,
                                          ErrorTracking::no_mcs};
    std::string mcs_str[] = {"moliere_forwards", "moliere_backwards", "no_mcs"};
    ErrorTracking propagator;
    for (size_t i = 0; i < 3; ++i) {
        propagator.SetMCSModel(mcs_str[i]);
        EXPECT_EQ(propagator.GetMCSModel(), mcs_enum[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
        propagator.SetMCSModel(mcs_enum[i]);
        EXPECT_EQ(propagator.GetMCSModel(), mcs_enum[i]);
    }
    try {
        propagator.SetMCSModel("error no such model");
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
}

TEST(ErrorTrackingTest, GetSetEStragTest) {
    ErrorTracking::EStragModel estrag_enum[] = {ErrorTracking::no_estrag,
                                         };
    std::string estrag_str[] = {"no_estrag"};  // "estrag_forwards", "estrag_backwards",
    ErrorTracking propagator;
    for (size_t i = 0; i < 1; ++i) {
        propagator.SetEStragModel(estrag_str[i]);
        EXPECT_EQ(propagator.GetEStragModel(), estrag_enum[i]);
    }
    for (size_t i = 0; i < 1; ++i) {
        propagator.SetEStragModel(estrag_enum[i]);
        EXPECT_EQ(propagator.GetEStragModel(), estrag_enum[i]);
    }
    try {
        propagator.SetEStragModel("estrag_forwards");
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
}

TEST(ErrorTrackingTest, GetSetTrackingTest) {
    ErrorTracking::TrackingModel tracking_enum[] = {
                                          ErrorTracking::em_forwards_dynamic,
                                          ErrorTracking::em_backwards_dynamic,
                                          ErrorTracking::em_forwards_static,
                                          ErrorTracking::em_backwards_static};
    std::string tracking_str[] = {"em_forwards_dynamic", "em_backwards_dynamic",
                             "em_forwards_static", "em_backwards_static"};
    ErrorTracking propagator;
    for (size_t i = 0; i < 4; ++i) {
        propagator.SetTrackingModel(tracking_str[i]);
        EXPECT_EQ(propagator.GetTrackingModel(), tracking_enum[i]);
    }
    for (size_t i = 0; i < 4; ++i) {
        propagator.SetTrackingModel(tracking_enum[i]);
        EXPECT_EQ(propagator.GetTrackingModel(), tracking_enum[i]);
    }
    try {
        propagator.SetTrackingModel("error no such model");
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
}

TEST(ErrorTrackingTest, GetSetGeometryTest) {
    ErrorTracking::GeometryModel geometry_enum[] = {
                                          ErrorTracking::geant4,
                                          ErrorTracking::axial_lookup};
    std::string geometry_str[] = {"geant4", "axial_lookup"};
    ErrorTracking propagator;
    for (size_t i = 0; i < 2; ++i) {
        propagator.SetGeometryModel(geometry_str[i]);
        EXPECT_EQ(propagator.GetGeometryModel(), geometry_enum[i]);
    }
    for (size_t i = 0; i < 2; ++i) {
        propagator.SetGeometryModel(geometry_enum[i]);
        EXPECT_EQ(propagator.GetGeometryModel(), geometry_enum[i]);
    }
    try {
        propagator.SetGeometryModel("error no such model");
        EXPECT_TRUE(false) << "Should have thrown";
    } catch (Exceptions::Exception& exc) {
        // pass
    }
}


TEST(ErrorTrackingTest, FieldDerivativeTest) {
    double b0 = 2;
    BTMultipole::TanhEndField* end = new BTMultipole::TanhEndField(10., 5., 9);
    DerivativesSolenoid sol(b0, 10., 20., 9, end);
    ErrorTracking propagator;
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    propagator.SetField(&sol);

    for (double z = 0.; z < 20.; ++z) {
        double point[4] = {0., 0., z, 0.};
        double deriv[6];
        propagator.FieldDerivative(point, deriv);
        EXPECT_NEAR(deriv[0], deriv[4], 1e-9);
        EXPECT_NEAR(0., deriv[1], 1e-9);
        EXPECT_NEAR(0., deriv[2], 1e-9);
        EXPECT_NEAR(0., deriv[3], 1e-9);
        EXPECT_NEAR(0., deriv[5], 1e-9);
        EXPECT_NEAR(deriv[0], -end->Function(z, 1)*b0/2., 1e-5);
    }
}

void tracking_test(ErrorTracking& propagator, double* x_in, double dz) {
    Squeak::mout(verbose) << "tracking test" << std::endl;
    std::vector<double> test_x(29, 0.);
    std::vector<double> ref_x(8, 0.);
    for (size_t i = 0; i < 8; ++i) {
        ref_x[i] = x_in[i];
        test_x[i] = x_in[i];
    }
    Squeak::mout(verbose) << "Propagating ";
    for (size_t i = 0; i < 8; ++i) {
        Squeak::mout(verbose) << ref_x[i] << " ";
    }
    Squeak::mout(verbose) << " to " << ref_x[3]+dz << std::endl;

    BTTracker::integrate(ref_x[3]+dz, &ref_x[0], propagator.GetField(), BTTracker::z, 1., 1.);
    propagator.Propagate(&test_x[0], test_x[3]+dz);
    Squeak::mout(verbose) << "BTTracker got ";
    for (size_t i = 0; i < 8; ++i) {
        Squeak::mout(verbose) << ref_x[i] << " ";
    }
    Squeak::mout(verbose) << std::endl << "ErrorTracker got ";
    for (size_t i = 0; i < 8; ++i) {
        Squeak::mout(verbose) << test_x[i] << " ";
    }
    Squeak::mout(verbose) << std::endl << "AKA\n";
    for (size_t i = 0; i < 8; ++i) {
        Squeak::mout(verbose) << i << " " << ref_x[i] << " " << test_x[i]
                              << " " << ref_x[i] - test_x[i] << " ** ";
        // EXPECT_NEAR(0., ref_x[i] - test_x[i], 1e-9);
    }
    Squeak::mout(verbose) << "Done" << std::endl;
}

std::vector< std::vector<double> > get_tm_numerical(ErrorTracking& propagator,
                                     double* x_in, double delta, double step) {
    std::vector< std::vector<double> > tm_numerical;
    for (size_t i = 0; i < 7; ++i) { // t, x, y, (z), E, px, py
        if (i == 3) // z is not varied
            ++i;
        double x_pos[] = {x_in[0], x_in[1], x_in[2], x_in[3],
                          x_in[4], x_in[5], x_in[6], x_in[7]};
        x_pos[i] += delta;
        x_pos[7] = ::sqrt(x_pos[4]*x_pos[4]-x_pos[5]*x_pos[5]-x_pos[6]*x_pos[6]-105.658*105.658);
        BTTracker::integrate(x_pos[3]+step, x_pos, propagator.GetField(), BTTracker::z, step, 1.);

        double x_neg[] = {x_in[0], x_in[1], x_in[2], x_in[3],
                          x_in[4], x_in[5], x_in[6], x_in[7]};
        x_neg[i] -= delta;
        x_neg[7] = ::sqrt(x_neg[4]*x_neg[4]-x_neg[5]*x_neg[5]-x_neg[6]*x_neg[6]-105.658*105.658);
        BTTracker::integrate(x_neg[3]+step, x_neg, propagator.GetField(), BTTracker::z, step, 1.);

        std::vector<double> dm_row;
        for (size_t j = 0; j < 7; ++j) {
            if (j == 3)
              ++j;
            // dm_{ij}/dz = \frac{1}{dz}\frac{\partial u_j/\partial u_i}
            dm_row.push_back((x_pos[j]-x_neg[j])/2./delta/step);
            if (i == j)
              dm_row.back() -= 1./step;
        }
        tm_numerical.push_back(dm_row);
    }
    // take the transpose
    std::vector< std::vector<double> > tm_numerical_trans(6, std::vector<double>(6, 0.));
    for (size_t i = 0; i < 6; ++i)
        for (size_t j = 0; j < 6; ++j)
            tm_numerical_trans[i][j] = tm_numerical[j][i];
    return tm_numerical_trans;
}

void tm_tracking_check(ErrorTracking& propagator, double* x_in, double delta, double step) {
    std::vector< std::vector<double> > tm_numerical_fine =
                                get_tm_numerical(propagator, x_in, delta, step);
    std::vector< std::vector<double> > tm_numerical_coarse_1 =
                            get_tm_numerical(propagator, x_in, delta, step*10.);
    std::vector< std::vector<double> > tm_numerical_coarse_2 =
                             get_tm_numerical(propagator, x_in, delta*10, step);
    std::vector< std::vector<double> > tm_numerical = tm_numerical_fine;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            if (fabs(tm_numerical_coarse_1[i][j]) < 1e-9) {
                tm_numerical[i][j] = 0; // approx 0
            } else if (fabs(tm_numerical_fine[i][j]/tm_numerical_coarse_1[i][j]) < 0.2) {
                tm_numerical[i][j] = 0; // convergence with step
            } else if  (fabs(tm_numerical_fine[i][j]/tm_numerical_coarse_2[i][j]) < 0.2) {
                tm_numerical[i][j] = 0; // convergence with delta
            }
        }
    }
    propagator.UpdateTransferMatrix(x_in, 1.);
    std::vector< std::vector<double> > tm_analytical = propagator.GetMatrix();
    Squeak::mout(verbose) << "Analytical" << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_analytical[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << "Numerical step " << step << " delta " << delta << std::endl;
    for (size_t j = 0; j < 8; ++j) {
        Squeak::mout(verbose) << std::right << std::setw(13) << x_in[j] << " ";
    }
    Squeak::mout(verbose) << std::endl << "Matrix" << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_numerical[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << "Numerical step*10 " << step*10. << " delta " << delta << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13)
                                  << tm_numerical_coarse_1[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << "Numerical step " << step << " delta*10 " << delta*10. << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13)
                                  << tm_numerical_coarse_2[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            if (fabs(tm_numerical[j][k]) < 1e-9) {
               // EXPECT_LT(fabs(tm_analytical[j][k]), 1e-9);
            } else {
                // 1 % tolerance between analytical and numerical solution
               // EXPECT_LT(fabs(tm_analytical[j][k]/tm_numerical[j][k] - 1), 1e-2);
            }
        }
    }
    MAUS::Matrix<double> matrix(4, 4, 0.);
    for (size_t i = 1; i < 5; ++i) {
        // x, px, y, py from t, x, y, E, px, py
        size_t mat_index[] = {0, 1, 4, 2, 5};
        for (size_t j = 1; j < 5; ++j) {
            size_t k = mat_index[i];
            size_t l = mat_index[j];
            matrix(i, j) = tm_analytical[k][l]*100.;
            if (i == j)
                matrix(i, j) += 1.;
        }
    }
    Squeak::mout(verbose) << "Matrix\n" << matrix;
    double transverse_determinant = determinant(matrix);
    Squeak::mout(verbose) << "Determinant: " << transverse_determinant << "\n" << std::endl;
}

TEST(ErrorTrackingTest, TransferMatrixConstFieldTest) {
    ::CLHEP::Hep3Vector brand(0., 0., 3.);
    BTConstantField field(1000., 1000., 1000., brand);
    ErrorTracking propagator;
    propagator.SetField(&field);
    double x_in[8] = {0., 0., 0., 0., ::sqrt(200.*200.+105.658*105.658), 0., 0., 200.};
    for (double deviation = 1e-3; deviation < 1.1e-3; deviation *= 10.) {
        propagator.SetDeviations(deviation, deviation, deviation, deviation);
        // t, x, y, z, E, px, py, pz
        tm_tracking_check(propagator, x_in, 0.1, 1.);
    }
}

// NOTES
// If px, py = 0, we get good agreement with the numerical transfer matrix
// If px, py =/= 0, we get bad agreement
void mass_shell_condition(double x_in[], double mass) {
    double energy = ::sqrt(x_in[5]*x_in[5]+x_in[6]*x_in[6]+x_in[7]*x_in[7]+mass*mass);
    x_in[4] = energy;
}

TEST(ErrorTrackingTest, TransferMatrixSolFieldTest) {
    for (double b0 = 0.001; b0 < 0.0011; b0 *= 10) { // 1 T
        BTMultipole::TanhEndField* end = new BTMultipole::TanhEndField(10., 5., 9);
        DerivativesSolenoid sol(b0, 10., 20., 9, end);
        for (double delta = 0.01; delta < 0.015; delta *= 10.)
            for (double step = 0.01; step < 0.015; step *= 10.) {
                Squeak::mout(verbose) << "delta " << delta
                                      << " step " << step
                                      << " b0 " << b0 << std::endl;
                ErrorTracking propagator;
                propagator.SetDeviations(delta, delta, delta, delta);
                propagator.SetField(&sol);
// BTTracker got 0.051625 10.3659 6.30369 15 307.837 149.384 156.591 191.74
// ErrorTracker got 0.0554901 12.6051 3.82044 15 307.837 244.952 -21.5023 152.106
// ErrorTracker got 0.0556614 12.6594 3.79158 15 307.837 245.883 -23.2551 150.346

                propagator.SetMinStepSize(0.1);
                propagator.SetMaxStepSize(0.1001);
                propagator.SetTrackingModel(ErrorTracking::em_forwards_static);
                propagator.SetEnergyLossModel(ErrorTracking::no_eloss);
                propagator.SetEStragModel(ErrorTracking::no_estrag);
                propagator.SetMCSModel(ErrorTracking::no_mcs);
                // t, x, y, z, E, px, py, pz
                double x_in_m1[8] = {0., 0., 0., 5., 0., 0., 0., 200.};
                mass_shell_condition(x_in_m1, 105.658);
                tm_tracking_check(propagator, x_in_m1, delta, step);
                double x_in_0[8] = {0., 1., 2., 5., 0., 0., 0., 200.};
                mass_shell_condition(x_in_0, 105.658);
                tm_tracking_check(propagator, x_in_0, delta, step);
                double x_in_1[8] = {0., 1., 2., 5., 0., 5., 6., 200.};
                mass_shell_condition(x_in_1, 105.658);
                tm_tracking_check(propagator, x_in_1, delta, step);
                double x_in_2[8] = {0., 1., 2., 5., 0., 50., 60., 200.};
                mass_shell_condition(x_in_2, 105.658);
                tm_tracking_check(propagator, x_in_2, delta, step);
                double x_in_3[8] = {0., 1., 2., 5., 0., 200., 60., 200.};
                mass_shell_condition(x_in_3, 105.658);
                tm_tracking_check(propagator, x_in_3, delta, step);
                tracking_test(propagator, x_in_3, 0.1);
        }
    }
}


void print_x(double * x_in) {
    Squeak::mout(verbose) << "x: ";
    for (size_t i = 0; i < 4; ++i)
        Squeak::mout(verbose) << std::right << std::setw(13) << x_in[i] << " ";
    Squeak::mout(verbose) << std::endl << "p: ";
    for (size_t i = 4; i < 8; ++i)
        Squeak::mout(verbose) << std::right << std::setw(13) << x_in[i] << " ";
    Squeak::mout(verbose) << std::endl << "var:\n";
    int k = 8;
    for (int j = 6; j > 0; --j) {
        for (int i = 0; i < 6-j; ++i)
            Squeak::mout(verbose) << std::right << std::setw(13) << " " << " ";
        for (int i = 0; i < j; ++i) {
            Squeak::mout(verbose) << std::right << std::setw(13) << x_in[k] << " ";
            ++k;
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << std::endl;
}

std::vector<double> drift_ellipse(double pz, double mass) {
    // ellipse and psv
    double x_in[29] = {0., 0., 0., 0.,
                      ::sqrt(pz*pz+mass*mass), 0., 0., pz,
                      1., 0.,  0.,  0.,  0.,  0.,
                          2.,  0.,  0.,  0.,  0.,
                               3.,  0.,  0.,  0.,
                                    4.,  0.,  0.,
                                         5.,  0.,
                                              6.,
    };

    std::vector<double> x_out(x_in, x_in+sizeof(x_in)/sizeof(double));
    return x_out;
}

// Propagate beam ellipse through a drift space
TEST(ErrorTrackingTest, PropagateEllipseDriftTest) {
    // field
    ::CLHEP::Hep3Vector brand(0., 0., 0.);
    BTConstantField field(1000., 1000., 1000., brand);

    // ErrorTracking
    ErrorTracking propagator;
    propagator.SetMaxStepSize(1000.1);
    propagator.SetMinStepSize(1000.);
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    propagator.SetField(&field);
    propagator.SetEnergyLossModel(ErrorTracking::no_eloss);
    propagator.SetMCSModel(ErrorTracking::no_mcs);

    double mass = 105.658;
    double pz = 200.;
    double c_l = 299.792458;
    double betagamma = pz/mass;
    double dz = 1000.;

    std::vector<double> x_in = drift_ellipse(pz, mass);
    std::vector<double> x_out = x_in;
    print_x(&x_in[0]);
    // propagate
    try {
        propagator.Propagate(&x_out[0], dz);
    } catch (...) {
        ASSERT_TRUE(false) << "Caught an exception";
    }

    print_x(&x_out[0]);
    EXPECT_NEAR(x_out[0], x_in[4]/x_in[7]/c_l*dz, 1e-3);
    EXPECT_NEAR(x_out[3], dz, 1e-3);
    for (size_t i = 0; i < 8; ++i) {
        if (i == 0 || i == 3)
            continue;
        EXPECT_NEAR(x_in[i], x_out[i], 1e-3);
    }

    double de = -dz/betagamma/betagamma/c_l/pz;
    for (size_t i = 8; i < 29; ++i) {
        if (i == 8) {
            EXPECT_NEAR(x_out[i]/(x_in[i]+x_in[23]*de*de), 1., 1e-6); // stt
        } else if (i == 11) {
            EXPECT_NEAR(x_out[i]/(x_in[23]*de), 1., 1e-6); // ste
        } else if (i == 14) {
            EXPECT_NEAR(x_out[i]/(x_in[i]+x_in[26]*dz*dz/pz/pz), 1., 1e-3)
                << " Error O(1e-3) independent of step size -> review maths "
                << (x_in[i]+x_in[26]*dz*dz/pz/pz) << std::endl; // sxx
        } else if (i == 17) {
            EXPECT_NEAR(x_out[i]/(x_in[26]*dz/pz), 1., 1e-6); // sxpx
        } else if (i == 19) {
            EXPECT_NEAR(x_out[i]/(x_in[i]+x_in[28]*dz*dz/pz/pz), 1., 1e-3); // syy
        } else if (i == 22) {
            EXPECT_NEAR(x_out[i]/(x_in[28]*dz/pz), 1., 1e-6); // sypy
        } else {
            EXPECT_NEAR(x_in[i], x_out[i], 1e-6);
        }
    }

    std::vector< std::vector<double> > tm_analytical = propagator.GetMatrix();
    Squeak::mout(verbose) << "Analytical tE " << de/dz << " xpx " << 1/pz << std::endl;
    Squeak::mout(verbose) << "Integrated" << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_analytical[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
}

// If we set up our beam ellipse right, then propagation through a constant
// field should yield a constant beam ellipse... let's check
TEST(ErrorTrackingTest, PropagateEllipseConstFieldTest) {
    // field
    ::CLHEP::Hep3Vector brand(0., 0., 1e-3*gRandom->Uniform()); // 0-1 T
    BTConstantField field(1000., 1000., 1000., brand);

    // ErrorTracking
    ErrorTracking propagator;
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    propagator.SetField(&field);
    propagator.SetEnergyLossModel(ErrorTracking::no_eloss);
    propagator.SetMCSModel(ErrorTracking::no_mcs);
    propagator.SetEStragModel(ErrorTracking::no_estrag);

    // ellipse and psv
    double norm_trans = 1.+10.*gRandom->Uniform()*105.658;; // 1 - 11 mm emit
    double norm_long = 10.+10.*gRandom->Uniform(); // 10. - 20. mm
    double ltwiddle = gRandom->Uniform();
    double pz = 200.;
    double kappa = brand.z()*0.15/pz*1e3;
    double beta = ::sqrt(1+ltwiddle*ltwiddle)/kappa;
    double gamma = (1+(beta*kappa-ltwiddle)*(beta*kappa-ltwiddle))/beta;
    double stt = norm_long*1.;
    double see = norm_long*10.;
    double sxx = norm_trans*beta/pz;
    double spxpx = norm_trans*gamma*pz;
    double sxpy = norm_trans*(beta*kappa-ltwiddle);
    // t, x, y, z, E, px, py, pz
    double x_in[29] = {0., 0., 0., 0.,
                      ::sqrt(pz*pz+105.658*105.658), 0., 0., pz,
                      stt, 0.,  0.,  0.,    0.,   0.,
                          sxx,  0.,  0.,    0., -sxpy,
                               sxx,  0.,  sxpy,   0.,
                                    see,    0.,   0.,
                                         spxpx,   0.,
                                               spxpx,
    };

    std::vector<double> x_out(x_in, x_in+sizeof(x_in)/sizeof(double));
    // propagate
    try {
        propagator.Propagate(&x_out[0], 100.);
    } catch (...) {
        ASSERT_TRUE(false) << "Caught an exception";
    }
    Squeak::mout(verbose) << "Bz " << brand.z() << " kappa " << kappa
                          << " beta " << beta << " L~ " << ltwiddle << std::endl;
    Squeak::mout(verbose) << "x_in" << std::endl;
    print_x(x_in);

    Squeak::mout(verbose) << "x_out" << std::endl;
    print_x(&x_out[0]);
    EXPECT_NEAR(x_out[0], x_in[4]/x_in[7]/299.792458*100., 1e-3);
    EXPECT_NEAR(x_out[3], 100., 1e-3);
    for (size_t i = 0; i < 8; ++i) {
        if (i == 0 || i == 3)
            continue;
        EXPECT_NEAR(x_in[i], x_out[i], 1e-3);
    }
    size_t index = 8;
    std::vector< std::vector<double> > var_in(6, std::vector<double>(6, 0.));
    std::vector< std::vector<double> > var_out(6, std::vector<double>(6, 0.));
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = i; j < 6; ++j) {
            var_in[i][j] = var_in[j][i] = x_in[index];
            var_out[i][j] = var_out[j][i] = x_out[index];
            if (i == 0 and j == 0) {
                // do nothing, this is stt, tested in drift
            } else if (i == 0 and j == 3) {
                // do nothing, this is stE, tested in drift
            } else if (i != j) {
                EXPECT_NEAR(var_in[i][j]/var_in[i][i], var_out[i][j]/var_out[i][i], 1e-3);
            } else if (i == j) {
                EXPECT_NEAR(var_in[i][i]/var_out[i][i], 1., 1e-3);
            }
            index++;
        }
    }
}

// Propagate ellipse forwards through a solenoid field; then
// propagate backwards; check inversion is okay
TEST(ErrorTrackingTest, PropagateSolFieldBackwardsTest) {
    double pz = 200.;
    double mass = 105.658;
    BTMultipole::TanhEndField* end = new BTMultipole::TanhEndField(10., 5., 9);
    DerivativesSolenoid sol(0.001, 10., 20., 9, end);
    ErrorTracking propagator;
    propagator.SetDeviations(0.1, 0.1, 0.1, 0.1);
    propagator.SetMinStepSize(0.1);
    propagator.SetMaxStepSize(0.10001);
    propagator.SetField(&sol);
    // t, x, y, z, E, px, py, pz
    std::vector<double> x_in = drift_ellipse(pz, mass);
    x_in[1] = 0.1;
    x_in[2] = 0.2;
    x_in[5] = 0.3;
    x_in[6] = 0.4;

    // EM FORWARDS (for if e.g. pz > 0)
    std::vector<double> x_out = x_in;
    propagator.SetTrackingModel(ErrorTracking::em_forwards_dynamic);
    // propagate forwards
    propagator.print(Squeak::mout(verbose), &x_out[0]);
    propagator.Propagate(&x_out[0], 50.);
    propagator.print(Squeak::mout(verbose), &x_out[0]);
    // propagate backwards
    propagator.Propagate(&x_out[0], 0.);
    propagator.print(Squeak::mout(verbose), &x_out[0]);
    // check for equality
    for (size_t i = 0; i < x_out.size(); ++i)
        EXPECT_NEAR(x_in[i], x_out[i], 1e-4);

    // EM BACKWARDS (for if e.g. pz < 0)
    x_in[7] = -x_in[7];
    x_out = x_in;
    // propagate backwards
    propagator.SetTrackingModel(ErrorTracking::em_backwards_dynamic);
    propagator.Propagate(&x_out[0], -50.);
    propagator.print(Squeak::mout(verbose), &x_out[0]);
    // propagate forwards
    propagator.Propagate(&x_out[0], 0.);
    propagator.print(Squeak::mout(verbose), &x_out[0]);
    // check for equality
    for (size_t i = 0; i < x_out.size(); ++i)
        EXPECT_NEAR(x_in[i], x_out[i], 1e-4);
}

TEST(ErrorTrackingTest, PropagateDriftELossTest) {
    // Check that energy loss through block of lead is correct
    // * dE/dz; dp/dz; mass conservation
    // * Var(px); Var(py)
    // * NOT Var(E) but should be added when it is implemented

    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField_2.dat";
    MaterialModel::EnableMaterial("G4_Pb");
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));
    double mass = 105.658;

    ErrorTracking propagator;
    propagator.SetMinStepSize(0.1);
    propagator.SetMaxStepSize(10.);
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    propagator.SetMCSModel(ErrorTracking::no_mcs);
    propagator.SetEStragModel(ErrorTracking::no_estrag);
    std::vector<double> x_in = drift_ellipse(200., mass);

    // Bethe Bloch forwards (energy decreases for h +ve)
    std::vector<double> x_out = x_in;
    Squeak::mout(verbose) << "BB Forwards" << std::endl;
    propagator.SetEnergyLossModel(ErrorTracking::bethe_bloch_forwards);
    print_x(&x_out[0]);
    propagator.Propagate(&x_out[0], 1200.);
    EXPECT_NEAR(x_out[4]*x_out[4],
                x_out[5]*x_out[5]+x_out[6]*x_out[6]+x_out[7]*x_out[7]+mass*mass,
                1);
    EXPECT_NEAR(x_out[7], 191.677, 1);
    EXPECT_NEAR(x_out[26], x_in[26]*(1-2*(1-x_out[7]/x_in[7])), 1);
    EXPECT_NEAR(x_out[28], x_in[28]*(1-2*(1-x_out[7]/x_in[7])), 0.1);
    print_x(&x_out[0]);
    // Bethe Bloch forwards; when tracking back, should return to original state
    propagator.Propagate(&x_out[0], 0.);
    print_x(&x_out[0]);
    EXPECT_NEAR(x_out[4], x_in[4], 1e-3);
    EXPECT_NEAR(x_out[7], x_in[7], 1e-2);
    EXPECT_NEAR(x_out[26], x_in[26], 1e-3);
    EXPECT_NEAR(x_out[28], x_in[28], 1e-3);

    // Bethe Bloch backwards (energy increases for h +ve)
    x_out = x_in;
    Squeak::mout(verbose) << "BB Backwards" << std::endl;
    propagator.SetEnergyLossModel(ErrorTracking::bethe_bloch_backwards);
    print_x(&x_out[0]);
    propagator.Propagate(&x_out[0], 1200.);
    EXPECT_NEAR(x_out[7], 208.323, 1);
    EXPECT_NEAR(x_out[4]*x_out[4],
                x_out[5]*x_out[5]+x_out[6]*x_out[6]+x_out[7]*x_out[7]+mass*mass,
                1);
    EXPECT_NEAR(x_out[26], x_in[26]*(1-2*(1-x_out[7]/x_in[7])), 1);
    EXPECT_NEAR(x_out[28], x_in[28]*(1-2*(1-x_out[7]/x_in[7])), 0.1);
    print_x(&x_out[0]);
    // Bethe Bloch backwards; when tracking back, should return to original state
    propagator.Propagate(&x_out[0], 0.);
    print_x(&x_out[0]);
    EXPECT_NEAR(x_out[4], x_in[4], 1e-3);
    EXPECT_NEAR(x_out[7], x_in[7], 1e-2);
    EXPECT_NEAR(x_out[26], x_in[26], 1e-2);
    EXPECT_NEAR(x_out[28], x_in[28], 1e-3);

    // No Bethe Bloch; should do nothing
    x_out = x_in;
    Squeak::mout(verbose) << "No BB" << std::endl;
    propagator.SetEnergyLossModel(ErrorTracking::no_eloss);
    print_x(&x_out[0]);
    propagator.Propagate(&x_out[0], 1200.);
    print_x(&x_out[0]);
    EXPECT_NEAR(x_out[7], x_in[7], 1e-9);
}

TEST(ErrorTrackingTest, PropagateDriftMCSTest) {
    // Check that MCS var(px); var(py); in absence of dE/dz
    // through block of lead is correct
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField_2.dat";
    MaterialModel::EnableMaterial("G4_Pb");
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));
    ErrorTracking propagator;
    propagator.SetMinStepSize(0.1);
    propagator.SetMaxStepSize(1.);
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    propagator.SetEnergyLossModel(ErrorTracking::no_eloss);
    propagator.SetMCSModel(ErrorTracking::moliere_forwards);
    propagator.SetEStragModel(ErrorTracking::no_estrag);
    std::vector<double> x_in = drift_ellipse(200., 105.658);
    x_in[3] = -200.;
    x_in[26] = 500.;
    x_in[28] = 600.;

    // moliere_forwards propagating forwards should increase Var(px), Var(py)
    std::vector<double> x_out = x_in;
    print_x(&x_out[0]);
    propagator.Propagate(&x_out[0], 1200.);
    print_x(&x_out[0]);
    EXPECT_LT(fabs(x_out[26] - x_in[26] - 217.786), 10);
    EXPECT_LT(fabs(x_out[28] - x_in[28] - 217.786), 10);
    // moliere_forwards propagating backwards should return to original state
    propagator.Propagate(&x_out[0], -200.);
    print_x(&x_out[0]);
    for (size_t i = 0; i < x_out.size(); ++i)
        EXPECT_LT(fabs(x_out[i] - x_in[i]), 1);

    // moliere_backwards propagating forwards should decrease Var(px), Var(py)
    propagator.SetMCSModel(ErrorTracking::moliere_backwards);
    x_out = x_in;
    print_x(&x_out[0]);
    propagator.Propagate(&x_out[0], 1200.);
    print_x(&x_out[0]);
    EXPECT_LT(fabs(x_out[26] - x_in[26] + 217.786), 10);
    EXPECT_LT(fabs(x_out[28] - x_in[28] + 217.786), 10);
    // moliere_backwards propagating backwards should return to original state
    propagator.Propagate(&x_out[0], -200.);
    print_x(&x_out[0]);
    for (size_t i = 0; i < x_out.size(); ++i)
        EXPECT_LT(fabs(x_out[i] - x_in[i]), 1)
           << i << " In: " << x_in[i] << " Out: " << x_out[i]
           << " Diff: " << x_out[i] - x_in[i];
}

TEST(ErrorTrackingTest, PropagateDriftGeometryModelTest) {
    // Check that MCS var(px); var(py); in absence of dE/dz
    // through block of lead is correct
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField_2.dat";
    MaterialModel::EnableMaterial("G4_Pb");
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));
    ErrorTracking propagator;
    propagator.SetMinStepSize(0.1);
    propagator.SetMaxStepSize(10.);
    propagator.SetDeviations(0.001, 0.001, 0.001, 0.001);
    propagator.SetEnergyLossModel(ErrorTracking::bethe_bloch_forwards);
    propagator.SetMCSModel(ErrorTracking::moliere_forwards);
    propagator.SetEStragModel(ErrorTracking::no_estrag);
    propagator.SetGeometryModel(ErrorTracking::geant4);
    propagator.SetTrackingModel(ErrorTracking::em_forwards_dynamic);
    std::vector<double> x_in = drift_ellipse(200., 105.658);
    x_in[3] = -200.;
    x_in[26] = 500.;
    x_in[28] = 600.;

    MaterialModelAxialLookup::PrintLookupTable(Squeak::mout(verbose));

    std::vector<double> x_out_geant4 = x_in;
    print_x(&x_out_geant4[0]);
    propagator.Propagate(&x_out_geant4[0], 1200.);
    print_x(&x_out_geant4[0]);
    propagator.SetGeometryModel(ErrorTracking::axial_lookup);
    std::vector<double> x_out_lookup = x_in;
    propagator.Propagate(&x_out_lookup[0], 1200.);
    print_x(&x_out_lookup[0]);
    propagator.Propagate(&x_out_lookup[0], -200.);
    print_x(&x_out_lookup[0]);
}

} // namespace Global
} // namespace Kalman
} // namespace MAUS


