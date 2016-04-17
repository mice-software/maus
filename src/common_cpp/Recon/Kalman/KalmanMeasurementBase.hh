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

#ifndef KALMAN_MEASUREMENT_BASE_HH
#define KALMAN_MEASUREMENT_BASE_HH

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

namespace MAUS {
namespace Kalman {

  /** @class Measurement_base
   *
   *  @brief Orchastrates the measurement proceedure
   */
  class Measurement_base {
    public :

      /** @brief returns the expected measurement value
       */
      virtual State Measure(const State& state);

      /** @brief Returns the Kalman Measurement Matrix at the current state
       */
      virtual TMatrixD CalculateMeasurementMatrix(const State& state) = 0;

      /** @brief Returns the last Kalman Measurement Matrix at the current state
       */
      TMatrixD GetMeasurementMatrix() const { return TMatrixD(MeasurementMatrix()); }

      // Measurement noise is only attributed to the data, hence that is where it is stored!
//      /** @brief Returns the covariance matrix of measurement noise
//       */
//      virtual TMatrixD CalculateMeasurementNoise(const State& state) = 0;
//
//      /** @brief Returns the covariance matrix of measurement noise
//       */
//      TMatrixD GetMeasurementNoise() const { return TMatrixD(MeasurementNoise()); }

      /** @brief Return dimension of state vector
       */
      unsigned int GetDimension() const { return _dimension; }

      /** @brief Return dimension of measurement
       */
      unsigned int GetMeasurementDimension() const { return _measurement_dimension; }


      /** @brief Destructor
       */
      virtual ~Measurement_base() {}

    protected :
      /** @brief Initialise the base class with the expected dimensions
       */
      Measurement_base(unsigned int dimension, unsigned int measurement_dimension);

      virtual TMatrixD& MeasurementMatrix() { return _base_measurement_matrix; }
      virtual const TMatrixD& MeasurementMatrix() const { return _base_measurement_matrix; }

//      virtual TMatrixD& MeasurementNoise() { return _base_measurement_noise; }
//      virtual const TMatrixD& MeasurementNoise() const { return _base_measurement_noise; }
    private :

      unsigned int _dimension;
      unsigned int _measurement_dimension;

      TMatrixD _base_measurement_matrix;
      TMatrixD _base_measurement_matrix_transpose;
//      TMatrixD _base_measurement_noise;
  };

} // namespace Kalman
} // namespace MAUS

#endif // KALMAN_MEASUREMENT_BASE_HH

