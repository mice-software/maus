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

#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"

namespace MAUS {

  SciFiStraightMeasurements::SciFiStraightMeasurements(SciFiPlaneGeometry& geo) :
    Kalman::Measurement_base(4, 1),
    _matrix(1, 4),
    _alignment(4, 1) {

    _matrix.Zero();
    _alignment.Zero();

    ThreeVector pos = geo.Position; // In tracker reference frame!
    ThreeVector dir = geo.Direction; // In tracker reference frame!

    _matrix(0, 0) =  dir.y();
    _matrix(0, 2) = -dir.x();

    _alignment(0, 0) = pos.x();
    _alignment(2, 0) = pos.y();
  }


  TMatrixD SciFiStraightMeasurements::CalculateMeasurementMatrix(const Kalman::State& state) {
    return _matrix;
  }


////////////////////////////////////////////////////////////////////////////////
  // Helical Tracks
////////////////////////////////////////////////////////////////////////////////

  SciFiHelicalMeasurements::SciFiHelicalMeasurements(SciFiPlaneGeometry& geo) :
    Kalman::Measurement_base(5, 1),
    _matrix(1, 5),
    _alignment(5, 1) {

    ThreeVector pos = geo.Position; // In tracker reference frame!
    ThreeVector dir = geo.Direction; // In tracker reference frame!

    _matrix(0, 0) =  dir.y();
    _matrix(0, 2) = -dir.x();

    _alignment(0, 0) = pos.x();
    _alignment(2, 0) = pos.y();
  }


  TMatrixD SciFiHelicalMeasurements::CalculateMeasurementMatrix(const Kalman::State& state) {
    return _matrix;
  }
}

