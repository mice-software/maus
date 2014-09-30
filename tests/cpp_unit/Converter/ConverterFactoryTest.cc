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

namespace MAUS {
std::string converter_factory_test_str =
    std::string("{\"daq_event_type\":\"\",\"errors\":{},")+
    std::string("\"maus_event_type\":\"Spill\",\"run_number\":99,")+
    std::string("\"spill_number\":666}");


class ConverterFactoryTest : public ::testing::Test {
 protected:
  ConverterFactoryTest()
    : _data(NULL), _json(new std::string("{}")), _nothing(new std::string("abc")) {
      _data = new std::string(converter_factory_test_str);
  }

  ~ConverterFactoryTest() {
      if (_data)
          delete _data;
      if (_json)
          delete _json;
      if (_nothing)
          delete _nothing;
  }

  std::string* _data;
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


/////////////// JSON TO X /////////////////////////////
TEST_F(ConverterFactoryTest, TestJsonToX) {
    Json::Value* json_data =
                    ConverterFactory().convert<std::string, Json::Value>(_data);
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

    // Json to MAUS
    MAUS::Data* maus_value = NULL;
    maus_value = ConverterFactory().convert<Json::Value, MAUS::Data>(json_data);
    EXPECT_EQ(maus_value->GetSpill()->GetRunNumber(), 99);
    delete maus_value;
    try {
        ConverterFactory().convert<Json::Value, MAUS::Data>(json_json);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}
    try {
        ConverterFactory().convert<Json::Value, MAUS::Data>(NULL);
        EXPECT_TRUE(false) << "SHOULD HAVE THROWN";
    } catch (std::exception& exc) {}

    delete json_json;
    delete json_data;
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

/////////////// PyDict TO X /////////////////////////////
TEST_F(ConverterFactoryTest, TestPyDictToX) {
    PyObject* py_data = ConverterFactory().convert<std::string, PyObject>(_data);
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
    std::string* str_val =
                    ConverterFactory().convert<std::string, std::string>(_data);
    ConverterFactory::delete_type(str_val);
}
}

