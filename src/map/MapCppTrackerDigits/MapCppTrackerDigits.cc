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

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/map/MapCppTrackerDigits/MapCppTrackerDigits.hh"

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

MapCppTrackerDigits::MapCppTrackerDigits(): _spill_json(NULL),
                                            _spill_cpp(NULL) {}

MapCppTrackerDigits::~MapCppTrackerDigits() {
  if (_spill_json != NULL) {
    delete _spill_json;
  }
  if (_spill_cpp != NULL) {
    delete _spill_cpp;
  }
}

bool MapCppTrackerDigits::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerDigits";
  return true;
}

bool MapCppTrackerDigits::death() {
  return true;
}

std::string MapCppTrackerDigits::process(std::string document) {
  Json::FastWriter writer;

  read_in_json(document);

  try {
    if ( _json_root.isMember("daq_data") && !(_json_root["daq_data"].isNull()) ) {
      // Get daq data.
      Json::Value daq = _json_root.get("daq_data", 0);
      // Fill spill object with
      RealDataDigitization real;
      real.initialise();
      real.process(_spill_cpp, daq);
      // Save to JSON output.
      save_to_json(_spill_cpp);
    } else {
      return writer.write(_json_root);
    }
  // If an exception is caught, the JSON file returned is the same that was read-in (_json_root)...
  } catch(Exception& exception) {
    exception.Print();
    return writer.write(_json_root);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _json_root["errors"] = errors;
    return writer.write(_json_root);
  }
  // ... if everything went alright, we return a modified spill (_spill_json).
  return writer.write(*_spill_json);
}

void MapCppTrackerDigits::read_in_json(std::string json_data) {
  Json::FastWriter writer;
  if (_spill_cpp != NULL) {
    delete _spill_cpp;
    _spill_cpp = NULL;
  }

  try {
    _json_root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    _spill_cpp = spill_proc.JsonToCpp(_spill_json);
  } catch(...) {
    Squeak::mout(Squeak::error) << "Bad json document" << std::endl;
    _spill_cpp = new Spill();
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _spill_cpp->GetErrors();
  }
}

void MapCppTrackerDigits::save_to_json(Spill *spill) {
  SpillProcessor spill_proc;
  if (_spill_json != NULL) {
      delete _spill_json;
      _spill_json = NULL;
  }
  _spill_json = spill_proc.CppToJson(*spill, "");
}

} // ~namespace MAUS
