/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#ifndef MAUS_MEASUREMENTS_HH
#define MAUS_MEASUREMENTS_HH

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

namespace MAUS {

  class SciFiMeasurements : Kalman::Measurement_base {
    public :
      SciFiMeasurements(SciFiGeometryHelper* geometry_helper);

//      virtual Kalman::State Measure(const State& state);

      virtual TMatrixD CalculateMeasurementMatrix(const State& state);

      virtual TMatrixD CalculateMeasurementNoise(const State& state);

    protected :

    private :

      SciFiGeometryHelper* _geometry_helper;

      TMatrixD _measurement_noise;

      std::map<int, TMatrixD> _matrix_map;
  };

}

#endif // MAUS_MEASUREMENTS_HH

