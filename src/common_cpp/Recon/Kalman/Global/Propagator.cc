#include "src/common_cpp/Recon/Kalman/Global/Propagator.hh"


namespace MAUS {
namespace Kalman {
namespace Global {

Propagator::Propagator() 
  : Propagator_base(6), _tracking(), _mass(0.) {
}

Propagator::~Propagator() {
}

void Propagator::Propagate(const TrackPoint& start_tp, TrackPoint& end_tp) {
    double x[29];
    Squeak::mout(Squeak::info) << "Global::Propagator::Propagate start:\n"
                                << start_tp << std::endl;
    Squeak::mout(Squeak::debug) << "Global::Propagator::Propagate _mass:"
                                << _mass << std::endl;

    TMatrixD vec = start_tp.GetPredicted().GetVector();
    TMatrixD cov = start_tp.GetPredicted().GetCovariance();
    for (size_t i = 0; i < 6; ++i) {
        if (i < 3) {
            x[i] = vec[i][0]; // t, x, y
        } else {
            x[i+1] = vec[i][0]; // energy, px, py
        }
    }
    // z
    x[3] = start_tp.GetPosition();
    // pz = e^2 - px^2 - py^2 - m^2
    x[7] = std::sqrt(x[4]*x[4] - x[5]*x[5] - x[6]*x[6] - _mass*_mass);

    size_t cov_index = 8;
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = i; j < 6; ++j) {
            x[cov_index] = cov[i][j];
            ++cov_index;
        }
    }

    double target_z = end_tp.GetPosition();

    Squeak::mout(Squeak::debug) << "Propagate before..." << std::endl;
    _tracking.print(Squeak::mout(Squeak::debug), x);
    Squeak::mout(Squeak::info) << "   Global::Propagator::Propagating to "
                               << target_z << std::endl;
    _tracking.Propagate(x, target_z);
    Squeak::mout(Squeak::debug) << "Propagate after..." << std::endl;
    _tracking.print(Squeak::mout(Squeak::debug), x);

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
    end_tp.GetPredicted().SetVector(vec);
    end_tp.GetPredicted().SetCovariance(cov);
    Squeak::mout(Squeak::debug) << "Global::Propagator::Propagate end:\n" << end_tp << std::endl;
}

TMatrixD Propagator::CalculatePropagator(const TrackPoint& start_tp, const TrackPoint& end_tp) {
    double x[44];
    TMatrixD vec = start_tp.GetPredicted().GetVector();
    TMatrixD cov = start_tp.GetPredicted().GetCovariance();
    for (size_t i = 0; i < 6; ++i) {
        if (i < 3) {
            x[i] = vec[i][0]; // t, x, y
        } else {
            x[i+1] = vec[i][0]; // energy, px, py
        }
    }
    // z
    x[3] = start_tp.GetPosition();
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

    double target_z = end_tp.GetPosition();
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

TMatrixD Propagator::CalculateProcessNoise(const TrackPoint& start_tp,
                                           const TrackPoint& end_tp) {
    TMatrixD vec = start_tp.GetPredicted().GetVector();
    TMatrixD cov_start = start_tp.GetPredicted().GetCovariance();
    TMatrixD cov_end = end_tp.GetPredicted().GetCovariance();
    TMatrixD propagator = CalculatePropagator(start_tp, end_tp);
    TMatrixD propagatorT = propagator;
    for (size_t i = 0; i < 6; ++i)
        for (size_t j = 0; j < 6; ++j)
            propagatorT[i][j] = propagator[j][i];
    TMatrixD noise = (propagatorT*cov_start*propagator) - cov_end;
    return noise;
}
} // namespace Global
} // namespace Kalman
} // namespace MAUS
