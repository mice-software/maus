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

    // vector of hypotheses
    // TODO(Pidcott) find a more elegant way of accessing hypotheses
    _hypotheses.push_back("200MeV_mu_plus");
    _hypotheses.push_back("200MeV_e_plus");
    _hypotheses.push_back("200MeV_pi_plus");

    for (unsigned int i =0; i < _hypotheses.size(); ++i) {
      // vector of pid vars
      _pid_vars.push_back(new MAUS::recon::global::PIDVarA(_histFile,
							   _hypotheses[i]));
      _pid_vars.push_back(new MAUS::recon::global::PIDVarB(_histFile,
                                                           _hypotheses[i]));
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
        MAUS::GlobalEvent* global_event =
        _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
        std::vector<MAUS::DataStructure::Global::Track*> *GlobalTrackArray =
        global_event->get_tracks();
        for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
             ++track_i) {
          MAUS::DataStructure::Global::Track* track =
          GlobalTrackArray->at(track_i);
          if (track->get_mapper_name() != "MapCppGlobalTrackMatching") continue;
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
        }
      }
    }
  }
} // ~MAUS
