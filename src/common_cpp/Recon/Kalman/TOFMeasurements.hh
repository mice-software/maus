#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

namespace MAUS {
class TOFSpacePointMeasurements : public Kalman::Measurement_base {
  public :
    explicit TOFSpacePointMeasurements();

    virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);

    virtual TMatrixD CalculateMeasurementNoise(const Kalman::State& state);

  protected :

  private :

    TMatrixD _measurement_noise;
    TMatrixD _measurement_matrix;
    double _bar_width = 30.;
};

class TOFTimeMeasurements : public Kalman::Measurement_base {
  public :
    explicit TOFTimeMeasurements();

    virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);

    virtual TMatrixD CalculateMeasurementNoise(const Kalman::State& state);

  protected :

  private :

    TMatrixD _measurement_noise;
    TMatrixD _measurement_matrix;
};
}

