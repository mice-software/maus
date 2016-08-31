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
#include <Python.h>

#include <fstream>
#include <sstream>
#include <cstdlib>
#include "json/json.h"
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"

namespace MAUS {
  class DataMapper : public MapBase<Data> {
  public:
    DataMapper() : MapBase<Data>("DataClass") {}
    DataMapper(const DataMapper& mm) : MapBase<Data>(mm) {}
    virtual ~DataMapper() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual void _process(Data* data) const {
        int run_number = data->GetSpill()->GetRunNumber();
        data->GetSpill()->SetRunNumber(run_number+1);
    }

  private:
    FRIEND_TEST(MapBaseTest, TestConstructor);
    FRIEND_TEST(MapBaseTest, TestCopyConstructor);
  };

  class PyObjectMapper : public MapBase<PyObject> {
  public:
    PyObjectMapper() : MapBase<PyObject>("JsonClass") {}
    PyObjectMapper(const PyObjectMapper& mm) : MapBase<PyObject>(mm) {}
    virtual ~PyObjectMapper() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual void _process(PyObject* data) const {
        PyObject* py_run = PyDict_GetItemString(data, "run_number");
        int run = PyLong_AsLong(py_run);
        py_run = PyLong_FromLong(run+1);
        PyDict_SetItemString(data, "run_number", py_run);
    }

  private:
    FRIEND_TEST(MapBaseTest, TestConstructor);
    FRIEND_TEST(MapBaseTest, TestCopyConstructor);
  };


  class DataMapper_maus_exception : public DataMapper {
  public:
    DataMapper_maus_exception() : DataMapper() {}

  private:
    virtual void _process(Data* data) const {
      throw Exceptions::Exception(Exceptions::recoverable,
		   "Expected Test Exceptions::Exception in _process",
		   "int* _process(double* t) const");
    }
  };

  class DataMapper_exception : public DataMapper {
  public:
    DataMapper_exception() : DataMapper() {}

  private:
    virtual void _process(Data* data) const {
      throw std::exception();
    }
  };

  class DataMapper_otherexcept : public DataMapper {
  public:
    DataMapper_otherexcept() : DataMapper() {}

  private:
    virtual void _process(Data* t) const {throw 17;}
  };

  TEST(MapBaseTest, TestConstructor) {
    DataMapper m;
    EXPECT_EQ("DataClass", m._classname);
  }

  TEST(MapBaseTest, TestCopyConstructor) {
    DataMapper tc1;
    DataMapper tc2(tc1);
    EXPECT_EQ("DataClass", tc2._classname);
  }

  TEST(MapBaseTest, TestBirth) {
    DataMapper tc1;
    tc1.birth("TestConfig");
  }

  TEST(MapBaseTest, TestDeath) {
    DataMapper tc1;
    tc1.death();
  }

  PyObject* data_as_pyobj() {
    Data* data_in = new Data();
    Spill* spill = new Spill();
    data_in->SetSpill(spill);
    EXPECT_EQ(spill->GetRunNumber(), 0);
    // py_data_0 now owns memory allocated to data_in
    PyObject* py_data_0 = PyObjectWrapper::wrap(data_in);
    return py_data_0;
  }

  TEST(MapBaseTest, TestDataMapperProcessData) {
    DataMapper map;
    PyObject* py_data_0 = data_as_pyobj();
    PyObject* py_data_1 = map.process_pyobj(py_data_0);
    Data* data_out = PyObjectWrapper::unwrap<Data>(py_data_1);
    EXPECT_EQ(data_out->GetSpill()->GetRunNumber(), 1);
    EXPECT_EQ(py_data_1->ob_refcnt, 1);
    EXPECT_EQ(py_data_0->ob_refcnt, 1);
    delete data_out;
    Py_DECREF(py_data_1);
    Py_DECREF(py_data_0);
  }

  TEST(MapBaseTest, TestPyObjectMapperProcessData) {
    PyErr_Clear();
    PyObjectMapper map;
    PyObject* py_data_0 = data_as_pyobj();
    PyObject* py_object_0 = PyObjectWrapper::unwrap<PyObject>(py_data_0);
    PyObject* py_object_1 = map.process_pyobj(py_object_0);
    Data* data_out = PyObjectWrapper::unwrap<Data>(py_object_1);
    EXPECT_EQ(data_out->GetSpill()->GetRunNumber(), 1);
    EXPECT_EQ(py_object_1->ob_refcnt, 1);
    EXPECT_EQ(py_object_0->ob_refcnt, 1);
    EXPECT_EQ(py_data_0->ob_refcnt, 1);
    delete data_out;
    Py_DECREF(py_object_1);
    Py_DECREF(py_object_0);
    Py_DECREF(py_data_0);
  }

  TEST(MapBaseTest, TestExceptionHandlingProcess) {
    PyObject* py_data_0 = data_as_pyobj();
    DataMapper_maus_exception().process_pyobj(py_data_0);
    DataMapper_exception().process_pyobj(py_data_0);
    EXPECT_THROW(DataMapper_otherexcept().process_pyobj(py_data_0), Exceptions::Exception);
  }
}  // end of namespace

