#include "src/common_cpp/Recon/Kalman/TOFMeasurements.hh"

namespace MAUS {
////////////////////////////////////////////////////////////////////////////////
  // TOF Track Fit using t, x, y
////////////////////////////////////////////////////////////////////////////////
TOFSpacePointMeasurements::TOFSpacePointMeasurements() :
  Kalman::Measurement_base(6, 2),
  _measurement_noise(3, 3),
  _measurement_matrix(3, 6) {

  _measurement_noise.Zero();
  _measurement_noise(0, 0) = 0.05; // 50 ps resolution
  _measurement_noise(1, 1) = _bar_width/::sqrt(12);
  _measurement_noise(2, 2) = _bar_width/::sqrt(12);

  _measurement_matrix.Zero();
  _measurement_matrix(0, 0) = 1.0;
  _measurement_matrix(1, 1) = 1.0;
  _measurement_matrix(2, 2) = 1.0;
}

TMatrixD TOFSpacePointMeasurements::CalculateMeasurementMatrix(
                                                const Kalman::State& state) {
  return _measurement_matrix;
}

TMatrixD TOFSpacePointMeasurements::CalculateMeasurementNoise(
                                                const Kalman::State& state) {
  return _measurement_noise; // Can cache the constant value
}

////////////////////////////////////////////////////////////////////////////////
// TOF Track Fit using only t
////////////////////////////////////////////////////////////////////////////////
TOFTimeMeasurements::TOFTimeMeasurements() :
  Kalman::Measurement_base(6, 1),
  _measurement_noise(1, 1),
  _measurement_matrix(1, 6) {

  _measurement_noise.Zero();
  _measurement_noise(0, 0) = 0.05; // 50 ps resolution

  _measurement_matrix.Zero();
  _measurement_matrix(0, 0) = 1.0;
}

TMatrixD TOFTimeMeasurements::CalculateMeasurementMatrix(
                                                const Kalman::State& state) {
  return _measurement_matrix;
}

TMatrixD TOFTimeMeasurements::CalculateMeasurementNoise(
                                                const Kalman::State& state) {
  return _measurement_noise; // Can cache the constant value
}
}
