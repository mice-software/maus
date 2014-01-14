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

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/JobHeaderData.hh"
#include "src/common_cpp/DataStructure/JobFooterData.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/DataStructure/RunFooterData.hh"

#include "src/common_cpp/Converter/DataConverters/CppJsonJobFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonJobHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonRunFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonRunHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

#include "src/input/InputCppRoot/InputCppRoot.hh"

namespace MAUS {

InputCppRoot::InputCppRoot(std::string filename)
            : InputBase<std::string>("InputCppRoot"), _infile(NULL),
              _infile_tree(""), _filename(filename),
              _event_type(_job_header_tp) {
    _current_run_number[_run_header_tp] = 0;
    _current_run_number[_spill_tp] = 0;
    _current_run_number[_run_footer_tp] = 0;
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
    try {
        // first check the cache
        event = uncache_event(_event_type);
        if (event != "") {
            return event;
        }
        // not cached? then we'd better extract from the file
        switch (_event_type) {
            case _job_header_tp:
                event = load_event<CppJsonJobHeaderConverter, JobHeaderData>
                                                    (std::string("job_header"));
                break;
            case _run_header_tp:
                event = load_event<CppJsonRunHeaderConverter, RunHeaderData>
                                                    (std::string("run_header"));
                break;
            case _spill_tp:
                event = load_event<CppJsonSpillConverter, Data>
                                                          (std::string("data"));
                break;
            case _run_footer_tp:
                event = load_event<CppJsonRunFooterConverter, RunFooterData>
                                                    (std::string("run_footer"));
                break;
            case _job_footer_tp:
                event = load_event<CppJsonJobFooterConverter, JobFooterData>
                                                    (std::string("job_footer"));
                break;
        }
        if (!use_event(event)) {
            // event number changed or we ran out of events of this type
            // advance event and recursively call emitter_cpp
            return advance_event_type();
        }
    // if there was no branch of this type at all then we might get an exception
    // - let's just try the next type
    } catch(const std::exception& exc) {
        std::cerr << (&exc)->what() << std::endl;
        return advance_event_type();
    }
    return event;
}

template <class ConverterT, class DataT>
std::string InputCppRoot::load_event(std::string branch_name) {
    if (_infile == NULL) {
      throw(Exception(
        Exception::recoverable,
        "InputCppRoot was not initialised properly",
        "InputCppRoot::getNextEvent"
      ));
    }
    std::string output = "";
    DataT data;
    if (_current_event_number.find(branch_name) == _current_event_number.end())
        _current_event_number[branch_name] = 0;
    if (_infile_tree != data.GetEventType()) {
        _infile->close();
        _infile_tree = data.GetEventType();
        _infile->open(_filename.c_str(), data.GetEventType().c_str());
        _infile->set_current_event(_current_event_number[branch_name]);
    }
    (*_infile) >> branchName(branch_name.c_str()) >> data;
    ++_current_event_number[branch_name];
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

bool InputCppRoot::use_event(std::string event) {
    if (event == "")
        return false;
    if (_event_type == _job_header_tp || _event_type == _job_footer_tp)
        return true;  // job header/footer
    Json::Value json = JsonWrapper::StringToJson(event);
    if (json.isMember("run_number") &&
        json["run_number"].asInt() != _current_run_number[_event_type]) {
        cache_event(_event_type, event);
        _current_run_number[_event_type] = json["run_number"].asInt();
        return false;  // run_number has changed - save this for next time
    } else {
        return true;  // run_number is same - keep on extracting this data
    }
}

std::string InputCppRoot::uncache_event(event_type type) {
    std::map<event_type, std::string>::iterator it = _cache.find(type);
    std::string event = "";
    if (it != _cache.end()) {
        event = it->second;
        _cache[it->first] = "";
    }
    return event;
}

void InputCppRoot::cache_event(event_type type, std::string event) {
    std::map<event_type, std::string>::iterator it = _cache.find(type);
    if (it != _cache.end() && it->second != "")
        throw(Exception(Exception::recoverable,
                     "Trying to uncache event but none saved",
                     "InputCppRoot::uncache_event"));
    _cache[type] = event;
}


std::string InputCppRoot::advance_event_type() {
      if (_event_type == _run_footer_tp) {
          if (_cache[_run_header_tp] == "" && _cache[_spill_tp] == "" &&
                                              _cache[_run_footer_tp] == "") {
              _event_type = _job_footer_tp;
          } else {
              _event_type = _run_header_tp;
          }
      } else if (_event_type == _job_footer_tp) {
          return "";
      } else {
          _event_type = event_type(_event_type+1);
      }
      return _emitter_cpp();
}
}  // namespace MAUS

