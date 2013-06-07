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

#ifndef _SRC_COMMON_CPP_UTILS_SCIFIGEOMETRYHELPER_HH
#define _SRC_COMMON_CPP_UTILS_SCIFIGEOMETRYHELPER_HH

#include <vector>
#include <map>

#include "src/common_cpp/Utils/Globals.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/legacy/Config/MiceModule.hh"
#include "Geant4/G4RotationMatrix.hh"
#include "src/common_cpp/Utils/ThreeVectorUtils.hh"

namespace MAUS {

/** @struct SciFiPlaneGeometry
 *
 *  @brief A simple struct for bookkeeping of plane info.
 *
 */
struct SciFiPlaneGeometry {
  ThreeVector Direction;
  ThreeVector Position;
  double CentralFibre;
  double Pitch;
};

/** @typedef SciFiGeometryMap
 *
 *  @brief type definition of a map of SciFiPlaneGeometry structs indexed by plane id (-15 to 15).
 */
typedef std::map<int, SciFiPlaneGeometry> SciFiGeometryMap;

/** @class SciFiGeometryHelper
 *
 *  @brief A helper class for the SciFi geometry handling.
 *
 */
class SciFiGeometryHelper {
 public:
  /** @brief Default constructor.
   */
  SciFiGeometryHelper();

  /** @brief Construct initializing vector of MiceModules.
   */
  explicit SciFiGeometryHelper(const std::vector<const MiceModule*> &modules);

  /** @brief Destructor.
   */
  ~SciFiGeometryHelper();

  /** @brief Builds map of plane structs and array with field values in the trackers.
   */
  void Build();

  /** @brief Finds the MiceModule corresponding to a tracker plane.
   */
  const MiceModule* FindPlane(int tracker, int station, int plane);

  /** @brief Finds the MiceModule corresponding to plane 0 of station 1 of a tracker -
   *  the origin of the local reference frame.
   */
  ThreeVector GetReferenceFramePosition(int tracker);

  /** @brief Writes plane info to screen.
   */
  void DumpPlanesInfo();

  /** @brief Finds the field value at a given position by calling the BTFieldConstructor.
   */
  double FieldValue(ThreeVector global_position, G4RotationMatrix plane_rotation);

  SciFiGeometryMap GeometryMap() const { return _geometry_map; }

  double GetFieldValue(int tracker) const { return _field_value[tracker]; }

 private:
  std::vector<const MiceModule*> _modules;

  SciFiGeometryMap _geometry_map;

  double _mT_to_T;

  double _field_value[2];
}; // Don't forget this trailing colon!!!!
} // ~namespace MAUS

#endif
