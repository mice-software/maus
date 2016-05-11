#include <map>

#include "TMatrixD.h"  // ROOT

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

#ifndef MAUS_KALMAN_GLOBAL_MEASUREMENT_HH_
#define MAUS_KALMAN_GLOBAL_MEASUREMENT_HH_

namespace MAUS {
namespace Kalman {
namespace Global {
/** Handles the transformation into the measurement coordinate system. As far as
 *  The measurement "state" holds the error information.
 */
class Measurement: public Measurement_base {
  public:
    typedef DataStructure::Global::DetectorPoint DetType;
    Measurement();
    Measurement(TMatrixD measurement);
    ~Measurement() {}
    
    TMatrixD CalculateMeasurementMatrix(const State& state) {return _measurement;}
    void SetMeasurementMatrix(TMatrixD measurement) {_measurement = measurement;}

    static void SetupDetectorToMeasurementMap();
    static std::map<DetType, Measurement_base*>& GetDetectorToMeasurementMap() {return _detector_to_measurement_map;}

  private:
    static std::map<DetType, Measurement_base*> _detector_to_measurement_map;

    TMatrixD _measurement;
};
#endif

}
}
}

