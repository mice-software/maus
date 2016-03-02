/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"


namespace MAUS {

  PyMODINIT_FUNC init_MapCppGlobalPID(void) {
    PyWrapMapBase<MAUS::MapCppGlobalPID>::PyWrapMapBaseModInit
                                            ("MapCppGlobalPID", "", "", "", "");
  }


  MapCppGlobalPID::MapCppGlobalPID()
     : MapBase<Data>("MapCppGlobalPID"), _configCheck(false) {
  }

  MapCppGlobalPID::~MapCppGlobalPID() {
  }

  void MapCppGlobalPID::_birth(const std::string& argJsonConfigDocument) {
    // Check if the JSON document can be parsed, else return error only.
    _configCheck = false;
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
      throw MAUS::Exception(Exception::recoverable,
	                          "Failed to parse Json configuration file",
                            "MapCppGlobalPID::_birth");
    }

    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    if (!pMAUS_ROOT_DIR) {
      throw MAUS::Exception(Exception::recoverable,
	          std::string("Could not find the $MAUS_ROOT_DIR env variable. ")+\
            std::string("Did you try running: source env.sh?"),
            "MapCppGlobalPID::_birth");
    }


    _hypotheses.clear();
    _pid_vars.clear();

    PDF_file = _configJSON["PID_PDFs_file"].asString();

    _histFile = new TFile(PDF_file.c_str(), "READ");

    Json::Value pid_bounds = _configJSON["pid_bounds"];

    _minA = pid_bounds["minA"].asDouble();
    _maxA = pid_bounds["maxA"].asDouble();
    _XminB = pid_bounds["XminB"].asDouble();
    _XmaxB = pid_bounds["XmaxB"].asDouble();
    _YminB = pid_bounds["YminB"].asDouble();
    _YmaxB = pid_bounds["YmaxB"].asDouble();
    _XminC = pid_bounds["XminC"].asDouble();
    _XmaxC = pid_bounds["XmaxC"].asDouble();
    _YminC = pid_bounds["YminC"].asDouble();
    _YmaxC = pid_bounds["YmaxC"].asDouble();
    _minD = pid_bounds["minD"].asDouble();
    _maxD = pid_bounds["maxD"].asDouble();
    _minE = pid_bounds["minE"].asDouble();
    _maxE = pid_bounds["maxE"].asDouble();
    _XminF = pid_bounds["XminF"].asDouble();
    _XmaxF = pid_bounds["XmaxF"].asDouble();
    _YminF = pid_bounds["YminF"].asDouble();
    _YmaxF = pid_bounds["YmaxF"].asDouble();
    _minG = pid_bounds["minG"].asDouble();
    _maxG = pid_bounds["maxG"].asDouble();
    _XminH = pid_bounds["XminH"].asDouble();
    _XmaxH = pid_bounds["XmaxH"].asDouble();
    _YminH = pid_bounds["YminH"].asDouble();
    _YmaxH = pid_bounds["YmaxH"].asDouble();
    _XminI = pid_bounds["XminI"].asDouble();
    _XmaxI = pid_bounds["XmaxI"].asDouble();
    _YminI = pid_bounds["YminI"].asDouble();
    _YmaxI = pid_bounds["YmaxI"].asDouble();
    _XminJ = pid_bounds["XminJ"].asDouble();
    _XmaxJ = pid_bounds["XmaxJ"].asDouble();
    _YminJ = pid_bounds["YminJ"].asDouble();
    _YmaxJ = pid_bounds["YmaxJ"].asDouble();
    _minComA = pid_bounds["minComA"].asDouble();
    _maxComA = pid_bounds["maxComA"].asDouble();
    _XminComB = pid_bounds["XminComB"].asDouble();
    _XmaxComB = pid_bounds["XmaxComB"].asDouble();
    _YminComB = pid_bounds["YminComB"].asDouble();
    _YmaxComB = pid_bounds["YmaxComB"].asDouble();
    _minComC = pid_bounds["minComC"].asDouble();
    _maxComC = pid_bounds["maxComC"].asDouble();
    _minComD = pid_bounds["minComD"].asDouble();
    _maxComD = pid_bounds["maxComD"].asDouble();
    _XminComE = pid_bounds["XminComE"].asDouble();
    _XmaxComE = pid_bounds["XmaxComE"].asDouble();
    _YminComE = pid_bounds["YminComE"].asDouble();
    _YmaxComE = pid_bounds["YmaxComE"].asDouble();
    _minComF = pid_bounds["minComF"].asDouble();
    _maxComF = pid_bounds["maxComF"].asDouble();
    _XminComG = pid_bounds["XminComG"].asDouble();
    _XmaxComG = pid_bounds["XmaxComG"].asDouble();
    _YminComG = pid_bounds["YminComG"].asDouble();
    _YmaxComG = pid_bounds["YmaxComG"].asDouble();
    _XminComH = pid_bounds["XminComH"].asDouble();
    _XmaxComH = pid_bounds["XmaxComH"].asDouble();
    _YminComH = pid_bounds["YminComH"].asDouble();
    _YmaxComH = pid_bounds["YmaxComH"].asDouble();
    _XminComI = pid_bounds["XminComI"].asDouble();
    _XmaxComI = pid_bounds["XmaxComI"].asDouble();
    _YminComI = pid_bounds["YminComI"].asDouble();
    _YmaxComI = pid_bounds["YmaxComI"].asDouble();

