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


#ifndef STRAIGHTTRACK_HH
#define STRAIGHTTRACK_HH

// C headers
#include <assert.h>
#include <math.h>

// C++ headers
#include <string>
#include <vector>

#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/Kalman/KalmanPropagator.hh"

namespace MAUS {

/** @class KalmanStraightPropagator
 *
 *  @brief Models the propagation of particles in the absence of fields.
 *
 *  Inherits from KalmanTrack, where most of the functionality is defined.
 *  The methods overwritten here are the ones related with particle propagation.
 *
 */
class KalmanStraightPropagator : public KalmanPropagator {
 public:
  /** @brief Straight Track constructor.
   */
  KalmanStraightPropagator();

  /** @brief Destructor.
   */
  virtual ~KalmanStraightPropagator();

  /** @brief Calculates the propagator (F), using Taylor expansion at current site.
   */
  void UpdatePropagator(const KalmanState *old_site, const KalmanState *new_site);

  /** @brief Calculates the projected state.
   */
  void CalculatePredictedState(const KalmanState *old_site, KalmanState *new_site);

  /** @brief Returns the momentum of the track. In the straight track case, p is just a guess.
   */
  double GetTrackMomentum(const KalmanState *a_site = 0);

  TMatrixD BuildQ(TMatrixD a, double L0, double w);

  TMatrixD GetIntermediateState(const KalmanState *old_site, double delta_z, TMatrixD &F);
};

} // ~namespace MAUS

#endif
