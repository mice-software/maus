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

#include "src/map/MapCppExampleJSONValueInput/MapCppExampleJSONValueInput.hh"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/API/APIExceptions.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppExampleJSONValueInput(void) {
  PyWrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleInitialisation();
}

MapCppExampleJSONValueInput::MapCppExampleJSONValueInput()
    : MapBase<Json::Value, Json::Value>("MapCppExampleJSONValueInput") {}

void MapCppExampleJSONValueInput::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw(Exception(Exception::nonRecoverable,
                    "Unable to Parse Json Config",
                    "MapCppExampleJSONValueInput::_birth"));
  }
}

void MapCppExampleJSONValueInput::_death() {}

Json::Value* MapCppExampleJSONValueInput::_process(Json::Value* json) const {
  if (!json) {
    throw(Exception(Exception::nonRecoverable,
                    "NULL Json::Value* passed to process.",
                    "MapCppExampleJSONValueInput::_process"));
  }

  std::cout << "Run: " << (*json)["run_number"].asInt()
            << "\tSpill: " << (*json)["spill_number"].asInt() << std::endl;
  return new Json::Value(*json);
}
} // ~MAUS
