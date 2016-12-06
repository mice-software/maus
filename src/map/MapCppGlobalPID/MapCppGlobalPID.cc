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

#include "Utils/Squeak.hh"
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
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
          "Failed to parse Json configuration file", "MapCppGlobalPID::_birth");
    }

    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    if (!pMAUS_ROOT_DIR) {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
          std::string("Could not find the $MAUS_ROOT_DIR env variable. ")+\
          std::string("Did you try running: source env.sh?"),
          "MapCppGlobalPID::_birth");
    }

    _hypotheses.clear();
    _pid_vars.clear();
    // Get the name of the PDF file from the config file and open
    PDF_file = _configJSON["PID_PDFs_file"].asString();
    _histFile = new TFile(PDF_file.c_str(), "READ");

    // Get variable cut values used in PID from the config file
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

    // Get settings from config file that determine which PID variables are
    // used, which tracks to perform pid against, and what confidence level
    // cut to use.
    _pid_config = _configJSON["pid_config"].asString();
    _pid_mode = _configJSON["pid_mode"].asString();
    _custom_pid_set = _configJSON["custom_pid_set"].asString();
    _pid_confidence_level = _configJSON["pid_confidence_level"].asInt();
    _pid_beamline_polarity = _configJSON["pid_beamline_polarity"].asString();
    _pid_beam_setting = _configJSON["pid_beam_setting"].asString();
    _pid_track_selection = _configJSON["pid_track_selection"].asString();

    // vector of pid hypotheses
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

    // PIDVar selection if Step IV running is selected. Different sets of
    // variables are used depending on whether the user selects an offline
    // set, online set, or a custom set
    if (_pid_config == "step_4") {
      std::vector<std::string> input_pid_vars;
      if (_pid_mode == "online") {
        input_pid_vars.insert(input_pid_vars.end(), {"PIDVarB", "PIDVarC", "PIDVarF",
                                                     "PIDVarH", "PIDVarI", "PIDVarJ"});
      } else if (_pid_mode == "offline") {
        input_pid_vars.insert(input_pid_vars.end(), {"PIDVarA", "PIDVarB", "PIDVarC",
            "PIDVarD", "PIDVarE", "PIDVarF", "PIDVarG", "PIDVarH", "PIDVarI", "PIDVarJ"});
      } else if (_pid_mode == "custom") {
        // read in space separated list of custom selection of PIDVars
        std::istringstream ss(_custom_pid_set);
        std::string token;
        while(std::getline(ss, token, ' ')) {
          input_pid_vars.push_back(token);
        }
      } else {
        throw MAUS::Exceptions::Exception(Exceptions::recoverable,
            "Invalid pid_mode set in datacard", "MapCppGlobalPID::_birth");
      }
      for (size_t i = 0; i < _hypotheses.size(); ++i) {
        for (size_t j = 0; j < input_pid_vars.size(); ++j) {
          // for each member of hypotheses vector, check if custom PIDVar list
          // contained a given variable before adding it to vector
          if (input_pid_vars[j] == "PIDVarA") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_histFile, _hypotheses[i],
                _minA, _maxA));
          } else if (input_pid_vars[j] == "PIDVarB") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_histFile, _hypotheses[i],
                _XminB, _XmaxB, _YminB, _YmaxB));
          } else if (input_pid_vars[j] == "PIDVarC") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarC(_histFile, _hypotheses[i],
                _XminC, _XmaxC, _YminC, _YmaxC));
          } else if (input_pid_vars[j] == "PIDVarD") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarD(_histFile, _hypotheses[i],
                _minD, _maxD));
          } else if (input_pid_vars[j] == "PIDVarE") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarE(_histFile, _hypotheses[i],
                _minE, _maxE));
          } else if (input_pid_vars[j] == "PIDVarF") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarF(_histFile, _hypotheses[i],
                _XminF, _XmaxF, _YminF, _YmaxF));
          } else if (input_pid_vars[j] == "PIDVarG") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarG(_histFile, _hypotheses[i],
                _minG, _maxG));
          } else if (input_pid_vars[j] == "PIDVarH") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarH(_histFile, _hypotheses[i],
                _XminH, _XmaxH, _YminH, _YmaxH));
          } else if (input_pid_vars[j] == "PIDVarI") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarI(_histFile, _hypotheses[i],
                _XminI, _XmaxI, _YminI, _YmaxI));
          } else if (input_pid_vars[j] == "PIDVarJ") {
            _pid_vars.push_back(new MAUS::recon::global::PIDVarJ(_histFile, _hypotheses[i],
                _XminJ, _XmaxJ, _YminJ, _YmaxJ));
          }
        }
      }
      if (_pid_vars.size() == 0) {
        throw MAUS::Exceptions::Exception(Exceptions::recoverable,
            "No valid PID variables given in custom_pid_set", "MapCppGlobalPID::_birth");
      }

      // PIDVar selection if commissioning running is selected. Different sets of
      // variables are used depending on whether the user selects an offline
      // set, online set, or a custom set
    } else if (_pid_config == "commissioning") {
      std::vector<std::string> input_pid_vars;
      if (_pid_mode == "online") {
        input_pid_vars.insert(input_pid_vars.end(), {"ComPIDVarB", "ComPIDVarE", "ComPIDVarG",
                                                     "ComPIDVarH", "ComPIDVarI"});
      } else if (_pid_mode == "offline") {
        input_pid_vars.insert(input_pid_vars.end(), {"ComPIDVarA", "ComPIDVarB", "ComPIDVarC",
            "ComPIDVarD", "ComPIDVarE", "ComPIDVarF", "ComPIDVarG", "ComPIDVarH", "ComPIDVarI"});
      } else if (_pid_mode == "custom") {
        // read in space separated list of custom selection of PIDVars
        std::istringstream ss(_custom_pid_set);
        std::string token;
        while(std::getline(ss, token, ' ')) {
          input_pid_vars.push_back(token);
        }
      } else {
        throw MAUS::Exceptions::Exception(Exceptions::recoverable,
            "Invalid pid_mode set in datacard", "MapCppGlobalPID::_birth");
      }
      for (size_t i = 0; i < _hypotheses.size(); ++i) {
        for (size_t j = 0; j < input_pid_vars.size(); ++j) {
          // for each member of hypotheses vector, check if custom PIDVar list
          // contained a given variable before adding it to vector
          if (input_pid_vars[j] == "ComPIDVarA") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarA(_histFile, _hypotheses[i],
                _minComA, _maxComA));
          } else if (input_pid_vars[j] == "ComPIDVarB") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarB(_histFile, _hypotheses[i],
                _XminComB, _XmaxComB, _YminComB, _YmaxComB));
          } else if (input_pid_vars[j] == "ComPIDVarC") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarC(_histFile, _hypotheses[i],
                _minComC, _maxComC));
          } else if (input_pid_vars[j] == "ComPIDVarD") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarD(_histFile, _hypotheses[i],
                _minComD, _maxComD));
          } else if (input_pid_vars[j] == "ComPIDVarE") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarE(_histFile, _hypotheses[i],
                _XminComE, _XmaxComE, _YminComE, _YmaxComE));
          } else if (input_pid_vars[j] == "ComPIDVarF") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarF(_histFile, _hypotheses[i],
                _minComF, _maxComF));
          } else if (input_pid_vars[j] == "ComPIDVarG") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarG(_histFile, _hypotheses[i],
                _XminComG, _XmaxComG, _YminComG, _YmaxComG));
          } else if (input_pid_vars[j] == "ComPIDVarH") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarH(_histFile, _hypotheses[i],
                _XminComH, _XmaxComH, _YminComH, _YmaxComH));
          } else if (input_pid_vars[j] == "ComPIDVarI") {
            _pid_vars.push_back(new MAUS::recon::global::ComPIDVarI(_histFile, _hypotheses[i],
                _XminComI, _XmaxComI, _YminComI, _YmaxComI));
          }
        }
      }
      if (_pid_vars.size() == 0) {
        throw MAUS::Exceptions::Exception(Exceptions::recoverable,
            "No valid PID variables given in custom_pid_set", "MapCppGlobalPID::_birth");
      }
    } else {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
          "Invalid pid_config", "MapCppGlobalPID::_birth");
    }
    _configCheck = true;
  }

  void MapCppGlobalPID::_death() {
    _pid_vars.clear();
  }

  void MapCppGlobalPID::_process(MAUS::Data* data) const {
    MAUS::Data* data_cpp = data;
    if (!data_cpp) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Data was NULL", "MapCppGlobalPID::process");
    }
    if (!_configCheck) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Birth was not called successfully", "MapCppGlobalPID::process");
    }

    const MAUS::Spill* _spill = data_cpp->GetSpill();
    if (_spill->GetReconEvents()) {
      for (size_t event_i = 0; event_i < _spill->GetReconEvents()->size(); ++event_i ) {
        if (_spill->GetReconEvents()->at(event_i)->GetGlobalEvent()) {
          MAUS::GlobalEvent* global_event = _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
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
              Segment_PID(GlobalTrackArray, global_event, "US");
              Segment_PID(GlobalTrackArray, global_event, "DS");
              Through_Segment_PID(GlobalTrackArray, global_event, "US");
              Through_Segment_PID(GlobalTrackArray, global_event, "DS");
              Segment_PID(GlobalTrackArray, global_event, "Through");
            } else if (track_types[q] == "US") {
              Segment_PID(GlobalTrackArray, global_event, "US");
            } else if (track_types[q] == "DS") {
              Segment_PID(GlobalTrackArray, global_event, "DS");
            } else if (track_types[q] == "Through-US") {
              Through_Segment_PID(GlobalTrackArray, global_event, "US");
            } else if (track_types[q] == "Through-DS") {
              Through_Segment_PID(GlobalTrackArray, global_event, "DS");
            } else if (track_types[q] == "Through") {
              Segment_PID(GlobalTrackArray, global_event, "Through");
            }
          }
        }
      }
    }
  }

  double MapCppGlobalPID::ConfidenceLevel(double LL_x, double sum_exp_LL) const {
    return (exp(LL_x)/sum_exp_LL)*100;
  }

  void MapCppGlobalPID::Segment_PID(
      std::vector<MAUS::DataStructure::Global::Track*>* GlobalTrackArray,
      MAUS::GlobalEvent* global_event, std::string segment) const {
    for (size_t track_i = 0; track_i < GlobalTrackArray->size(); ++track_i) {
      MAUS::DataStructure::Global::Track* track = GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching_" + segment) {
        perform_pid(segment, track, global_event);
      }
    }
  }

  void MapCppGlobalPID::Through_Segment_PID(
      std::vector<MAUS::DataStructure::Global::Track*>* GlobalTrackArray,
      MAUS::GlobalEvent* global_event, std::string segment) const {
    for (size_t track_i = 0; track_i < GlobalTrackArray->size(); ++track_i) {
      MAUS::DataStructure::Global::Track* track = GlobalTrackArray->at(track_i);
      if (track->get_mapper_name() == "MapCppGlobalTrackMatching_Through") {
        std::vector<const MAUS::DataStructure::Global::Track*> const_tracks =
          track->GetConstituentTracks();
        for (size_t track_j = 0; track_j < const_tracks.size(); ++track_j) {
          if (const_tracks.at(track_j)->get_mapper_name() ==
              "MapCppGlobalTrackMatching_" + segment) {
            MAUS::DataStructure::Global::Track* segment_track =
                const_cast<MAUS::DataStructure::Global::Track*> (const_tracks.at(track_j));
            perform_pid("Through-" + segment, segment_track, global_event);
          }
        }
      }
    }
  }

  void MapCppGlobalPID::perform_pid(std::string function_name,
                                    MAUS::DataStructure::Global::Track* track,
                                    MAUS::GlobalEvent* global_event) const {

    MAUS::DataStructure::Global::Track* pidtrack = track->Clone();
    global_event->add_track_recursive(pidtrack);
    std::string candidate_name = "MapCppGlobalPID_" + function_name + "_Candidate";
    pidtrack->set_mapper_name(candidate_name);
    pidtrack->set_pid(MAUS::DataStructure::Global::kNoPID);
    // doubles to hold cumulative log likelihoods for each hypothesis
    double logL_mu = 0;
    double logL_e = 0;
    double logL_pi = 0;
    /// Keeping track of whether the log likelihood for the PID variable
    /// returned an error value. Only used for the purposes of efficiency
    /// calculations, does not affect PID performance
    size_t logL_1_sum = 0;
    for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
         ++pid_var_count) {
      std::string polarity_sign;
      if (_pid_beamline_polarity == "positive") {
        polarity_sign = "plus";
      } else if (_pid_beamline_polarity == "negative") {
        polarity_sign = "minus";
      }
      logL_1_sum = 0;
      std::string hyp = _pid_vars[pid_var_count]->Get_hyp();
      if (hyp == (_pid_beam_setting + "_mu_" + polarity_sign)) {
        if (_pid_vars[pid_var_count]->logL(track) == 1) {
          logL_1_sum++;
          continue;
        } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
          continue;
        } else {
          logL_mu += _pid_vars[pid_var_count]->logL(track);
          std::cerr << "logL_mu : " << logL_mu << std::endl;
        }
      } else if (hyp == (_pid_beam_setting + "_e_" + polarity_sign)) {
        if (_pid_vars[pid_var_count]->logL(track) == 1) {
          logL_1_sum++;
          continue;
        } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
          continue;
        } else {
          logL_e += _pid_vars[pid_var_count]->logL(track);
        }
      } else if (hyp == (_pid_beam_setting + "_pi_" + polarity_sign)) {
        if (_pid_vars[pid_var_count]->logL(track) == 1) {
          logL_1_sum++;
          continue;
        } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
          continue;
        } else {
          logL_pi += _pid_vars[pid_var_count]->logL(track);
        }
      } else {
        Squeak::mout(Squeak::debug) << "Unrecognised particle hypothesis,"
                  << " MapCppGlobalPID::perform_pid" << std::endl;
      }
    }
    if ((logL_1_sum/3) == _pid_vars.size()) {
      logL_mu = 1;
      logL_pi = 1;
      logL_e = 1;
    } else if (logL_mu == 0 || logL_pi == 0 || logL_e == 0) {
      logL_mu = 2;
      logL_pi = 2;
      logL_e = 2;
    }
    // make pid_ll_values
    // creates object in data structure with loglikelihoods for each particle hypothesis
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
    if (logL_mu < 0 || logL_pi < 0 || logL_e < 0) {
      // calculate CLs
      double sum_exp_LLs = exp(logL_mu) + exp(logL_e) + exp(logL_pi);
      double CL_mu = ConfidenceLevel(logL_mu, sum_exp_LLs);
      double CL_e = ConfidenceLevel(logL_e, sum_exp_LLs);
      double CL_pi = ConfidenceLevel(logL_pi, sum_exp_LLs);
      // compare CLs and select winning hypothesis. set g.o.f. of track to CL

      MAUS::DataStructure::Global::PID track_pid = MAUS::DataStructure::Global::kNoPID;
      double CL;
      
      if (CL_mu - CL_e > _pid_confidence_level && CL_mu - CL_pi > _pid_confidence_level) {
        CL = CL_mu;
        if (_pid_beamline_polarity == "positive") {
          track_pid = MAUS::DataStructure::Global::kMuPlus;
        } else if (_pid_beamline_polarity == "negative") {
          track_pid = MAUS::DataStructure::Global::kMuMinus;
        }
      } else if (CL_pi - CL_e > _pid_confidence_level && CL_pi - CL_mu > _pid_confidence_level) {
        CL = CL_pi;
        if (_pid_beamline_polarity == "positive") {
          track_pid = MAUS::DataStructure::Global::kPiPlus;
        } else if (_pid_beamline_polarity == "negative") {
          track_pid = MAUS::DataStructure::Global::kPiMinus;
        }
      } else if (CL_e - CL_mu > _pid_confidence_level && CL_e - CL_pi > _pid_confidence_level) {
        CL = CL_e;
        if (_pid_beamline_polarity == "positive") {
          track_pid = MAUS::DataStructure::Global::kEPlus;
        } else if (_pid_beamline_polarity == "negative") {
          track_pid = MAUS::DataStructure::Global::kEMinus;
        }
      } else {
        Squeak::mout(Squeak::debug) << "PID for track could not be determined." << std::endl;
        return;
      }

      pidtrack->set_pid(track_pid);
      pidtrack->set_goodness_of_fit(CL);
      if (track->get_pid() == track_pid) {
        MAUS::DataStructure::Global::Track* final_pidtrack = pidtrack->Clone();
        global_event->add_track_recursive(final_pidtrack);
        final_pidtrack->set_mapper_name("MapCppGlobalPID_" + function_name);
      }
    }
  }
} // ~MAUS
