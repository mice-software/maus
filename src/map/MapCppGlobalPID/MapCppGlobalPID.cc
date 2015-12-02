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

    _minA = _configJSON["minA"].asDouble();
    _maxA = _configJSON["maxA"].asDouble();
    _XminB = _configJSON["XminB"].asDouble();
    _XmaxB = _configJSON["XmaxB"].asDouble();
    _YminB = _configJSON["YminB"].asDouble();
    _YmaxB = _configJSON["YmaxB"].asDouble();
    _XminC = _configJSON["XminC"].asDouble();
    _XmaxC = _configJSON["XmaxC"].asDouble();
    _YminC = _configJSON["YminC"].asDouble();
    _YmaxC = _configJSON["YmaxC"].asDouble();
    _minD = _configJSON["minD"].asDouble();
    _maxD = _configJSON["maxD"].asDouble();
    _minE = _configJSON["minE"].asDouble();
    _maxE = _configJSON["maxE"].asDouble();
    _XminF = _configJSON["XminF"].asDouble();
    _XmaxF = _configJSON["XmaxF"].asDouble();
    _YminF = _configJSON["YminF"].asDouble();
    _YmaxF = _configJSON["YmaxF"].asDouble();
    _minComA = _configJSON["minComA"].asDouble();
    _maxComA = _configJSON["maxComA"].asDouble();
    _XminComB = _configJSON["XminComB"].asDouble();
    _XmaxComB = _configJSON["XmaxComB"].asDouble();
    _YminComB = _configJSON["YminComB"].asDouble();
    _YmaxComB = _configJSON["YmaxComB"].asDouble();
    _minComC = _configJSON["minComC"].asDouble();
    _maxComC = _configJSON["maxComC"].asDouble();
    _minComD = _configJSON["minComD"].asDouble();
    _maxComD = _configJSON["maxComD"].asDouble();
    _XminComE = _configJSON["XminComE"].asDouble();
    _XmaxComE = _configJSON["XmaxComE"].asDouble();
    _YminComE = _configJSON["YminComE"].asDouble();
    _YmaxComE = _configJSON["YmaxComE"].asDouble();
    _minComF = _configJSON["minComF"].asDouble();
    _maxComF = _configJSON["maxComF"].asDouble();
    _XminComG = _configJSON["XminComG"].asDouble();
    _XmaxComG = _configJSON["XmaxComG"].asDouble();
    _YminComG = _configJSON["YminComG"].asDouble();
    _YmaxComG = _configJSON["YmaxComG"].asDouble();

    _pid_config = _configJSON["pid_config"].asString();
    _pid_mode = _configJSON["pid_mode"].asString();
    _custom_pid_set = _configJSON["custom_pid_set"].asString();
    _pid_confidence_level = _configJSON["pid_confidence_level"].asInt();

    std::cerr << _pid_config << std::endl;
    std::cerr << _pid_mode << std::endl;
    std::cerr << _custom_pid_set << std::endl;

    // vector of hypotheses
    // TODO(Pidcott) find a more elegant way of accessing hypotheses
    _hypotheses.push_back("200MeV_mu_plus");
    _hypotheses.push_back("200MeV_e_plus");
    _hypotheses.push_back("200MeV_pi_plus");

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
	      std::cerr << "selected PIDVarB" << std::endl;
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
    for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
	   ++pid_var_count) {
	delete _pid_vars[pid_var_count];
      }
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
	  for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
	       ++track_i) {
	    MAUS::DataStructure::Global::Track* track =
	      GlobalTrackArray->at(track_i);
	    if (track->get_mapper_name() == "MapCppGlobalTrackMatching-US" ||
		track->get_mapper_name() == "MapCppGlobalTrackMatching-DS") {
	      // int recon_track_pid = track->get_pid();
	      MAUS::DataStructure::Global::Track* pidtrack = track->Clone();
	      global_event->add_track_recursive(pidtrack);
	      pidtrack->set_mapper_name("MapCppGlobalPID-Candidate");
	      pidtrack->set_pid(MAUS::DataStructure::Global::kNoPID);
	      // doubles to hold cumulative log likelihoods for each hypothesis
	      double logL_200MeV_mu_plus = 0;
	      double logL_200MeV_e_plus = 0;
	      double logL_200MeV_pi_plus = 0;
	      /// Bools to check if log likelihood for PID variable returned an
	      /// "error value" logL, 1 for an unsuitable track, 2 for a track
	      /// that fails the PID due to a user defined cut on the variable
	      /// value. Only used for the purposes of efficiency calculations,
	      /// does not affect PID performance
	      bool logL_1 = false;
	      bool logL_2 = false;
	      std::cerr << "_pid_vars.size() : " << _pid_vars.size() << std::endl;
	      for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
		   ++pid_var_count) {
		logL_1 = false;
		logL_2 = false;
		std::string hyp = _pid_vars[pid_var_count]->Get_hyp();
		if (hyp == "200MeV_mu_plus") {
		  if (_pid_vars[pid_var_count]->logL(track) == 1) {
		    logL_1 = true;
		    continue;
		  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
		    logL_2 = true;
		    continue;
		  } else {
		    logL_200MeV_mu_plus += _pid_vars[pid_var_count]->logL(track);
		    logL_1 = false;
		    logL_2 = false;
		  }
		} else if (hyp == "200MeV_e_plus") {
		  if (_pid_vars[pid_var_count]->logL(track) == 1) {
		    logL_1 = true;
		    continue;
		  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
		    logL_2 = true;
		    continue;
		  } else {
		    logL_200MeV_e_plus += _pid_vars[pid_var_count]->logL(track);
		    logL_1 = false;
		    logL_2 = false;
		  }
		} else if (hyp == "200MeV_pi_plus") {
		  if (_pid_vars[pid_var_count]->logL(track) == 1) {
		    logL_1 = true;
		    continue;
		  } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
		    logL_2 = true;
		    continue;
		  } else {
		    logL_200MeV_pi_plus += _pid_vars[pid_var_count]->logL(track);
		    logL_1 = false;
		    logL_2 = false;
		  }
		} else {
		  Squeak::mout(Squeak::debug) << "Unrecognised particle hypothesis,"
					      << " MapCppGlobalPID::process" << std::endl;
		}
	      }
	      if ((logL_200MeV_mu_plus == 0 || logL_200MeV_pi_plus == 0 ||
		   logL_200MeV_e_plus == 0) && logL_1 == true) {
		logL_200MeV_mu_plus = 1;
		logL_200MeV_pi_plus = 1;
		logL_200MeV_e_plus = 1;
	      } else if ((logL_200MeV_mu_plus == 0 || logL_200MeV_pi_plus == 0 ||
			  logL_200MeV_e_plus == 0) && logL_2 == true) {
		logL_200MeV_mu_plus = 2;
		logL_200MeV_pi_plus = 2;
		logL_200MeV_e_plus = 2;
	      }
	      // make pid_ll_values
	      MAUS::DataStructure::Global::PIDLogLPair mu_plus_LL(-13, logL_200MeV_mu_plus);
	      MAUS::DataStructure::Global::PIDLogLPair e_plus_LL(-11, logL_200MeV_e_plus);
	      MAUS::DataStructure::Global::PIDLogLPair pi_plus_LL(-211, logL_200MeV_pi_plus);
	      pidtrack->AddPIDLogLValues(mu_plus_LL);
	      pidtrack->AddPIDLogLValues(pi_plus_LL);
	      pidtrack->AddPIDLogLValues(e_plus_LL);
	      std::vector<MAUS::DataStructure::Global::PIDLogLPair>
		pid_ll_values = pidtrack->get_pid_logL_values();
	      for (std::vector<MAUS::DataStructure::Global::PIDLogLPair>::const_iterator
		     i = pid_ll_values.begin(); i != pid_ll_values.end(); ++i) {
		std::cerr << i->first << "\t" << i->second << std::endl;
	      }
	      if (logL_200MeV_mu_plus < 0 || logL_200MeV_pi_plus < 0 ||
		  logL_200MeV_e_plus < 0) {
		// calculate CLs
		double sum_exp_LLs = exp(logL_200MeV_mu_plus) +
		  exp(logL_200MeV_e_plus) + exp(logL_200MeV_pi_plus);
		std::cerr << "sum exp LLs: " << sum_exp_LLs << std::endl;
		double CL_mu_plus = ConfidenceLevel(logL_200MeV_mu_plus, sum_exp_LLs);
		double CL_e_plus = ConfidenceLevel(logL_200MeV_e_plus, sum_exp_LLs);
		double CL_pi_plus = ConfidenceLevel(logL_200MeV_pi_plus, sum_exp_LLs);
		std::cerr << "CL_mu " << CL_mu_plus << std::endl;
		std::cerr << "CL_e " << CL_e_plus << std::endl;
		std::cerr << "CL_pi " << CL_pi_plus << std::endl;
		// compare CLs and select winning hypothesis. set g.o.f. of track to CL
		if (CL_mu_plus - CL_e_plus > _pid_confidence_level &&
		    CL_mu_plus - CL_pi_plus > _pid_confidence_level) {
		  pidtrack->set_pid(MAUS::DataStructure::Global::kMuPlus);
		  pidtrack->set_goodness_of_fit(CL_mu_plus);
		  if (track->get_pid() == MAUS::DataStructure::Global::kMuPlus) {
		    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
		    global_event->add_track_recursive(final_pidtrack);
		    final_pidtrack->set_mapper_name("MapCppGlobalPID-Final");
		  }
		} else if (CL_pi_plus - CL_e_plus > _pid_confidence_level &&
			   CL_pi_plus - CL_mu_plus > _pid_confidence_level) {
		  pidtrack->set_pid(MAUS::DataStructure::Global::kPiPlus);
		  pidtrack->set_goodness_of_fit(CL_pi_plus);
		  if (track->get_pid() == MAUS::DataStructure::Global::kPiPlus) {
		    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
		    global_event->add_track_recursive(final_pidtrack);
		    final_pidtrack->set_mapper_name("MapCppGlobalPID-Final");
		  }
		} else if (CL_e_plus - CL_mu_plus > _pid_confidence_level &&
			   CL_e_plus - CL_pi_plus > _pid_confidence_level) {
		  pidtrack->set_pid(MAUS::DataStructure::Global::kEPlus);
		  pidtrack->set_goodness_of_fit(CL_e_plus);
		  if (track->get_pid() == MAUS::DataStructure::Global::kEPlus) {
		    MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
		    global_event->add_track_recursive(final_pidtrack);
		    final_pidtrack->set_mapper_name("MapCppGlobalPID-Final");
		  }
		} else {
		  Squeak::mout(Squeak::debug) << "PID for track could not be" <<
		    " determined." << std::endl;
		  continue;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  double MapCppGlobalPID::ConfidenceLevel(double LL_x, double sum_L) const {
    return (exp(LL_x)/sum_L)*100;
  }
} // ~MAUS
