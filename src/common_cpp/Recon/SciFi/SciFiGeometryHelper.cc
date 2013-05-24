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

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

namespace MAUS {

SciFiGeometryHelper::SciFiGeometryHelper(std::vector<const MiceModule*> modules)
                                        : _modules(modules) {}

SciFiGeometryHelper::~SciFiGeometryHelper() {}

std::map<int, SciFiPlaneGeometry> SciFiGeometryHelper::build_geometry_map() {
  // This is the map to be returned.
  std::map<int, SciFiPlaneGeometry> geometry_map;

  // Iterate over existing modules, adding planes to the map.
  std::vector<const MiceModule*>::iterator iter;
  for ( iter = _modules.begin(); iter != _modules.end(); iter++ ) {
    const MiceModule* module = (*iter);
    if ( module->propertyExists("Tracker", "int") &&
         module->propertyExists("Station", "int") &&
         module->propertyExists("Plane", "int") ) {
      int tracker_n = module->propertyInt("Tracker");
      int station_n = module->propertyInt("Station");
      int plane_n   = module->propertyInt("Plane");

      double pitch        = module->propertyDouble("Pitch");
      double centralfibre = module->propertyDouble("CentralFibre");
      ThreeVector direction(0., 1., 0.);
      G4RotationMatrix rel_rot(module->relativeRotation(module->mother()    // plane
                                                              ->mother()    // tracker
                                                              ->mother())); // solenoid
      direction *= rel_rot;

      ThreeVector position  = clhep_to_root(module->globalPosition());
      ThreeVector reference = get_reference_frame_pos(tracker_n);
      ThreeVector tracker_ref_frame_pos = position - reference;

      SciFiPlaneGeometry this_plane;
      this_plane.Direction    = direction;
      this_plane.Position     = tracker_ref_frame_pos;
      this_plane.CentralFibre = centralfibre;
      this_plane.Pitch        = pitch;

      int plane_id =  3*(station_n-1) + (plane_n+1);
      plane_id     = ( tracker_n == 0 ? -plane_id : plane_id );
      geometry_map.insert(std::make_pair(plane_id, this_plane));
    }
  }
  return geometry_map;
}

const MiceModule* SciFiGeometryHelper::find_plane(int tracker, int station, int plane) {
  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < _modules.size(); ++j ) {
    // Find the right module
    if ( _modules[j]->propertyExists("Tracker", "int") &&
         _modules[j]->propertyExists("Station", "int") &&
         _modules[j]->propertyExists("Plane", "int")  &&
         _modules[j]->propertyInt("Tracker") ==
         tracker &&
         _modules[j]->propertyInt("Station") ==
         station &&
         _modules[j]->propertyInt("Plane") ==
         plane ) {
         // Save the module
      this_plane = _modules[j];
    }
  }
  if ( this_plane == NULL ) {
    throw(Squeal(Squeal::nonRecoverable,
    "Failed to find tracker plane.",
    "SciFiGeometryHelper::find_plane"));
  }
  return this_plane;
}

ThreeVector SciFiGeometryHelper::get_reference_frame_pos(int tracker) {
  // Reference plane is plane 0, station 1 of current tracker.
  int station = 1;
  int plane   = 0;
  const MiceModule* reference_plane = NULL;
  reference_plane = find_plane(tracker, station, plane);

  assert(reference_plane != NULL);
  ThreeVector reference_pos = clhep_to_root(reference_plane->globalPosition());

  return reference_pos;
}

} // ~namespace MAUS
