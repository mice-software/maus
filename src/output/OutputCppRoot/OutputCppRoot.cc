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
#include "src/common_cpp/Utils/CppErrorHandler.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppConverter.hh"

#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppHeaderConverter.hh"

#include "src/output/OutputCppRoot/OutputCppRoot.hh"

namespace MAUS {
OutputCppRoot::OutputCppRoot() : OutputBase<std::string>("OutputCppRoot"),
     _outfile(NULL), _spill(NULL), _jsonCppConverter(NULL), _header(NULL),
     _jsonCppHeaderConverter(NULL), _fname("") {
}

void OutputCppRoot::_birth(const std::string& json_datacards) {
  try {
    // clean up any allocated memory or throw an exception
    death();
    // load datacards
    Json::Value datacards = JsonWrapper::StringToJson(json_datacards);
    _fname = JsonWrapper::GetProperty(datacards,
                  "output_root_file_name", JsonWrapper::stringValue).asString();
    // Setup output stream
    _outfile = new orstream(_fname.c_str(), "Spill", "MAUS output data", "RECREATE");
    _spill = new Data();
    _jsonCppConverter = new JsonCppConverter();
    _jsonCppHeaderConverter = new JsonCppHeaderConverter();
    (*_outfile) << branchName("data") << _spill;
  } catch(std::exception& exc) {
    death();
    throw exc;
  }
}

bool OutputCppRoot::_save_spill(std::string json_spill_document) {
    if (_jsonCppConverter == NULL || _spill == NULL || _outfile == NULL) {
        throw(Squeal(Squeal(
          Squeal::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::save_spill"
        )));
    }
    if (!_outfile->is_open())
        _outfile->open(_fname.c_str(), "Spill", "MAUS output data", "UPDATE");
    if (json_spill_document != "") {
        Json::Value json_spill = JsonWrapper::StringToJson(json_spill_document);
        _spill->SetSpill( (*_jsonCppConverter)(&json_spill) );
        (*_outfile) << fillEvent;
        return true;
    } else {
        return false;
    }
}

bool OutputCppRoot::_save_job_header(std::string job_header) {
    // watch for double frees (does close() delete _header)?
    if (_jsonCppHeaderConverter == NULL || _header != NULL || _outfile == NULL) {
        throw(Squeal(Squeal(
          Squeal::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::save_job_header"
        )));
    }
    if (job_header == "")
        return false;
    Json::Value json_header = JsonWrapper::StringToJson(job_header);
    try {
        if (_outfile->is_open()) {
            _outfile->close();
        }
        _outfile->open(_fname.c_str(), "JobHeader", "MAUS output data", "UPDATE");
        _header = (*_jsonCppHeaderConverter)(&json_header);
        (*_outfile) << branchName("job_header") << _header;
        (*_outfile) << fillEvent;
        _outfile->close();
    } catch(...) {
        _outfile->close();
        if (_header != NULL)
            delete _header;  // double free?
        _header = NULL;
        throw; // raise the exception
    }
    delete _header;  // double free?
    _header = NULL;
    return true;
}

void OutputCppRoot::_death() {
  if (_outfile != NULL) {
    if (_outfile->is_open())
        _outfile->close();
    delete _outfile;
    _outfile = NULL;  // deletes spill
    _spill = NULL;
  } else if (_spill != NULL) {
    delete _spill;
    _spill = NULL;
  }
  if (_jsonCppConverter != NULL) {
    delete _jsonCppConverter;
    _jsonCppConverter = NULL;
  }
}
}

