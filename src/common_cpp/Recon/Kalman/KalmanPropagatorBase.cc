
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
    TMatrixD propT; propT.Transpose(prop);
    _noise_matrix = this->CalculateProcessNoise(start_state, end_state);

    TMatrixD vec = start_state.GetVector();
    TMatrixD cov = start_state.GetCovariance();

    TMatrixD new_vec = prop * vec;
    TMatrixD new_cov = prop * cov * propT + noise;

    end_state.SetVector(new_vec);
    end_state.SetCovaraince(new_cov);
  }

}
}

