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

/** @class MapCppGlobalRecon
 *  Digitize events by running Global electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPGlobalRecon_H_
#define _SRC_MAP_MAPCPPGlobalRecon_H_
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

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/GlobalEvent.hh"

namespace MAUS {

class MapCppGlobalRecon : public MapBase<MAUS::Data, MAUS::Data> {
 public:
  /** Constructor, setting the internal variable #_classname */
  MapCppGlobalRecon();

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

  /** process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   * \param document a line/spill from the JSON input
   */
  MAUS::Data* _process(MAUS::Data* data) const;

  /** Import the existing MAUS::ReconEvent, creating a new
   * MAUS::GlobalEvent and populating an early
   * MAUS::recon::global::PrimaryChain object, with one track per
   * input object (even if a sub-detector has only provided a single
   * space_point).
   */
  MAUS::GlobalEvent* Import(MAUS::ReconEvent* recon_event) const;

 private:
  /// This will contain the configuration
  Json::Value _configJSON;
  // /// This will contain the root value after parsing
  // Json::Value root;
  ///  JsonCpp setup
  Json::Reader _reader;
}; // Don't forget this trailing colon!!!!
} // ~MAUS

#endif
