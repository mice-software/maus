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

#include "src/legacy/Interface/Squeal.hh"

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
     _outfile(NULL), _fname(""), _end_of_run_dir("./"), _outfile_branch(""), 
     _run_numbers(), _mode(one_big_file) {
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
    if (branch_name == "")
        return false;

    ConverterT conv;
    data_cpp = conv.convert(&data_json);
    check_file_exists(data_cpp);
    if (_outfile == NULL) {
        throw(Squeal(
          Squeal::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::write_event"
        ));
    }

    std::cerr << "JSON EV TYPE " << data_json["maus_event_type"] <<
              << "CPP EV TYPE " << data_cpp->GetEventType() << std::endl;
    std::string data_type = data_cpp->GetEventType();
    if (_outfile_branch != data_type) {
        if (_outfile->is_open())
            _outfile->close();
        int run = run_number(data_cpp);
        _outfile->open(file_name(run).c_str(), data_type.c_str(),
                       "MAUS output data", "UPDATE");
        _outfile_branch = data_type;
        std::cerr << "Reopened "+_outfile_branch << std::endl;
    }
    std::cerr << "Writing to "+_outfile_branch << std::endl;
    (*_outfile) << branchName(branch_name.c_str()) << data_cpp;

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

std::string OutputCppRoot::file_name(int run_number) {
    std::string run_numb_str = STLUtils::ToString(run_number);
    switch (_mode) {
        case one_big_file:
            return _fname;
        case one_file_per_run: {
            size_t dot = _fname.find_last_of('.');
            if (dot == std::string::npos) {   // file -> file_1234
                return _fname+"_"+run_numb_str;
            } else {  // file.root -> file_1234.root
                std::string one = _fname.substr(0, dot);
                std::string two = _fname.substr(dot, _fname.size() - dot);
                return one+"_"+run_numb_str+two;
            }
            }
        case end_of_run_file_per_run:
            return _end_of_run_dir+"/"+run_numb_str+"/"+_fname;
    }
    return "";
}

template <class DataT>
void OutputCppRoot::check_file_exists(DataT data_cpp) {
    std::string event = data_cpp->GetEventType();
    int run = run_number(data_cpp);
    switch (_mode) {
        case one_big_file:
            if (_run_numbers.size() > 0)
                return;
        case one_file_per_run:
        case end_of_run_file_per_run:
            if (std::binary_search(_run_numbers.begin(), _run_numbers.end(), run))
                return;
    }
    std::cerr << "Creating file "+file_name(run)+" with event type "+data_cpp->GetEventType() << std::endl;
    _outfile = new orstream(file_name(run).c_str(),
                            data_cpp->GetEventType().c_str(),
                            "MAUS output data", "RECREATE");
    _outfile_branch = data_cpp->GetEventType();
    std::vector<int>::iterator it = std::upper_bound(_run_numbers.begin(), _run_numbers.end(), run);
    _run_numbers.insert(it, run);
}

template <>
int OutputCppRoot::run_number(MAUSEvent<Spill>* data_cpp) {
    return data_cpp->GetEvent()->GetRunNumber();
}

template <>
int OutputCppRoot::run_number(MAUSEvent<RunHeader>* data_cpp) {
    return data_cpp->GetEvent()->GetRunNumber();
}

template <>
int OutputCppRoot::run_number(MAUSEvent<RunFooter>* data_cpp) {
    return data_cpp->GetEvent()->GetRunNumber();
}

template <>
int OutputCppRoot::run_number(MAUSEvent<JobHeader>* data_cpp) {
    return 0;
}

template <>
int OutputCppRoot::run_number(MAUSEvent<JobFooter>* data_cpp) {
    return 0;
}
}

