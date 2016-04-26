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

#ifndef KALMAN_PROPAGATOR_BASE_HH
#define KALMAN_PROPAGATOR_BASE_HH

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

#include "TMatrixD.h"

namespace MAUS {
namespace Kalman {

  /** @class Propagator_base
   *
   *  @brief Propagates the kalman states
   */
  class Propagator_base {
    public :

      /** @brief Propagate the start state to the end state
       * 
       *  This function must calculate a new propagator and noise matrix!
       *  Rewrite this function to implement an extended kalman filter
       */
      virtual void Propagate(const TrackPoint& start_tp, TrackPoint& end_tp);

      /** @brief Calculates a new propagator matrix
       */
      virtual TMatrixD CalculatePropagator(const TrackPoint& start_tp,
                                                                     const TrackPoint& end_tp) = 0;

      /** @brief Returns the last propagator matrix
       */
      virtual TMatrixD GetPropagator() const { return TMatrixD(PropagatorMatrix()); }

      /** @brief Returns the process noise covariance matrix
       */
      virtual TMatrixD CalculateProcessNoise(const TrackPoint& start_tp,
                                                                     const TrackPoint& end_tp) = 0;

      /** @brief Returns the last process noise covariance matrix
       */
      virtual TMatrixD GetProcessNoise() const { return TMatrixD(NoiseMatrix()); }

      /** @brief Return the dimension of the state vector
       */
      unsigned int GetDimension() const { return _dimension; }

      /** @brief Destructor
       */
      virtual ~Propagator_base() {}

    protected:
      /** @brief Initialise the base class with the expected dimensions
       */
      explicit Propagator_base(unsigned int dimension);

      /** @brief Derived class have controllable access to this variable
       */
      virtual TMatrixD& PropagatorMatrix() { return _propagator_matrix; }
      virtual const TMatrixD& PropagatorMatrix() const { return _propagator_matrix; }

      virtual TMatrixD& NoiseMatrix() { return _noise_matrix; }
      virtual const TMatrixD& NoiseMatrix() const { return _noise_matrix; }

    private:

      unsigned int _dimension;

      TMatrixD _propagator_matrix;

      TMatrixD _noise_matrix;
  };
} // namespace Kalman
} // namespace MAUS

#endif // KALMAN_PROPAGATOR_BASE_HH


