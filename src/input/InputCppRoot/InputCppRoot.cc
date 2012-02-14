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

#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/input/InputCppRoot/InputCppRoot.hh"

#include "src/common_cpp/JsonCppStreamer/JsonCppConverter.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"
#include "src/common_cpp/DataStructure/MausEventStruct.hh"

namespace MAUS {

InputCppRoot::InputCppRoot(std::string filename) : _infile(NULL),
              _jsonCppConverter(NULL),  _md(NULL), _val(), _filename(filename) {
}

InputCppRoot::~InputCppRoot() {
  death();
}


bool InputCppRoot::birth(std::string json_datacards) {
  Json::Value json_dc = JsonWrapper::StringToJson(json_datacards);
  if (_filename == "") {
    _filename = JsonWrapper::GetProperty(json_dc,
                   "root_input_filename", JsonWrapper::stringValue).asString();
  }
  _infile = new irstream(_filename.c_str());

  _d = new Digits();
  _mc = new MC();
  _md = new MausData(_d, _mc);

  (*_infile) >> branchName("digits") >> _d;
  (*_infile) >> branchName("mc") >> _mc;

  _jsonCppConverter = new JsonCppConverter(&_val);
  return true;
}

bool InputCppRoot::death() {
  if (_infile != NULL) {
    delete _infile;
    _infile = NULL;
  }
  if (_jsonCppConverter != NULL) {
    delete _jsonCppConverter;
    _jsonCppConverter = NULL;
  }
  if (_md != NULL) {
    delete _md;
    _md = NULL;
  }
  if (_mc != NULL) {
    delete _mc;
    _mc = NULL;
  }
  if (_d != NULL) {
    delete _d;
    _d = NULL;
  }
  return true;
}

std::string InputCppRoot::getNextEvent() {
  if (_jsonCppConverter == NULL || _infile == NULL) {
    throw(Squeal(Squeal(
      Squeal::recoverable,
      "InputCppRoot was not initialised properly",
      "InputCppRoot::getNextEvent"
    )));
  }
  if ((*_infile) >> readEvent == NULL) {
    return "";
  }
  (*_jsonCppConverter)(_md);
  Json::FastWriter writer;
  return writer.write(_val);
}
}

