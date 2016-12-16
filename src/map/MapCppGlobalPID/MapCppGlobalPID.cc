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

#include <set>

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

    if (_pid_track_selection != "all" and _pid_track_selection != "through"
                                      and _pid_track_selection != "constituents") {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
          "Invalid pid_track_selection set in configuration", "MapCppGlobalPID::birth");
    }

    // vector of pid hypotheses
    if (_pid_beamline_polarity == "positive") {
      _hypotheses.push_back((_pid_beam_setting + "_e_plus"));
      _hypotheses.push_back((_pid_beam_setting + "_mu_plus"));
      _hypotheses.push_back((_pid_beam_setting + "_pi_plus"));
      _pol_mult = 1;
    } else if (_pid_beamline_polarity == "negative") {
      _hypotheses.push_back((_pid_beam_setting + "_e_minus"));
      _hypotheses.push_back((_pid_beam_setting + "_mu_minus"));
      _hypotheses.push_back((_pid_beam_setting + "_pi_minus"));
      _pol_mult = -1;
    } else {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
          "Invalid pid_beamline_polarity set in configuration", "MapCppGlobalPID::birth");
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
    if (!_spill->GetReconEvents()) {
      return;
    }
    for (size_t event_i = 0; event_i < _spill->GetReconEvents()->size(); ++event_i ) {
      MAUS::GlobalEvent* global_event = _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
      if (!global_event) {
        continue;
      }
      // Regardless of whether we PID through tracks or not, we first have to PID the constituents
      if (_pid_track_selection == "all" or _pid_track_selection == "constituents") {
        PerformPID(global_event->GetNonThroughPrimaryChains(), global_event);
      } else if (_pid_track_selection == "through") {
        PerformPID(global_event->GetUSPrimaryChains(), global_event);
        PerformPID(global_event->GetDSPrimaryChains(), global_event);
      }
      // Selecting through chains now means all constituent tracks are already PID'd
      if (_pid_track_selection == "all" or _pid_track_selection == "through") {
        std::vector<MAUS::DataStructure::Global::PrimaryChain*> through_chains =
            global_event->GetThroughPrimaryChains();
        for (MAUS::DataStructure::Global::PrimaryChain* through_chain: through_chains) {
          std::vector<MAUS::DataStructure::Global::Track*> through_tracks =
              through_chain->GetMatchedTracks();
          for (MAUS::DataStructure::Global::Track* through_track: through_tracks) {
            ThroughPID(through_track, through_chain, global_event);
          }
        }
      }
    }
  }

  void MapCppGlobalPID::PerformPID(
      std::vector<MAUS::DataStructure::Global::PrimaryChain*> primary_chains,
      MAUS::GlobalEvent* global_event) const {
    for (MAUS::DataStructure::Global::PrimaryChain* primary_chain : primary_chains) {
      std::vector<MAUS::DataStructure::Global::Track*> tracks = primary_chain->GetMatchedTracks();
      for (MAUS::DataStructure::Global::Track* track : tracks) {
        // doubles to hold cumulative log likelihoods for each hypothesis
        std::map<std::string, double> logL_values = {{"e", 0.0}, {"mu", 0.0}, {"pi", 0.0}};
        // Keeping track of whether the log likelihood for the PID variable
        // returned an error value. Only used for the purposes of efficiency
        // calculations, does not affect PID performance
        size_t logL_1_sum = 0;
        // loop over all PID vars with all particle hypotheses
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
          std::string pidstring;
          if (hyp == (_pid_beam_setting + "_e_" + polarity_sign)) {
            pidstring = "e";
          } else if (hyp == (_pid_beam_setting + "_mu_" + polarity_sign)) {
            pidstring = "mu";
          } else if (hyp == (_pid_beam_setting + "_pi_" + polarity_sign)) {
            pidstring = "pi";
          } else {
            Squeak::mout(Squeak::debug) << "Unrecognised particle hypothesis,"
                                        << " MapCppGlobalPID::perform_pid" << std::endl;
            continue;
          }
          // Increment logL values if current value does not indicate an error
          if (_pid_vars[pid_var_count]->logL(track) == 1) {
            logL_1_sum++;
            continue;
          } else if (_pid_vars[pid_var_count]->logL(track) == 2) {
            continue;
          } else {
            logL_values[pidstring] += _pid_vars[pid_var_count]->logL(track);
          }
        }
        if ((logL_1_sum/3) == _pid_vars.size()) {
          // error value for unsuitable track
          logL_values["e"] = 1;
          logL_values["mu"] = 1;
          logL_values["pi"] = 1;
        } else if (logL_values["e"] == 0 or logL_values["mu"] == 0 or logL_values["pi"] == 0) {
          // error value for track that failed PID due to user-defined cut on variable value
          logL_values["e"] = 2;
          logL_values["mu"] = 2;
          logL_values["pi"] = 2;
        }
        // make pid_ll_values
        // creates object in data structure with loglikelihoods for each particle hypothesis
        MAUS::DataStructure::Global::PIDLogLPair e_LL(_pol_mult*(-11), logL_values["e"]);
        MAUS::DataStructure::Global::PIDLogLPair mu_LL(_pol_mult*(-13), logL_values["mu"]);
        MAUS::DataStructure::Global::PIDLogLPair pi_LL(_pol_mult*211, logL_values["pi"]);
        track->AddPIDLogLValues(e_LL);
        track->AddPIDLogLValues(mu_LL);
        track->AddPIDLogLValues(pi_LL);
        if (logL_values["e"] < 0 and logL_values["mu"] < 0 and logL_values["pi"] < 0) {
          MAUS::DataStructure::Global::PID track_pid = MAUS::DataStructure::Global::kNoPID;
          double CL = CLandPID(track_pid, track->get_pid(),
                               logL_values["e"], logL_values["mu"], logL_values["pi"]);
          track->set_pid_confidence_level(CL);
          if (track->get_pid() == track_pid) {
            MAUS::DataStructure::Global::Track* final_pidtrack = track->Clone();
            global_event->add_track_recursive(final_pidtrack);
            // It is in principle conceivable (though EXTREMELY unlikely), that the following line
            // is called twice for the same chain. In this case, SetPIDTrack() will throw an error
            // message and the track will not be added to the chain.
            primary_chain->SetPIDTrack(final_pidtrack);
          }
        }
      }
    }
  }

  void MapCppGlobalPID::ThroughPID(MAUS::DataStructure::Global::Track* through_track,
                                   MAUS::DataStructure::Global::PrimaryChain* through_chain,
                                   MAUS::GlobalEvent* global_event) const {
    std::vector<const MAUS::DataStructure::Global::Track*> constituent_tracks =
        through_track->GetConstituentTracks();
    // Make sure that we have an intact through-constituent set. There should be exactly two
    // constituents and the through track should have all the detectors the constituents have
    // with no overlaps
    if (!(constituent_tracks.size() == 2 and
        (constituent_tracks.at(0)->get_detectorpoints() +
         constituent_tracks.at(1)->get_detectorpoints() ==
         through_track->get_detectorpoints()))) {
      return;
    }
    std::vector<MAUS::DataStructure::Global::PIDLogLPair> pairs_0 =
        constituent_tracks.at(0)->get_pid_logL_values();
    std::vector<MAUS::DataStructure::Global::PIDLogLPair> pairs_1 =
        constituent_tracks.at(1)->get_pid_logL_values();
    if (!(pairs_0.size() == 3 and pairs_1.size() == 3)) {
      Squeak::mout(Squeak::error) << "Through PID failed due to incorrect number of PIDLogLPairs "
                                  << "in constituents. MapCppGlobalPID::ThroughPID()" << std::endl;
      return;
    }
    // Assemble all logL pairs into a single vector so we can iterate over it
    std::vector<MAUS::DataStructure::Global::PIDLogLPair> pairs_through;
    pairs_through.push_back(pairs_0.at(0));
    pairs_through.push_back(pairs_0.at(1));
    pairs_through.push_back(pairs_0.at(2));
    pairs_through.push_back(pairs_1.at(0));
    pairs_through.push_back(pairs_1.at(1));
    pairs_through.push_back(pairs_1.at(2));
    double e_LL_sum = 0.0, mu_LL_sum = 0.0, pi_LL_sum = 0.0;
    // Add corresponding values together
    for (MAUS::DataStructure::Global::PIDLogLPair pair: pairs_through) {
      if (std::abs(pair.get_PID()) == 11) {
        e_LL_sum += pair.get_logL();
      } else if (std::abs(pair.get_PID()) == 13) {
        mu_LL_sum += pair.get_logL();
      } else if (std::abs(pair.get_PID()) == 211) {
        pi_LL_sum += pair.get_logL();
      }
    }
    MAUS::DataStructure::Global::PIDLogLPair e_LL(_pol_mult*(-11), e_LL_sum);
    MAUS::DataStructure::Global::PIDLogLPair mu_LL(_pol_mult*(-13), mu_LL_sum);
    MAUS::DataStructure::Global::PIDLogLPair pi_LL(_pol_mult*211, pi_LL_sum);
    through_track->AddPIDLogLValues(e_LL);
    through_track->AddPIDLogLValues(mu_LL);
    through_track->AddPIDLogLValues(pi_LL);
    if (e_LL_sum < 0 and mu_LL_sum < 0 and pi_LL_sum < 0) {
      MAUS::DataStructure::Global::PID track_pid = MAUS::DataStructure::Global::kNoPID;
      double CL = CLandPID(track_pid, through_track->get_pid(), e_LL_sum, mu_LL_sum, pi_LL_sum);
      through_track->set_pid_confidence_level(CL);
      if (through_track->get_pid() == track_pid) {
        MAUS::DataStructure::Global::Track* final_pidtrack = through_track->Clone();
        // Remove constituent tracks
        final_pidtrack->ClearTracks();
        // Get daughter chains and obtain PID'd tracks from those
        MAUS::DataStructure::Global::PrimaryChain* us_chain = through_chain->GetUSDaughter();
        MAUS::DataStructure::Global::PrimaryChain* ds_chain = through_chain->GetDSDaughter();
        MAUS::DataStructure::Global::Track* us_track = us_chain->GetPIDTrack();
        MAUS::DataStructure::Global::Track* ds_track = ds_chain->GetPIDTrack();
        // These should always have the same PID as the through track, but check just to make sure
        if (us_track->get_pid() == track_pid and ds_track->get_pid() == track_pid) {
          final_pidtrack->AddTrack(us_track);
          final_pidtrack->AddTrack(ds_track);
          global_event->add_track_recursive(final_pidtrack);
          through_chain->SetPIDTrack(final_pidtrack);
        }
      }
    }
  }

  double MapCppGlobalPID::CLandPID(MAUS::DataStructure::Global::PID &track_pid,
                                   MAUS::DataStructure::Global::PID pid_hypothesis,
                                   double logL_e, double logL_mu, double logL_pi) const {
    double sum_exp_LLs = exp(logL_e) + exp(logL_mu) + exp(logL_pi);
    double CL_e = (exp(logL_e)/sum_exp_LLs)*100;
    double CL_mu = (exp(logL_mu)/sum_exp_LLs)*100;
    double CL_pi = (exp(logL_pi)/sum_exp_LLs)*100;
    double CL = 0.0;
    if (CL_e - CL_mu > _pid_confidence_level and
        CL_e - CL_pi > _pid_confidence_level) {
      if (_pid_beamline_polarity == "positive") {
        track_pid = MAUS::DataStructure::Global::kEPlus;
      } else if (_pid_beamline_polarity == "negative") {
        track_pid = MAUS::DataStructure::Global::kEMinus;
      }
    } else if (CL_mu - CL_e > _pid_confidence_level and
               CL_mu - CL_pi > _pid_confidence_level) {
      if (_pid_beamline_polarity == "positive") {
        track_pid = MAUS::DataStructure::Global::kMuPlus;
      } else if (_pid_beamline_polarity == "negative") {
        track_pid = MAUS::DataStructure::Global::kMuMinus;
      }
    } else if (CL_pi - CL_e > _pid_confidence_level and
               CL_pi - CL_mu > _pid_confidence_level) {
      if (_pid_beamline_polarity == "positive") {
        track_pid = MAUS::DataStructure::Global::kPiPlus;
      } else if (_pid_beamline_polarity == "negative") {
        track_pid = MAUS::DataStructure::Global::kPiMinus;
      }
    } else {
      Squeak::mout(Squeak::debug) << "PID for track could not be determined." << std::endl;
    }
    if (pid_hypothesis == MAUS::DataStructure::Global::kEPlus or
        pid_hypothesis == MAUS::DataStructure::Global::kEMinus) {
      CL = CL_e;
    } else if (pid_hypothesis == MAUS::DataStructure::Global::kMuPlus or
               pid_hypothesis == MAUS::DataStructure::Global::kMuMinus) {
      CL = CL_mu;
    } else if (pid_hypothesis == MAUS::DataStructure::Global::kPiPlus or
               pid_hypothesis == MAUS::DataStructure::Global::kPiMinus) {
      CL = CL_pi;
    }
    return CL;
  }
  
} // ~MAUS
