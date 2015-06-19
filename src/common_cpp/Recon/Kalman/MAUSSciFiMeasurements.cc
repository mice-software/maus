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

  SciFiStraightMeasurements::SciFiStraightMeasurements(SciFiGeometryHelper* geo) :
    Kalman::Measurement_base(4, 1),
    _geometry_helper(geo),
    _measurement_noise(1, 1),
    _matrix_map() {
//    _measurement_noise(0, 0) = _geometry_helper->GetChannelWidth() *
//                                                      _geometry_helper->GetChannelWidth() / 12.0;
    _measurement_noise(0, 0) = 3.5*0.427*3.5*0.427 / 12.0;

    SciFiTrackerMap geo_map = _geometry_helper->GeometryMap();

    for (SciFiTrackerMap::iterator track_it = geo_map.begin();
                                                           track_it != geo_map.end(); ++track_it) {
      int tracker_const = (track_it->first == 0 ? -1 : 1);

      for (SciFiPlaneMap::iterator plane_it = track_it->second.Planes.begin();
                                           plane_it != track_it->second.Planes.end(); ++plane_it) {
        TMatrix H(1, 4);
        H.Zero();

        int id = plane_it->first * tracker_const;
        ThreeVector dir = plane_it->second.Direction; // In tracker reference frame!

        double dx = dir.x();
        double dy = dir.y();
//        double channel_width = _geometry_helper->GetFibreParameters().Pitch;
        H(0, 0) =  dy; // / channel_width;
        H(0, 2) = -dx; // / channel_width;

        _matrix_map.insert(std::make_pair(id, H));
  //      _matrix_map[id] = H;
      }
    }
  }


  TMatrixD SciFiStraightMeasurements::CalculateMeasurementMatrix(const Kalman::State& state) {
    return _matrix_map[state.GetId()];
  }


  TMatrixD SciFiStraightMeasurements::CalculateMeasurementNoise(const Kalman::State& state) {
    return _measurement_noise; // Can cache the constant value
  }


////////////////////////////////////////////////////////////////////////////////
  // Helical Tracks
////////////////////////////////////////////////////////////////////////////////

  SciFiHelicalMeasurements::SciFiHelicalMeasurements(SciFiGeometryHelper* geo) :
    Kalman::Measurement_base(5, 1),
    _geometry_helper(geo),
    _measurement_noise(1, 1),
    _matrix_map() {
//    _measurement_noise(0, 0) = _geometry_helper->GetChannelWidth() *
//                                                      _geometry_helper->GetChannelWidth() / 12.0;
    _measurement_noise(0, 0) = 3.5*0.427*3.5*0.427 / 12.0;

    SciFiTrackerMap geo_map = _geometry_helper->GeometryMap();

    for (SciFiTrackerMap::iterator track_it = geo_map.begin();
                                                           track_it != geo_map.end(); ++track_it) {
      int tracker_const = (track_it->first == 0 ? -1 : 1);

      for (SciFiPlaneMap::iterator plane_it = track_it->second.Planes.begin();
                                           plane_it != track_it->second.Planes.end(); ++plane_it) {
        TMatrix H(1, 5);
        H.Zero();

        int id = plane_it->first * tracker_const;
        ThreeVector dir = plane_it->second.Direction;

        double dx = dir.x();
        double dy = dir.y();
//        double channel_width = _geometry_helper->GetFibreParameters().Pitch;
        H(0, 0) =  dy; // / channel_width;
        H(0, 2) = -dx; // / channel_width;

        _matrix_map.insert(std::make_pair(id, H));
  //      _matrix_map[id] = H;
      }
    }
  }


  TMatrixD SciFiHelicalMeasurements::CalculateMeasurementMatrix(const Kalman::State& state) {
    return _matrix_map[state.GetId()];
  }


  TMatrixD SciFiHelicalMeasurements::CalculateMeasurementNoise(const Kalman::State& state) {
    return _measurement_noise; // Can cache the constant value
  }
}

