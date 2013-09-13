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

#include "Utils/Exception.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppStreamer/ORStream.hh"

#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppJobHeaderConverter.hh"

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppRunHeaderConverter.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/common_cpp/DataStructure/RunFooter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppRunFooterConverter.hh"

#include "src/common_cpp/DataStructure/JobFooter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppJobFooterConverter.hh"

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
    _outfile->close();
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
        throw(Exception(Exception(
          Exception::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::write_event"
        )));
    }
    if (branch_name == "")
        return false;
    std::string data_type = data_cpp->GetEventType();
    if (_outfile_branch != data_type) {
        if (_outfile->is_open())
            _outfile->close();
        _outfile->open
              (_fname.c_str(), data_type.c_str(), "MAUS output data", "UPDATE");
        _outfile_branch = data_type;
    }
    (*_outfile) << branchName(branch_name.c_str()) << data_cpp;
    ConverterT conv;
    data_cpp = conv.convert(&data_json);
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
    return true;
}

bool OutputCppRoot::_save(std::string data_str) {
    Json::Value data_json = JsonWrapper::StringToJson(data_str);
    event_type my_tp = get_event_type(data_json);
    switch (my_tp) {
        case _job_header_tp:
            return write_event<JsonCppJobHeaderConverter, JobHeader>
                                    (&_job_header_cpp, data_json, "job_header");
        case _run_header_tp:
            return write_event<JsonCppRunHeaderConverter, RunHeader>
                                    (&_run_header_cpp, data_json, "run_header");
        case _spill_tp:
            return write_event<JsonCppSpillConverter, Spill>
                                               (&_spill_cpp, data_json, "data");
        case _run_footer_tp:
            return write_event<JsonCppRunFooterConverter, RunFooter>
                                    (&_run_footer_cpp, data_json, "run_footer");
        case _job_footer_tp:
            return write_event<JsonCppJobFooterConverter, JobFooter>
                                    (&_job_footer_cpp, data_json, "job_footer");
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
    } else if (type == "JobFooter") {
        return _job_footer_tp;
    } else if (type == "RunHeader") {
        return _run_header_tp;
    } else if (type == "RunFooter") {
        return _run_footer_tp;
    } else {
        throw Exception(Exception::recoverable,
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