    _pid_config = _configJSON["pid_config"].asString();
    _pid_mode = _configJSON["pid_mode"].asString();
    _custom_pid_set = _configJSON["custom_pid_set"].asString();
    _pid_confidence_level = _configJSON["pid_confidence_level"].asInt();
    _pid_beamline_polarity = _configJSON["pid_beamline_polarity"].asString();
    _pid_beam_setting = _configJSON["pid_beam_setting"].asString();
    _pid_track_selection = _configJSON["pid_track_selection"].asString();

    // vector of hypotheses
    if (_pid_beamline_polarity == "positive") {
    	_hypotheses.push_back((_pid_beam_setting + "_mu_plus"));
    	_hypotheses.push_back((_pid_beam_setting + "_e_plus"));
    	_hypotheses.push_back((_pid_beam_setting + "_pi_plus"));
    } else if (_pid_beamline_polarity == "negative") {
    	_hypotheses.push_back((_pid_beam_setting + "_mu_minus"));
    	_hypotheses.push_back((_pid_beam_setting + "_e_minus"));
    	_hypotheses.push_back((_pid_beam_setting + "_pi_minus"));
    } else {
	    Squeak::mout(Squeak::warning) << "Invalid pid_beamline_polarity "
				  << "set in ConfigurationDefaults, "
				  << "MapCppGlobalPID::_birth" << std::endl;
	  }

