#include <map>

#include "TMatrixD.h"  // ROOT

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

namespace MAUS {
namespace Kalman {

/** Handles the rotation into the measurement coordinate system and measurement
 *  errors. As far as I can tell the measurement State only holds information
 *  about the measured value (not the errors)
 */
class GlobalMeasurement : public Measurement_base {
  typedef DataStructure::Global::DetectorPoint DetType;
  public:
    GlobalMeasurement();
    ~GlobalMeasurement();
    
    TMatrixD CalculateMeasurementMatrix(const State& state);

    TMatrixD CalculateMeasurementNoise(const State& state);

    std::map<int, DetType> GetStateToDetectorMap() const;
    void SetStateToDetectorMap(std::map<int, DetType> s2d_map);
    static void SetupDetectorToMeasurementMap();
    static std::map<DetType, Measurement_base*> GetDetectorToMeasurementMap();

    void SetTofSigmaT(double sigma_t) {_tof_sigma_t = sigma_t;}
    void SetSFSigmaX(double sigma_x) {_sf_sigma_x = sigma_x;}

  private:
    Measurement_base* GetMeasurement(const State& state) const;
    std::map<int, DetType> _state_to_detector_map;
    std::map<int, Measurement_base*> _state_to_measurement_map;
    static std::map<DetType, Measurement_base*> _detector_to_measurement_map;

    static double _tof_sigma_t;
    static double _sf_sigma_x;
};

// BUG move to own file
class DetectorMeasurement: public Measurement_base {
  public:
    DetectorMeasurement();
    DetectorMeasurement(TMatrixD measurement, TMatrixD noise);
    ~DetectorMeasurement() {}
    
    TMatrixD CalculateMeasurementMatrix(const State& state) {return _measurement;}
    void SetMeasurementMatrix(TMatrixD measurement) {_measurement = measurement;}

    TMatrixD CalculateMeasurementNoise(const State& state) {return _noise;}
    void SetMeasurementNoise(TMatrixD noise) {_noise = noise;}

  private:
    TMatrixD _measurement;
    TMatrixD _noise;
};

}
}

