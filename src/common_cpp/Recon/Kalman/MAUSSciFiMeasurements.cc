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

  SciFiMeasurements::SciFiMeasurements(SciFiGeometryHelper* geo) :
    _geometry_helper(geo),
    _measurement_noise(1, 1),
    _matrix_map() {
    _measurement_noise(0, 0) = _geometry_helper->GetChannelWidth()/sqrt(12.0);

    GeometryMap geo_map = _geometry_helper->GeometryMap();

    for (GeometryMap::iterator it = geo_map.begin(); it != geo_map.end()) {
      TMatrix H(1, 5);
      H.Zero();

      int id = it->first;
      ThreeVector dir = it->second.Direction;

      double dx = dir.x();
      double dy = dir.y();
      double channel_width = _geometry_helper->GetFibreParameters().Pitch;
      H[0](0, 0) =  dy/channel_width;
      H[0](0, 2) = -dx/channel_width;

      _matrices[id] = H;
    }
  }


  TMatrixD SciFiMeasurements::CalculateMeasurementMatrix(const State& state) {
    return _matrix_map[state.GetId()];
  }


  TMatrixD SciFiMeasurements::CalculateMeasurementNoise(const State& state) {
    return _measurement_noise; // Can cache the constant value
  }

}

