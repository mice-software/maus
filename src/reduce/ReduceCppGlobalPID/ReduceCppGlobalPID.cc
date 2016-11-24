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
        Squeak::mout(Squeak::error) << "Config did not contain a valid "
                                    << "pid_beam_setting, which is required for PDF production, "
                                    << "ReduceCppGlobalPID::birth" << std::endl;
      }
      if (_unique_identifier.empty() || !_configJSON.isMember("unique_identifier")) {
        Squeak::mout(Squeak::error) << "Config did not contain a valid "
                                    << "unique_identifier, which is required for PDF production, "
                                    << "ReduceCppGlobalPID::birth" << std::endl;
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
        Squeak::mout(Squeak::warning) << "Invalid pid_beamline_polarity "
                                      << "set in ConfigurationDefaults, "
                                      << "ReduceCppGlobalPID::_birth" << std::endl;
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
        Squeak::mout(Squeak::warning) << "Invalid pid_config, "
                                      << " ReduceCppGlobalPID::birth" << std::endl;
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
                      "Birth was not called successfully",
                      "ReduceCppGlobalPID::process");
    }

    /*if (data_cpp->GetSpill()->GetReconEvents() == NULL)
        throw Exceptions::Exception(Exceptions::recoverable, "ReconEvents were NULL",
	"ReduceCppGlobalPID::_process");*/

    _spill = data_cpp->GetSpill();

    if (_spill and _spill->GetReconEvents()) {
      for (size_t event_i = 0;
            event_i < _spill->GetReconEvents()->size(); ++event_i) {
        if (_spill->GetReconEvents()->at(event_i)->GetGlobalEvent()) {
          // FOR EACH GLOBAL EVENT
          MAUS::GlobalEvent* global_event =
              _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
          // Get Tracks
          std::vector<MAUS::DataStructure::Global::Track*> *GlobalTrackArray =
          global_event->get_tracks();
          bool through_track_check = false;
          for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
              ++track_i) {
            // FOR EACH TRACK
            MAUS::DataStructure::Global::Track* track = GlobalTrackArray->at(track_i);
            // only deal with through tracks
            if (track->get_mapper_name() != "MapCppGlobalTrackMatching_Through") continue;
            through_track_check = true;
            if (_pid_config == "step_4") {
              // get constituent tracks
              std::vector<const MAUS::DataStructure::Global::Track*> const_tracks =
                  track->GetConstituentTracks();
              for (unsigned int track_j = 0; track_j < const_tracks.size();
                   ++track_j) {
                int tracker_num;
                std::string track_mapper_name = const_tracks.at(track_j)->get_mapper_name();
                if (track_mapper_name == "MapCppGlobalTrackMatching_US") {
                  tracker_num = 0;
                } else if (track_mapper_name == "MapCppGlobalTrackMatching_DS") {
                  tracker_num = 1;
                } else {
                  continue;
                }
                MAUS::DataStructure::Global::Track* segment_track =
                    const_cast<MAUS::DataStructure::Global::Track*> (const_tracks.at(track_j));
                if ((_pid_beamline_polarity == "positive" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == -13) ||
                    (_pid_beamline_polarity == "negative" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == 13)) {
                  for (size_t pid_var_count = 0; pid_var_count < _mu_pid_vars.size();
                       ++pid_var_count) {
                    _mu_pid_vars[pid_var_count]->Fill_Hist(segment_track);
                  } // loop over mu_pid_vars
                } else if ((_pid_beamline_polarity == "positive" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == -11) ||
                    (_pid_beamline_polarity == "negative" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == 11)) {
                  for (size_t pid_var_count = 0; pid_var_count < _e_pid_vars.size();
                       ++pid_var_count) {
                    _e_pid_vars[pid_var_count]->Fill_Hist(segment_track);
                  } // loop over e_pid_vars
                } else if ((_pid_beamline_polarity == "positive" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == 211) ||
                    (_pid_beamline_polarity == "negative" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == -211)) {
                  for (size_t pid_var_count = 0; pid_var_count < _pi_pid_vars.size();
                       ++pid_var_count) {
                    _pi_pid_vars[pid_var_count]->Fill_Hist(segment_track);
                  } // loop over pi_pid_vars
                } // Beamline segment mc pid check
              } // loop over constituent tracks
            } else if (_pid_config == "commissioning") {
              if ((_pid_beamline_polarity == "positive" &&
                  _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), 1) == -13) ||
                  (_pid_beamline_polarity == "negative" &&
                  _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), 1) == 13)) {
                for (size_t pid_var_count = 0; pid_var_count < _mu_pid_vars.size();
                    ++pid_var_count) {
                  _mu_pid_vars[pid_var_count]->Fill_Hist(track);
                } // loop over mu_pid_vars
              } else if ((_pid_beamline_polarity == "positive" &&
                  _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), 1) == -11) ||
                  (_pid_beamline_polarity == "negative" &&
                  _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), 1) == 11)) {
                for (size_t pid_var_count = 0; pid_var_count < _e_pid_vars.size();
                    ++pid_var_count) {
                  _e_pid_vars[pid_var_count]->Fill_Hist(track);
                } // loop over e_pid_vars
              } else if ((_pid_beamline_polarity == "positive" &&
                  _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), 1) == 211) ||
                  (_pid_beamline_polarity == "negative" &&
                  _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), 1) == -211)) {
                for (size_t pid_var_count = 0; pid_var_count < _pi_pid_vars.size();
                    ++pid_var_count) {
                  _pi_pid_vars[pid_var_count]->Fill_Hist(track);
                } // loop over pi_pid_vars
              } // check straight through track mc pid at DS tracker ref plane
            } // change behaviour based on straight or helical tracks
          } // loop over global tracks
          if (through_track_check == false) {
            for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
                ++track_i) {
              MAUS::DataStructure::Global::Track* track = GlobalTrackArray->at(track_i);
              int tracker_num;
              std::string track_mapper_name = track->get_mapper_name();
              if (track_mapper_name == "MapCppGlobalTrackMatching_US") {
                tracker_num = 0;
              } else if (track_mapper_name == "MapCppGlobalTrackMatching_DS") {
                tracker_num = 1;
              } else {
                continue;
              }
              if ((_pid_beamline_polarity == "positive" &&
                   _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == -13) ||
                  (_pid_beamline_polarity == "negative" &&
                   _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == 13)) {
                for (size_t pid_var_count = 0; pid_var_count < _mu_pid_vars.size();
                     ++pid_var_count) {
                  _mu_pid_vars[pid_var_count]->Fill_Hist(track);
                } // loop over mu_pid_vars
              } else if ((_pid_beamline_polarity == "positive" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == -11) ||
                   (_pid_beamline_polarity == "negative" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == 11)) {
                for (size_t pid_var_count = 0; pid_var_count < _e_pid_vars.size();
                     ++pid_var_count) {
                  _e_pid_vars[pid_var_count]->Fill_Hist(track);
                } // loop over e_pid_vars
              } else if ((_pid_beamline_polarity == "positive" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == 211) ||
                   (_pid_beamline_polarity == "negative" &&
                    _mc_pid_tracker_ref(_spill->GetMCEvents()->at(event_i), tracker_num) == -211)) {
                for (size_t pid_var_count = 0; pid_var_count < _pi_pid_vars.size();
                     ++pid_var_count) {
                  _pi_pid_vars[pid_var_count]->Fill_Hist(track);
                } // loop over pi_pid_vars
              }
            } // temporary measure for when through tracks haven't been formed
          } // get global event
        } // get recon event
      } // get recon event array
    } else {
      Squeak::mout(Squeak::error) << "Failed to import spill from data\n";
    } // check for spill
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
