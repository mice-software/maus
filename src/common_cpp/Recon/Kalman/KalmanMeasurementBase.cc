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

#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

namespace MAUS {
namespace Kalman {

  Measurement_base::Measurement_base(unsigned int dim, unsigned int meas_dim) :
    _dimension(dim),
    _measurement_dimension(meas_dim) {
  }

  State Measurement_base::Measure(const State& state) {
    _measurement_matrix = this->GetMeasurementMatrix(state);
    TMatrixD measurementT; measurementT.Transpose(_measurement_matrix);
    _measurement_noise = this->GetMeasurementNoise();

    TMatrixD vec = state.GetVector();
    TMatrixD cov = state.GetCovariance();

    TMatrixD new_vec = measurement * vec;
    TMatrixD new_cov = measurement * cov * measurementT + noise;

    State measured_state(new_vec, new_cov, state.GetPosition());
    measured_state.SetId(state.GetId());

    return measured_state;
  }

}
}

