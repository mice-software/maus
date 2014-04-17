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

#include "src/map/MapCppGlobalReconImport/MapCppGlobalReconImport.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"

namespace MAUS {
  MapCppGlobalReconImport::MapCppGlobalReconImport() {
    _classname = "MapCppGlobalReconImport";
  }

  bool MapCppGlobalReconImport::birth(std::string argJsonConfigDocument) {
    // Check if the JSON document can be parsed, else return error only.
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
      _configCheck = false;
      return false;
    }
    _configCheck = true;
    return true;
  }

  bool MapCppGlobalReconImport::death() {
    return true;
  }

  std::string MapCppGlobalReconImport::process(std::string document) const {
    Json::FastWriter writer;
    Json::Value root;

    if (document.empty()) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Empty document passed to process";
      errors["bad_json_document"] = ss.str();
      root["errors"] = errors;
      return writer.write(root);
    }

    if (!_configCheck) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: process passed an invalid configuration";
      errors["bad_json_document"] = ss.str();
      root["errors"] = errors;
      return writer.write(root);
    }

    // Prepare converters, spill and json objects
    JsonCppSpillConverter json2cppconverter;
    CppJsonSpillConverter cpp2jsonconverter;
    Json::Value *data_json = NULL;
    MAUS::Data *data_cpp = NULL;

    // Read string and convert to a Json object
    try {
      Json::Value imported_json = JsonWrapper::StringToJson(document);
      data_json = new Json::Value(imported_json);
    } catch (...) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Bad json document";
      errors["bad_json_document"] = ss.str();
      root["errors"] = errors;
      delete data_json;
      return writer.write(root);
    }


    if (!data_json || data_json->isNull()) {
      if (data_json) delete data_json;
      return std::string("{\"errors\":{\"bad_json_document\":")+
	std::string("\"Failed to parse input document\"}}");
    }

    if (data_json->empty()) {
      delete data_json;
      return std::string("{\"errors\":{\"bad_json_document\":")+
	std::string("\"Failed to parse input document\"}}");
    }

    std::string maus_event = JsonWrapper::GetProperty(
	*data_json, "maus_event_type",
	JsonWrapper::stringValue).asString();

    if ( maus_event.compare("Spill") != 0 ) {
      Squeak::mout(Squeak::error) << "Line of json document did not contain "
	  << "a Spill" << std::endl;
      delete data_json;
      return document;
    }

    std::string daq_event = JsonWrapper::GetProperty(
        *data_json, "daq_event_type",
        JsonWrapper::stringValue).asString();

    if ( daq_event.compare("physics_event") != 0 ) {
      Squeak::mout(Squeak::error) << "daq_event_type did not return a "
	  << "physics event" << std::endl;
      delete data_json;
      return document;
    }

    // Convert Json into MAUS::Spill object.  In future, this will all
    // be done for me, and process will take/return whichever object we
    // prefer.
    try {
      data_cpp = json2cppconverter(data_json);
      delete data_json;
    } catch (...) {
      Squeak::mout(Squeak::error) << "Missing required branch daq_event_type"
	  << "converting json->cpp, MapCppGlobalReconImport" << std::endl;
    }

    if (!data_cpp) {
      return std::string("{\"errors\":{\"failed_json_cpp_conversion\":")+
	std::string("\"Failed to convert Json to Cpp Spill object\"}}");
    }

    const MAUS::Spill* spill = data_cpp->GetSpill();

    MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();

    if (!recon_events) {
      delete data_cpp;
      return document;
    }

    MAUS::GlobalEvent* global_event;
    MAUS::ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
	 recon_event_iter != recon_events->end();
	 ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      MAUS::ReconEvent* recon_event = (*recon_event_iter);
      global_event = recon_event->GetGlobalEvent();
      global_event = Import(recon_event);
    }

    data_json = cpp2jsonconverter(data_cpp);

    if (!data_json) {
      delete data_cpp;
      return std::string("{\"errors\":{\"failed_cpp_json_conversion\":")+
	std::string("\"Failed to convert Cpp to Json Spill object\"}}");
    }

    std::string output_document = JsonWrapper::JsonToString(*data_json);
    delete data_json;
    delete data_cpp;
    return output_document;
  }

  MAUS::GlobalEvent*
  MapCppGlobalReconImport::Import(MAUS::ReconEvent* recon_event) const {
    if (!recon_event) {
      throw(Exception(Exception::recoverable,
		      "Trying to import an empty recon event.",
		      "MapCppGlobalReconImport::Import"));
    }

    MAUS::GlobalEvent* global_event = recon_event->GetGlobalEvent();

    MAUS::TOFEvent* tof_event = recon_event->GetTOFEvent();

    MAUS::SciFiEvent* scifi_event = recon_event->GetSciFiEvent();

    if (tof_event) {
      MAUS::recon::global::ImportTOFRecon tofrecon_importer;
      tofrecon_importer.process((*tof_event), global_event, _classname);
    }
    if (scifi_event) {
      MAUS::recon::global::ImportSciFiRecon scifirecon_importer;
      scifirecon_importer.process((*scifi_event), global_event, _classname);
    }
    // Return the new GlobalEvent, to be added to the ReconEvent
    return global_event;
  }
} // ~MAUS
