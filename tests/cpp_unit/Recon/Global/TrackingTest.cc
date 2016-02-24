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
#include "src/legacy/Interface/Squeak.hh"
#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Maths/Matrix.hh"
#include "src/common_cpp/FieldTools/DerivativesSolenoid.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Global/Tracking.hh"

namespace MAUS {
Squeak::errorLevel verbose = Squeak::fatal;

TEST(TrackingZTest, GetSetTest) {
    TrackingZ tz;
    tz.SetDeviations(1., 2., 3., 4.);
    std::vector<double> dev = tz.GetDeviations();
    ASSERT_EQ(dev.size(), 4);
    for (size_t i = 0; i < dev.size(); ++i) {
        EXPECT_NEAR(dev[i], i+1, 1e-12);
    }

    ::CLHEP::Hep3Vector bvec(1., 1., 1.); 
    BTConstantField test_field(1000., 1000., 1000., bvec);
    tz.SetField(&test_field);
    ASSERT_EQ(tz.GetField(), &test_field);

    EXPECT_TRUE(false) << "Missing some accessors/mutators";
}

TEST(TrackingZTest, FieldDerivativeTest) {
    double b0 = 2;
    BTMultipole::TanhEndField* end = new BTMultipole::TanhEndField(10., 5., 9);
    DerivativesSolenoid sol(b0, 10., 20., 9, end);
    TrackingZ tz;
    tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
    tz.SetField(&sol);

    for (double z = 0.; z < 20.; ++z) {
        double point[4] = {0., 0., z, 0.};
        double deriv[6];
        tz.FieldDerivative(point, deriv);
        EXPECT_NEAR(deriv[0], deriv[4], 1e-9);
        EXPECT_NEAR(0., deriv[1], 1e-9);
        EXPECT_NEAR(0., deriv[2], 1e-9);
        EXPECT_NEAR(0., deriv[3], 1e-9);
        EXPECT_NEAR(0., deriv[5], 1e-9);
        EXPECT_NEAR(deriv[0], -end->Function(z, 1)*b0/2., 1e-5);
    }
}

std::vector< std::vector<double> > get_tm_numerical(TrackingZ& tz, double* x_in, double delta, double step) {
    std::vector< std::vector<double> > tm_numerical;
    for (size_t i = 0; i < 7; ++i) { // t, x, y, (z), E, px, py
        if (i == 3) // z is not varied
            ++i;
        double x_pos[] = {x_in[0], x_in[1], x_in[2], x_in[3],
                      x_in[4], x_in[5], x_in[6], x_in[7]};
        x_pos[i] += delta;
        x_pos[7] = ::sqrt(x_pos[4]*x_pos[4]-x_pos[5]*x_pos[5]-x_pos[6]*x_pos[6]-105.658*105.658);
        BTTracker::integrate(x_pos[3]+step, x_pos, tz.GetField(), BTTracker::z, step, 1.);

        double x_neg[] = {x_in[0], x_in[1], x_in[2], x_in[3],
                      x_in[4], x_in[5], x_in[6], x_in[7]};
        x_neg[i] -= delta;
        x_neg[7] = ::sqrt(x_neg[4]*x_neg[4]-x_neg[5]*x_neg[5]-x_neg[6]*x_neg[6]-105.658*105.658);
        BTTracker::integrate(x_neg[3]+step, x_neg, tz.GetField(), BTTracker::z, step, 1.);

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

void tm_tracking_check(TrackingZ& tz, double* x_in, double delta, double step) {
    std::vector< std::vector<double> > tm_numerical_fine = get_tm_numerical(tz, x_in, delta, step);
    std::vector< std::vector<double> > tm_numerical_coarse_1 = get_tm_numerical(tz, x_in, delta, step*10.);
    std::vector< std::vector<double> > tm_numerical_coarse_2 = get_tm_numerical(tz, x_in, delta*10, step);
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
    tz.UpdateTransferMatrix(x_in);
    std::vector< std::vector<double> > tm_analytical = tz.GetMatrix();
    Squeak::mout(verbose) << "Analytical" << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_analytical[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << "Numerical step " << step << " delta " << delta << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_numerical[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << "Numerical step*10 " << step*10. << " delta " << delta << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_numerical_coarse_1[j][k] << " ";
        }
        Squeak::mout(verbose) << std::endl;
    }
    Squeak::mout(verbose) << "Numerical step " << step << " delta*10 " << delta*10. << std::endl;
    for (size_t j = 0; j < 6; ++j) {
        for (size_t k = 0; k < 6; ++k) {
            Squeak::mout(verbose) << std::right << std::setw(13) << tm_numerical_coarse_2[j][k] << " ";
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
            // Squeak::mout(verbose) << i << " " << j << " ** " << k << " " << l << " " << matrix(i, j) << std::endl;
            if (i == j)
                matrix(i, j) += 1.;
        }
    }
    Squeak::mout(verbose) << "Matrix\n" << matrix;
    double transverse_determinant = determinant(matrix);
    Squeak::mout(verbose) << "Determinant: " << transverse_determinant << "\n" << std::endl;
}

TEST(TrackingZTest, TransferMatrixConstFieldTest) {
    ::CLHEP::Hep3Vector brand(0., 0., 3.); 
    BTConstantField field(1000., 1000., 1000., brand);
    TrackingZ tz;
    tz.SetField(&field);
    double x_in[8] = {0., 0., 0., 0., ::sqrt(200.*200.+105.658*105.658), 0., 0., 200.};
    for (double deviation = 1e-4; deviation < 1.1e-3; deviation *= 10.) {
        tz.SetDeviations(deviation, deviation, deviation, deviation);
        // t, x, y, z, E, px, py, pz
        tm_tracking_check(tz, x_in, 0.1, 1.);
    }
}

// NOTES
// If px, py = 0, we get good agreement with the numerical transfer matrix
// If px, py =/= 0, we get bad agreement
void mass_shell_condition(double x_in[], double mass) {
    double energy = ::sqrt(x_in[5]*x_in[5]+x_in[6]*x_in[6]+x_in[7]*x_in[7]+mass*mass);
    x_in[4] = energy;
}

TEST(TrackingZTest, TransferMatrixSolFieldTest) {
    for (double b0 = 0.001; b0 < 0.0011; b0 *= 10) { // 1 T
        BTMultipole::TanhEndField* end = new BTMultipole::TanhEndField(10., 5., 9);
        DerivativesSolenoid sol(b0, 10., 20., 9, end);
        for (double delta = 0.01; delta < 0.015; delta *= 10.)
            for (double step = 0.01; step < 0.015; step *= 10.) {
                Squeak::mout(verbose) << "delta " << delta 
                                      << " step " << step
                                      << " b0 " << b0 << std::endl;
                TrackingZ tz;
                tz.SetDeviations(delta, delta, delta, delta);
                tz.SetField(&sol);
                // t, x, y, z, E, px, py, pz
                double x_in_m1[8] = {0., 0., 0., 5., 0., 0., 0., 200.};
                mass_shell_condition(x_in_m1, 105.658);
                tm_tracking_check(tz, x_in_m1, delta, step);
                double x_in_0[8] = {0., 1., 2., 5., 0., 0., 0., 200.};
                mass_shell_condition(x_in_0, 105.658);
                tm_tracking_check(tz, x_in_0, delta, step);
                double x_in_1[8] = {0., 1., 2., 5., 0., 5., 6., 200.};
                mass_shell_condition(x_in_1, 105.658);
                tm_tracking_check(tz, x_in_1, delta, step);
                double x_in_2[8] = {0., 1., 2., 5., 0., 50., 60., 200.};
                mass_shell_condition(x_in_2, 105.658);
                tm_tracking_check(tz, x_in_2, delta, step);
                double x_in_3[8] = {0., 1., 2., 5., 0., 200., 60., 200.};
                mass_shell_condition(x_in_3, 105.658);
                tm_tracking_check(tz, x_in_3, delta, step);
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
TEST(TrackingZTest, PropagateEllipseDriftTest) {
    // field
    ::CLHEP::Hep3Vector brand(0., 0., 0.); 
    BTConstantField field(1000., 1000., 1000., brand);

    // trackingZ
    TrackingZ tz;
    tz.SetStepSize(100.);
    tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
    tz.SetField(&field);
    tz.SetEnergyLossModel(TrackingZ::no_eloss);
    tz.SetMCSModel(TrackingZ::no_mcs);

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
        tz.Propagate(&x_out[0], dz);
    } catch (...) {
        ASSERT_TRUE(false) << "Caught an exception";
    }

    print_x(&x_out[0]);
    EXPECT_NEAR(x_out[0], x_in[4]/x_in[7]/299.792458*dz, 1e-3);
    EXPECT_NEAR(x_out[3], dz, 1e-3);
    for (size_t i = 0; i < 8; ++i) {
        if (i == 0 || i == 3)
            continue;
        EXPECT_NEAR(x_in[i], x_out[i], 1e-3);
    }
    
    double de = -dz/betagamma/betagamma/c_l/pz;
    for (size_t i = 8; i < 29; ++i) {
        if (i == 8) {
            EXPECT_NEAR(x_out[i]/(x_in[i]+x_in[23]*de*de), 1., 1e-9); // stt
        } else if (i == 11) {
            EXPECT_NEAR(x_out[i]/(x_in[23]*de), 1., 1e-9); // ste
        } else if (i == 14) {
            EXPECT_NEAR(x_out[i]/(x_in[i]+x_in[26]*dz*dz/pz/pz), 1., 1e-9); // sxx
        } else if (i == 17) {
            EXPECT_NEAR(x_out[i]/(x_in[26]*dz/pz), 1., 1e-9); // sxpx
        } else if (i == 19) {
            EXPECT_NEAR(x_out[i]/(x_in[i]+x_in[28]*dz*dz/pz/pz), 1., 1e-9); // syy
        } else if (i == 22) {
            EXPECT_NEAR(x_out[i]/(x_in[28]*dz/pz), 1., 1e-9); // sypy
        } else {
            EXPECT_NEAR(x_in[i], x_out[i], 1e-9);
        }
    }

    std::vector< std::vector<double> > tm_analytical = tz.GetMatrix();
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
TEST(TrackingZTest, PropagateEllipseConstFieldTest) {
    // field
    ::CLHEP::Hep3Vector brand(0., 0., 1e-3*gRandom->Uniform()); // 0-1 T 
    BTConstantField field(1000., 1000., 1000., brand);

    // trackingZ
    TrackingZ tz;
    tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
    tz.SetField(&field);
    tz.SetEnergyLossModel(TrackingZ::no_eloss);
    tz.SetMCSModel(TrackingZ::no_mcs);
    tz.SetEStragModel(TrackingZ::no_estrag);

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
        tz.Propagate(&x_out[0], 100.);
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
        if (i==0 || i == 3)
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

TEST(TrackingZTest, PropagateDriftELossTest) {
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule("Test.dat"));
    TrackingZ tz;
    tz.SetStepSize(10.);
    tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
    tz.SetEnergyLossModel(TrackingZ::bethebloch_forwards);
    tz.SetMCSModel(TrackingZ::no_mcs);
    tz.SetEStragModel(TrackingZ::no_estrag);
    std::vector<double> x_in = drift_ellipse(200., 105.658);
    x_in[3] = -200.;
    std::vector<double> x_out = x_in;

    Squeak::mout(verbose) << "BB Forwards" << std::endl;
    print_x(&x_out[0]);
    tz.Propagate(&x_out[0], 200.);
    print_x(&x_out[0]); 

    x_out = x_in;
    Squeak::mout(verbose) << "BB Backwards" << std::endl;
    print_x(&x_out[0]);
    tz.SetEnergyLossModel(TrackingZ::bethebloch_backwards);
    tz.Propagate(&x_out[0], 200.);
    print_x(&x_out[0]); 

    x_out = x_in;
    Squeak::mout(verbose) << "No BB" << std::endl;
    print_x(&x_out[0]);
    tz.SetEnergyLossModel(TrackingZ::no_eloss);
    tz.Propagate(&x_out[0], 200.);
    print_x(&x_out[0]); 

    EXPECT_TRUE(false);
}

TEST(TrackingZTest, PropagateDriftMCSTest) {
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule("Test.dat"));
    TrackingZ tz;
    tz.SetStepSize(10.);
    tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
    tz.SetEnergyLossModel(TrackingZ::no_eloss);
    tz.SetMCSModel(TrackingZ::moliere_forwards);
    tz.SetEStragModel(TrackingZ::no_estrag);
    std::vector<double> x_in = drift_ellipse(200., 105.658);
    std::vector<double> x_out = x_in;
    x_out[3] = -200.;
    print_x(&x_out[0]);
    tz.Propagate(&x_out[0], 200.);
    print_x(&x_out[0]); 

    EXPECT_TRUE(false);
}



}
