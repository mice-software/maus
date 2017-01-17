/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

Measurement::Measurement(const Measurement& meas)
    : Measurement_base(meas._measurement.GetNcols(), meas._measurement.GetNrows()),
      _measurement(meas._measurement) {
    
}

Measurement* Measurement::Clone() const {
    Measurement* meas = new Measurement(_measurement);
    return meas;
}

/*
void Measurement::SetMeasurementMatrix(TMatrixD measurement) {
    if (measurement.GetNcols() != _measurement.GetNcols() ||
        measurement.GetNrows() != _measurement.GetNrows()) {
        throw Exceptions::Exception(Exceptions::recoverable,
                    "Can't change matrix dimension dynamically.",
                    "Measurement::SetMeasurementMatrix");
    }
    _measurement = measurement;
}
*/

void Measurement::SetupDetectorToMeasurementMap() {
    using namespace DataStructure::Global;
    // NB we ignore tof position measurements for now
    TMatrixD tof_matrix(1, 6);
    tof_matrix[0][0] = 1.;
    Measurement* t_meas = new Measurement(tof_matrix);

    _detector_to_measurement_map[DataStructure::Global::kTOF0] = t_meas;
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

