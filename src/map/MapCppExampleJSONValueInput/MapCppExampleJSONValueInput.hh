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

/** @class MapCppExampleJSONValueInput
 *  Digitize events by running Global electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPEXAMPLEJSONVALUEINPUT_H_
#define _SRC_MAP_MAPCPPEXAMPLEJSONVALUEINPUT_H_
// Python / C API
#include <Python.h>

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// Other headers
#include "Interface/Squeak.hh"

#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

class MapCppExampleJSONValueInput : public MapBase<Json::Value, Json::Value> {
 public:
  /** Constructor, setting the internal variable #_classname */
  MapCppExampleJSONValueInput();

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

  /** process Spill as Json::Value*
   *
   *  Confirm that the mapper has been passed the spill in a
   *  Json::Value* format.
   *  \param spill in Json::Value* format
   */
  Json::Value* _process(Json::Value* json) const;

 private:
  /// This will contain the configuration
  Json::Value _configJSON;

  /// JSON Reader for parsing the JSON Config Document
  Json::Reader _reader;
  
}; // Don't forget this trailing colon!!!!
} // ~MAUS

#endif
