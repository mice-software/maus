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
    gsl_odeiv_control* control = MAUS::Kalman::Global::ErrorTrackingControl::gsl_odeiv_control_et_new(10., 100.);
    control->type->hadjust(control->state, 0, 0, &point[0], NULL, NULL, &step);
    COMPILES AND RUNS; NOW ADD TESTS(!)
}
