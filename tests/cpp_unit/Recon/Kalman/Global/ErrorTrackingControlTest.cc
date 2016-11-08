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
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControl.hh"

TEST(ErrorTrackingControlTest, SetStepSizeTest) {
    std::string mod = getenv("MAUS_ROOT_DIR");
    mod += "/tests/cpp_unit/Recon/Global/TestGeometries/PropagationTest_NoField.dat";
    MAUS::GlobalsManager::SetMonteCarloMiceModules(new MiceModule(mod));
    std::vector<double> point(44., 0.);
    double step = 50.;
    gsl_odeiv_control* control = MAUS::Kalman::Global::ErrorTrackingControl::gsl_odeiv_control_et_new(1., 100.);
    // boundaries at 1 m +/- 0.0025 m (transverse +/- 1 m)
    // no where near a boundary; take max value
    int ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 100., 1e-9);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_INC);
    // no change, return NIL
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 100., 1e-9);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_NIL);
    // near a boundary, take distance to boundary (50 mm)
    point[3] = 1000.-2.5-50.;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 50., 1e-9);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
    // before a boundary, take min step size
    point[3] = 1000.-2.5-0.1;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 1., 1e-9);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_DEC);
    // after a boundary, take min step size
    point[3] = 1000.-2.5+0.1;
    ret_val = control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    EXPECT_NEAR(step, 1., 1e-9);
    EXPECT_EQ(ret_val, GSL_ODEIV_HADJ_NIL);
    gsl_odeiv_control_free(control);
}
