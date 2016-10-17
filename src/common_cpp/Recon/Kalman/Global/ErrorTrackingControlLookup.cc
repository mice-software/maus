#include <iostream>

#include "gsl/gsl_errno.h"

#include "src/common_cpp/Utils/Squeak.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControlLookup.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControlLookup {

typedef struct {
    double _min_step_size;
    double _max_step_size;
} et_control_lookup_state_t;

static void* et_control_lookup_alloc() {
    et_control_lookup_state_t* state =
        static_cast<et_control_lookup_state_t *> (malloc (sizeof(et_control_lookup_state_t)));
    if (state == NULL) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "Failed to allocate memory for et_control",
                              "ErrorTrackingControlType::et_control_lookup_alloc");
    }
    return state;
}

static int et_control_lookup_init(void* vstate, double min_step_size, 
                           double max_step_size, double dummy2, double dummy3) {
    if (min_step_size < 0) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "global track fit must have positive step size",
                              "ErrorTrackingControlType::et_control_lookup_init");
    }
    if (min_step_size > max_step_size) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "global track fit must have min step size more than max step size",
                              "ErrorTrackingControlType::et_control_lookup_init");
    }
    et_control_lookup_state_t* state =
                               static_cast<et_control_lookup_state_t *>(vstate);
    state->_min_step_size = min_step_size;
    state->_max_step_size = max_step_size;
    return GSL_SUCCESS;
}

int et_control_lookup_hadjust(void * vstate, size_t dim, unsigned int ord,
                              const double y[], const double yerr[],
                              const double yp[], double * h) {
    et_control_lookup_state_t* state =
                                static_cast<et_control_lookup_state_t*>(vstate);
    if (!MaterialModelAxialLookup::IsReady()) {
        std::cerr << "Control geometry lookup was not ready in " <<
                      "Recon/Kalman/Global/ErrorTrackingControlAxialLookup.hh" << std::endl;
        return GSL_FAILURE;
    }

    int return_value = GSL_ODEIV_HADJ_NIL;
    double lower_bound = 0;
    double upper_bound = 0;
    double step_size = state->_max_step_size;

    double sign = *h/fabs(*h);
    if (sign != sign)
        sign = 1.;

    MaterialModelAxialLookup::GetBounds(y[3], lower_bound, upper_bound);
    double delta = upper_bound - y[3];
    if (sign < 0) {
        delta = y[3] - lower_bound;
    }
    if (delta < step_size && delta > state->_min_step_size) {
        step_size = delta - state->_min_step_size/2.;
        return_value = GSL_ODEIV_HADJ_DEC;
    } else if (delta < state->_max_step_size) {
        step_size = state->_min_step_size;
        return_value = GSL_ODEIV_HADJ_DEC;
    } else if (step_size > *h) { // e.g. we just stepped away from boundary
        return_value = GSL_ODEIV_HADJ_INC;
    }
    *h = fabs(step_size)*sign;

    return return_value;
}

static void et_control_lookup_free (void * vstate) {
   et_control_lookup_state_t *state = (et_control_lookup_state_t *) vstate;
   free(state);
}

static const gsl_odeiv_control_type et_control_lookup_type = {
   "maus_recon_kalman_global_error_tracking", /* name */
   &et_control_lookup_alloc,
   &et_control_lookup_init,
   &et_control_lookup_hadjust,
   &et_control_lookup_free
};
const gsl_odeiv_control_type *gsl_odeiv_control_lookup_et = &et_control_lookup_type;

gsl_odeiv_control* gsl_odeiv_control_lookup_et_new(double min_step_size, double max_step_size) {
    gsl_odeiv_control* control =  gsl_odeiv_control_alloc(gsl_odeiv_control_lookup_et);
    int status = et_control_lookup_init(control->state, min_step_size, max_step_size, 0., 0.);
    if (status != GSL_SUCCESS) {
        gsl_odeiv_control_free (control);
        throw Exceptions::Exception(Exceptions::recoverable,
                             "Failed to initialise error tracking control",
                             "ErrorTrackingControlType::gsl_odeiv_control_lookup_et_new");
    }
    return control;
}

}  // namespace ErrorTrackingControlLookup
}  // namespace Global
}  // namespace Kalman
}  // namespace MAUS

