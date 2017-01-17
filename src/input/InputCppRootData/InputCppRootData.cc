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

#include <algorithm>

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

#include "src/input/InputCppRootData/InputCppRootData.hh"

namespace MAUS {
PyMODINIT_FUNC init_InputCppRootData(void) {
    PyWrapInputBase<MAUS::InputCppRootData>::PyWrapInputBaseModInit(
                  "InputCppRootData", "", "", "", "");
}

InputCppRootData::InputCppRootData()
            : InputBase<MAUS::Data>("InputCppRootData"), _infile(NULL),
              _infile_tree(""), _filename_list(),
              _event_type(_job_header_tp),
              _select_spills(false) {
}

InputCppRootData::~InputCppRootData() {
  death();
}

void InputCppRootData::parse_file_name(Json::Value json_dc) {
  _filename_list = std::vector<std::string>();
  Json::Value json_fnames = json_dc["input_root_file_name"];
  if (json_fnames.isArray()) {
      for (size_t i = 0; i < json_fnames.size(); ++i) {
          _filename_list.push_back(JsonWrapper::GetItem(json_fnames, i, JsonWrapper::stringValue).asString());
      }
  } else if (json_fnames.isString()) {
      _filename_list.push_back(json_fnames.asString());
  }
  _filename_it = _filename_list.begin();
}

void InputCppRootData::_birth(const std::string& json_datacards) {
  Json::Value json_dc = JsonWrapper::StringToJson(json_datacards);
  parse_file_name(json_dc);
  std::cerr << "InputCppRootData opening " << *_filename_it << std::endl;
  _infile = new irstream(_filename_it->c_str(), "Spill");
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
        _highest_spill_number = *std::max_element(_selected_spill_numbers.begin(),
                                                 _selected_spill_numbers.end());
        _select_spills = true;
        Squeak::mout(Squeak::info) << "Found " << _selected_spill_numbers.size()
          << " spills in the \"selected_spills\" data card." << std::endl;
      }
    }
  }
}

void InputCppRootData::_death() {
  if (_infile != NULL) {
    delete _infile;
    _infile = NULL;
  }
}

MAUS::Data InputCppRootData::_emit_cpp() {
    Data spillData;
    while (true) {
        bool loaded = load_event<Data>(std::string("data"), spillData);
        if (!loaded) { // read failed, maybe we ran out of data?
            throw NoInputException();
        }
        if (_select_spills == false)
            break; // any spill will do
        int spillNum = spillData.GetSpill()->GetSpillNumber();
        if (_selected_spill_numbers.find(spillNum) !=
           _selected_spill_numbers.end()) {
            break; // we found a spill from the list
        }
        if (spillNum > _highest_spill_number) { // ran out of spills
            throw NoInputException();
        }
    }
    return spillData;
}

template <class DataT>
bool InputCppRootData::load_event(std::string branch_name, DataT& data) {
    if (_infile == NULL) {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "InputCppRootData was not initialised properly",
        "InputCppRootData::load_event"
     ));
    }
    if (_infile_tree != data.GetEventType()) {
        _infile->close();
        _infile_tree = data.GetEventType();
        _infile->open((*_filename_it).c_str(), data.GetEventType().c_str());
    }
    (*_infile) >> branchName(branch_name.c_str()) >> data;
    // if there was no branch of this type at all then we might get an
    // exception - let's just try the next type
    if ((*_infile) >> readEvent == NULL) {
        ++_filename_it;
        if (_filename_it == _filename_list.end())
            return false;
        _infile_tree = ""; // force into the "open new file" branch on recurse
        return load_event(branch_name, data); // try again (recurse)
    }
    return true;
}

bool InputCppRootData::is_selected_spill(int spillNum) const {
  if (!_select_spills) return true;

  if (_selected_spill_numbers.find(spillNum) ==
     _selected_spill_numbers.end()) {
    // Squeak::mout(Squeak::info) << "Skipping Spill " << spillNum << std::endl;
    return false;
  }
  return true;
}
}  // namespace MAUS

