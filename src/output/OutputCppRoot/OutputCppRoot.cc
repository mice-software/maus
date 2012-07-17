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

#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppStreamer/ORStream.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppConverter.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/DataStructure/Data.hh"

#include "src/output/OutputCppRoot/OutputCppRoot.hh"

namespace MAUS {
OutputCppRoot::OutputCppRoot() : _outfile(NULL), _data(NULL),
                          _jsonCppConverter(NULL), _classname("OutputCppRoot") {
}

bool OutputCppRoot::birth(std::string json_datacards) {
  try {
    // clean up any allocated memory or throw an exception
    if (!death()) {
        throw(Squeal(
            Squeal::nonRecoverable,
            "Failed to clean up memory",
            "OutputCppRoot::birth"
        ));
    }
    // load datacards
    Json::Value datacards = JsonWrapper::StringToJson(json_datacards);
    std::string root_output = JsonWrapper::GetProperty(datacards,
                  "output_root_file_name", JsonWrapper::stringValue).asString();
    // Setup output stream
    _outfile = new orstream(root_output.c_str(), "Spill");
    _data = new Data();
    _jsonCppConverter = new JsonCppConverter();
    (*_outfile) << branchName("data") << _data;
    return true;
  } catch(Squeal squee) {
    death();
    CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
    Squeak::mout(Squeak::debug) << squee.GetStackTrace() << std::endl;
    return false;
  } catch(std::exception exc) {
    death();
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    return false;
  }
}

bool OutputCppRoot::save(std::string json_spill_document) {
  try {
      if (_jsonCppConverter == NULL || _data == NULL || _outfile == NULL) {
          throw(Squeal(Squeal(
            Squeal::recoverable,
            "OutputCppRoot was not initialised properly",
            "OutputCppRoot::save"
          )));
      }
      if (json_spill_document != "") {
          Json::Value json_spill = JsonWrapper::StringToJson(json_spill_document);
          _data->SetSpill( (*_jsonCppConverter)(json_spill) );
          (*_outfile) << fillEvent;
          return true;
      } else {
          return false;
      }
  } catch(Squeal squee) {
    CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
    return false;
  } catch(std::exception exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    return false;
  }
}

bool OutputCppRoot::death() {
  if (_outfile != NULL) {
    _outfile->close();
    delete _outfile;
    _outfile = NULL;  // deletes spill
    _data = NULL;
  } else if (_data != NULL) {
    delete _data;
    _data = NULL;
  }
  if (_jsonCppConverter != NULL) {
    delete _jsonCppConverter;
    _jsonCppConverter = NULL;
  }
  return true;
}
}

