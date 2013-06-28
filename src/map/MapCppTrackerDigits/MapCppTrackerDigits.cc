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

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

MapCppTrackerDigits::MapCppTrackerDigits()
    : _spill_json(NULL), _spill_cpp(NULL) {
}

bool MapCppTrackerDigits::~MapCppTrackerDigits() {
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

void MapCppTrackerDigits::read_in_json(std::string document) {
  Json::FastWriter writer;
  if (_spill_cpp != NULL) {
    delete _spill_cpp;
    _spill_cpp = NULL;
  }

  try {
    json_root = JsonWrapper::StringToJson(document);
    if ( json_root.isMember("daq_data") && !(json_root["daq_data"].isNull()) ) {
      SpillProcessor spill_proc;
      _spill_cpp = spill_proc.JsonToCpp(json_root);
    }
  } catch(...) {
    Squeak::mout(Squeak::error) << "Bad json document" << std::endl;
    _spill_cpp = new Spill();
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _spill_cpp->GetErrors();
  }

  // Check for existant pointers to ReconEvents and DAQData
  if ( _spill_cpp.GetDAQData() == NULL )
    _spill_cpp.SetDAQData(new DAQData());

  if (_spill_cpp.GetReconEvents() == NULL)
    _spill_cpp.SetReconEvents(new ReconEventArray());
}


std::string MapCppTrackerDigits::process(std::string document) {
  Json::FastWriter writer;

  read_in_json(document);
  Spill& spill = *_spill_cpp;

  try {
    if ( spill ) {
      // Get daq data.
      Json::Value daq = _json_root.get("daq_data", 0);
      // Fill spill object with 
      RealDataDigitization real;
      real.initialise();
      real.process(spill, daq);
      // Save to JSON output.
      save_to_json(spill);
    }
  } catch(Squeal& squee) {
    squee.Print();
    root = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(root, squee, _classname);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  return writer.write(root);
}

void MapCppTrackerDigits::save_to_json(Spill &spill) {
  SpillProcessor spill_proc;
  root = *spill_proc.CppToJson(spill, "");

    SpillProcessor spill_proc;
    if (_spill_json != NULL) {
        delete _spill_json;
        _spill_json = NULL;
    }
    _spill_json = spill_proc.CppToJson(spill, "");
}

} // ~namespace MAUS
