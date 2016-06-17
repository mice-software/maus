#include <iostream>

#include "gsl/gsl_errno.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include "src/legacy/Interface/Squeak.hh"

#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControl.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControl {

typedef struct {
    double _max_step_size;
    double _min_step_size;
    GeometryNavigator* _navigator;
} et_control_state_t;

static void* et_control_alloc() {
    et_control_state_t* state =
        static_cast<et_control_state_t *> (malloc (sizeof(et_control_state_t)));
    if (state == NULL) {
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to allocate memory for et_control",
                              "ErrorTrackingControlType::et_control_alloc");
    }
    return state;
}

static int et_control_init(void* vstate, double min_step_size, 
                           double max_step_size, double dummy2, double dummy3) {
    if (max_step_size < 0) {
        throw MAUS::Exception(Exception::recoverable,
                              "et_control must have positive step size",
                              "ErrorTrackingControlType::et_control_init");
    }
    et_control_state_t* state = static_cast<et_control_state_t *>(vstate);
    state->_max_step_size = max_step_size;
    state->_min_step_size = min_step_size;
    state->_navigator = Globals::GetMCGeometryNavigator();
    return GSL_SUCCESS;
}

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
    // std::cerr << "Revise step size from " << *h
    //          << " to " << step
    //          << " navigator " << state->_navigator->ComputeStep(pos, dir, state->_max_step_size)
    //          << " max " << state->_max_step_size << std::endl;
    *h = step;

    Squeak::mout(Squeak::debug) << "    ErrorTrackingControl::hadjust"
              << " Pos " << y[1] << " " << y[2] << " " << y[3]
              << " Mom " << y[5] << " " << y[6] << " " << y[7] << std::endl;
    Squeak::mout(Squeak::debug) << "                                 "
              << " Pos' " << yp[1] << " " << yp[2] << " " << yp[3]
              << " Mom' " << yp[5] << " " << yp[6] << " " << yp[7] << std::endl;
    Squeak::mout(Squeak::debug) << "                                 "
              << " Pos err " << yerr[1] << " " << yerr[2] << " " << yerr[3]
              << " Mom err " << yerr[5] << " " << yerr[6] << " " << yerr[7]
              << std::endl;
    Squeak::mout(Squeak::debug) << "                                 "
                                << " step " << step << std::endl;
    return return_value;
}

static void et_control_free (void * vstate) {
   et_control_state_t *state = (et_control_state_t *) vstate;
   free(state);
}

static const gsl_odeiv_control_type et_control_type = {
   "maus_recon_kalman_global_error_tracking", /* name */
   &et_control_alloc,
   &et_control_init,
   &et_control_hadjust,
   &et_control_free
};
const gsl_odeiv_control_type *gsl_odeiv_control_et = &et_control_type;

gsl_odeiv_control* gsl_odeiv_control_et_new(double min_step_size,
                                            double max_step_size) {
  gsl_odeiv_control* control =  gsl_odeiv_control_alloc(gsl_odeiv_control_et);
    int status = gsl_odeiv_control_init(control, min_step_size, max_step_size, 0., 0.);
    if (status != GSL_SUCCESS) {
        gsl_odeiv_control_free (control);
        throw MAUS::Exception(Exception::recoverable,
                             "Failed to initialise error tracking control",
                             "ErrorTrackingControlType::gsl_odeiv_control_et_new");
    }
    return control;
}

}  // namespace ErrorTrackingControl
}  // namespace Global
}  // namespace Kalman
}  // namespace MAUS

