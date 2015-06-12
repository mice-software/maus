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

  class SciFiStraightMeasurements : public Kalman::Measurement_base {
    public :
      SciFiStraightMeasurements(SciFiGeometryHelper* geometry_helper);

//      virtual Kalman::State Measure(const State& state);

      virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);

      virtual TMatrixD CalculateMeasurementNoise(const Kalman::State& state);

    protected :

    private :

      SciFiGeometryHelper* _geometry_helper;

      TMatrixD _measurement_noise;

      std::map<int, TMatrixD> _matrix_map;
  };

  class SciFiHelicalMeasurements : public Kalman::Measurement_base {
    public :
      SciFiHelicalMeasurements(SciFiGeometryHelper* geometry_helper);

//      virtual Kalman::State Measure(const State& state);

      virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);

      virtual TMatrixD CalculateMeasurementNoise(const Kalman::State& state);

    protected :

    private :

      SciFiGeometryHelper* _geometry_helper;

      TMatrixD _measurement_noise;

      std::map<int, TMatrixD> _matrix_map;
  };


  template<unsigned int DIM>
  class SciFiSpacepointMeasurements : public Kalman::Measurement_base {
    public :
      SciFiSpacepointMeasurements(double noise = 0.0);

      virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);

      virtual TMatrixD CalculateMeasurementNoise(const Kalman::State& state);

    protected :

    private :

      TMatrixD _measurement_noise;
      TMatrixD _measurement_matrix;
  };





////////////////////////////////////////////////////////////////////////////////
  // SpacePoint Track Fit
////////////////////////////////////////////////////////////////////////////////

  template<unsigned int DIM>
  SciFiSpacepointMeasurements<DIM>::SciFiSpacepointMeasurements(double noise) :
    Kalman::Measurement_base(DIM, 2),
    _measurement_noise(2, 2),
    _measurement_matrix(2, DIM) {

    _measurement_noise.Zero();
    _measurement_noise(0, 0) = noise;
    _measurement_noise(1, 1) = noise;

    _measurement_matrix.Zero();
    _measurement_matrix(0, 0) = 1.0;
    _measurement_matrix(1, 2) = 1.0;
  }


  template<unsigned int DIM>
  TMatrixD SciFiSpacepointMeasurements<DIM>::CalculateMeasurementMatrix(const Kalman::State& state) {
    return _measurement_matrix;
  }


  template<unsigned int DIM>
  TMatrixD SciFiSpacepointMeasurements<DIM>::CalculateMeasurementNoise(const Kalman::State& state) {
    return _measurement_noise; // Can cache the constant value
  }
}

#endif // MAUS_MEASUREMENTS_HH

