
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"
#include "src/common_cpp/Recon/Kalman/TOFMeasurements.hh"
#include "src/common_cpp/Recon/Kalman/GlobalMeasurement.hh"


namespace MAUS {
namespace Kalman {

std::map<GlobalMeasurement::DetType, Measurement_base*>
                                GlobalMeasurement::_detector_to_measurement_map;
double GlobalMeasurement::_tof_sigma_t = 0.;
double GlobalMeasurement::_sf_sigma_x = 0.;


GlobalMeasurement::GlobalMeasurement() : Measurement_base(6, 6) {
}

GlobalMeasurement::~GlobalMeasurement() {
}

TMatrixD GlobalMeasurement::CalculateMeasurementMatrix(const State& state) {
    return GetMeasurement(state)->CalculateMeasurementMatrix(state);
}

TMatrixD GlobalMeasurement::CalculateMeasurementNoise(const State& state) {
    return GetMeasurement(state)->CalculateMeasurementNoise(state);
}

Measurement_base* GlobalMeasurement::GetMeasurement(const State& state) const {
    Measurement_base* meas = NULL;
    try {
        // C++11 "range checked" map lookup
        meas = _state_to_measurement_map.at(state.GetId());
    } catch (std::exception& exc) {
        // this exception means we don't know what detector made the measurement
        // so we don't know what coordinate system it is in or what the
        // measurement error is; merde.
        throw MAUS::Exception(
                  Exception::recoverable,
                  "State ID not registered with a detector measurement type",
                  "MAUS::Kalman::GlobalMeasurement::GetMeasurement"
        );
    }
    return meas;
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
    TMatrixD identity(6, 6);
    TMatrixD zeroes(6, 6);
    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            identity[i][j] = 0.;
            zeroes[i][j] = 0.;
        }
        identity[i][i] = 1.;
    }

    TMatrixD tof_noise(1, 1);
    tof_noise[0][0] = _tof_sigma_t*_tof_sigma_t;  // 50 ps
    // NB we ignore tof position measurements for now
    TMatrixD tof_matrix(1, 6);
    tof_matrix[0][0] = 1.;
    DetectorMeasurement* t_meas = new DetectorMeasurement(tof_matrix, tof_noise);

    _detector_to_measurement_map[DataStructure::Global::kTOF0] = t_meas;
    _detector_to_measurement_map[DataStructure::Global::kTOF1] = t_meas;
    _detector_to_measurement_map[DataStructure::Global::kTOF2] = t_meas;

    TMatrixD sf_noise(2, 2);
    sf_noise[0][0] = _sf_sigma_x*_sf_sigma_x;
    sf_noise[1][1] = _sf_sigma_x*_sf_sigma_x;
    TMatrixD sf_matrix(2, 6);
    sf_matrix[0][1] = 1.;
    sf_matrix[1][2] = 1.;

    DetectorMeasurement* sf_meas = new DetectorMeasurement(sf_matrix, sf_noise);
    
    // virtuals are for writing only, we make no measurements here
    DetectorMeasurement* virt_meas = new DetectorMeasurement(TMatrixD(), TMatrixD());
    _detector_to_measurement_map[DataStructure::Global::kVirtual] = virt_meas;


    _detector_to_measurement_map[DataStructure::Global::kTracker0_1] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_2] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_3] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_4] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_5] = sf_meas;

    _detector_to_measurement_map[DataStructure::Global::kTracker1_1] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_2] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_3] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_4] = sf_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_5] = sf_meas;
}

std::map<GlobalMeasurement::DetType, Measurement_base*>
                              GlobalMeasurement::GetDetectorToMeasurementMap() {
    return _detector_to_measurement_map;
}

DetectorMeasurement::DetectorMeasurement()
    : Measurement_base(6, 6), _measurement(6, 6), _noise(6, 6) {
}

DetectorMeasurement::DetectorMeasurement(TMatrixD measurement, TMatrixD noise) 
    : Measurement_base(measurement.GetNcols(), measurement.GetNrows()),
      _measurement(measurement), _noise(noise)  {
}

}
}

