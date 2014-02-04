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

#include "src/map/MapCppGlobalPID/MapCppGlobalPID.hh"

#include "Interface/Squeak.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"


namespace MAUS {
  MapCppGlobalPID::MapCppGlobalPID() {
    _classname = "MapCppGlobalPID";
  }

  bool MapCppGlobalPID::birth(std::string argJsonConfigDocument) {
    // Check if the JSON document can be parsed, else return error only.
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
      _configCheck = false;
      return false;
    }

    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    if (!pMAUS_ROOT_DIR) {
      Squeak::mout(Squeak::error)
	<< "Could not find the $MAUS_ROOT_DIR env variable." << std::endl;
      Squeak::mout(Squeak::error)
	<< "Did you try running: source env.sh ?" << std::endl;
      return false;
    }

    _configCheck = true;

     _hypotheses.clear();
    _pid_vars.clear();

    std::string filename = std::string(pMAUS_ROOT_DIR) + "/PIDhists.root";

    _histFile = new TFile(filename.c_str(), "READ");

    // vector of hypotheses
    // TODO(Pidcott) find a more elegant way of accessing hypotheses
    _hypotheses.push_back("200MeV_mu_plus");
    _hypotheses.push_back("200MeV_e_plus");
    _hypotheses.push_back("200MeV_pi_plus");

    for (unsigned int i =0; i < _hypotheses.size(); ++i) {
      // vector of pid vars
      _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_histFile,
							   _hypotheses[i]));
      // _pid_vars.push_back(new MAUS::recon::global::PIDVarB(histFile,
      //                                                      _hypotheses[i]));
      // etc.
      }

    return true;
  }

  bool MapCppGlobalPID::death() {
    return true;
  }

  std::string MapCppGlobalPID::process(std::string document) const {
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
      ss << _classname << " says: process was not passed a valid configuration";
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
    } catch(...) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Bad json document";
      errors["bad_json_document"] = ss.str();
      root["errors"] = errors;
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
    } catch(...) {
      Squeak::mout(Squeak::error) << "Missing required branch daq_event_type"
          << " converting json->cpp, MapCppGlobalPID" << std::endl;
    }

    if (!data_cpp) {
      return std::string("{\"errors\":{\"failed_json_cpp_conversion\":")+
	std::string("\"Failed to convert Json to Cpp Spill object\"}}");
    }

    const MAUS::Spill* _spill = data_cpp->GetSpill();

    if ( _spill->GetReconEvents() ) {
    std::cerr << "spill" << std::endl;
      for ( unsigned int event_i = 0;
	    event_i < _spill->GetReconEvents()->size(); ++event_i ) {
	    std::cerr << "going through recon events" << std::endl;
        MAUS::GlobalEvent* global_event =
	  _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
        std::vector<MAUS::DataStructure::Global::Track*> *GlobalTrackArray =
	  global_event->get_tracks();
	  std::cerr << "number of tracks in GE : " << GlobalTrackArray->size() << std::endl;
        for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	     ++track_i) {
          MAUS::DataStructure::Global::Track* track =
	    GlobalTrackArray->at(track_i);
	  if (track->get_mapper_name() != "MapCppGlobalReconImport") continue;
          // doubles to hold cumulative log likelihoods for each hypothesis
          double logL_200MeV_mu_plus = 0;
          double logL_200MeV_e_plus = 0;
          double logL_200MeV_pi_plus = 0;
          for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
	       ++pid_var_count) {
            std::string hyp = _pid_vars[pid_var_count]->Get_hyp();
            if (hyp == "200MeV_mu_plus") {
              if (_pid_vars[pid_var_count]->logL(track) == 1) {
                continue;
              } else {
                logL_200MeV_mu_plus += _pid_vars[pid_var_count]->logL(track);
              }
            } else if (hyp == "200MeV_e_plus") {
              if (_pid_vars[pid_var_count]->logL(track) == 1) {
                continue;
              } else {
                logL_200MeV_e_plus += _pid_vars[pid_var_count]->logL(track);
              }
            } else if (hyp == "200MeV_pi_plus") {
              if (_pid_vars[pid_var_count]->logL(track) == 1) {
                continue;
              } else {
                logL_200MeV_pi_plus += _pid_vars[pid_var_count]->logL(track);
              }
            } else {
              Squeak::mout(Squeak::error) << "Unrecognised particle hypothesis,"
	          << " MapCppGlobalPID::process" << std::endl;
	    }
          }
          if ((logL_200MeV_mu_plus - logL_200MeV_e_plus > 0.5) &&
              (logL_200MeV_mu_plus - logL_200MeV_pi_plus > 0.5)) {
            track->set_pid(MAUS::DataStructure::Global::kMuPlus);
          } else if ((logL_200MeV_e_plus - logL_200MeV_mu_plus > 0.5) &&
		     (logL_200MeV_e_plus - logL_200MeV_pi_plus > 0.5)) {
            track->set_pid(MAUS::DataStructure::Global::kEPlus);
          } else if ((logL_200MeV_pi_plus - logL_200MeV_mu_plus > 0.5) &&
		     (logL_200MeV_pi_plus - logL_200MeV_e_plus > 0.5)) {
            track->set_pid(MAUS::DataStructure::Global::kPiPlus);
          } else {
            Squeak::mout(Squeak::error) << "PID for track could not be" <<
	      " determined." << std::endl;
            continue;
          }
          Squeak::mout(Squeak::error) << "PID of track : " <<
	    track->get_pid() << std::endl;
        }
      }
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
    std::cerr << "end of process" << std::endl;
    return output_document;
  }
} // ~MAUS
