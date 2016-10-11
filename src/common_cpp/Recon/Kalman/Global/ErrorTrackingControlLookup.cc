#include <iostream>

#include "gsl/gsl_errno.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include "src/legacy/Interface/Squeak.hh"

#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControlLookup.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControlLookup {

typedef struct {
    double _step_size;
    MaterialModelAxialLookup* _lookup;
} et_control_lookup_state_t;

static void* et_control_lookup_alloc() {
    et_control_lookup_state_t* state =
        static_cast<et_control_lookup_state_t *> (malloc (sizeof(et_control_lookup_state_t)));
    if (state == NULL) {
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to allocate memory for et_control",
                              "ErrorTrackingControlType::et_control_lookup_alloc");
    }
    return state;
}

static int et_control_lookup_init(void* vstate, double step_size, 
                           double dummy1, double dummy2, double dummy3) {
    if (step_size < 0) {
        throw MAUS::Exception(Exception::recoverable,
                              "et_control_lookup must have positive step size",
                              "ErrorTrackingControlType::et_control_lookup_init");
    }
    et_control_lookup_state_t* state =
                               static_cast<et_control_lookup_state_t *>(vstate);
    state->_step_size = step_size;
    state->_lookup = NULL;
    return GSL_SUCCESS;
}

static int et_control_lookup_hadjust(void * vstate, size_t dim, unsigned int ord,
                              const double y[], const double yerr[],
                              const double yp[], double * h) {
    et_control_lookup_state_t* state = static_cast<et_control_lookup_state_t *>(vstate);
    if (state->_lookup == NULL) {
        std::cerr << "Control geometry lookup was NULL in " <<
                      "Recon/Kalman/Global/ErrorTrackingControlAxialLookup.hh" << std::endl;
        return GSL_FAILURE;
    }
    double lower_bound = 0;
    double upper_bound = 0;
    state->_lookup->GetBounds(y[3], lower_bound, upper_bound);
    // find the largest step size that divides the volume into an integer number
    // of steps and is less than _step_size
    double n_steps = (upper_bound - lower_bound)/state->_step_size;
    double step_size = (upper_bound - lower_bound)/(floor(n_steps)+1.);
    
    double sign = *h/fabs(*h);
    if (sign != sign)
        sign = 1.;
    step_size = sign*fabs(step_size);

    int return_value = GSL_ODEIV_HADJ_NIL;
    if (fabs(step_size) > fabs(*h)) {
        return_value = GSL_ODEIV_HADJ_INC;
    } else if (fabs(step_size) < fabs(*h)) {
        return_value = GSL_ODEIV_HADJ_DEC;
    }
    *h = step_size;

    Squeak::mout(Squeak::debug) << "    ErrorTrackingControlAxialLookup::hadjust"
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
                                << " step " << step_size << std::endl;
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

gsl_odeiv_control* gsl_odeiv_control_lookup_et_new(double min_step_size,
                                            double max_step_size) {
  gsl_odeiv_control* control =  gsl_odeiv_control_alloc(gsl_odeiv_control_lookup_et);
    int status = gsl_odeiv_control_init(control, min_step_size, max_step_size, 0., 0.);
    if (status != GSL_SUCCESS) {
        gsl_odeiv_control_free (control);
        throw MAUS::Exception(Exception::recoverable,
                             "Failed to initialise error tracking control",
                             "ErrorTrackingControlType::gsl_odeiv_control_lookup_et_new");
    }
    return control;
}

}  // namespace ErrorTrackingControlLookup
}  // namespace Global
}  // namespace Kalman
}  // namespace MAUS

