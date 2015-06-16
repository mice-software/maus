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

#include <string>
#include <sstream>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/reduce/ReduceCppGlobalPID/ReduceCppGlobalPID.hh"

namespace MAUS {

  bool ReduceCppGlobalPID::birth(std::string argJsonConfigDocument) {
    _configCheck = false;

    _classname = "ReduceCppGlobalPID";

    // JsonCpp setup - check file parses correctly, if not return false
    Json::Value configJSON;
    try {
      configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
      _hypothesis_name = configJSON["global_pid_hypothesis"].asString();
      _unique_identifier = configJSON["unique_identifier"].asString();
      if (_hypothesis_name.empty() ||
          !configJSON.isMember("global_pid_hypothesis")) {
        Squeak::mout(Squeak::error) << "Json did not contain a valid "
	    << "global_pid_hypothesis, which is required for PDF production, "
	    << "ReduceCppGlobalPID::birth" << std::endl;
        return false;
      }
      if (_unique_identifier.empty() ||
          !configJSON.isMember("unique_identifier")) {
        Squeak::mout(Squeak::error) << "Json did not contain a valid "
            << "unique_identifier, which is required for PDF production, "
	    << "ReduceCppGlobalPID::birth" << std::endl;
        return false;
      }

      // TODO(Pidcott) this vector will contain several PID variables, once
      // they have been determined
      _pid_vars.clear();
      _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_hypothesis_name,
							   _unique_identifier));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_hypothesis_name,
							   _unique_identifier));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarC(_hypothesis_name,
							   _unique_identifier));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarD(_hypothesis_name,
							   _unique_identifier));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarE(_hypothesis_name,
							   _unique_identifier));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarF(_hypothesis_name,
							   _unique_identifier));


      _configCheck = true;
      return true;
    } catch (Exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
    } catch (std::exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    }
    return false;
  }

  std::string ReduceCppGlobalPID::process(std::string document) {
    Json::FastWriter writer;

    if (document.empty()) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Empty document passed to process";
      errors["bad_json_document"] = ss.str();
      _root["errors"] = errors;
      return writer.write(_root);
    }

    if (!_configCheck) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: process was not passed a valid configuration";
      errors["bad_json_document"] = ss.str();
      _root["errors"] = errors;
      return writer.write(_root);
    }

    JsonCppSpillConverter json2cppconverter;
    Json::Value *data_json = NULL;
    MAUS::Data *data_cpp = NULL;

    // Read string and convert to a Json object
    try {
      Json::Value imported_json = JsonWrapper::StringToJson(document);
      data_json = new Json::Value(imported_json);
    } catch (Exception& exception) {
      MAUS::CppErrorHandler::getInstance()->
	HandleExceptionNoJson(exception, _classname);
      std::cerr << "String to Json conversion failed,"
		<< "ReduceCppGlobalPID::process" << std::endl;
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Bad json document";
      errors["bad_json_document"] = ss.str();
      _root["errors"] = errors;
      delete data_json;
      return writer.write(_root);
    } catch (std::exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
      std::cerr << "String to Json conversion failed,"
		<< "ReduceCppGlobalPID::process" << std::endl;
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Bad json document";
      errors["bad_json_document"] = ss.str();
      _root["errors"] = errors;
      delete data_json;
      return writer.write(_root);
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
    } catch (...) {
      Squeak::mout(Squeak::error) << "Missing required branch daq_event_type"
          << "converting json->cpp, ReduceCppGlobalPID" << std::endl;
    }

    if (!data_cpp) {
      return std::string("{\"errors\":{\"failed_json_cpp_conversion\":")+
	std::string("\"Failed to convert Json to Cpp Spill object\"}}");
    }

    _spill = data_cpp->GetSpill();

    if (_spill) {
      if ( _spill->GetReconEvents() ) {
        for ( unsigned int event_i = 0;
              event_i < _spill->GetReconEvents()->size(); ++event_i) {
	  if (_spill->GetReconEvents()->at(event_i)->GetGlobalEvent()) {
	    MAUS::GlobalEvent* global_event =
	      _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
	    std::vector<MAUS::DataStructure::Global::Track*> *GlobalTrackArray =
	      global_event->get_tracks();
	    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
		 ++track_i) {
	      MAUS::DataStructure::Global::Track* track =
		GlobalTrackArray->at(track_i);
	      if (track->get_mapper_name() != "MapCppGlobalTrackMatching") continue;
	      for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
		   ++pid_var_count) {
		_pid_vars[pid_var_count]->Fill_Hist(track);
	      }
	    }
	  }
        }
      }
    } else {
      Squeak::mout(Squeak::error) << "Failed to import json to spill\n";
    }

    std::string output_document = JsonWrapper::JsonToString(*data_json);
    delete data_json;
    delete data_cpp;
    return output_document;
  }

  bool ReduceCppGlobalPID::death()  {
    if (!_configCheck) {
      return true;
    } else {
      for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
	   ++pid_var_count) {
	delete _pid_vars[pid_var_count];
      }
      return true;
    }
  }
} // ~namespace MAUS
