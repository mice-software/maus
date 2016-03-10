#include "src/common_cpp/Recon/Kalman/GlobalPropagator.hh"


namespace MAUS {
namespace Kalman {

GlobalPropagator::GlobalPropagator() 
  : Propagator_base(6), _tracking(), _mass(0.) {
}

GlobalPropagator::~GlobalPropagator() {
}

void GlobalPropagator::Propagate(const State& start_state, State& end_state) {
    double x[29];
    TMatrixD vec = start_state.GetVector();
    TMatrixD cov = start_state.GetCovariance();
    for (size_t i = 0; i < 6; ++i) {
        if (i < 3) {
            x[i] = vec[i][0]; // t, x, y
        } else {
            x[i+1] = vec[i][0]; // energy, px, py
        }
    }
    // z
    x[3] = start_state.GetPosition();
    // pz = e^2 - px^2 - py^2 - m^2
    x[7] = std::sqrt(x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - _mass*_mass);

    size_t cov_index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = i; j < 6; ++j) {
            x[cov_index] = cov[i][j];
            ++cov_index;
        }
    }

    double target_z = end_state.GetPosition();
    _tracking.Propagate(x, target_z);
    for (size_t i = 0; i < 3; ++i) {
        vec[i][0] = x[i]; // t, x, y
        vec[i+3][0] = x[i+4]; // energy, px, py        
    }

    cov_index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = i; j < 6; ++j) {
            cov[i][j] = x[cov_index];
            cov[j][i] = x[cov_index];
            ++cov_index;
        }
    }
    end_state.SetVector(vec);
    end_state.SetCovariance(cov);
}

TMatrixD GlobalPropagator::CalculatePropagator(const State& start_state, const State& end_state) {
    double x[44];
    TMatrixD vec = start_state.GetVector();
    TMatrixD cov = start_state.GetCovariance();
    for (size_t i = 0; i < 6; ++i) {
        if (i < 3) {
            x[i] = vec[i][0]; // t, x, y
        } else {
            x[i+1] = vec[i][0]; // energy, px, py
        }
    }
    // z
    x[3] = start_state.GetPosition();
    // pz = e^2 - px^2 - py^2 - m^2
    x[7] = std::sqrt(x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - _mass*_mass);

    size_t tm_index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            if (i == j) {
                x[tm_index] = 1.;
            } else {
                x[tm_index] = 0.;
            }
            ++tm_index;
        }
    }

    double target_z = end_state.GetPosition();
    _tracking.PropagateTransferMatrix(x, target_z);
    tm_index = 8;
    TMatrixD transfer_matrix(6, 6);
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            transfer_matrix[i][j] = x[tm_index];
            ++tm_index;
        }
    }
    return transfer_matrix;
}

TMatrixD GlobalPropagator::CalculateProcessNoise(const State& start_state, const State& end_state) {
    TMatrixD vec = start_state.GetVector();
    TMatrixD cov_start = start_state.GetCovariance();
    TMatrixD cov_end = end_state.GetCovariance();
    TMatrixD propagator = CalculatePropagator(start_state, end_state);
    TMatrixD propagatorT = propagator;
    for (size_t i = 0; i < 6; ++i)
        for (size_t j = 0; j < 6; ++j)
            propagatorT[i][j] = propagator[j][i];
    TMatrixD noise = (propagatorT*cov_start*propagator) - cov_end;
    return noise;
}

}
}
