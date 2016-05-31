
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"
#include "src/common_cpp/Recon/Kalman/Global/Measurement.hh"


namespace MAUS {
namespace Kalman {
namespace Global {

std::map<Measurement::DetType, Measurement_base*> Measurement::_detector_to_measurement_map;

Measurement::Measurement()
    : Measurement_base(0, 0), _measurement() {
}

Measurement::Measurement(TMatrixD measurement) 
    : Measurement_base(measurement.GetNcols(), measurement.GetNrows()),
      _measurement(measurement) {
}

Measurement* Measurement::Clone() const {
    Measurement* meas = new Measurement(_measurement);
    return meas;
}


void Measurement::SetupDetectorToMeasurementMap() {
    using namespace DataStructure::Global;
    // NB we ignore tof position measurements for now
    TMatrixD tof_matrix(1, 6);
    tof_matrix[0][0] = 1.;
    Measurement* t_meas = new Measurement(tof_matrix);

    _detector_to_measurement_map[kTOF0] = t_meas;
    _detector_to_measurement_map[DataStructure::Global::kTOF1] = t_meas;
    _detector_to_measurement_map[DataStructure::Global::kTOF2] = t_meas;

    TMatrixD sf_sp_matrix(2, 6);
    sf_sp_matrix[0][1] = 1.;
    sf_sp_matrix[1][2] = 1.;
    Measurement* sf_sp_meas = new Measurement(sf_sp_matrix);

    _detector_to_measurement_map[DataStructure::Global::kTracker0_1] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_2] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_3] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_4] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker0_5] = sf_sp_meas;

    _detector_to_measurement_map[DataStructure::Global::kTracker1_1] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_2] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_3] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_4] = sf_sp_meas;
    _detector_to_measurement_map[DataStructure::Global::kTracker1_5] = sf_sp_meas;

    
    // virtuals are for writing only, we make no measurements here
    Measurement* virt_meas = new Measurement(TMatrixD());
    _detector_to_measurement_map[DataStructure::Global::kVirtual] = virt_meas;
}
} // namespace Global
} // namespace Kalman
} // namespace MAUS

