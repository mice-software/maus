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

#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>

#include "Utils/Exception.hh"

#include "src/common_cpp/Utils/PyObjectWrapper.hh"
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

#include "src/common_cpp/API/PyWrapOutputBase.hh"
#include "src/output/OutputCppRoot/OutputCppRoot.hh"

namespace MAUS {
OutputCppRoot::OutputCppRoot() : OutputBase("OutputCppRoot"),
     _outfile(NULL), _fname(""), _end_of_run_dir("./"), _outfile_branch(""),
     _job_header_cpp(NULL), _run_header_cpp(NULL), _spill_cpp(NULL),
     _run_footer_cpp(NULL), _job_footer_cpp(NULL), _run_numbers(),
     _mode(one_big_file) {
}

void OutputCppRoot::_death() {
    if (_outfile != NULL) {
      if (_outfile->is_open())
          _outfile->close();
      delete _outfile;
      _outfile = NULL;  // deletes spill
    }
    if (_job_header_cpp) {
        delete _job_header_cpp;
        _job_header_cpp = NULL;
    }
    if (_run_header_cpp) {
        delete _run_header_cpp;
        _run_header_cpp = NULL;
    }
    if (_spill_cpp) {
        delete _spill_cpp;
        _spill_cpp = NULL;
    }
    if (_run_footer_cpp) {
        delete _run_footer_cpp;
        _run_footer_cpp = NULL;
    }
    if (_job_footer_cpp) {
        delete _job_footer_cpp;
        _job_footer_cpp = NULL;
    }
}

void OutputCppRoot::_birth(const std::string& json_datacards) {
  try {
    // clean up any allocated memory or throw an exception
    death();
    // load datacards
    Json::Value datacards = JsonWrapper::StringToJson(json_datacards);
    _fname = JsonWrapper::GetProperty(datacards,
                  "output_root_file_name", JsonWrapper::stringValue).asString();
    if (_fname == "") {
        throw(Exception(
          Exception::recoverable,
          "output_root_file_name is empty",
          "OutputCppRoot::birth"
        ));
    }
    std::string mode = JsonWrapper::GetProperty(datacards,
                  "output_root_file_mode", JsonWrapper::stringValue).asString();
    if (mode == "one_big_file") {
        _mode = one_big_file;
    } else if (mode == "one_file_per_run") {
        _mode = one_file_per_run;
    } else if (mode == "end_of_run_file_per_run") {
        _mode = end_of_run_file_per_run;
    } else {
        throw(Exception(
          Exception::recoverable,
          "output_root_file_name '"+mode+"' is not valid; should be one of\n"+
          std::string("   one_big_file\n")+
          std::string("   one_file_per_run\n")+
          std::string("   end_of_run_file_per_run\n"),
          "OutputCppRoot::birth"
        ));
    }
    _end_of_run_dir = JsonWrapper::GetProperty(
                                          datacards,
                                          "end_of_run_output_root_directory",
                                          JsonWrapper::stringValue).asString();
  } catch (std::exception& exc) {
    std::cerr << (&exc)->what() << std::endl;
    death();
  } catch (...) {
    death();
    throw;
  }
}

template <class DataT>
bool OutputCppRoot::write_event(PyObject* py_data) {
    DataT* data_cpp = NULL;
    try {
        data_cpp = PyObjectWrapper::unwrap<DataT>(py_data);
        if (data_cpp == NULL)
            return false;
    } catch (Exception) {
        return false;
    }
    check_file_exists(data_cpp);
    if (_outfile == NULL) {
        throw(Exception(
          Exception::recoverable,
          "OutputCppRoot was not initialised properly",
          "OutputCppRoot::write_event"
        ));
    }

    std::string branch_name = get_branch_name(data_cpp);
    std::string data_type = data_cpp->GetEventType();
    if (_outfile_branch != data_type) {
        if (_outfile->is_open())
            _outfile->close();
        int run = run_number(data_cpp);
        _outfile->open(file_name(run).c_str(), data_type.c_str(),
                       "MAUS output data", "UPDATE");
        _outfile_branch = data_type;
    }
    (*_outfile) << branchName(branch_name.c_str()) << data_cpp;

    if (data_cpp->GetEvent() == NULL) {  // failed on conversion
        return false;
    }
    // delete data_cpp;
    // data_cpp = NULL;
    try {
        (*_outfile) << fillEvent;
    } catch (...) {
        if (data_cpp != NULL)
            data_cpp->SetEvent(NULL);  // double free?
        throw; // raise the exception
    }
    if (data_cpp) {
        delete data_cpp;
        data_cpp = NULL;
    }
    return true;
}

bool OutputCppRoot::_save(PyObject* py_data) {
    if (write_event<Data>(py_data))
        return true;
    if (write_event<RunHeaderData>(py_data))
        return true;
    if (write_event<RunFooterData>(py_data))
        return true;
    if (write_event<JobHeaderData>(py_data))
        return true;
    if (write_event<JobFooterData>(py_data))
        return true;
    return false;
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

std::string OutputCppRoot::dir_name(int run_number) {
    std::string file = file_name(run_number);
      size_t slash = file.find_last_of('/');
      if (slash == std::string::npos) {   // file -> file_1234
          return "./";
      } else {  // file.root -> file_1234.root
          return file.substr(0, slash);
      }
}

template <class DataT>
void OutputCppRoot::check_file_exists(DataT* data_cpp) {
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
    if (_outfile != NULL) {
        if (_outfile->is_open())
            _outfile->close();
        delete _outfile;
    }
    struct stat attributes;
    std::string dir = dir_name(run);
    if (stat(dir.c_str(), &attributes) < 0 || !S_ISDIR(attributes.st_mode)) {
        // if stat fails, maybe the directory doesnt exists
        if (mkdir(dir.c_str(), ACCESSPERMS) == -1) {
            throw Exception(Exception::recoverable, "Failed to make directory "+dir,
                         "OutputCppRoot::check_file_exists");
        }
    }
    // S_ISDIR(attributes.st_mode) - returns True if it's a directory...
    _outfile = new orstream(file_name(run).c_str(),
                            data_cpp->GetEventType().c_str(),
                            "MAUS output data", "RECREATE");
    _outfile_branch = data_cpp->GetEventType();
    std::vector<int>::iterator it = std::upper_bound(_run_numbers.begin(), _run_numbers.end(), run);
    _run_numbers.insert(it, run);
}

template <>
int OutputCppRoot::run_number(Data* data_cpp) {
    return data_cpp->GetEvent()->GetRunNumber();
}

template <>
int OutputCppRoot::run_number(RunHeaderData* data_cpp) {
    return data_cpp->GetEvent()->GetRunNumber();
}

template <>
int OutputCppRoot::run_number(RunFooterData* data_cpp) {
    return data_cpp->GetEvent()->GetRunNumber();
}

template <>
int OutputCppRoot::run_number(JobHeaderData* data_cpp) {
    return 0;
}

template <>
int OutputCppRoot::run_number(JobFooterData* data_cpp) {
    return 0;
}

PyMODINIT_FUNC init_OutputCppRoot(void) {
    PyWrapOutputBase<OutputCppRoot>::PyWrapOutputBaseModInit("OutputCppRoot", "", "", "", "");
}
}

