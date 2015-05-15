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

    _minA = _configJSON["minA"].asInt();
    _maxA = _configJSON["maxA"].asInt();
    _XminB = _configJSON["XminB"].asInt();
    _XmaxB = _configJSON["XmaxB"].asInt();
    _YminB = _configJSON["YminB"].asInt();
    _YmaxB = _configJSON["YmaxB"].asInt();
    _XminC = _configJSON["XminC"].asInt();
    _XmaxC = _configJSON["XmaxC"].asInt();
    _YminC = _configJSON["YminC"].asInt();
    _YmaxC = _configJSON["YmaxC"].asInt();
    _minD = _configJSON["minD"].asInt();
    _maxD = _configJSON["maxD"].asInt();
    _minE = _configJSON["minE"].asInt();
    _maxE = _configJSON["maxE"].asInt();
    _minComA = _configJSON["minComA"].asInt();
    _maxComA = _configJSON["maxComA"].asInt();

    // vector of hypotheses
    // TODO(Pidcott) find a more elegant way of accessing hypotheses
    _hypotheses.push_back("200MeV_mu_plus");
    _hypotheses.push_back("200MeV_e_plus");
    _hypotheses.push_back("200MeV_pi_plus");

    for (unsigned int i =0; i < _hypotheses.size(); ++i) {
      // vector of pid vars
      _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_histFile,
        						   _hypotheses[i], _minA, _maxA));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_histFile,
      _hypotheses[i], _XminB, _XmaxB, _YminB, _YmaxB));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarC(_histFile,
                                                           _hypotheses[i], _XminC, _XmaxC, _YminC, _YmaxC));
      // etc.
      }
    _configCheck = true;
  }

  void MapCppGlobalPID::_death() {
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
	    if (track->get_mapper_name() != "MapCppGlobalTrackMatching-US" ||
		track->get_mapper_name() != "MapCppGlobalTrackMatching-DS")
	      continue;
	    //int recon_track_pid = track->get_pid();
	    MAUS::DataStructure::Global::Track* pidtrack = track->Clone();
	    global_event->add_track_recursive(pidtrack);
	    pidtrack->set_mapper_name("MapCppGlobalPID-Candidate");
	    pidtrack->set_pid(MAUS::DataStructure::Global::kNoPID);
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
		Squeak::mout(Squeak::debug) << "Unrecognised particle hypothesis,"
					    << " MapCppGlobalPID::process" << std::endl;
	      }
	    }
	    //make pid_ll_values
	    MAUS::DataStructure::Global::PIDLogLPair mu_plus_LL(-13,logL_200MeV_mu_plus);
	    MAUS::DataStructure::Global::PIDLogLPair e_plus_LL(-11,logL_200MeV_e_plus);
	    MAUS::DataStructure::Global::PIDLogLPair pi_plus_LL(-211,logL_200MeV_pi_plus);
	    pidtrack->AddPIDLogLValues(mu_plus_LL);
	    pidtrack->AddPIDLogLValues(pi_plus_LL);
	    pidtrack->AddPIDLogLValues(e_plus_LL);
	    std::vector<MAUS::DataStructure::Global::PIDLogLPair> pid_ll_values = pidtrack->get_pid_logL_values();
	    for( std::vector<MAUS::DataStructure::Global::PIDLogLPair>::const_iterator i = pid_ll_values.begin(); i != pid_ll_values.end(); ++i) {
	      std::cerr << i->first << "\t" << i->second << std::endl;
	      }
	    //calculate CLs
	    double sum_exp_LLs = exp(logL_200MeV_mu_plus) + exp(logL_200MeV_e_plus) + exp(logL_200MeV_pi_plus);
	    std::cerr << "sum exp LLs: " << sum_exp_LLs << std::endl;
	    double CL_mu_plus = ConfidenceLevel(logL_200MeV_mu_plus, sum_exp_LLs);
	    double CL_e_plus = ConfidenceLevel(logL_200MeV_e_plus, sum_exp_LLs);
	    double CL_pi_plus = ConfidenceLevel(logL_200MeV_pi_plus, sum_exp_LLs);
	    std::cerr << "CL_mu " << CL_mu_plus << std::endl;
	    std::cerr << "CL_e " << CL_e_plus << std::endl;
	    std::cerr << "CL_pi " << CL_pi_plus << std::endl;
	    //compare CLs and select winning hypothesis. set g.o.f. of track to CL
	    if (CL_mu_plus - CL_e_plus > 5 && CL_mu_plus - CL_pi_plus > 5) {
	      pidtrack->set_pid(MAUS::DataStructure::Global::kMuPlus);
	      pidtrack->set_goodness_of_fit(CL_mu_plus);
	      if (track->get_pid() == MAUS::DataStructure::Global::kMuPlus) {
		MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
		global_event->add_track_recursive(final_pidtrack);
		final_pidtrack->set_mapper_name("MapCppGlobalPID-Final");
	      }
	    } else if (CL_pi_plus - CL_e_plus > 5 && CL_pi_plus - CL_mu_plus > 5) {
	      pidtrack->set_pid(MAUS::DataStructure::Global::kPiPlus);
	      pidtrack->set_goodness_of_fit(CL_pi_plus);
	      if (track->get_pid() == MAUS::DataStructure::Global::kPiPlus) {
		MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
		global_event->add_track_recursive(final_pidtrack);
		final_pidtrack->set_mapper_name("MapCppGlobalPID-Final");
	      }
	    } else if (CL_e_plus - CL_mu_plus > 5 && CL_e_plus - CL_pi_plus > 5) {
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

  double MapCppGlobalPID::ConfidenceLevel(double LL_x, double sum_L) const
  {
    return (exp(LL_x)/sum_L)*100;
  }
} // ~MAUS
