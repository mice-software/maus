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
#include "src/common_cpp/Utils/Constants.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/legacy/Config/MiceModule.hh"
#include "CLHEP/Vector/Rotation.h"
#include "src/common_cpp/Utils/ThreeVectorUtils.hh"

#include "TMath.h"

namespace MAUS {


  // Some Forward declarations.
struct SciFiPlaneGeometry;
struct SciFiTrackerGeometry;
struct SciFiMaterialParams;

  // Some useful typdefs.
typedef std::map<int, SciFiTrackerGeometry> SciFiTrackerMap;
typedef std::map<int, SciFiPlaneGeometry> SciFiPlaneMap;
typedef std::vector< std::pair<const SciFiMaterialParams*, double> > SciFiMaterialsList;



/** @brief Structure for storing parameters relating to scifi materials
 */
struct SciFiMaterialParams {
  /// Atomic number.
  double Z;
  /// Width of the material
  double Plane_Width;
  /// Fibre radiation lenght in mm
  double Radiation_Length;
  /// Fractional Radiation Length
  double L(double length) const { return length/Radiation_Length; }
  /// Density in g.cm-3
  double Density;
  /// Mean excitation energy in eV.
  double Mean_Excitation_Energy;
  /// Atomic number in g.mol-1
  double A;
  /// Channel width in mm
  double Pitch;
  /// Station Radius in mm
  double Station_Radius;
  /// Density correction.
  double Density_Correction;
  /// RMS per channel measurement (um).
  double RMS;
};



/** @struct SciFiPlaneGeometry
 *
 *  @brief A simple struct for bookkeeping of plane info.
 *
 */
struct SciFiPlaneGeometry {
  ThreeVector Direction;
  ThreeVector Position;
  ThreeVector GlobalPosition;
  double CentralFibre;
  double Pitch;
};



/** @struct SciFiTrackerGeometry
 *
 *  @brief A simple struct for bookkeeping of tracker info.
 *
 */
struct SciFiTrackerGeometry {
  HepRotation Rotation;
  ThreeVector Position;
  double Field;
  SciFiPlaneMap Planes;
};



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
  const MiceModule* FindPlane(int tracker, int station, int plane) const;

  /** @brief Returns the Z position of the specified plane
   */
  double GetPlanePosition(int tracker, int station, int plane) const;

  /** @brief Writes plane info to screen.
   */
//  void DumpPlanesInfo();

  /** @brief Finds the field value at a given position by calling the BTFieldConstructor.
   */
  double FieldValue(ThreeVector global_position, HepRotation plane_rotation);

  /** @brief Finds the average on axis field value of a cylinder by calling the BTFieldConstructor.
   */
  double FieldValue(const MiceModule* trackerModule);

  SciFiTrackerMap& GeometryMap() { return _geometry_map; }
  const SciFiTrackerMap& GeometryMap() const { return _geometry_map; }

  double GetFieldValue(int tracker) const { return _geometry_map.find(tracker)->second.Field; }

  ThreeVector GetReferencePosition(int tracker) const { return _geometry_map.find(tracker)->second.Position; }

  HepRotation GetReferenceRotation(int tracker) const { return _geometry_map.find(tracker)->second.Rotation; }

  ThreeVector FindReferenceFramePosition(int tracker) const;

  double GetChannelWidth() const { return FibreParameters.Pitch; }

  const SciFiMaterialParams& GetFibreParameters() const { return FibreParameters; }
  const SciFiMaterialParams& GetGasParameters() const { return GasParameters; }
  const SciFiMaterialParams& GetMylarParameters() const { return MylarParameters; }

  static double HighlandFormula(double L, double beta, double p);

  static double BetheBlochStoppingPower(double p, const SciFiMaterialParams* material);

  void FillMaterialsList(int start_id, int end_id, SciFiMaterialsList& materials_list);

 private:
  std::vector<const MiceModule*> _modules;

  const MiceModule* _module;

  SciFiTrackerMap _geometry_map;

  SciFiMaterialParams FibreParameters;

  SciFiMaterialParams GasParameters;

  SciFiMaterialParams MylarParameters;

}; // Don't forget this trailing colon!!!!
} // ~namespace MAUS

#endif
