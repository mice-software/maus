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


#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"

namespace MAUS {
namespace Kalman {

  Propagator_base::Propagator_base(unsigned int dim) :
    _dimension(dim),
    _propagator_matrix(_dimension, _dimension),
    _noise_matrix(_dimension, _dimension) {
  }

  void Propagator_base::Propagate(const State& start_state, State& end_state) {
    _propagator_matrix = this->CalculatePropagator(start_state, end_state);
    TMatrixD propT(TMatrixD::kTransposed, _propagator_matrix);
    _noise_matrix = this->CalculateProcessNoise(start_state, end_state);

    TMatrixD new_vec(GetDimension(), 1);
    new_vec = _propagator_matrix * start_state.GetVector();
    TMatrixD new_cov(GetDimension(), GetDimension());
    new_cov = _propagator_matrix * start_state.GetCovariance() * propT + _noise_matrix;

    end_state.SetVector(new_vec);
    end_state.SetCovariance(new_cov);
  }
}
}

