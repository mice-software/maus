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

/** @class MapCppExampleMAUSDataInput
 *  Lightweight example of the API
 *
 */

#ifndef _SRC_MAP_MAPCPPEXAMPLEMAUSDATAINPUT_H_
#define _SRC_MAP_MAPCPPEXAMPLEMAUSDATAINPUT_H_
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
#include "Utils/Squeak.hh"

#include "src/common_cpp/API/MapBase.hh"

#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {

class MapCppExampleMAUSDataInput : public MapBase<MAUS::Data> {
 public:
  /** Constructor, setting the internal variable #_classname */
  MapCppExampleMAUSDataInput();

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

  /** process Spill as MAUS::Data*
   *
   *  Confirm that the mapper has been passed the spill in a
   *  MAUS::Data* format.
   *  \param spill in MAUS::Data* format
   */
  void _process(MAUS::Data* data) const;

  /// This will contain the configuration
  Json::Value _configJSON;

  /// JSON Reader for parsing the JSON Config Document
  Json::Reader _reader;
}; // Don't forget this trailing colon!!!!
} // ~MAUS
#endif
