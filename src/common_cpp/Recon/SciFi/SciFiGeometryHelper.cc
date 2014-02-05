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

/*
TODO: take back changes in plane position.
*/

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

namespace MAUS {

SciFiGeometryHelper::SciFiGeometryHelper() {}

SciFiGeometryHelper::SciFiGeometryHelper(const std::vector<const MiceModule*> &modules)
                                        : _modules(modules) {}

SciFiGeometryHelper::~SciFiGeometryHelper() {}

void SciFiGeometryHelper::Build() {
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

      // G4RotationMatrix global_fibre_rotation = G4RotationMatrix(module->globalRotation());
      const MiceModule* plane = module->mother();
      G4RotationMatrix internal_fibre_rotation(module->relativeRotation(module->mother() // plane
                                               ->mother()));  // tracker/ station??

      direction     *= internal_fibre_rotation;

      // The plane rotation wrt to the solenoid. Identity matrix for tracker 1,
      // [ -1, 0, 0],[ 0, 1, 0],[ 0, 0, -1] for tracker 0 (180 degrees rot. around y).
      // const MiceModule* plane = module->mother();
      G4RotationMatrix plane_rotation(plane->relativeRotation(plane->mother()  // tracker
                                                              ->mother()));    // solenoid

      ThreeVector position  = clhep_to_root(module->globalPosition());
      ThreeVector reference = GetReferenceFramePosition(tracker_n);

      ThreeVector tracker_ref_frame_pos = position-reference;
      tracker_ref_frame_pos *= plane_rotation;

      SciFiPlaneGeometry this_plane;
      this_plane.Direction     = direction;
      // TO BE REMOVED. Ed
      tracker_ref_frame_pos.setX(0);
      tracker_ref_frame_pos.setY(0);
      this_plane.Position      = tracker_ref_frame_pos;
      this_plane.CentralFibre  = centralfibre;
      this_plane.Pitch         = pitch;
      int plane_id =  3*(station_n-1) + (plane_n+1);
      plane_id     = ( tracker_n == 0 ? -plane_id : plane_id );
      _geometry_map.insert(std::make_pair(plane_id, this_plane));
      _field_value[tracker_n] = FieldValue(reference, plane_rotation);
    }
  }
}

double SciFiGeometryHelper::FieldValue(ThreeVector global_position,
                                       G4RotationMatrix plane_rotation) {
  double EMfield[6]  = {0., 0., 0., 0., 0., 0.};
  double position[4] = {global_position.x(), global_position.y(), global_position.z(), 0.};
  BTFieldConstructor* field = Globals::GetMCFieldConstructor();
  field->GetElectroMagneticField()->GetFieldValue(position, EMfield);
  ThreeVector B_field(EMfield[0], EMfield[1], EMfield[2]);
  B_field *= plane_rotation;
  double Tracker_Bz = B_field.z();
  return Tracker_Bz;
}

const MiceModule* SciFiGeometryHelper::FindPlane(int tracker, int station, int plane) {
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
    throw(Exception(Exception::nonRecoverable,
    "Failed to find tracker plane.",
    "SciFiGeometryHelper::find_plane"));
  }
  return this_plane;
}

ThreeVector SciFiGeometryHelper::GetReferenceFramePosition(int tracker) {
  // Reference plane is plane 0, station 1 of current tracker.
  int station = 1;
  int plane   = 0;
  const MiceModule* reference_plane = NULL;
  reference_plane = FindPlane(tracker, station, plane);

  assert(reference_plane != NULL);
  ThreeVector reference_pos = clhep_to_root(reference_plane->globalPosition());
  return reference_pos;
}

void SciFiGeometryHelper::DumpPlanesInfo() {
  std::map<int, SciFiPlaneGeometry>::iterator plane;
  for ( plane = _geometry_map.begin(); plane != _geometry_map.end(); ++plane ) {
    Squeak::mout(Squeak::info) << "Plane ID: " << plane->first << "\n"
                               << "Direction: "<< plane->second.Direction << "\n"
                               << "Position: " << plane->second.Position << "\n"
                               << "CentralFibre: "<< plane->second.CentralFibre << "\n"
                               << "Pitch: "       << plane->second.Pitch << "\n";
  }
}

} // ~namespace MAUS
