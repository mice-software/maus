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
#include "CLHEP/Vector/Rotation.h"
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
  ThreeVector GlobalPosition;
  double CentralFibre;
  double Pitch;
};

/** @brief Structure for storing parameters relating to scifi materials
 */
struct MaterialParams {
  /// Polystyrene's atomic number.
  double Z;
  /// Width of the fibre plane in mm.
  double Plane_Width;
  /// Fibre radiation lenght in mm
  double Radiation_Length;
  /// Fractional Radiation Length
  double L(double length) { return length/Radiation_Length; }
  /// Density in g.cm-3
  double Density;
  /// Mean excitation energy in eV.
  double Mean_Excitation_Energy;
  /// Atomic number in g.mol-1 per styrene monomer
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

/** @typedef SciFiGeometryMap
 *
 *  @brief type definition of a map of SciFiPlaneGeometry structs indexed by plane id (-15 to 15).
 */
typedef std::map<int, SciFiPlaneGeometry> SciFiGeometryMap;

typedef std::vector< std::pair<MaterialParams, double> > SciFiMaterialsList;

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

  /** @brief Finds the MiceModule corresponding to a tracker plane.
   */
  double GetPlanePosition(int tracker, int station, int plane);

  /** @brief Finds the MiceModule corresponding to plane 0 of station 1 of a tracker -
   *  the origin of the local reference frame.
   */
  ThreeVector GetReferenceFramePosition(int tracker);

  /** @brief Writes plane info to screen.
   */
  void DumpPlanesInfo();

  /** @brief Finds the field value at a given position by calling the BTFieldConstructor.
   */
  double FieldValue(ThreeVector global_position, HepRotation plane_rotation);

  /** @brief Finds the average on axis field value of a cylinder by calling the BTFieldConstructor.
   */
  double FieldValue(const MiceModule* trackerModule);

  SciFiGeometryMap& GeometryMap() const { return _geometry_map; }

  double GetFieldValue(int tracker) const { return _field_value[tracker]; }

  std::vector<ThreeVector> RefPos()  const { return _RefPos; }

  std::vector<HepRotation> Rot()     const { return _Rot;    }

  double GetChannelWidth() const { return w_channel; }

  const MaterialParams& GetFibreParameters() const { return FibreParameters; }
  const MaterialParams& GetGasParameters() const { return GasParameters; }
  const MaterialParams& GetMylarParameters() const { return MylarParameters; }

  static double HighlandFormula(double L, double beta, double p);

  static double BetheBlochStoppingPower(double p, MaterialParams& material);

  void FillMaterialsList(int start_id, int end_id, SciFiMaterialsList& materials_list);

 private:
  std::vector<const MiceModule*> _modules;

  SciFiGeometryMap _geometry_map;

  double _field_value[2];

  std::vector<ThreeVector> _RefPos;

  std::vector<HepRotation> _Rot;

  MaterialParams FibreParameters;

  MaterialParams GasParameters;

  MaterialParams MylarParameters;

  double w_fibre;
  double w_mylar;
  double w_channel;

}; // Don't forget this trailing colon!!!!
} // ~namespace MAUS

#endif
