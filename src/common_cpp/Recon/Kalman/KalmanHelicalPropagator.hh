/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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


#ifndef HELICALTRACK_HH
#define HELICALTRACK_HH

// C headers
#include <assert.h>
#include <math.h>

// C++ headers
#include <string>
#include <vector>

#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanPropagator.hh"

namespace MAUS {

/** @class KalmanHelicalPropagator
 *
 *  @brief Models the propagation of particles in the tracker solenoidal field.
 *
 *  Inherits from KalmanTrack, where most of the functionality is defined.
 *  The methods overwritten here are the ones related with particle propagation.
 *
 */
class KalmanHelicalPropagator : public KalmanPropagator {
 public:
  /** @brief Helical Track constructor.
   *
   *  @param flags for use of Multiple Scattering and Eloss models.
   */
  KalmanHelicalPropagator();

  explicit KalmanHelicalPropagator(double Bz);

  /** @brief Destructor.
   */
  virtual ~KalmanHelicalPropagator();

  /** @brief Calculates the propagator (F), using Taylor expansion at current site.
   */
  void UpdatePropagator(const KalmanSite *old_site, const KalmanSite *new_site);

  /** @brief Calculates the projected state.
   */
  void CalculatePredictedState(const KalmanSite *old_site, KalmanSite *new_site);

 private:
  double _Bz;
};

} // ~namespace MAUS

#endif
