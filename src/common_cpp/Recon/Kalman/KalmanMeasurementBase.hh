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
   * 
   *  Applies a measurement matrix to a state vector such that the measured state vector can be 
   *   used in the Kalman Filter proceedure.
   *
   *  This class does not require knowledge of the value of the measurement-noise, as that is 
   *   only required when in the presence of data, not prediction.
   *
   *  The real data values should be stored with a covariance matrix that describes the noise 
   *   associated with that measurement.
   */
  class Measurement_base {
    public :

      /** @brief Deep copy (member function so that it works with derived classes)
       */
      virtual Measurement_base* Clone() const = 0;

      /** @brief returns the expected measurement value
       */
      virtual State Measure(const State& state);

      /** @brief Returns the Kalman Measurement Matrix at the current state
       */
      virtual TMatrixD CalculateMeasurementMatrix(const State& state) = 0;

      /** @brief Returns the last Kalman Measurement Matrix at the current state
       */
      TMatrixD GetMeasurementMatrix() const { return TMatrixD(MeasurementMatrix()); }

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

    private :

      unsigned int _dimension;
      unsigned int _measurement_dimension;

      TMatrixD _base_measurement_matrix;
      TMatrixD _base_measurement_matrix_transpose;
  };

} // namespace Kalman
} // namespace MAUS

#endif // KALMAN_MEASUREMENT_BASE_HH

