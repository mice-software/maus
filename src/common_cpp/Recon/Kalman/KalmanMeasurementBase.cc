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

#include <iostream>

namespace MAUS {
namespace Kalman {

  Measurement_base::Measurement_base(unsigned int dim, unsigned int meas_dim) :
    _dimension(dim),
    _measurement_dimension(meas_dim),
    _base_measurement_matrix(meas_dim, dim),
    _base_measurement_matrix_transpose(dim, meas_dim),
    _base_measurement_noise(meas_dim, meas_dim) {
  }

  State Measurement_base::Measure(const State& state) {
    std::cerr << "Measurement_base::Measure A" << std::endl;
    std::cerr << "Base\n" << std::endl;
    for (int i = 0; i < _base_measurement_matrix.GetNrows(); ++i) {
        for (int j = 0; j < _base_measurement_matrix.GetNcols(); ++j) {
            std::cerr << _base_measurement_matrix[i][j] << " ";
        }
        std::cerr << std::endl;
    }
    std::cerr << "Calc\n" << std::endl;
    for (int i = 0; i < this->CalculateMeasurementMatrix(state).GetNrows(); ++i) {
        for (int j = 0; j < this->CalculateMeasurementMatrix(state).GetNcols(); ++j) {
            std::cerr << this->CalculateMeasurementMatrix(state)[i][j] << " ";
        }
        std::cerr << std::endl;
    }
    TMatrixD measurement = this->CalculateMeasurementMatrix(state);
    if (measurement.GetNcols() == 0) {
        State measured_state(0, state.GetPosition());
        measured_state.SetId(state.GetId());
        return measured_state;
    }
    TMatrixD noise = this->CalculateMeasurementNoise(state);
    TMatrixD measurement_transpose(measurement.GetNcols(), measurement.GetNrows());
    measurement_transpose.Transpose(measurement);

    _base_measurement_matrix.ResizeTo(measurement.GetNrows(), measurement.GetNcols());
    _base_measurement_noise.ResizeTo(noise.GetNrows(), noise.GetNcols());
    _base_measurement_matrix_transpose.ResizeTo(measurement_transpose.GetNrows(), measurement_transpose.GetNcols());

    _base_measurement_matrix = measurement;
    _base_measurement_noise = noise;
    _base_measurement_matrix_transpose = measurement_transpose;

    _measurement_dimension = _base_measurement_noise.GetNrows();



    TMatrixD new_vec(_measurement_dimension, 1);
    std::cerr << "Measurement_base::Measure E" << std::endl;
    TMatrixD new_cov(_measurement_dimension, _measurement_dimension);
    std::cerr << "Measurement_base::Measure F" << std::endl;

    new_vec = _base_measurement_matrix * state.GetVector();
    std::cerr << "Measurement_base::Measure G" << std::endl;
    new_cov = (_base_measurement_matrix*state.GetCovariance()*_base_measurement_matrix_transpose);
    std::cerr << "Measurement_base::Measure H" << std::endl;

    State measured_state(new_vec, new_cov, state.GetPosition());
    std::cerr << "Measurement_base::Measure I" << std::endl;
    measured_state.SetId(state.GetId());
    std::cerr << "Measurement_base::Measure J" << std::endl;

    return measured_state;
  }
}
}

