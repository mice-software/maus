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

/** @class MapCppTrackerPatternRecognition
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPTrackerPatternRecognition_H_
#define _SRC_MAP_MAPCPPTrackerPatternRecognition_H_

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

#include "TFile.h"
#include "TVectorD.h"

#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

namespace MAUS {

struct SciFiPlaneGeometry;

class MapCppTrackerPatternRecognition : public MapBase<Data> {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerPatternRecognition();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerPatternRecognition();

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

  /** Deduce and fill the reference plane position and momentum for
   * helical tracks
   */
  void extrapolate_helical_reference(SciFiEvent& event) const;

  /** Deduce and fill the reference plane position and momentum for
    * straight tracks
    */
  void extrapolate_straight_reference(SciFiEvent& event) const;

  /** Deduce and fill the seed plane position and momentum for
   * helical tracks
   */
  void extrapolate_helical_seed(SciFiEvent& event) const;

  /** Deduce and fill the seed plane position and momentum for
    * straight tracks
    */
  void extrapolate_straight_seed(SciFiEvent& event) const;

  /**
   * @brief Populate global parameter output for the straight pat rec tracks
   */
  void set_straight_prtrack_global_output(const SciFiStraightPRTrackPArray& trks) const;


 private:
  void _set_field_values(SciFiEvent* event) const;
  void _load_momentum_corrections(std::string filename);

  /// This will contain the configuration
  Json::Value _configJSON;

  bool _correct_seed_momentum;
  TVectorD _upstream_correction;
  TVectorD _downstream_correction;

  bool _up_straight_pr_on;
  bool _down_straight_pr_on;
  bool _up_helical_pr_on;
  bool _down_helical_pr_on;
  bool _patrec_on;
  bool _patrec_debug_on;

  /// Pattern Recognitin Class
  PatternRecognition _pattern_recognition;

  ///  Map of the planes geometry.
  SciFiGeometryHelper _geometry_helper;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
