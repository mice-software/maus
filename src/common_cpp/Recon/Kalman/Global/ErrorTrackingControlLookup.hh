#include "gsl/gsl_odeiv.h"

#ifndef _recon_kalman_global_ErrorTrackingControl_hh_
#define _recon_kalman_global_ErrorTrackingControl_hh_

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControlLookup {

/** Initialise a new gsl_odeiv_control object with step size control to step to
 *  material boundaries usign the AxialLookup MaterialModel
 *
 *  Sets step size according to ...
 */
gsl_odeiv_control* gsl_odeiv_control_lookup_et_new(MaterialModelAxialLookup* lookup,
                                            double step_size);

}
}
}
}

#endif  // _recon_kalman_global_ErrorTrackingControl_hh_

