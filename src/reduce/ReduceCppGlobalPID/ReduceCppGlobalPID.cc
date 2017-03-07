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
#include "Utils/Squeak.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/reduce/ReduceCppGlobalPID/ReduceCppGlobalPID.hh"

namespace MAUS {

  ReduceCppGlobalPID::~ReduceCppGlobalPID() {
    }

  void ReduceCppGlobalPID::_birth(const std::string& argJsonConfigDocument) {
    _configCheck = false;

    _classname = "ReduceCppGlobalPID";

    // JsonCpp setup - check file parses correctly, if not return false
    Json::Value _configJSON;
    try {
      _configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
      _pid_beam_setting = _configJSON["pid_beam_setting"].asString();
      _unique_identifier = _configJSON["unique_identifier"].asString();
      if (_pid_beam_setting.empty() || !_configJSON.isMember("pid_beam_setting")) {
        throw Exceptions::Exception(Exceptions::recoverable,
            "Config did not contain a valid pid_beam_setting, which is required for PDF production",
            "ReduceCppGlobalPID::birth");
      }
      if (_unique_identifier.empty() || !_configJSON.isMember("unique_identifier")) {
        throw Exceptions::Exception(Exceptions::recoverable,
            "Config did not contain valid unique_identifier, which is required for PDF production",
            "ReduceCppGlobalPID::birth");
      }

      _pid_config = _configJSON["pid_config"].asString();
      _pid_beamline_polarity = _configJSON["pid_beamline_polarity"].asString();

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
        throw Exceptions::Exception(Exceptions::recoverable,
          "Invalid pid_beamline_polarity set in configuration", "ReduceCppGlobalPID::birth");
      }

      if (_pid_config == "step_4") {
        for (size_t i = 0; i < 3; i++) {
          std::vector<MAUS::recon::global::PIDBase*> particle_pid_vars;
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarA(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarB(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarC(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarD(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarE(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarF(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarG(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarH(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarI(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::PIDVarJ(_hypotheses[i],
              _unique_identifier));
          if (i == 0) {
            _mu_pid_vars = particle_pid_vars;
          } else if (i == 1) {
            _e_pid_vars = particle_pid_vars;
          } else if (i == 2) {
            _pi_pid_vars = particle_pid_vars;
          }
        }
      } else if (_pid_config == "commissioning") {
        for (size_t i = 0; i < 3; i++) {
          std::vector<MAUS::recon::global::PIDBase*> particle_pid_vars;
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarA(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarB(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarC(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarD(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarE(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarF(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarG(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarH(_hypotheses[i],
              _unique_identifier));
          particle_pid_vars.push_back(new MAUS::recon::global::ComPIDVarI(_hypotheses[i],
              _unique_identifier));
          if (i == 0) {
            _mu_pid_vars = particle_pid_vars;
          } else if (i == 1) {
            _e_pid_vars = particle_pid_vars;
          } else if (i == 2) {
            _pi_pid_vars = particle_pid_vars;
          }
        }
      } else {
        throw Exceptions::Exception(Exceptions::recoverable,
          "Invalid pid_config set in configuration", "ReduceCppGlobalPID::birth");
      }
      _configCheck = true;
    } catch (Exceptions::Exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
    } catch (std::exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    }
  }

  void ReduceCppGlobalPID::_process(MAUS::Data* data_cpp) {
    if (data_cpp == NULL)
      throw Exceptions::Exception(Exceptions::recoverable, "Data was NULL",
          "ReduceCppMCProp::_process");
    if (data_cpp->GetSpill() == NULL)
      throw Exceptions::Exception(Exceptions::recoverable, "Spill was NULL",
          "ReduceCppMCProp::_process");
    if (data_cpp->GetSpill()->GetDaqEventType() != "physics_event") {
    }
    if (!_configCheck) {
      throw Exceptions::Exception(Exceptions::recoverable,
          "Birth was not called successfully", "ReduceCppGlobalPID::process");
    }
    _spill = data_cpp->GetSpill();
    if (!(_spill and _spill->GetReconEvents())) {
      Squeak::mout(Squeak::error) << "Failed to import spill from data\n";
      return;
    }
    for (size_t event_i = 0; event_i < _spill->GetReconEvents()->size(); ++event_i) {
      MAUS::GlobalEvent* global_event = _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
      if (!global_event) {
        continue;
      }
      std::vector<MAUS::DataStructure::Global::PrimaryChain*> primary_chains;
      if (_pid_config == "step_4") {
        // Get Primary Chains (through tracks don't matter here, so we pick all types of
        // non-through primary chains
        primary_chains = global_event->GetNonThroughPrimaryChains();
      } else if (_pid_config == "commissioning") {
        // For straight track PID we need through tracks
        primary_chains = global_event->GetThroughPrimaryChains();
      }
      for (MAUS::DataStructure::Global::PrimaryChain* primary_chain: primary_chains) {
        std::vector<MAUS::DataStructure::Global::Track*> tracks = primary_chain->GetMatchedTracks();
        MAUS::DataStructure::Global::ChainType chain_type = primary_chain->get_chain_type();
        size_t tracker_num = 1;
        if (chain_type == MAUS::DataStructure::Global::kUSOrphan or
            chain_type == MAUS::DataStructure::Global::kUS) {
          tracker_num = 0;
        } else if (chain_type == MAUS::DataStructure::Global::kNoChainType) {
          continue;
        }
        for (MAUS::DataStructure::Global::Track* track: tracks) {
          int pid = _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num);
          _fill_pid_histograms(track, pid);
        }
      }
    }
  }

  void ReduceCppGlobalPID::_death()  {
    if (_configCheck) {
      // _pid_vars.clear();
      for (size_t pid_var_count = 0; pid_var_count < _mu_pid_vars.size();
           ++pid_var_count) {
        delete _mu_pid_vars[pid_var_count];
      }
      for (size_t pid_var_count = 0; pid_var_count < _e_pid_vars.size();
           ++pid_var_count) {
        delete _e_pid_vars[pid_var_count];
      }
      for (size_t pid_var_count = 0; pid_var_count < _pi_pid_vars.size();
           ++pid_var_count) {
        delete _pi_pid_vars[pid_var_count];
      }
    }
  }

  void ReduceCppGlobalPID::_fill_pid_histograms(
      MAUS::DataStructure::Global::Track* track, int pid) {
    std::vector<MAUS::recon::global::PIDBase*> pid_vars;
    if ((_pid_beamline_polarity == "positive" and pid == -13) or
        (_pid_beamline_polarity == "negative" and pid == 13)) {
      pid_vars = _mu_pid_vars;
    } else if ((_pid_beamline_polarity == "positive" and pid == -11) or
        (_pid_beamline_polarity == "negative" and pid == 11)) {
      pid_vars = _e_pid_vars;
    } else if ((_pid_beamline_polarity == "positive" and pid == 211) or
        (_pid_beamline_polarity == "negative" and pid == -211)) {
      pid_vars = _pi_pid_vars;
    } else {
      return;
    }
    for (MAUS::recon::global::PIDBase* pid_var: pid_vars) {
      pid_var->Fill_Hist(track);
    }
  }

  int ReduceCppGlobalPID::_mc_pid_tracker_ref(MAUS::MCEvent* mc_event, int tracker_number) {
    int pid = 0;
    MAUS::SciFiHitArray* scifihits = mc_event->GetSciFiHits();
    std::vector<MAUS::SciFiHit*>::iterator scifihit;
    for ( size_t i = 0; i < scifihits->size(); ++i ) {
      MAUS::SciFiHit scifihit = scifihits->at(i);
      if (scifihit.GetChannelId()->GetTrackerNumber() == tracker_number &&
          scifihit.GetChannelId()->GetStationNumber() == 1 &&
          scifihit.GetChannelId()->GetPlaneNumber() == 0) {
        pid = scifihit.GetParticleId();
        break;
      }
    }
    return pid;
  }

  PyMODINIT_FUNC init_ReduceCppGlobalPID(void) {
    PyWrapReduceBase<ReduceCppGlobalPID>::PyWrapReduceBaseModInit(
                                  "ReduceCppGlobalPID", "", "", "", "");
  }
} // ~namespace MAUS
