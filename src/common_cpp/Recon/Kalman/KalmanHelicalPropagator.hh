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

#include "src/common_cpp/Recon/Kalman/KalmanState.hh"
#include "src/common_cpp/Recon/Kalman/KalmanPropagator.hh"

#include "CLHEP/Units/PhysicalConstants.h"

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
  void UpdatePropagator(const KalmanState *old_site, const KalmanState *new_site);

  /** @brief Calculates the projected state.
   */
  void CalculatePredictedState(const KalmanState *old_site, KalmanState *new_site);

  /** @brief Returns the momentum of the track at the point.
   */
  double GetTrackMomentum(const KalmanState *a_site);

  TMatrixD BuildQ(TMatrixD a, double L0, double w);

  TMatrixD GetIntermediateState(const KalmanState *old_site, double delta_z, TMatrixD &F);

 private:
  double _Bz;
};

} // ~namespace MAUS

#endif
