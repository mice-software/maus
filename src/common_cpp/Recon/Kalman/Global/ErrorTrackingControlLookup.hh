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
 */

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
 *  Sets step size according to distance to next material.
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

