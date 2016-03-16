/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

#include "gtest/gtest.h"
#include "json/json.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Converter/ConverterFactory.hh"

#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/DataStructure/JobFooter.hh"
#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"

namespace MAUS {
std::string data_test_str =
    std::string("{\"daq_event_type\":\"\",\"errors\":{},")+
    std::string("\"event_id\":1,")+
    std::string("\"maus_event_type\":\"Spill\",\"run_number\":99,")+
    std::string("\"spill_number\":666,\"time_stamp\":1}");

std::string jh_test_str =
  std::string("{")+
  std::string("\"start_of_job\":{\"date_time\":\"1976-04-04T00:00:00.000000\"},")+
  std::string("\"bzr_configuration\":\"\", ")+
  std::string("\"bzr_revision\":\"\", ")+
  std::string("\"bzr_status\":\"\", ")+
  std::string("\"maus_version\":\"99\", ")+
  std::string("\"json_configuration\":\"output cpp root test\", ")+
  std::string("\"maus_event_type\":\"JobHeader\"")+
  std::string("}");

std::string jf_test_str =
  std::string("{")+
  std::string("\"end_of_job\":{\"date_time\":\"1976-04-04T00:00:00.000000\"},")+
  std::string("\"maus_event_type\":\"JobFooter\"")+
  std::string("}");

std::string rh_test_str =
  std::string("{")+
  std::string("\"run_number\":-7, ")+
  std::string("\"maus_event_type\":\"RunHeader\"")+
  std::string("}");

std::string rf_test_str =
  std::string("{")+
  std::string("\"run_number\":-7, ")+
  std::string("\"maus_event_type\":\"RunFooter\"")+
  std::string("}");

class ConverterFactoryTest : public ::testing::Test {
 protected:
  ConverterFactoryTest()
    : _data(NULL), _jh(NULL), _jf(NULL), _rh(NULL), _rf(NULL),
      _json(new std::string("{}")), _nothing(new std::string("abc")) {
      _data = new std::string(data_test_str);
      _jh = new std::string(jh_test_str);
      _jf = new std::string(jf_test_str);
      _rh = new std::string(rh_test_str);
      _rf = new std::string(rf_test_str);
  }

  ~ConverterFactoryTest() {
      if (_data)
          delete _data;
      if (_jh)
          delete _jh;
      if (_jf)
          delete _jf;
      if (_rh)
          delete _rh;
      if (_rf)
          delete _rf;
      if (_json)
          delete _json;
      if (_nothing)
          delete _nothing;
  }

