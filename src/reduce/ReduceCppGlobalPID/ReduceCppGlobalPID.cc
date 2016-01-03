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

  ReduceCppGlobalPID::~ReduceCppGlobalPID() {
    }

  void ReduceCppGlobalPID::_birth(const std::string& argJsonConfigDocument) {
    _configCheck = false;

    _classname = "ReduceCppGlobalPID";

    // JsonCpp setup - check file parses correctly, if not return false
    Json::Value configJSON;
    try {
      //std::cerr << "in the try" << std::endl;
      configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
      _hypothesis_name = configJSON["global_pid_hypothesis"].asString();
      _unique_identifier = configJSON["unique_identifier"].asString();
      if (_hypothesis_name.empty() ||
          !configJSON.isMember("global_pid_hypothesis")) {
        Squeak::mout(Squeak::error) << "Json did not contain a valid "
	    << "global_pid_hypothesis, which is required for PDF production, "
	    << "ReduceCppGlobalPID::birth" << std::endl;
      }
      if (_unique_identifier.empty() ||
          !configJSON.isMember("unique_identifier")) {
        Squeak::mout(Squeak::error) << "Json did not contain a valid "
            << "unique_identifier, which is required for PDF production, "
	    << "ReduceCppGlobalPID::birth" << std::endl;
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
      _pid_vars.push_back(new MAUS::recon::global::PIDVarG(_hypothesis_name,
							   _unique_identifier));

      _configCheck = true;
    } catch (Exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
    } catch (std::exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    }
  }

  void ReduceCppGlobalPID::_process(MAUS::Data* data_cpp) {
    if (data_cpp == NULL)
      throw Exception(Exception::recoverable, "Data was NULL",
		      "ReduceCppMCProp::_process");
    if (data_cpp->GetSpill() == NULL)
      throw Exception(Exception::recoverable, "Spill was NULL",
		      "ReduceCppMCProp::_process");
    if (data_cpp->GetSpill()->GetDaqEventType() != "physics_event") {
      /*ImageData * image_data = new ImageData();
      image_data->SetImage(new Image());
      return image_data;*/
    }
    if (!_configCheck) {
      throw Exception(Exception::recoverable,
                      "Birth was not called successfully",
                      "ReduceCppGlobalPID::process");
    }

    if (data_cpp->GetSpill()->GetReconEvents() == NULL)
        throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppGlobalPID::_process");

    const MAUS::Spill* _spill = data_cpp->GetSpill();

    if (_spill) {
      if ( _spill->GetReconEvents() ) {
        for ( unsigned int event_i = 0;
              event_i < _spill->GetReconEvents()->size(); ++event_i) {
	  if (_spill->GetReconEvents()->at(event_i)->GetGlobalEvent()) {
	    MAUS::GlobalEvent* global_event =
	      _spill->GetReconEvents()->at(event_i)->GetGlobalEvent();
	    //temp line to be removed, hack whilst waiting for JG TM to work
	    int tr_count = 0;
	    std::vector<MAUS::DataStructure::Global::Track*> *GlobalTrackArray =
	      global_event->get_tracks();
	    for (unsigned int track_i = 0; track_i < GlobalTrackArray->size();
		 ++track_i) {
	      if (tr_count > 0) break;
	      MAUS::DataStructure::Global::Track* track =
		GlobalTrackArray->at(track_i);
	      if (track->get_mapper_name() != "MapCppGlobalTrackMatching-Through") continue;
	      tr_count++;
	      for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
		   ++pid_var_count) {
		_pid_vars[pid_var_count]->Fill_Hist(track);
	      }
	    }
	  }
        }
      }
    } else {
      Squeak::mout(Squeak::error) << "Failed to import spill from data\n";
    }
    /*ImageData * image_data = new ImageData();
    image_data->SetImage(new Image());
    return image_data;*/
  }

  void ReduceCppGlobalPID::_death()  {
    if (_configCheck) {
      //_pid_vars.clear();
      for (size_t pid_var_count = 0; pid_var_count < _pid_vars.size();
      	   ++pid_var_count) {
      	delete _pid_vars[pid_var_count];
      }
    }
  }

  PyMODINIT_FUNC init_ReduceCppGlobalPID(void) {
    PyWrapReduceBase<ReduceCppGlobalPID>::PyWrapReduceBaseModInit(
                                  "ReduceCppGlobalPID", "", "", "", "");
}
} // ~namespace MAUS
