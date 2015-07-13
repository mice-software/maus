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

#include <string>

#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"
#include "src/common_cpp/API/ReduceBase.hh"

namespace MAUS {


  class MyReducer : public ReduceBase<std::string, std::string> {
  public:
    MyReducer() : ReduceBase<std::string, std::string>("TestClass") {}
    MyReducer(const MyReducer& mr) : ReduceBase<std::string, std::string>(mr) {}
    virtual ~MyReducer() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual std::string* _process(std::string* t) {
      return new std::string(*t);
  }

  private:
    FRIEND_TEST(ReduceBaseTest, TestConstructor);
    FRIEND_TEST(ReduceBaseTest, TestCopyConstructor);
  };

  class MyReducer_maus_exception : public MyReducer {
  public:
    MyReducer_maus_exception() : MyReducer() {}

  private:
    virtual std::string* _process(std::string* t) {
      throw MAUS::Exception(MAUS::Exception::recoverable,
           "Expected Test MAUS::Exception in _process",
           "std::string* _process(std::string* t) const");
    }
  };

  class MyReducer_exception : public MyReducer {
  public:
    MyReducer_exception() : MyReducer() {}

  private:
    virtual std::string* _process(std::string* t) {
      throw std::exception();
    }
  };

  class MyReducer_otherexcept : public MyReducer {
  public:
    MyReducer_otherexcept() : MyReducer() {}

  private:
    virtual std::string* _process(std::string* t) {throw 17;}
  };


  TEST(ReduceBaseTest, TestConstructor) {
    MyReducer m;

    ASSERT_FALSE(strcmp("TestClass", m._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(ReduceBaseTest, TestCopyConstructor) {
    MyReducer tc1;
    MyReducer tc2(tc1);

    ASSERT_FALSE(strcmp("TestClass", tc2._classname.c_str()))
      << "Fail: Copy Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(ReduceBaseTest, TestBirth) {
    MyReducer tc1;
    try {
      tc1.birth("TestConfig");
    }
    catch (...) {
      ASSERT_TRUE(false)
        <<"Fail: Birth function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(ReduceBaseTest, TestDeath) {
    MyReducer tc1;
    try {
      tc1.death();
    }
    catch (...) {
      ASSERT_TRUE(false)
        <<"Fail: Death function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(ReduceBaseTest, TestProcess) {
    MyReducer mm;

    std::string* str1 = new std::string("-27");
    PyObject* py_str1 = PyObjectWrapper::wrap(str1); // python owns str1
    PyObject* py_str2 = mm.process_pyobj(py_str1);
    std::string* str2 = PyObjectWrapper::unwrap<std::string>(py_str2);
    EXPECT_EQ(*str2, std::string("-27"))
          <<"Fail: _process method not called properly"
          << *str2 << std::endl;
    delete str2;
    Py_DECREF(py_str2);
    Py_DECREF(py_str1);
  }

  TEST(ReduceBaseTest, TestProcessNULL) {
    MyReducer mm;
    PyObject* py_str1 = NULL;
    PyObject* py_out = mm.process_pyobj(py_str1);
    EXPECT_TRUE(py_out == NULL)
      << "Fail: Python return object not NULL"
      << std::endl;
    PyErr_Clear();
  }

  TEST(ReduceBaseTest, TestProcessMAUSException) {
    std::string* str1 = new std::string("-27");
    PyObject* py_str1 = PyObjectWrapper::wrap(str1); // python owns str1
    MyReducer_maus_exception mm_e;
    try {
      mm_e.process_pyobj(py_str1);
    }
    catch (MAUS::Exception& e) {}
    catch (...) {
      EXPECT_TRUE(false)
        << "Fail: Expected exception of type MAUS::Exception to be thrown"
        << std::endl;
    }
    Py_DECREF(py_str1);
  }

  TEST(ReduceBaseTest, TestProcessStdException) {
    std::string* str1 = new std::string("-27");
    PyObject* py_str1 = PyObjectWrapper::wrap(str1); // python owns str1
    MyReducer_exception mm_e;
    try {
      mm_e.process_pyobj(py_str1);
    }
    catch (MAUS::Exception& e) {}
    catch (...) {
      EXPECT_TRUE(false)
        << "Fail: Expected exception of type MAUS::Exception to be thrown"
        << std::endl;
    }
    Py_DECREF(py_str1);
  }

  TEST(ReduceBaseTest, TestProcessOtherException) {
    std::string* str1 = new std::string("-27");
    PyObject* py_str1 = PyObjectWrapper::wrap(str1); // python owns str1
    MyReducer_otherexcept mm_oe;
    try {
      mm_oe.process_pyobj(py_str1);
      EXPECT_TRUE(false)
        << "Fail: No exception thrown"
        << std::endl;
    }
    catch (UnhandledException& e) {}
    catch (...) {
      EXPECT_TRUE(false)
        << "Fail: Expected exception of type UnhandledException to be thrown"
        << std::endl;
    }
    Py_DECREF(py_str1);
  }

}// end of namespace