  std::string* _data;
  std::string* _jh;
  std::string* _jf;
  std::string* _rh;
  std::string* _rf;
  std::string* _json;
  std::string* _nothing;
};

// Note: EXPECT_THROW does not work for me (it fails to interpret the template
// argument I think)

////////////////////// STRING TO X ///////////////////////////////

TEST_F(ConverterFactoryTest, TestStringToJson) {
    Json::Value* json_value =
                    ConverterFactory().convert<std::string, Json::Value>(_data);
    EXPECT_EQ((*json_value)["run_number"], 99);
    delete json_value;
    json_value = ConverterFactory().convert<std::string, Json::Value>(_json);
    EXPECT_EQ(*json_value, Json::Value(Json::objectValue));
    delete json_value;
    try {
        ConverterFactory().convert<std::string, Json::Value>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToString) {
    std::string* str_value = NULL;
    str_value = ConverterFactory().convert<std::string, std::string>(_data);
    EXPECT_EQ(*str_value, *_data);
    delete str_value;
    str_value = ConverterFactory().convert<std::string, std::string>(_json);
    EXPECT_EQ(*str_value, *_json);
    delete str_value;
    str_value = ConverterFactory().convert<std::string, std::string>(_nothing);
    EXPECT_EQ(*str_value, *_nothing);
    delete str_value;
    try {
        ConverterFactory().convert<std::string, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToPython) {
    PyObject* py_value = NULL;
    py_value = ConverterFactory().convert<std::string, PyObject>(_data);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    EXPECT_EQ(py_value->ob_refcnt, 1);
    // py_run_number is a borrowed ref (so no decref)
    PyObject* py_run_number = PyDict_GetItemString(py_value, "run_number");
    EXPECT_TRUE(py_run_number != NULL);
    int run_number =  PyInt_AsLong(py_run_number);
    EXPECT_EQ(run_number, 99);
    Py_DECREF(py_value);
    py_value = ConverterFactory().convert<std::string, PyObject>(_json);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<std::string, PyObject>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToData) {
    MAUS::Data* maus_value = NULL;
    maus_value = ConverterFactory().convert<std::string, MAUS::Data>(_data);
    EXPECT_EQ(maus_value->GetSpill()->GetRunNumber(), 99);
    delete maus_value;
    try {
        ConverterFactory().convert<std::string, Data>(_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, Data>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, Data>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToJobHeader) {
    MAUS::JobHeaderData* jh_value = NULL;
    jh_value = ConverterFactory().convert<std::string, JobHeaderData>(_jh);
    EXPECT_EQ(std::string(jh_value->GetJobHeader()->GetMausVersionNumber()),
              std::string("99"));
    delete jh_value;
    try {
        ConverterFactory().convert<std::string, JobHeaderData>(_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, JobHeaderData>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, JobHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToJobFooter) {
    MAUS::JobFooterData* jf_value = NULL;
    jf_value = ConverterFactory().convert<std::string, JobFooterData>(_jf);
    delete jf_value;
    try {
        ConverterFactory().convert<std::string, JobFooterData>(_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, JobFooterData>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, JobFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToRunHeader) {
    MAUS::RunHeaderData* rh_value = NULL;
    rh_value = ConverterFactory().convert<std::string, RunHeaderData>(_rh);
    EXPECT_EQ(rh_value->GetRunHeader()->GetRunNumber(),
              -7);
    delete rh_value;
    try {
        ConverterFactory().convert<std::string, RunHeaderData>(_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, RunHeaderData>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, RunHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestStringToRunFooter) {
    MAUS::RunFooterData* rf_value = NULL;
    rf_value = ConverterFactory().convert<std::string, RunFooterData>(_rf);
    delete rf_value;
    try {
        ConverterFactory().convert<std::string, RunFooterData>(_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, RunFooterData>(_nothing);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<std::string, RunFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}



/////////////// JSON TO X /////////////////////////////
TEST_F(ConverterFactoryTest, TestJsonToX) {

    Json::Value* json_data =
                    ConverterFactory().convert<std::string, Json::Value>(_data);
    Json::Value* json_jh =
                    ConverterFactory().convert<std::string, Json::Value>(_jh);
    Json::Value* json_jf =
                    ConverterFactory().convert<std::string, Json::Value>(_jf);
    Json::Value* json_rh =
                    ConverterFactory().convert<std::string, Json::Value>(_rh);
    Json::Value* json_rf =
                    ConverterFactory().convert<std::string, Json::Value>(_rf);
    Json::Value* json_json =
                    ConverterFactory().convert<std::string, Json::Value>(_json);
    // Json to string
    std::string* str_value = NULL;
    str_value = ConverterFactory().convert<Json::Value, std::string>(json_data);
    EXPECT_EQ(*str_value, *_data);
    delete str_value;
    str_value = ConverterFactory().convert<Json::Value, std::string>(json_json);
    EXPECT_EQ(*str_value, *_json);
    delete str_value;
    try {
        ConverterFactory().convert<Json::Value, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // Json to Json
    Json::Value* json_value = NULL;
    json_value = ConverterFactory().convert<Json::Value, Json::Value>(json_data);
    EXPECT_EQ((*json_value)["run_number"], 99);
    delete json_value;
    json_value = ConverterFactory().convert<Json::Value, Json::Value>(json_json);
    EXPECT_EQ(*json_value, *json_json);
    delete json_value;
    try {
        ConverterFactory().convert<Json::Value, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // Json to PyDict
    PyObject* py_value = NULL;
    py_value = ConverterFactory().convert<Json::Value, PyObject>(json_data);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    py_value = ConverterFactory().convert<Json::Value, PyObject>(json_json);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<Json::Value, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // Json to Data
    MAUS::Data* data_value = NULL;
    data_value = ConverterFactory().convert<Json::Value, MAUS::Data>(json_data);
    EXPECT_EQ(data_value->GetSpill()->GetRunNumber(), 99);
    delete data_value;
    try {
        ConverterFactory().convert<Json::Value, MAUS::Data>(json_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<Json::Value, MAUS::Data>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    delete json_data;

    // Json to JobHeaderData
    MAUS::JobHeaderData* jh_value = NULL;
    jh_value = ConverterFactory().convert<Json::Value, JobHeaderData>(json_jh);
    EXPECT_EQ(std::string(jh_value->GetJobHeader()->GetMausVersionNumber()),
              std::string("99"));
    delete jh_value;
    try {
        ConverterFactory().convert<Json::Value, JobHeaderData>(json_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<Json::Value, JobHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    delete json_jh;

    // Json to JobFooterData
    MAUS::JobFooterData* jf_value = NULL;
    jf_value = ConverterFactory().convert<Json::Value, JobFooterData>(json_jf);
    delete jf_value;
    try {
        ConverterFactory().convert<Json::Value, JobFooterData>(json_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<Json::Value, JobFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    delete json_jf;

    // Json to RunHeaderData
    MAUS::RunHeaderData* rh_value = NULL;
    rh_value = ConverterFactory().convert<Json::Value, RunHeaderData>(json_rh);
    EXPECT_EQ(rh_value->GetRunHeader()->GetRunNumber(), -7);
    delete rh_value;
    try {
        ConverterFactory().convert<Json::Value, RunHeaderData>(json_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<Json::Value, RunHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    delete json_rh;

    // Json to RunFooterData
    MAUS::RunFooterData* rf_value = NULL;
    rf_value = ConverterFactory().convert<Json::Value, RunFooterData>(json_rf);
    EXPECT_EQ(rf_value->GetRunFooter()->GetRunNumber(), -7);
    delete rf_value;
    try {
        ConverterFactory().convert<Json::Value, RunFooterData>(json_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<Json::Value, RunFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    delete json_rf;

    delete json_json;
}

/////////////// Data TO X /////////////////////////////
TEST_F(ConverterFactoryTest, TestDataToX) {
    Data* maus_data = ConverterFactory().convert<std::string, MAUS::Data>(_data);

    // Data to string
    std::string* str_value =
                      ConverterFactory().convert<Data, std::string>(maus_data);
    EXPECT_EQ(*str_value, *_data);
    delete str_value;
    try {
        ConverterFactory().convert<Data, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // Data to Json
    Json::Value* json_value =
                      ConverterFactory().convert<Data, Json::Value>(maus_data);
    EXPECT_EQ((*json_value)["run_number"], 99);
    delete json_value;
    try {
        ConverterFactory().convert<Data, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // Data to PyObject
    PyObject* py_value = ConverterFactory().convert<Data, PyObject>(maus_data);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<Data, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // Data to Data
    Data* maus_value = ConverterFactory().convert<Data, Data>(maus_data);
    EXPECT_EQ(maus_value->GetSpill()->GetRunNumber(), 99);
    delete maus_value;
    try {
        ConverterFactory().convert<Data, Data>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    delete maus_data;
}

TEST_F(ConverterFactoryTest, TestJobHeaderToX) {
    JobHeaderData* jobhead = ConverterFactory().
                                 convert<std::string, JobHeaderData>(_jh);
    // JobHeaderData to string
    std::string* str_jh = ConverterFactory().
                                 convert<JobHeaderData, std::string>(jobhead);
    EXPECT_NE((*str_jh).find("\"maus_version\":\"99\""), std::string::npos);
    delete str_jh;
    try {
        ConverterFactory().convert<JobHeaderData, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // JobHeaderData to json
    Json::Value* json_jh = ConverterFactory().
                                 convert<JobHeaderData, Json::Value>(jobhead);
    EXPECT_EQ((*json_jh)["maus_version"], "99");
    delete json_jh;
    try {
        ConverterFactory().convert<JobHeaderData, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // JobHeaderData to PyObject
    PyObject* py_value = ConverterFactory().
                                      convert<JobHeaderData, PyObject>(jobhead);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<JobHeaderData, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // JobHeaderData to JobHeaderData
    JobHeaderData* jh_value = ConverterFactory().
                                 convert<JobHeaderData, JobHeaderData>(jobhead);
    EXPECT_EQ(jh_value->GetJobHeader()->GetMausVersionNumber(),
              jobhead->GetJobHeader()->GetMausVersionNumber());
    delete jh_value;

    try {
        ConverterFactory().convert<JobHeaderData, JobHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestJobFooterToX) {
    JobFooterData* jobfoot = ConverterFactory().
                                 convert<std::string, JobFooterData>(_jf);
    // JobFooterData to string
    std::string* str_jf = ConverterFactory().
                                 convert<JobFooterData, std::string>(jobfoot);
    EXPECT_EQ(*str_jf, *_jf);
    delete str_jf;
    try {
        ConverterFactory().convert<JobFooterData, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // JobFooterData to json
    Json::Value* json_jf = ConverterFactory().
                                 convert<JobFooterData, Json::Value>(jobfoot);
    EXPECT_EQ((*json_jf)["maus_event_type"], "JobFooter");
    delete json_jf;
    try {
        ConverterFactory().convert<JobFooterData, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // JobFooterData to PyObject
    PyObject* py_value = ConverterFactory().
                                      convert<JobFooterData, PyObject>(jobfoot);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<JobFooterData, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // JobFooterData to JobFooterData
    JobFooterData* jf_value = ConverterFactory().
                                 convert<JobFooterData, JobFooterData>(jobfoot);
    EXPECT_EQ(jf_value->GetJobFooter()->GetEndOfJob().GetDateTime(),
              jobfoot->GetJobFooter()->GetEndOfJob().GetDateTime());
    delete jf_value;

    try {
        ConverterFactory().convert<JobFooterData, JobFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestRunHeaderToX) {
    RunHeaderData* runhead = ConverterFactory().
                                 convert<std::string, RunHeaderData>(_rh);
    // RunHeaderData to string
    std::string* str_rh = ConverterFactory().
                                 convert<RunHeaderData, std::string>(runhead);
    EXPECT_NE((*str_rh).find("\"run_number\":-7"), std::string::npos);
    delete str_rh;
    try {
        ConverterFactory().convert<RunHeaderData, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // RunHeaderData to json
    Json::Value* json_rh = ConverterFactory().
                                 convert<RunHeaderData, Json::Value>(runhead);
    EXPECT_EQ((*json_rh)["run_number"], -7);
    delete json_rh;
    try {
        ConverterFactory().convert<RunHeaderData, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // RunHeaderData to PyObject
    PyObject* py_value = ConverterFactory().
                                      convert<RunHeaderData, PyObject>(runhead);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<RunHeaderData, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // RunHeaderData to RunHeaderData
    RunHeaderData* rh_value = ConverterFactory().
                                 convert<RunHeaderData, RunHeaderData>(runhead);
    EXPECT_EQ(rh_value->GetRunHeader()->GetRunNumber(),
              runhead->GetRunHeader()->GetRunNumber());
    delete rh_value;

    try {
        ConverterFactory().convert<RunHeaderData, RunHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

TEST_F(ConverterFactoryTest, TestRunFooterToX) {
    RunFooterData* runfoot = ConverterFactory().
                                 convert<std::string, RunFooterData>(_rf);
    // RunFooterData to string
    std::string* str_rf = ConverterFactory().
                                 convert<RunFooterData, std::string>(runfoot);
    EXPECT_NE((*str_rf).find("\"run_number\":-7"), std::string::npos);
    delete str_rf;
    try {
        ConverterFactory().convert<RunFooterData, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // RunFooterData to json
    Json::Value* json_rf = ConverterFactory().
                                 convert<RunFooterData, Json::Value>(runfoot);
    EXPECT_EQ((*json_rf)["maus_event_type"], "RunFooter");
    delete json_rf;
    try {
        ConverterFactory().convert<RunFooterData, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // RunFooterData to PyObject
    PyObject* py_value = ConverterFactory().
                                      convert<RunFooterData, PyObject>(runfoot);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<RunFooterData, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // RunFooterData to RunFooterData
    RunFooterData* rf_value = ConverterFactory().
                                 convert<RunFooterData, RunFooterData>(runfoot);
    EXPECT_EQ(rf_value->GetRunFooter()->GetRunNumber(),
              runfoot->GetRunFooter()->GetRunNumber());
    delete rf_value;

    try {
        ConverterFactory().convert<RunFooterData, RunFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
}

/////////////// PyDict TO X /////////////////////////////
TEST_F(ConverterFactoryTest, TestPyDictToX) {
    PyObject* py_data = ConverterFactory().convert<std::string, PyObject>(_data);
    PyObject* py_jh = ConverterFactory().convert<std::string, PyObject>(_jh);
    PyObject* py_jf = ConverterFactory().convert<std::string, PyObject>(_jf);
    PyObject* py_rh = ConverterFactory().convert<std::string, PyObject>(_rh);
    PyObject* py_rf = ConverterFactory().convert<std::string, PyObject>(_rf);
    PyObject* py_json = ConverterFactory().convert<std::string, PyObject>(_json);

    // PyObject to string
    std::string* str_value = NULL;
    str_value = ConverterFactory().convert<PyObject, std::string>(py_data);
    Data* maus_test = ConverterFactory().convert<std::string, Data>(str_value);
    EXPECT_EQ(maus_test->GetSpill()->GetRunNumber(), 99);
    delete maus_test;
    delete str_value;
    str_value = ConverterFactory().convert<PyObject, std::string>(py_json);
    EXPECT_EQ(*str_value, *_json);
    delete str_value;
    try {
        ConverterFactory().convert<PyObject, std::string>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to PyObject
    PyObject* py_value = NULL;
    py_value = ConverterFactory().convert<PyObject, PyObject>(py_data);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    py_value = ConverterFactory().convert<PyObject, PyObject>(py_json);
    EXPECT_EQ(PyDict_Check(py_value), 1);
    Py_DECREF(py_value);
    try {
        ConverterFactory().convert<PyObject, PyObject>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to Data
    MAUS::Data* maus_value = NULL;
    maus_value = ConverterFactory().convert<PyObject, MAUS::Data>(py_data);
    EXPECT_EQ(maus_value->GetSpill()->GetRunNumber(), 99);
    delete maus_value;
    try {
        ConverterFactory().convert<PyObject, Data>(py_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<PyObject, Data>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to JobHeader
    JobHeaderData* maus_jh = NULL;
    maus_jh = ConverterFactory().convert<PyObject, JobHeaderData>(py_jh);
    EXPECT_EQ(std::string(maus_jh->GetJobHeader()->GetMausVersionNumber()),
              std::string("99"));
    delete maus_jh;
    try {
        ConverterFactory().convert<PyObject, JobHeaderData>(py_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<PyObject, JobHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to JobFooter
    JobFooterData* maus_jf = NULL;
    maus_jf = ConverterFactory().convert<PyObject, JobFooterData>(py_jf);
    delete maus_jf;
    try {
        ConverterFactory().convert<PyObject, JobFooterData>(py_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<PyObject, JobFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to RunHeader
    RunHeaderData* maus_rh = NULL;
    maus_rh = ConverterFactory().convert<PyObject, RunHeaderData>(py_rh);
    EXPECT_EQ(maus_rh->GetRunHeader()->GetRunNumber(), -7);
    delete maus_rh;
    try {
        ConverterFactory().convert<PyObject, RunHeaderData>(py_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<PyObject, RunHeaderData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to RunFooter
    RunFooterData* maus_rf = NULL;
    maus_rf = ConverterFactory().convert<PyObject, RunFooterData>(py_rf);
    EXPECT_EQ(maus_rf->GetRunFooter()->GetRunNumber(), -7);
    delete maus_rf;
    try {
        ConverterFactory().convert<PyObject, RunFooterData>(py_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<PyObject, RunFooterData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    // PyObject to Json
    Json::Value* json_value = NULL;
    json_value = ConverterFactory().convert<PyObject, Json::Value>(py_data);
    EXPECT_EQ((*json_value)["run_number"], 99);
    delete json_value;
    json_value = ConverterFactory().convert<PyObject, Json::Value>(py_json);
    EXPECT_EQ(*json_value, Json::Value(Json::objectValue));
    delete json_value;
    try {
        ConverterFactory().convert<PyObject, Json::Value>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    Py_DECREF(py_json);
    Py_DECREF(py_data);
}

/////////////// ImageData TO ImageData /////////////////////////////
TEST_F(ConverterFactoryTest, TestImageData) {
    ImageData ref;
    Image* image = new Image();
    image->SetRunNumber(99);
    ref.SetImage(image);
    ImageData* test = ConverterFactory().convert<ImageData, ImageData>(&ref);
    EXPECT_EQ(ref.GetImage()->GetRunNumber(), test->GetImage()->GetRunNumber());
    try {
        ConverterFactory().convert<ImageData, ImageData>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    delete test;
}

//////// delete_type //////////////////
TEST_F(ConverterFactoryTest, TestDeleteType) {
    PyObject* py_val = ConverterFactory().convert<std::string, PyObject>(_data);
    EXPECT_EQ(py_val->ob_refcnt, 1);
    ConverterFactory::delete_type(py_val);
    EXPECT_EQ(py_val->ob_refcnt, 0);
    Json::Value* json_val =
                    ConverterFactory().convert<std::string, Json::Value>(_data);
    ConverterFactory::delete_type(json_val);
    Data* maus_val = ConverterFactory().convert<std::string, Data>(_data);
    ConverterFactory::delete_type(maus_val);
    JobHeaderData* maus_jh = ConverterFactory().convert<std::string, JobHeaderData>(_jh);
    ConverterFactory::delete_type(maus_jh);
    JobFooterData* maus_jf = ConverterFactory().convert<std::string, JobFooterData>(_jf);
    ConverterFactory::delete_type(maus_jf);
    RunHeaderData* maus_rh = ConverterFactory().convert<std::string, RunHeaderData>(_rh);
    ConverterFactory::delete_type(maus_rh);
    RunFooterData* maus_rf = ConverterFactory().convert<std::string, RunFooterData>(_rf);
    ConverterFactory::delete_type(maus_rf);
    ImageData* image_data = new ImageData();
    ConverterFactory::delete_type(image_data);
    std::string* str_val =
                    ConverterFactory().convert<std::string, std::string>(_data);
    ConverterFactory::delete_type(str_val);
}
}

