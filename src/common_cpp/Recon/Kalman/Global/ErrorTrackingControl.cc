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
 * Copyright Chris Rogers, 2016-2017
 */

#include <iostream>

#include "gsl/gsl_errno.h"

#include "src/common_cpp/Utils/Squeak.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControl.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControl {

// control state struct
typedef struct {
    double _max_step_size;
    double _min_step_size;
    GeometryNavigator* _navigator;
} et_control_state_t;

// Allocate memory
static void* et_control_alloc() {
    et_control_state_t* state =
        static_cast<et_control_state_t *>(malloc(sizeof(et_control_state_t)));
    if (state == NULL) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "Failed to allocate memory for et_control",
                              "ErrorTrackingControlType::et_control_alloc");
    }
    return state;
}

// Initialise min/max step size; the geometry navigator comes from
// Globals::GetMCGeometryNavigator
static int et_control_init(void* vstate, double min_step_size,
                           double max_step_size, double dummy2, double dummy3) {
    if (max_step_size < 0) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "et_control must have positive step size",
                              "ErrorTrackingControlType::et_control_init");
    }
    et_control_state_t* state = static_cast<et_control_state_t *>(vstate);
    state->_max_step_size = max_step_size;
    state->_min_step_size = min_step_size;
    state->_navigator = Globals::GetMCGeometryNavigator();
    return GSL_SUCCESS;
}

// Actually choose a step size
static int et_control_hadjust(void * vstate, size_t dim, unsigned int ord,
                              const double y[], const double yerr[],
                              const double yp[], double * h) {
    et_control_state_t* state = static_cast<et_control_state_t *>(vstate);


    ThreeVector pos(y[1], y[2], y[3]);
    ThreeVector dir(y[5], y[6], y[7]);
    dir /= dir.mag();

    double sign = *h/fabs(*h);
    if (sign != sign)
        sign = 1.;
    double step = state->_navigator->ComputeStep(pos, dir, state->_max_step_size);
    if (step > state->_max_step_size) {
        step = state->_max_step_size;
    } else if (step < state->_min_step_size) {
        step = state->_min_step_size;
    }

    int return_value = GSL_ODEIV_HADJ_NIL;
    if (state->_max_step_size > *h) {
        return_value = GSL_ODEIV_HADJ_INC;
    } else if (state->_max_step_size < *h) {
        return_value = GSL_ODEIV_HADJ_DEC;
    }

    step = sign*fabs(step);
    *h = step;

    return return_value;
}

// Free memory
static void et_control_free(void * vstate) {
    et_control_state_t *state = reinterpret_cast<et_control_state_t *>(vstate);
    free(state);
}

// In C, we define vtable literally
static const gsl_odeiv_control_type et_control_type = {
    "maus_recon_kalman_global_error_tracking", /* name */
    &et_control_alloc,
    &et_control_init,
    &et_control_hadjust,
    &et_control_free
};
const gsl_odeiv_control_type *gsl_odeiv_control_et = &et_control_type;

// Allocate memory and assign data
gsl_odeiv_control* gsl_odeiv_control_et_new(double min_step_size,
                                            double max_step_size) {
  gsl_odeiv_control* control =  gsl_odeiv_control_alloc(gsl_odeiv_control_et);
    int status = gsl_odeiv_control_init(control, min_step_size, max_step_size, 0., 0.);
    if (status != GSL_SUCCESS) {
        gsl_odeiv_control_free(control);
        throw Exceptions::Exception(Exceptions::recoverable,
                             "Failed to initialise error tracking control",
                             "ErrorTrackingControlType::gsl_odeiv_control_et_new");
    }
    return control;
}

}  // namespace ErrorTrackingControl
}  // namespace Global
}  // namespace Kalman
}  // namespace MAUS

