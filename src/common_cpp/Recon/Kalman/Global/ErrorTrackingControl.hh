#include "gsl/gsl_odeiv.h"

#ifndef _recon_kalman_global_ErrorTrackingControl_hh_
#define _recon_kalman_global_ErrorTrackingControl_hh_

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControl {

/** Initialise a new gsl_odeiv_control object with dynamic step size control
 *
 *  Sets step size according to ...
 */
gsl_odeiv_control* gsl_odeiv_control_et_new(double min_step_size,
                                            double max_step_size);

}
}
}
}

#endif  // _recon_kalman_global_ErrorTrackingControl_hh_

