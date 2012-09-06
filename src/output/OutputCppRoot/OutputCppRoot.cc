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

#include "src/common_cpp/DataStructure/JobHeaderData.hh"
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
    _outfile = new orstream(_fname.c_str(), "Spill", "MAUS output data", "RECREATE");
  } catch(std::exception& exc) {
    death();
    throw exc;
  }
}

template <class ConverterT, class DataT>
bool OutputCppRoot::write_event(MAUSEvent<DataT>* data_cpp,
                                std::string data_str,
                                std::string branch_name) {
    // watch for double frees (does close() delete data_cpp?)
    if (_outfile == NULL) {
        throw(Squeal(Squeal(
          Squeal::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::save_job_header"
        )));
    }
    if (data_str == "" || branch_name == "")
        return false;
    std::string data_type = data_cpp->GetEventType();
    if (_outfile_branch != data_type) {
        _outfile->close();
        _outfile->open(_fname.c_str(), data_type.c_str(), "MAUS output data", "UPDATE");
        _outfile_branch = data_type;
        (*_outfile) << branchName(branch_name.c_str()) << data_cpp;
    }
    // next two lines could be pulled into a converter<std::string, Spill>
    Json::Value data_json = JsonWrapper::StringToJson(data_str);
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

bool OutputCppRoot::_save_spill(std::string spill) {
    bool success = false;
    Data* spill_data = new Data();
    try {
        success = write_event<JsonCppConverter, Spill>
                                                  (spill_data, spill, "data");
    } catch(...) {
        delete spill_data;
        throw;
    }
    delete spill_data;
    return success;
}

bool OutputCppRoot::_save_job_header(std::string job_header) {
    bool success = false;
    JobHeaderData* jh = new JobHeaderData();
    try {
        success = write_event<JsonCppHeaderConverter, JobHeader>
                                               (jh, job_header, "job_header");
    } catch(...) {
        delete jh;
        throw;
    }
    delete jh;
    return success;
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

