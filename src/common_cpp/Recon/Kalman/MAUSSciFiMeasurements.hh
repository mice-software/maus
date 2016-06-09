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

#include <map>

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

namespace MAUS {

  class SciFiStraightMeasurements : public Kalman::Measurement_base {
    public :
      explicit SciFiStraightMeasurements(SciFiPlaneGeometry& geometry_helper);

      virtual Kalman::State Measure(const Kalman::State& state);

      virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);
      Kalman::Measurement_base* Clone() const {
          throw MAUS::Exception(Exception::recoverable,
                                "Not implemented",
                                "SciFiStraightTrackMeasurements::Clone");
      }
    protected :

    private :

      TMatrixD _matrix;
      TMatrixD _alignment;
  };

  class SciFiHelicalMeasurements : public Kalman::Measurement_base {
    public :
      explicit SciFiHelicalMeasurements(SciFiPlaneGeometry& geometry_helper);

      virtual Kalman::State Measure(const Kalman::State& state);

      virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);
      Kalman::Measurement_base* Clone() const {
          throw MAUS::Exception(Exception::recoverable,
                                "Not implemented",
                                "SciFiStraightTrackMeasurements::Clone");
      }

    protected :

    private :

      TMatrixD _matrix;
      TMatrixD _alignment;
  };


  template<unsigned int DIM>
  class SciFiSpacepointMeasurements : public Kalman::Measurement_base {
    public :
      explicit SciFiSpacepointMeasurements();

      virtual TMatrixD CalculateMeasurementMatrix(const Kalman::State& state);
      Kalman::Measurement_base* Clone() const {
          throw MAUS::Exception(Exception::recoverable,
                                "Not implemented",
                                "SciFiStraightTrackMeasurements::Clone");
      }

    protected :

    private :

      TMatrixD _measurement_matrix;
  };





////////////////////////////////////////////////////////////////////////////////
  // SpacePoint Track Fit
////////////////////////////////////////////////////////////////////////////////

  template<unsigned int DIM>
  SciFiSpacepointMeasurements<DIM>::SciFiSpacepointMeasurements() :
    Kalman::Measurement_base(DIM, 2),
    _measurement_matrix(2, DIM) {
    _measurement_matrix.Zero();
    _measurement_matrix(0, 0) = 1.0;
    _measurement_matrix(1, 2) = 1.0;
  }


  template<unsigned int DIM>
  TMatrixD SciFiSpacepointMeasurements<DIM>::CalculateMeasurementMatrix(
                                                                      const Kalman::State& state) {
    return _measurement_matrix;
  }
}

#endif // MAUS_MEASUREMENTS_HH