    if (_pid_config == "step_4") {
      if (_pid_mode == "online") {
	for (unsigned int i =0; i < _hypotheses.size(); ++i) {
	  // vector of pid vars
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_histFile,
							       _hypotheses[i],
							       _XminB, _XmaxB,
							       _YminB, _YmaxB));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarC(_histFile,
							       _hypotheses[i],
							       _XminC, _XmaxC,
							       _YminC, _YmaxC));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarF(_histFile,
							       _hypotheses[i],
							       _XminF, _XmaxF,
							       _YminF, _YmaxF));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarH(_histFile,
							       _hypotheses[i],
							       _XminH, _XmaxH,
							       _YminH, _YmaxH));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarI(_histFile,
							       _hypotheses[i],
							       _XminI, _XmaxI,
							       _YminI, _YmaxI));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarJ(_histFile,
							       _hypotheses[i],
							       _XminJ, _XmaxJ,
							       _YminJ, _YmaxJ));
	}
      } else if (_pid_mode == "offline") {
	for (unsigned int i =0; i < _hypotheses.size(); ++i) {
	  // vector of pid vars
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_histFile,
							       _hypotheses[i],
							       _minA, _maxA));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_histFile,
							       _hypotheses[i],
							       _XminB, _XmaxB,
							       _YminB, _YmaxB));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarC(_histFile,
							       _hypotheses[i],
							       _XminC, _XmaxC,
							       _YminC, _YmaxC));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarD(_histFile,
							       _hypotheses[i],
							       _minD, _maxD));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarE(_histFile,
							       _hypotheses[i],
							       _minE, _maxE));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarF(_histFile,
							       _hypotheses[i],
							       _XminF, _XmaxF,
							       _YminF, _YmaxF));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarG(_histFile,
							       _hypotheses[i],
							       _minG, _maxG));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarH(_histFile,
							       _hypotheses[i],
							       _XminH, _XmaxH,
							       _YminH, _YmaxH));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarI(_histFile,
							       _hypotheses[i],
							       _XminI, _XmaxI,
							       _YminI, _YmaxI));
	  _pid_vars.push_back(new MAUS::recon::global::PIDVarJ(_histFile,
							       _hypotheses[i],
							       _XminJ, _XmaxJ,
							       _YminJ, _YmaxJ));
	}
      } else if (_pid_mode == "custom") {
	std::istringstream ss(_custom_pid_set);
	std::string token;
	std::vector<std::string> input_pid_vars;
	while(std::getline(ss, token, ' ')) {
	  input_pid_vars.push_back(token);
	}
	if (input_pid_vars.size() == 0) {
	  Squeak::mout(Squeak::warning) << "No PID variables in custom_pid_set,"
					<< " MapCppGlobalPID::birth" << std::endl;
	} else {
	  for (unsigned int i =0; i < _hypotheses.size(); ++i) {
	    // vector of pid vars
	    if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
			  "PIDVarA") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_histFile,
								   _hypotheses[i],
								   _minA, _maxA));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarB") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_histFile,
								   _hypotheses[i],
								   _XminB, _XmaxB,
								   _YminB, _YmaxB));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarC") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarC(_histFile,
								   _hypotheses[i],
								   _XminC, _XmaxC,
								   _YminC, _YmaxC));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarD") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarD(_histFile,
								   _hypotheses[i],
								   _minD, _maxD));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarE") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarE(_histFile,
								   _hypotheses[i],
								   _minE, _maxE));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarF") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarF(_histFile,
								   _hypotheses[i],
								   _XminF, _XmaxF,
								   _YminF, _YmaxF));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarG") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarG(_histFile,
								   _hypotheses[i],
								   _minG, _maxG));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarH") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarH(_histFile,
								   _hypotheses[i],
								   _XminH, _XmaxH,
								   _YminH, _YmaxH));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarI") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarI(_histFile,
								   _hypotheses[i],
								   _XminI, _XmaxI,
								   _YminI, _YmaxI));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				 "PIDVarJ") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::PIDVarJ(_histFile,
								   _hypotheses[i],
								   _XminJ, _XmaxJ,
								   _YminJ, _YmaxJ));
	    } else {
	      Squeak::mout(Squeak::warning) << "No valid PID variables given in "
					    << "custom_pid_set, "
					    << "MapCppGlobalPID::birth" << std::endl;
	    }
	  }
	}
      } else {
	Squeak::mout(Squeak::warning) << "Invalid pid_mode, "
				    << " MapCppGlobalPID::birth" << std::endl;
      }
    } else if (_pid_config == "commissioning") {
      if (_pid_mode == "online") {
	for (unsigned int i =0; i < _hypotheses.size(); ++i) {
	  // vector of pid vars
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarB(_histFile,
								  _hypotheses[i],
								  _XminComB, _XmaxComB,
								  _YminComB, _YmaxComB));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarE(_histFile,
								  _hypotheses[i],
								  _XminComE, _XmaxComE,
								  _YminComE, _YmaxComE));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarG(_histFile,
								  _hypotheses[i],
								  _XminComG, _XmaxComG,
								  _YminComG, _YmaxComG));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarH(_histFile,
								  _hypotheses[i],
								  _XminComH, _XmaxComH,
								  _YminComH, _YmaxComH));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarI(_histFile,
								  _hypotheses[i],
								  _XminComI, _XmaxComI,
								  _YminComI, _YmaxComI));
	}
      } else if (_pid_mode == "offline") {
	for (unsigned int i =0; i < _hypotheses.size(); ++i) {
	  // vector of pid vars
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarA(_histFile,
							       _hypotheses[i],
							       _minComA, _maxComA));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarB(_histFile,
							       _hypotheses[i],
							       _XminComB, _XmaxComB,
							       _YminComB, _YmaxComB));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarC(_histFile,
							       _hypotheses[i],
							       _minComC, _maxComC));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarD(_histFile,
							       _hypotheses[i],
							       _minComD, _maxComD));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarF(_histFile,
							       _hypotheses[i],
							       _minComF, _maxComF));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarE(_histFile,
							       _hypotheses[i],
							       _XminComE, _XmaxComE,
							       _YminComE, _YmaxComE));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarG(_histFile,
							       _hypotheses[i],
							       _XminComG, _XmaxComG,
							       _YminComG, _YmaxComG));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarH(_histFile,
								  _hypotheses[i],
								  _XminComH, _XmaxComH,
								  _YminComH, _YmaxComH));
	  _pid_vars.push_back(new MAUS::recon::global::ComPIDVarI(_histFile,
								  _hypotheses[i],
								  _XminComI, _XmaxComI,
								  _YminComI, _YmaxComI));
	}
      } else if (_pid_mode == "custom") {
	std::istringstream ss(_custom_pid_set);
	std::string token;
	std::vector<std::string> input_pid_vars;
	while(std::getline(ss, token, ' ')) {
	  input_pid_vars.push_back(token);
	}
	if (input_pid_vars.size() == 0) {
	  Squeak::mout(Squeak::warning) << "No PID variables in custom_pid_set,"
					<< " MapCppGlobalPID::birth" << std::endl;
	} else {
	  for (unsigned int i =0; i < _hypotheses.size(); ++i) {
	    // vector of pid vars
	    if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
			 "ComPIDVarA") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarA(_histFile,
								      _hypotheses[i],
								      _minComA, _maxComA));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarB") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarB(_histFile,
								      _hypotheses[i],
								      _XminComB, _XmaxComB,
								      _YminComB, _YmaxComB));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarC") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarC(_histFile,
								      _hypotheses[i],
								      _minComC, _maxComC));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarD") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarD(_histFile,
								      _hypotheses[i],
								      _minComD, _maxComD));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarE") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarE(_histFile,
								      _hypotheses[i],
								      _XminComE, _XmaxComE,
								      _YminComE, _YmaxComE));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarF") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarF(_histFile,
								      _hypotheses[i],
								      _minComF, _maxComF));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarG") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarG(_histFile,
								      _hypotheses[i],
								      _XminComG, _XmaxComG,
								      _YminComG, _YmaxComG));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarH") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarH(_histFile,
								      _hypotheses[i],
								      _XminComH, _XmaxComH,
								      _YminComH, _YmaxComH));
	    } else if (std::find(input_pid_vars.begin(), input_pid_vars.end(),
				"ComPIDVarI") != input_pid_vars.end()) {
	      _pid_vars.push_back(new MAUS::recon::global::ComPIDVarI(_histFile,
								      _hypotheses[i],
								      _XminComI, _XmaxComI,
								      _YminComI, _YmaxComI));
	    } else {
	      Squeak::mout(Squeak::warning) << "No valid PID variables given in "
					  << "custom_pid_set, "
					  << "MapCppGlobalPID::birth" << std::endl;
	    }
	  }
	}
      } else {
	Squeak::mout(Squeak::warning) << "Invalid pid_mode, "
				    << " MapCppGlobalPID::birth" << std::endl;
      }
    } else {
      Squeak::mout(Squeak::warning) << "Invalid pid_config, "
				    << " MapCppGlobalPID::birth" << std::endl;
    }

    _configCheck = true;
  }

  void MapCppGlobalPID::_death() {
    _pid_vars.clear();
  }

  void MapCppGlobalPID::_process(MAUS::Data* data) const {
    MAUS::Data* data_cpp = data;
    if (!data_cpp) {
      throw Exception(Exception::recoverable,
                      "Data was NULL",
                      "MapCppGlobalPID::process");
    }
    if (!_configCheck) {
      throw Exception(Exception::recoverable,
                      "Birth was not called successfully",
                      "MapCppGlobalPID::process");
    }

    const MAUS::Spill* _spill = data_cpp->GetSpill();

    if ( _spill->GetReconEvents() ) {
      for ( unsigned int event_i = 0;
	    event_i < _spill->GetReconEvents()->size(); ++event_i ) {
	if (_spill->GetReconEvents()->at(event_i)->GetGlobalEvent()) {
	  MAUS::GlobalEvent* global_event =
	    _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
	  std::vector<MAUS::DataStructure::Global::Track*> *GlobalTrackArray =
	    global_event->get_tracks();
	  std::istringstream track_selector(_pid_track_selection);
	  std::string token;
	  std::vector<std::string> track_types;
	  while (std::getline(track_selector, token, ' ')) {
	    track_types.push_back(token);
	  }
	  if (track_types.size() == 0) {
	    Squeak::mout(Squeak::warning) << "No tracks have been selected to be ID'd,"
					  << " MapCppGlobalPID::_process" << std::endl;
	  }
	  for (unsigned int q = 0; q < track_types.size(); ++q) {
	    if (track_types[q] == "all") {
	      US_PID(GlobalTrackArray, global_event);
	      DS_PID(GlobalTrackArray, global_event);
	      Through_US_PID(GlobalTrackArray, global_event);
	      Through_DS_PID(GlobalTrackArray, global_event);
	      Through_PID(GlobalTrackArray, global_event);
	    } else if (track_types[q] == "US") {
	      US_PID(GlobalTrackArray, global_event);
	    } else if (track_types[q] == "DS") {
	      DS_PID(GlobalTrackArray, global_event);
	    } else if (track_types[q] == "Through_US") {
	      Through_US_PID(GlobalTrackArray, global_event);
	    } else if (track_types[q] == "Through_DS") {
	      Through_DS_PID(GlobalTrackArray, global_event);
	    } else if (track_types[q] == "Through") {
	      Through_PID(GlobalTrackArray, global_event);
	    }
	  }
	}
      }
    }
  }

  double MapCppGlobalPID::ConfidenceLevel(double LL_x, double sum_L) const {
    return (exp(LL_x)/sum_L)*100;
  }

  void MapCppGlobalPID::US_PID(std::vector<MAUS::DataStructure::Global::Track*>*
			       GlobalTrackArray,
			       MAUS::GlobalEvent* global_event) const {

    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	 ++track_i) {
      MAUS::DataStructure::Global::Track* track =
  	GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching-US") {
	perform_pid("MapCppGlobalPID-Final_US",
		    "US_PID", track, _pid_vars,
		    _pid_beamline_polarity,
		    _pid_beam_setting, global_event);
      }
    }
  }

  void MapCppGlobalPID::DS_PID(std::vector<MAUS::DataStructure::Global::Track*>*
			       GlobalTrackArray,
			       MAUS::GlobalEvent* global_event) const {

    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	 ++track_i) {
      MAUS::DataStructure::Global::Track* track =
	GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching-DS") {
	perform_pid("MapCppGlobalPID-Final_DS",
		    "DS_PID", track, _pid_vars,
		    _pid_beamline_polarity,
		    _pid_beam_setting, global_event);
      }
    }
  }

  void MapCppGlobalPID::Through_US_PID(std::vector<MAUS::DataStructure::Global::Track*>*
				       GlobalTrackArray,
				       MAUS::GlobalEvent* global_event) const {

    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	 ++track_i) {
      MAUS::DataStructure::Global::Track* track =
  	GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	std::vector<const MAUS::DataStructure::Global::Track*> const_tracks =
	  track->GetConstituentTracks();
	for (unsigned int track_j = 0; track_j < const_tracks.size();
	     ++track_j) {
	  if (const_tracks.at(track_j)->get_mapper_name() == "MapCppGlobalTrackMatching-US") {
	    MAUS::DataStructure::Global::Track* US_track =
	      const_cast<MAUS::DataStructure::Global::Track*>
	      (const_tracks.at(track_j));
	    perform_pid("MapCppGlobalPID-Final_Through_US",
			"Through_DS_PID", US_track, _pid_vars,
			_pid_beamline_polarity,
			_pid_beam_setting, global_event);
	  }
	}
      }
    }
  }

  void MapCppGlobalPID::Through_DS_PID(std::vector<MAUS::DataStructure::Global::Track*>*
				       GlobalTrackArray,
				       MAUS::GlobalEvent* global_event) const {

    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	 ++track_i) {
      MAUS::DataStructure::Global::Track* track =
  	GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	std::vector<const MAUS::DataStructure::Global::Track*> const_tracks =
	  track->GetConstituentTracks();
	for (unsigned int track_j = 0; track_j < const_tracks.size();
	     ++track_j) {
	  if (const_tracks.at(track_j)->get_mapper_name() == "MapCppGlobalTrackMatching-DS") {
	    MAUS::DataStructure::Global::Track* DS_track =
	      const_cast<MAUS::DataStructure::Global::Track*>
	      (const_tracks.at(track_j));
	    perform_pid("MapCppGlobalPID-Final_Through_DS",
			"Through_DS_PID", DS_track, _pid_vars,
			_pid_beamline_polarity,
			_pid_beam_setting, global_event);
	  }
	}
      }
    }
  }

  void MapCppGlobalPID::Through_PID(std::vector<MAUS::DataStructure::Global::Track*>*
				    GlobalTrackArray,
				    MAUS::GlobalEvent* global_event) const {

    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	 ++track_i) {
      MAUS::DataStructure::Global::Track* track =
	GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	perform_pid("MapCppGlobalPID-Final_Through",
		    "Through_PID", track, _pid_vars,
		    _pid_beamline_polarity,
		    _pid_beam_setting, global_event);
      }
    }
  }

  void MapCppGlobalPID::perform_pid(std::string output_track_name,
				    std::string function_name,
				    MAUS::DataStructure::Global::Track* track,
				    std::vector<MAUS::recon::global::PIDBase*> _pid_vars,
				    std::string _pid_beamline_polarity,
				    std::string _pid_beam_setting,
				    MAUS::GlobalEvent* global_event) const {

    MAUS::DataStructure::Global::Track* pidtrack = track->Clone();
    global_event->add_track_recursive(pidtrack);
    std::string candidate_name = "MapCppGlobalPID-Candidate_" + function_name;
    pidtrack->set_mapper_name(candidate_name);
    pidtrack->set_pid(MAUS::DataStructure::Global::kNoPID);
    // doubles to hold cumulative log likelihoods for each hypothesis
    double logL_mu = 0;
    double logL_e = 0;
    double logL_pi = 0;
    /// Bools to check if log likelihood for PID variable returned an
    /// "error value" logL, 1 for an unsuitable track, 2 for a track
    /// that fails the PID due to a user defined cut on the variable
    /// value. Only used for the purposes of efficiency calculations,
    /// does not affect PID performance
    bool logL_1 = false;
    bool logL_2 = false;
    // std::cerr << "_pid_vars.size() : " << _pid_vars.size() << std::endl;
    for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
	 ++pid_var_count) {
      // TODO Pidcott: Need to do this separately for +ve and -ve
      // particles for now. Figure out a better way to organise this.
      if (_pid_beamline_polarity == "positive") {
	logL_1 = false;
	logL_2 = false;
	std::string hyp = _pid_vars[pid_var_count]->Get_hyp();
	if (hyp == (_pid_beam_setting + "_mu_plus")) {
	  if (_pid_vars[pid_var_count]->logL(track) == 1) {
	    logL_1 = true;
	    continue;
	  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
	    logL_2 = true;
	    continue;
	  } else {
	    logL_mu += _pid_vars[pid_var_count]->logL(track);
	    logL_1 = false;
	    logL_2 = false;
	  }
	} else if (hyp == (_pid_beam_setting + "_e_plus")) {
	  if (_pid_vars[pid_var_count]->logL(track) == 1) {
	    logL_1 = true;
	    continue;
	  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
	    logL_2 = true;
	    continue;
	  } else {
	    logL_e += _pid_vars[pid_var_count]->logL(track);
	    logL_1 = false;
	    logL_2 = false;
	  }
	} else if (hyp == (_pid_beam_setting + "_pi_plus")) {
	  if (_pid_vars[pid_var_count]->logL(track) == 1) {
	    logL_1 = true;
	    continue;
	  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
	    logL_2 = true;
	    continue;
	  } else {
	    logL_pi += _pid_vars[pid_var_count]->logL(track);
	    logL_1 = false;
	    logL_2 = false;
	  }
	} else {
	  Squeak::mout(Squeak::debug) << "Unrecognised particle hypothesis,"
				      << " MapCppGlobalPID::perform_pid" << std::endl;
	}
      } else if (_pid_beamline_polarity == "negative") {
	logL_1 = false;
	logL_2 = false;
	std::string hyp = _pid_vars[pid_var_count]->Get_hyp();
	if (hyp == (_pid_beam_setting + "_mu_minus")) {
	  if (_pid_vars[pid_var_count]->logL(track) == 1) {
	    logL_1 = true;
	    continue;
	  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
	    logL_2 = true;
	    continue;
	  } else {
	    logL_mu += _pid_vars[pid_var_count]->logL(track);
	    logL_1 = false;
	    logL_2 = false;
	  }
	} else if (hyp == (_pid_beam_setting + "_e_minus")) {
	  if (_pid_vars[pid_var_count]->logL(track) == 1) {
	    logL_1 = true;
	    continue;
	  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
	    logL_2 = true;
	    continue;
	  } else {
	    logL_e += _pid_vars[pid_var_count]->logL(track);
	    logL_1 = false;
	    logL_2 = false;
	  }
	} else if (hyp == (_pid_beam_setting + "_pi_minus")) {
	  if (_pid_vars[pid_var_count]->logL(track) == 1) {
	    logL_1 = true;
	    continue;
	  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
	    logL_2 = true;
	    continue;
	  } else {
	    logL_pi += _pid_vars[pid_var_count]->logL(track);
	    logL_1 = false;
	    logL_2 = false;
	  }
	} else {
	  Squeak::mout(Squeak::debug) << "Unrecognised particle hypothesis,"
				      << " MapCppGlobalPID::perform_pid" << std::endl;
	}
      }
    }
    if ((logL_mu == 0 || logL_pi == 0 ||
	 logL_e == 0) && logL_1 == true) {
      logL_mu = 1;
      logL_pi = 1;
      logL_e = 1;
    } else if ((logL_mu == 0 || logL_pi == 0 ||
		logL_e == 0) && logL_2 == true) {
      logL_mu = 2;
      logL_pi = 2;
      logL_e = 2;
    }
    // make pid_ll_values
    if (_pid_beamline_polarity == "positive") {
      MAUS::DataStructure::Global::PIDLogLPair mu_LL(-13, logL_mu);
      MAUS::DataStructure::Global::PIDLogLPair e_LL(-11, logL_e);
      MAUS::DataStructure::Global::PIDLogLPair pi_LL(211, logL_pi);
      pidtrack->AddPIDLogLValues(mu_LL);
      pidtrack->AddPIDLogLValues(pi_LL);
      pidtrack->AddPIDLogLValues(e_LL);
    } else if (_pid_beamline_polarity == "negative") {
      MAUS::DataStructure::Global::PIDLogLPair mu_LL(13, logL_mu);
      MAUS::DataStructure::Global::PIDLogLPair e_LL(11, logL_e);
      MAUS::DataStructure::Global::PIDLogLPair pi_LL(-211, logL_pi);
      pidtrack->AddPIDLogLValues(mu_LL);
      pidtrack->AddPIDLogLValues(pi_LL);
      pidtrack->AddPIDLogLValues(e_LL);
    }
    /*std::vector<MAUS::DataStructure::Global::PIDLogLPair>
      pid_ll_values = pidtrack->get_pid_logL_values();
      for (std::vector<MAUS::DataStructure::Global::PIDLogLPair>::const_iterator
      i = pid_ll_values.begin(); i != pid_ll_values.end(); ++i) {
      std::cerr << i->first << "\t" << i->second << std::endl;
      }*/
    if (logL_mu < 0 || logL_pi < 0 || logL_e < 0) {
      // calculate CLs
      double sum_exp_LLs = exp(logL_mu) + exp(logL_e) + exp(logL_pi);
      // std::cerr << "sum exp LLs: " << sum_exp_LLs << std::endl;
      double CL_mu = ConfidenceLevel(logL_mu, sum_exp_LLs);
      double CL_e = ConfidenceLevel(logL_e, sum_exp_LLs);
      double CL_pi = ConfidenceLevel(logL_pi, sum_exp_LLs);
      // std::cerr << "CL_mu " << CL_mu << std::endl;
      // std::cerr << "CL_e " << CL_e << std::endl;
      // std::cerr << "CL_pi " << CL_pi << std::endl;
      // compare CLs and select winning hypothesis. set g.o.f. of track to CL
      if (CL_mu - CL_e > _pid_confidence_level &&
	  CL_mu - CL_pi > _pid_confidence_level) {
	if (_pid_beamline_polarity == "positive") {
	  pidtrack->set_pid(MAUS::DataStructure::Global::kMuPlus);
	  pidtrack->set_goodness_of_fit(CL_mu);
	  if (track->get_pid() == MAUS::DataStructure::Global::kMuPlus) {
	    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
	    global_event->add_track_recursive(final_pidtrack);
	    final_pidtrack->set_mapper_name(output_track_name);
	  }
	} else if (_pid_beamline_polarity == "negative") {
	  pidtrack->set_pid(MAUS::DataStructure::Global::kMuMinus);
	  pidtrack->set_goodness_of_fit(CL_mu);
	  if (track->get_pid() == MAUS::DataStructure::Global::kMuMinus) {
	    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
	    global_event->add_track_recursive(final_pidtrack);
	    final_pidtrack->set_mapper_name(output_track_name);
	  }
	}
      } else if (CL_pi - CL_e > _pid_confidence_level &&
		 CL_pi - CL_mu > _pid_confidence_level) {
	if (_pid_beamline_polarity == "positive") {
	  pidtrack->set_pid(MAUS::DataStructure::Global::kPiPlus);
	  pidtrack->set_goodness_of_fit(CL_pi);
	  if (track->get_pid() == MAUS::DataStructure::Global::kPiPlus) {
	    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
	    global_event->add_track_recursive(final_pidtrack);
	    final_pidtrack->set_mapper_name(output_track_name);
	  }
	} else if (_pid_beamline_polarity == "negative") {
	  pidtrack->set_pid(MAUS::DataStructure::Global::kPiMinus);
	  pidtrack->set_goodness_of_fit(CL_pi);
	  if (track->get_pid() == MAUS::DataStructure::Global::kPiMinus) {
	    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
	    global_event->add_track_recursive(final_pidtrack);
	    final_pidtrack->set_mapper_name(output_track_name);
	  }
	}
      } else if (CL_e - CL_mu > _pid_confidence_level &&
		 CL_e - CL_pi > _pid_confidence_level) {
	if (_pid_beamline_polarity == "positive") {
	  pidtrack->set_pid(MAUS::DataStructure::Global::kEPlus);
	  pidtrack->set_goodness_of_fit(CL_e);
	  if (track->get_pid() == MAUS::DataStructure::Global::kEPlus) {
	    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
	    global_event->add_track_recursive(final_pidtrack);
	    final_pidtrack->set_mapper_name(output_track_name);
	  }
	} else if (_pid_beamline_polarity == "negative") {
	  pidtrack->set_pid(MAUS::DataStructure::Global::kEMinus);
	  pidtrack->set_goodness_of_fit(CL_e);
	  if (track->get_pid() == MAUS::DataStructure::Global::kEMinus) {
	    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
	    global_event->add_track_recursive(final_pidtrack);
	    final_pidtrack->set_mapper_name(output_track_name);
	  }
	}
      } else {
	Squeak::mout(Squeak::debug) << "PID for track could not be" <<
	  " determined." << std::endl;
      }
    }
  }
} // ~MAUS
