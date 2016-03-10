
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"
#include "src/common_cpp/Recon/Kalman/TOFMeasurements.hh"
#include "src/common_cpp/Recon/Kalman/GlobalMeasurement.hh"


namespace MAUS {
namespace Kalman {

std::map<GlobalMeasurement::DetType, Measurement_base*>
                                GlobalMeasurement::_detector_to_measurement_map;
GlobalMeasurement::GlobalMeasurement() : Measurement_base(6, 6) {
}

GlobalMeasurement::~GlobalMeasurement() {
}

TMatrixD GlobalMeasurement::CalculateMeasurementMatrix(const State& state) {
    Measurement_base* meas = _state_to_measurement_map[state.GetId()];
    return meas->CalculateMeasurementMatrix(state);
}

TMatrixD GlobalMeasurement::CalculateMeasurementNoise(const State& state) {
    Measurement_base* meas = _state_to_measurement_map[state.GetId()];
    return meas->CalculateMeasurementNoise(state);
}

std::map<int, GlobalMeasurement::DetType>
                              GlobalMeasurement::GetStateToDetectorMap() const {
    return _state_to_detector_map;
}

void GlobalMeasurement::SetStateToDetectorMap
                          (std::map<int, GlobalMeasurement::DetType> s2d_map) {
    _state_to_detector_map = s2d_map;
    _state_to_measurement_map = std::map<int, Measurement_base*>();
    typedef std::map<int, DetType>::iterator s2d_iterator;
    for (s2d_iterator it = s2d_map.begin(); it != s2d_map.end(); ++it) {
        try {
            _state_to_measurement_map[it->first] =
                                      _detector_to_measurement_map[it->second];
        } catch (std::exception exc) {
            throw MAUS::Exception(Exception::recoverable,
                                  "Detector type not implemented",
                                  "GlobalMeasurement::SetStateToDetectorMap");
        }
    }
}

void GlobalMeasurement::SetupDetectorToMeasurementMap() {
  TOFTimeMeasurements* t_meas = new TOFTimeMeasurements();
  _detector_to_measurement_map[DataStructure::Global::kTOF0] = t_meas;
  _detector_to_measurement_map[DataStructure::Global::kTOF1] = t_meas;
  _detector_to_measurement_map[DataStructure::Global::kTOF2] = t_meas;

  SciFiSpacepointMeasurements<6>* scifi = new SciFiSpacepointMeasurements<6>(0.4);
  _detector_to_measurement_map[DataStructure::Global::kTracker0_1] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker0_2] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker0_3] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker0_4] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker0_5] = scifi;

  _detector_to_measurement_map[DataStructure::Global::kTracker1_1] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker1_2] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker1_3] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker1_4] = scifi;
  _detector_to_measurement_map[DataStructure::Global::kTracker1_5] = scifi;
}
}
}

