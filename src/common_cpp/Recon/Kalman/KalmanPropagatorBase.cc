
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

