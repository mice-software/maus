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

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/JobHeaderData.hh"
#include "src/common_cpp/DataStructure/JobFooterData.hh"

#include "src/common_cpp/Converter/DataConverters/CppJsonJobFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonJobHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

namespace MAUS {

InputCppRoot::InputCppRoot(std::string filename)
            : InputBase<std::string>("InputCppRoot"), _infile(NULL),
              _infile_tree(""), _filename(filename),
              _next_event_type(_job_header_tp) {
}

InputCppRoot::~InputCppRoot() {
  death();
}

void InputCppRoot::_birth(const std::string& json_datacards) {
  Json::Value json_dc = JsonWrapper::StringToJson(json_datacards);
  if (_filename == "") {
    _filename = JsonWrapper::GetProperty(json_dc,
               "input_root_file_name", JsonWrapper::stringValue).asString();
  }
  _infile = new irstream(_filename.c_str(), "Spill");
  _infile_tree = "Spill";
}

void InputCppRoot::_death() {
  if (_infile != NULL) {
    delete _infile;
    _infile = NULL;
  }
}

std::string InputCppRoot::_emitter_cpp() {
  std::string event = "";
  switch (_next_event_type) {
      case _job_header_tp:
          try {
              event = _load_event<CppJsonHeaderConverter, JobHeaderData>
                                                    (std::string("job_header"));
              if (event == "") {
                  _next_event_type = _spill_tp;
                  return _emitter_cpp();
              }
              return event;
          } catch(const std::exception& exc) {
              _next_event_type = _spill_tp;
              return _emitter_cpp();
          }
      case _spill_tp:
          try {
              event = _load_event<CppJsonSpillConverter, Data>
                                                          (std::string("data"));
              if (event == "") {
                  _next_event_type = _job_footer_tp;
                  return _emitter_cpp();
              }
              return event;
          } catch(const std::exception& exc) {
              _next_event_type = _job_footer_tp;
              return _emitter_cpp();
          }
      case _job_footer_tp:
          event = _load_event<CppJsonFooterConverter, JobFooterData>
                                                    (std::string("job_footer"));
          return event;
  }
  return event;
}

template <class ConverterT, class DataT>
std::string InputCppRoot::_load_event(std::string branch_name) {
    if (_infile == NULL) {
      throw(Squeal(
        Squeal::recoverable,
        "InputCppRoot was not initialised properly",
        "InputCppRoot::getNextEvent"
      ));
    }
    std::string output = "";
    DataT data;
    if (_infile_tree != data.GetEventType()) {
        _infile->close();
        _infile->open(_filename.c_str(), data.GetEventType().c_str());
        _infile_tree = data.GetEventType();
    }
    (*_infile) >> branchName(branch_name.c_str()) >> data;

    if ((*_infile) >> readEvent == NULL) {
        return output;
    }
    Json::Value* value = ConverterT()(&data);
    if (value == NULL)
        return output;
    if (value->isNull()) {
        delete value;
        return output;
    }
    Json::FastWriter writer;
    output = writer.write(*value);
    delete value;
    return output;
}
}

