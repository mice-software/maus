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

#include "src/common_cpp/Converter/DataConverters/JsonCppConverter.hh"

#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppJobHeaderConverter.hh"

#include "src/output/OutputCppRoot/OutputCppRoot.hh"

namespace MAUS {
OutputCppRoot::OutputCppRoot() : OutputBase<std::string>("OutputCppRoot"),
     _outfile(NULL), _fname(""), _outfile_branch("") {
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
    _outfile = new orstream
                  (_fname.c_str(), "JobHeader", "MAUS output data", "RECREATE");
    _outfile_branch = "JobHeader";
  } catch(...) {
    death();
    throw;
  }
}

template <class ConverterT, class DataT>
bool OutputCppRoot::write_event(MAUSEvent<DataT>* data_cpp,
                                const Json::Value& data_json,
                                std::string branch_name) {
    // watch for double frees (does close() delete data_cpp?)
    if (_outfile == NULL) {
        throw(Squeal(Squeal(
          Squeal::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::save_job_header"
        )));
    }
    if (branch_name == "")
        return false;
    std::string data_type = data_cpp->GetEventType();
    if (_outfile_branch != data_type) {
        _outfile->close();
        _outfile->open
              (_fname.c_str(), data_type.c_str(), "MAUS output data", "UPDATE");
        _outfile_branch = data_type;
        (*_outfile) << branchName(branch_name.c_str()) << data_cpp;
    }

    data_cpp->SetEvent(ConverterT()(&data_json));
    if (data_cpp->GetEvent() == NULL) {  // failed on conversion
        return false;
    }
    try {
        (*_outfile) << fillEvent;
    } catch(...) {
        if (data_cpp != NULL)
            data_cpp->SetEvent(NULL);  // double free?
        throw; // raise the exception
    }
    data_cpp->SetEvent(NULL);  // double free?
    return true;
}

bool OutputCppRoot::_save(std::string data_str) {
    Json::Value data_json = JsonWrapper::StringToJson(data_str);
    event_type my_tp = get_event_type(data_json);
    switch (my_tp) {
        case _job_header_tp:
            return write_event<JsonCppHeaderConverter, JobHeader>
                                    (&_job_header_cpp, data_json, "job_header");
        case _spill_tp:
            return write_event<JsonCppConverter, Spill>
                                               (&_spill_cpp, data_json, "data");
    }
    return false;
}

OutputCppRoot::event_type OutputCppRoot::get_event_type
                                                (const Json::Value& data_json) {
    std::string type = JsonWrapper::GetProperty
             (data_json, "maus_event_type", JsonWrapper::stringValue).asString();
    if (type == "Spill") {
        return _spill_tp;
    } else if (type == "JobHeader") {
        return _job_header_tp;
    } else {
        throw Squeal(Squeal::recoverable,
                     "Failed to recognise maus_event_type "+type,
                     "OutputCppRoot::get_event_type");
    }
}

void OutputCppRoot::_death() {
  if (_outfile != NULL) {
    if (_outfile->is_open())
        _outfile->close();
    delete _outfile;
    _outfile = NULL;  // deletes spill
  }
}
}

