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

#ifndef _SRC_MAP_MAPCPPTrackerRadiationNoiseModel_H_
#define _SRC_MAP_MAPCPPTrackerRadiationNoiseModel_H_

// #define KALMAN_TEST

// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <string>
#include <sstream>
#include <vector>
#include <map>

// Other headers
#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"
#include "Config/MiceModule.hh"

#include "TRandom3.h"

#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

namespace MAUS {

struct SciFiPlaneGeometry;

/** @class MapCppTrackerRadiationNoiseModel
 *  Group channel hits in the tracker to make clusters.
 *
 */
class MapCppTrackerRadiationNoiseModel : public MapBase<Data> {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerRadiationNoiseModel();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerRadiationNoiseModel();

 private:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  void _birth(const std::string& argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  void _death();

  /** Process MAUS data object
   *
   *  Receive a data object with digits (either MC or real) and then call the higher level
   *  reconstruction algorithms
   *
   * \param document a line/spill from the JSON input
   */
  void _process(Data* data) const;

 private:

  int _getNumberDigits(int plane) const;
  int _getRandomChannel() const;

  std::map<int, double> _planeNoiseRates;

  double _cavityBrightness;
  double _npe_mean;
  double _npe_rms;
  int _centralFibre;
  TRandom3* _random;

  ///  Map of the planes geometry.
  SciFiGeometryHelper _geometry_helper;
};

} // ~namespace MAUS

#endif
