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

#include <Python.h>

#include "gtest/gtest.h"
#include "json/json.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Converter/ConverterFactory.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"

namespace MAUS {
// Stupid C++, why can't I do EXPECT_EQ(PyErr_Occurred(), NULL)?
PyObject* null = NULL;
// force the compiler to build conversions from all supported types...
// This does nothing, just checks we can compile the code
TEST(PyObjectWrapperTest, TestUnwrapNullObject) {
  PyObject* args = NULL;
  EXPECT_THROW(PyObjectWrapper::unwrap<Data>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<JobHeaderData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<JobFooterData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<RunHeaderData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<RunFooterData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<ImageData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<std::string>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<PyObject>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<Json::Value>(args), Exceptions::Exception);
  EXPECT_EQ(PyErr_Occurred(), null);
}

TEST(PyObjectWrapperTest, TestUnwrapBadObject) {
  PyObject* args = PyLong_FromLong(0);
  EXPECT_THROW(PyObjectWrapper::unwrap<Data>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<JobHeaderData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<JobFooterData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<RunHeaderData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<RunFooterData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<ImageData>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<std::string>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<PyObject>(args), Exceptions::Exception);
  EXPECT_THROW(PyObjectWrapper::unwrap<Json::Value>(args), Exceptions::Exception);
  Py_DECREF(args);
  EXPECT_EQ(PyErr_Occurred(), null);
}

template <class TEMP>
void test_unwrap(PyObject* py_data) {
  // Unwrap as data
  TEMP* data_out = PyObjectWrapper::unwrap<TEMP>(py_data);
  EXPECT_EQ(data_out->GetSpill()->GetRunNumber(), 99);
  delete data_out;

  // Unwrap as data
  std::string* str_out = PyObjectWrapper::unwrap<std::string>(py_data);
  Json::Value* json_tmp =
                  ConverterFactory().convert<std::string, Json::Value>(str_out);
  EXPECT_EQ((*json_tmp)["run_number"], 99);
  delete json_tmp;
  delete str_out;

  // Unwrap as Json
  Json::Value* json_out = PyObjectWrapper::unwrap<Json::Value>(py_data);
  EXPECT_EQ((*json_out)["run_number"], 99);
  delete json_out;

  // Unwrap as PyObject
  PyObject* py_out = PyObjectWrapper::unwrap<PyObject>(py_data);
  EXPECT_EQ(py_out->ob_refcnt, 1);
  // py_run_number is a borrowed ref (so no decref)
  PyObject* py_run_number = PyDict_GetItemString(py_out, "run_number");
  int run_number =  PyInt_AsLong(py_run_number);
  EXPECT_EQ(run_number, 99);
  Py_DECREF(py_out);
  EXPECT_EQ(PyErr_Occurred(), null);
}

TEST(PyObjectWrapperTest, TestWrapUnwrapDataObject) {
  std::string test =
    std::string("{\"daq_event_type\":\"\",\"errors\":{},")+
    std::string("\"maus_event_type\":\"Spill\",\"run_number\":99,")+
    std::string("\"spill_number\":666}");
  std::cerr << "Data " << std::flush;
  Data* data_in = ConverterFactory().convert<std::string, Data>(&test);
  PyObject* py_data = PyObjectWrapper::wrap(data_in);
  EXPECT_EQ(py_data->ob_refcnt, 1);
  test_unwrap<Data>(py_data);
  EXPECT_EQ(py_data->ob_refcnt, 1);
  Py_DECREF(py_data);

  std::cerr << "String " << std::flush;
  PyObject* py_str = PyObjectWrapper::wrap(new std::string(test));
  EXPECT_EQ(py_str->ob_refcnt, 1);
  test_unwrap<Data>(py_str);
  Py_DECREF(py_str);

  std::cerr << "Json " << std::flush;
  Json::Value* json_value = ConverterFactory().convert<std::string, Json::Value>
                                                        (new std::string(test));
  PyObject* py_json = PyObjectWrapper::wrap(json_value);
  EXPECT_EQ(py_json->ob_refcnt, 1);
  test_unwrap<Data>(py_json);
  Py_DECREF(py_json);

  std::cerr << "PyObject" << std::endl;
  PyObject* py_dict = ConverterFactory().convert<std::string, PyObject>
                                                        (new std::string(test));
  EXPECT_EQ(py_dict->ob_refcnt, 1);
  PyObject* py_run_number = PyDict_GetItemString(py_dict, "run_number");
  int run_number =  PyInt_AsLong(py_run_number);
  EXPECT_EQ(run_number, 99);

  PyObject* py_py_dict = PyObjectWrapper::wrap(py_dict);
  EXPECT_EQ(py_py_dict->ob_refcnt, 1);
  EXPECT_EQ(py_py_dict, py_dict);  // it was a null op
  test_unwrap<Data>(py_dict);
  Py_DECREF(py_dict);
  EXPECT_EQ(PyErr_Occurred(), null);
}

TEST(PyObjectWrapperTest, TestDeleteJsonCppPyCapsule) {
  EXPECT_THROW(PyObjectWrapper::delete_jsoncpp_pycapsule(NULL), Exceptions::Exception);
  PyObject* py_int = PyLong_FromLong(0);
  EXPECT_THROW(PyObjectWrapper::delete_jsoncpp_pycapsule(py_int), Exceptions::Exception);
  Py_DECREF(py_int);

  Json::Value* json = new Json::Value();
  void* void_json = static_cast<void*>(json);
  PyObject *py_no_name = PyCapsule_New(void_json, "", NULL);
  EXPECT_THROW(PyObjectWrapper::delete_jsoncpp_pycapsule(py_no_name), Exceptions::Exception);
  Py_DECREF(py_no_name);

  PyObject* py_okay = PyCapsule_New(void_json, "JsonCpp", NULL);
  PyObjectWrapper::delete_jsoncpp_pycapsule(py_okay);  // and this deletes json
  Py_DECREF(py_okay);
  EXPECT_EQ(PyErr_Occurred(), null);
}
} // namespace MAUS


