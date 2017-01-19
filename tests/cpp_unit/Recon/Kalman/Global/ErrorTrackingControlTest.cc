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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 *
 * Copyright Chris Rogers, 2016
 */

#include "gtest/gtest.h"
#include "gsl/gsl_odeiv.h"

#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Utils/Squeak.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControl.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControlLookup.hh"

namespace MAUS {

TEST(ErrorTrackingControlTest, SetStepSizeG4Test) {
    Squeak::errorLevel err = Squeak::debug;
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField_2.dat";
    MaterialModelAxialLookup::SetZTolerance(0.001);
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));
    std::vector<double> point(44, 0.);
    // this is forwards-going track
    point[4] = 150.;
    point[7] = 100.;
    double step = 50.;
    gsl_odeiv_control* control =
       Kalman::Global::ErrorTrackingControl::gsl_odeiv_control_et_new(1., 100.);
    double tolerance = 1e-9;
    for (double sign = 1.; sign > -1.5; sign -= 2.) {
        Squeak::mout(err) << "Direction " << sign << std::endl;
        // boundaries at 1 m +/- 0.0025 m (transverse +/- 1 m)
        // no where near a boundary; take max value
        step *= sign;
        point[7] *= sign;
        point[3] = 0.;
        int ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
        EXPECT_NEAR(sign*step, 100., tolerance);
        EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_INC);
        // no change, return NIL
        ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
        EXPECT_NEAR(sign*step, 100., tolerance);
        EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_NIL);
        // near a boundary, take distance to boundary (50 mm)
        point[3] = 1000.-2.5-50.;
        ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
        EXPECT_NEAR(sign*step, 50., tolerance);
        EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
        // before a boundary, take min step size
        point[3] = 1000.-2.5-0.1;
        ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
        EXPECT_NEAR(sign*step, 1., tolerance);
        EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
        // after a boundary; g4 takes min step size; axial takes distance to next boundary
        // this is because we didn't manage to persuade g4 to use the direction
        point[3] = 1000.-2.5+0.1;
        ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
        EXPECT_NEAR(sign*step, 1., tolerance);
        EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_NIL);
    }
    gsl_odeiv_control_free(control);
}

TEST(ErrorTrackingControlTest, SetStepSizeLookupTest) {
    Squeak::errorLevel err = Squeak::debug;
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField_2.dat";
    MaterialModelAxialLookup::SetZTolerance(0.001);
    GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));
    std::vector<double> point(44, 0.);
    // this is forwards-going track
    point[4] = 150.;
    point[7] = 100.;
    double step = 50.;
    gsl_odeiv_control* control = Kalman::Global::ErrorTrackingControlLookup::
                                     gsl_odeiv_control_lookup_et_new(1., 100.);
    double tolerance = MaterialModelAxialLookup::GetZTolerance();
    MaterialModelAxialLookup::PrintLookupTable(Squeak::mout(err));
    // boundaries at 1 m +/- 0.0025 m (transverse +/- 1 m)
    // no where near a boundary; take max value
    point[3] = 0.;
    int ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 100., tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_INC);
    // no change, return NIL
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 100., tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_NIL);
    // near a boundary, take distance to boundary (50 mm)
    point[3] = 1000.-2.5-50.;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 50., tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
    // before a boundary, take min step size
    point[3] = 1000.-2.5-0.1;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 1., tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
    // after a boundary; g4 takes min step size; axial takes distance to next boundary
    // this is because we didn't manage to persuade g4 to use the direction
    point[3] = 1000.-2.5+0.1;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 4.9, tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_INC);

    // DIRECTION CHANGE - now swimming upstream
    step *= -1;
    point[7] = -100.;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, -1., tolerance); // just downstream of boundary; take min step
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
    point[3] = 1000.-2.5+4.9;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, -4.9, tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_INC);
    point[3] = 1000.-2.5-0.1;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, -100., tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_INC); // just upstream of boundary; take max step
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, -100., tolerance);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_NIL); // max step again (check ret val)


    gsl_odeiv_control_free(control);
}
}
