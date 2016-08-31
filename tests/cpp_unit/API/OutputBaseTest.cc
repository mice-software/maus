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
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "src/common_cpp/API/OutputBase.hh"

namespace MAUS {


  class MyOutputter : public OutputBase {
    public:
      MyOutputter() : OutputBase("TestClass") {}
      MyOutputter(const MyOutputter& mr) : OutputBase(mr) {}
      virtual ~MyOutputter() {}

    private:
      virtual void _birth(const std::string&) {}
      virtual void _death() {}

      virtual bool _save(PyObject* i) {
        if (!i) { throw NullInputException(_classname); }
        return i == Py_True;
      }

    private:
      FRIEND_TEST(OutputBaseTest, TestConstructor);
      FRIEND_TEST(OutputBaseTest, TestCopyConstructor);
  };

  class MyOutputter_maus_exception : public MyOutputter {
    public:
      MyOutputter_maus_exception() : MyOutputter() {}

    private:
      virtual bool _save(PyObject* i) {
        throw Exceptions::Exception(Exceptions::recoverable,
           "Expected Test Exceptions::Exception in _save",
           "int* _save(int* t) const");
      }
  };

  class MyOutputter_exception : public MyOutputter {
    public:
      MyOutputter_exception() : MyOutputter() {}

    private:
      virtual bool _save(PyObject* i) {
        throw std::exception();
      }
  };

  class MyOutputter_otherexcept : public MyOutputter {
    public:
      MyOutputter_otherexcept() : MyOutputter() {}

    private:
      virtual bool _save(PyObject* i) {throw 17;}
  };

  TEST(OutputBaseTest, TestConstructor) {
    MyOutputter m;

    ASSERT_FALSE(strcmp("TestClass", m._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(OutputBaseTest, TestCopyConstructor) {
    MyOutputter tc1;
    MyOutputter tc2(tc1);

    ASSERT_FALSE(strcmp("TestClass", tc2._classname.c_str()))
      << "Fail: Copy Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(OutputBaseTest, TestBirth) {
    MyOutputter tc1;
    try {
      tc1.birth("TestConfig");
    }
    catch (...) {
      ASSERT_TRUE(false)
        <<"Fail: Birth function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(OutputBaseTest, TestDeath) {
    MyOutputter tc1;
    try {
      tc1.death();
    }
    catch (...) {
      ASSERT_TRUE(false)
        <<"Fail: Death function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(OutputBaseTest, TestSaveSpill) {
    MyOutputter mm;

    ASSERT_EQ(Py_True, mm.save_pyobj(Py_True))
      <<"Fail: _save method not called properly didn't return 'true' for save(new int(27))"
      <<std::endl;

    ASSERT_EQ(Py_False, mm.save_pyobj(Py_False))
      <<"Fail: _save method not called properly didn't return 'false' for save(new int(19))"
      <<std::endl;

    /////////////////////////////////////////////////////
    MyOutputter mm2;
    ASSERT_EQ(Py_False, mm2.save_pyobj(NULL));
    /////////////////////////////////////////////////////
    MyOutputter_maus_exception mm_s;
    try {
      mm_s.save_pyobj(Py_True);
    }
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: Exceptions::Exception should have been handled"
        << std::endl;
    }

    /////////////////////////////////////////////////////
    MyOutputter_exception mm_e;
    try {
      mm_e.save_pyobj(Py_True);
    }
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: Exceptions::Exception should have been handled"
        << std::endl;
    }

    /////////////////////////////////////////////////////
    MyOutputter_otherexcept mm_oe;
    try {
      mm_oe.save_pyobj(Py_True);
      ASSERT_TRUE(false)
        << "Fail: No exception thrown"
        << std::endl;
    }
    catch (UnhandledException& e) {}
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: Expected exception of type UnhandledException to be thrown"
        << std::endl;
    }
  }
}// end of namespace

