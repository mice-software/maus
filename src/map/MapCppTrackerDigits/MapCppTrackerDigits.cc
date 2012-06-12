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

#include "src/map/MapCppTrackerDigits/MapCppTrackerDigits.hh"

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
  // Writes a line in the JSON document.
  Json::FastWriter writer;
  SciFiSpill spill;

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

void MapCppTrackerDigits::save_to_json(SciFiSpill &spill) {
  int number_particle_ev = spill.events().size();
  for ( int event_i = 0; event_i < number_particle_ev; ++event_i ) {
    SciFiEvent evt = *(spill.events()[event_i]);
    // ------- DIGITS -------------------------------------------------------
    Json::Value sci_fi_digits_tracker0;
    Json::Value sci_fi_digits_tracker1;
    for ( unsigned int dig_i = 0; dig_i < evt.digits().size(); dig_i++ ) {
      Json::Value digit;
      int tracker = evt.digits()[dig_i]->get_tracker();
      digit["tracker"]= tracker;
      digit["station"]= evt.digits()[dig_i]->get_station();
      digit["plane"]  = evt.digits()[dig_i]->get_plane();
      digit["channel"]= evt.digits()[dig_i]->get_channel();
      digit["npe"]    = evt.digits()[dig_i]->get_npe();
      digit["time"]   = evt.digits()[dig_i]->get_time();
      if ( tracker == 0 ) sci_fi_digits_tracker0.append(digit);
      if ( tracker == 1 ) sci_fi_digits_tracker1.append(digit);
    }
    //
    // Save everything in data structrure tree.
    //
    // Tracker 0 -------------------------------------------------------------------
    root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["tracker0"]
                                                 = sci_fi_digits_tracker0;
    // Tracker 1 -------------------------------------------------------------------
    root["recon_events"][event_i]["sci_fi_event"]["sci_fi_digits"]["tracker1"]
                                                 = sci_fi_digits_tracker1;
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
