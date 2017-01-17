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

#include "gsl/gsl_odeiv.h"

#ifndef _recon_kalman_global_ErrorTrackingControl_hh_
#define _recon_kalman_global_ErrorTrackingControl_hh_

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControl {

/** Initialise a new gsl_odeiv_control object with dynamic step size control
 *
 *  - min_step_size: minimum step size that will be used
 *  - max_step_size: maximum step size that will be used
 *
 *  Sets step size according to the Geant4 reported distance to nearest material
 *  so long as that would keep the step size within the min/max limits. If it
 *  would set the step size outside this boundary, step size stays on the
 *  boundary.
 *
 *  This is defined in a C object-oriented/inheritance style. The only public
 *  member function is the gsl_odeiv_control_et_new; everything else is handled
 *  through the gsl_odeiv_control interface struct, which exposes an alloc, init
 *  etc methods. See gsl docs.
 */
gsl_odeiv_control* gsl_odeiv_control_et_new(double min_step_size,
                                            double max_step_size);
}
}
}
}

#endif  // _recon_kalman_global_ErrorTrackingControl_hh_

