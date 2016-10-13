#include "gsl/gsl_odeiv.h"

#ifndef _recon_kalman_global_ErrorTrackingControlLookup_hh_
#define _recon_kalman_global_ErrorTrackingControlLookup_hh_

namespace MAUS {

class MaterialModelAxialLookup;

namespace Kalman {
namespace Global {
namespace ErrorTrackingControlLookup {

/** Initialise a new gsl_odeiv_control object with step size control to step to
 *  material boundaries usign the AxialLookup MaterialModel
 *
 *  Sets step size according to ...
 */
gsl_odeiv_control* gsl_odeiv_control_lookup_et_new(double min_step_size,
                                                   double max_step_size);

int et_control_lookup_hadjust(void * vstate, size_t dim, unsigned int ord,
                              const double y[], const double yerr[],
                              const double yp[], double * h);

}
}
}
}

#endif  // _recon_kalman_global_ErrorTrackingControl_hh_

