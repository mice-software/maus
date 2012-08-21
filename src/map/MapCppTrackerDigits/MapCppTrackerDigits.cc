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

namespace MAUS {

bool MapCppTrackerDigits::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerDigits";

  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only.
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // Get the tracker modules.
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  MiceModule* _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  // Get minPE cut value.
  assert(_configJSON.isMember("SciFiNPECut"));
  minPE = _configJSON["SciFiNPECut"].asDouble();

  return true;
}

bool MapCppTrackerDigits::death() {
  return true;
}

std::string MapCppTrackerDigits::process(std::string document) {
  std::cout << "Digitising tracker data\n";

  Json::FastWriter writer;
  Spill spill;

  try {
    // Load input.
    root = JsonWrapper::StringToJson(document);
    if ( root.isMember("daq_data") && !root["daq_data"].isNull() ) {
      // Get daq data.
      Json::Value daq = root.get("daq_data", 0);
      // Process the input.
      RealDataDigitization real;
      real.process(spill, daq);
      // Save to JSON output.
      save_to_json(spill);
    }
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
  root = *spill_proc.CppToJson(spill);
}

void MapCppTrackerDigits::sfspill_to_mausspill(SciFiSpill &sfspill, Spill &mspill) {

  mspill.SetReconEvents(new ReconEventArray());

  for ( unsigned int event_i = 0; event_i < sfspill.events().size(); ++event_i ) {
    ReconEvent * revt = new ReconEvent();
    // Pointers should be deep copied
    revt->SetSciFiEvent(new SciFiEvent(*(sfspill.events()[event_i])));
    mspill.GetReconEvents()->push_back(revt);
  }
}

// The following two functions are added for testing purposes only
Json::Value MapCppTrackerDigits::ConvertToJson(std::string jsonString) {
  Json::Value newJson;
  // JsonCpp string -> JSON::Value converter
  Json::Reader reader;

  newJson = JsonWrapper::StringToJson(jsonString);
  return newJson;
}

std::string MapCppTrackerDigits::JsonToString(Json::Value json_in) {
  std::stringstream ss_io;
  JsonWrapper::Print(ss_io, json_in);
  return ss_io.str();
}

} // ~namespace MAUS
