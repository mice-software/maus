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

#include "TFile.h"
#include "TTree.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

#include "src/input/InputCppRoot/InputCppRoot.hh"

#include "src/common_cpp/DataStructure/DAQData.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

#include "src/common_cpp/Converter/DataConverters/JsonCppConverter.hh"
//#include "src/common_cpp/JsonCppStreamer/JsonCppConverter.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

namespace MAUS {

InputCppRoot::InputCppRoot(std::string filename) : _infile(NULL),
              _jsonCppConverter(NULL),  _spill(NULL), _filename(filename) {
}

InputCppRoot::~InputCppRoot() {
  death();
}

bool InputCppRoot::birth(std::string json_datacards) {
  try {
      Json::Value json_dc = JsonWrapper::StringToJson(json_datacards);
      if (_filename == "") {
        _filename = JsonWrapper::GetProperty(json_dc,
                   "input_root_file_name", JsonWrapper::stringValue).asString();
      }
      _infile = new irstream(_filename.c_str(), "Spill");
      _jsonCppConverter = new JsonCppConverter();
      _spill = new Spill();
      (*_infile) >> branchName("spill") >> _spill;
  } catch(Squeal squee) {
    death();
    CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
    return false;
  } catch(std::exception exc) {
    death();
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    return false;
  }
  return true;
}

bool InputCppRoot::death() {
  // if _infile != NULL, _infile will delete spill
  if (_infile == NULL && _spill != NULL) {
    delete _spill;
    _spill = NULL;
  }

  if (_infile != NULL) {
    delete _infile;
    _infile = NULL;
  }

  if (_jsonCppConverter != NULL) {
    delete _jsonCppConverter;
    _jsonCppConverter = NULL;
  }
  return true;
}

std::string InputCppRoot::getNextEvent() {
  try {
      if (_jsonCppConverter == NULL || _infile == NULL) {
        throw(Squeal(
          Squeal::recoverable,
          "InputCppRoot was not initialised properly",
          "InputCppRoot::getNextEvent"
        ) );
      }
      if ((*_infile) >> readEvent == NULL) {
        return "";
      }
      Json::Value* value = (*_jsonCppConverter)(_spill);
      Json::FastWriter writer;
      std::string output = writer.write(*value);
      delete value;
      return output;
  } catch(Squeal squee) {
    CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
    return "";
  } catch(std::exception exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    return "";
  }
}

int InputCppRoot::my_sizeof() {
  Spill spill;
  return sizeof(spill);
}
}

