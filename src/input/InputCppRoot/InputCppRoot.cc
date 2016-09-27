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
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/JobHeaderData.hh"
#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/DataStructure/JobFooterData.hh"
#include "src/common_cpp/DataStructure/JobFooter.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooterData.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"

#include "src/common_cpp/Converter/DataConverters/CppJsonJobFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonJobHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonRunFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonRunHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"
#include "src/common_cpp/API/PyWrapInputBase.hh"

#include "src/input/InputCppRoot/InputCppRoot.hh"

namespace MAUS {
PyMODINIT_FUNC init_InputCppRoot(void) {
    PyWrapInputBase<MAUS::InputCppRoot>::PyWrapInputBaseModInit(
                  "InputCppRoot", "", "", "", "");
}

InputCppRoot::InputCppRoot(std::string filename)
            : InputBase<std::string>("InputCppRoot"), _infile(NULL),
              _infile_tree(""), _filename(filename),
              _event_type(_job_header_tp),
              _select_spills(false) {
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

  if (json_dc.isMember("selected_spills")) {
    Json::Value spill_list = JsonWrapper::GetProperty(json_dc,
                  "selected_spills", JsonWrapper::arrayValue);
    if (!spill_list.isArray()) {
      _select_spills = false;
      Squeak::mout(Squeak::warning) << "Expected array of spill numbers"
        << " for \"selected_spills\" data card.\n"
        << "Assuming None Supplied" << std::endl;
    } else {
      for (unsigned int i = 0; i < spill_list.size(); ++i)
        _selected_spill_numbers.insert(JsonWrapper::GetItem(spill_list, i,
                                              JsonWrapper::intValue).asInt());
      if (_selected_spill_numbers.size() == 0) {
        _select_spills = false;
        Squeak::mout(Squeak::info) << "Loading all spills from data file"
          << std::endl;
      } else {
        _select_spills = true;
        Squeak::mout(Squeak::info) << "Found " << _selected_spill_numbers.size()
          << " spills in the \"selected_spills\" data card." << std::endl;
      }
    }
  }
}

void InputCppRoot::_death() {
  if (_infile != NULL) {
    delete _infile;
    _infile = NULL;
  }
}

std::string InputCppRoot::_emit_cpp() {
    std::string event = "";
    bool isUseful = true;
    JobHeaderData JHData;
    RunHeaderData RHData;
    Data spillData;
    RunFooterData RFData;
    JobFooterData JFData;
    try {
        // first check the cache
        event = uncache_event(_event_type);
        if (event != "") {
            return event;
        }
        // not cached? then we'd better extract from the file
        switch (_event_type) {
            case _job_header_tp:
                isUseful = load_event<JobHeaderData>(std::string("job_header"), JHData);
                if (isUseful) event = convert_data<CppJsonJobHeaderConverter,
                                                                            JobHeaderData>(JHData);
            break;

            case _run_header_tp:
                isUseful = load_event<RunHeaderData>(std::string("run_header"), RHData);
                if (isUseful) {
                    if (RHData.GetRunHeader()->GetRunNumber() !=
                                                                _current_run_number[_event_type]) {
                        cache_event(_event_type, convert_data<CppJsonRunHeaderConverter,
                                                                           RunHeaderData>(RHData));
                        _current_run_number[_event_type] = RHData.GetRunHeader()->GetRunNumber();
                        isUseful = false;
                    } else {
                        event = convert_data<CppJsonRunHeaderConverter, RunHeaderData>(RHData);
                    }
                }
                break;

            case _spill_tp:
                do {
                    isUseful  = load_event<Data>(std::string("data"), spillData);

                    if (isUseful) {
                        if (!is_selected_spill(spillData.GetEvent()->GetSpillNumber())) {
                            isUseful = false;
                        } else if (spillData.GetEvent()->GetRunNumber() !=
                                                                _current_run_number[_event_type]) {
                            cache_event(_event_type, convert_data<CppJsonSpillConverter,
                                                                                 Data>(spillData));
                            _current_run_number[_event_type] =
                                                              spillData.GetEvent()->GetRunNumber();
                            isUseful = false;
                            break;
                        } else {
                            event = convert_data<CppJsonSpillConverter, Data>(spillData);
                        }
                    } else {
                        break;
                    }
                } while (!isUseful);
                break;

            case _run_footer_tp:
                isUseful = load_event<RunFooterData>(std::string("run_footer"), RFData);
                if (isUseful) {
                    if (RFData.GetRunFooter()->GetRunNumber() !=
                                                                _current_run_number[_event_type]) {
                        cache_event(_event_type, convert_data<CppJsonRunFooterConverter,
                                                                           RunFooterData>(RFData));
                        _current_run_number[_event_type] = RFData.GetRunFooter()->GetRunNumber();
                        isUseful = false;
                    } else {
                        event = convert_data<CppJsonRunFooterConverter, RunFooterData>(RFData);
                    }
                }
                break;

            case _job_footer_tp:
                isUseful = load_event<JobFooterData>(std::string("job_footer"), JFData);
                if (isUseful) event = convert_data<CppJsonJobFooterConverter,
                                                                            JobFooterData>(JFData);
                break;

            default:
                isUseful = false;
                break;
        }
        if (!isUseful) {
            // event number changed or we ran out of events of this type
            // advance event and recursively call emitter_cpp
            return advance_event_type();
        }
    // if there was no branch of this type at all then we might get an exception
    // - let's just try the next type
    } catch (const std::exception& exc) {
        std::cerr << (&exc)->what() << std::endl;
        return advance_event_type();
    }
    return event;
}

template <class DataT>
bool InputCppRoot::load_event(std::string branch_name, DataT& data) {
    if (_infile == NULL) {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "InputCppRoot was not initialised properly",
        "InputCppRoot::load_event"
     ));
    }
    if (_current_event_number.find(branch_name) == _current_event_number.end())
        _current_event_number[branch_name] = 0;
    if (_infile_tree != data.GetEventType()) {
        _infile->close();
        _infile_tree = data.GetEventType();
        _infile->open(_filename.c_str(), data.GetEventType().c_str());
        _infile->set_current_event(_current_event_number[branch_name]);
    }
    (*_infile) >> branchName(branch_name.c_str()) >> data;
    // if there was no branch of this type at all then we might get an exception
    // - let's just try the next type
    ++_current_event_number[branch_name];
    if ((*_infile) >> readEvent == NULL) {
        return false;
    }
    return true;
}

template <class ConverterT, class DataT>
std::string InputCppRoot::convert_data(DataT& data) {
    std::string event("");
    Json::Value* value = ConverterT()(&data);
    if (value == NULL)
        return event;
    if (value->isNull()) {
        delete value;
        return event;
    }
    Json::FastWriter writer;
    event = writer.write(*value);
    delete value;
    return event;
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
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Trying to cache event but cannot find event type in map",
                     "InputCppRoot::uncache_event"));
    _cache[type] = event;
}


bool InputCppRoot::is_selected_spill(int spillNum) const {
  if (!_select_spills) return true;

  if (_selected_spill_numbers.find(spillNum) ==
     _selected_spill_numbers.end()) {
    // Squeak::mout(Squeak::info) << "Skipping Spill " << spillNum << std::endl;
    return false;
  }
  return true;
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
      return _emit_cpp();
}
}  // namespace MAUS

