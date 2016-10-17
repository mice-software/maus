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
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "src/common_cpp/API/InputBase.hh"

namespace MAUS {


  class MyInputter : public InputBase<int*> {
  public:
    MyInputter() : InputBase<int*>("TestClass") {}
    MyInputter(const MyInputter& mr) : InputBase<int*>(mr) {}
    virtual ~MyInputter() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual int* _emit_cpp() {
      return new int(27);
    }


  private:
    FRIEND_TEST(InputBaseTest, TestConstructor);
    FRIEND_TEST(InputBaseTest, TestCopyConstructor);
  };

  class MyInputter_maus_exception : public MyInputter {
  public:
    MyInputter_maus_exception() : MyInputter() {}

  private:
    virtual int* _emit_cpp() {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                   "Expected Test MAUS::Exceptions::Exception in _emit_cpp",
                   "int* _emit_cpp()");
    }
  };

  class MyInputter_exception : public MyInputter {
  public:
    MyInputter_exception() : MyInputter() {}

  private:
    virtual int* _emit_cpp() {
      throw std::exception();
    }
  };

  class MyInputter_otherexcept : public MyInputter {
  public:
    MyInputter_otherexcept() : MyInputter() {}

  private:
    virtual int* _emit_cpp() {throw 17;}
  };

  TEST(InputBaseTest, TestConstructor) {
    MyInputter m;

    ASSERT_FALSE(strcmp("TestClass", m._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(InputBaseTest, TestCopyConstructor) {
    MyInputter tc1;
    MyInputter tc2(tc1);

    ASSERT_FALSE(strcmp("TestClass", tc2._classname.c_str()))
      << "Fail: Copy Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(InputBaseTest, TestBirth) {
    MyInputter tc1;
    try {
      tc1.birth("TestConfig");
    }
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: Birth function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(InputBaseTest, TestDeath) {
    MyInputter tc1;
    try {
      tc1.death();
    }
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: Death function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(InputBaseTest, TestEmitter) {
    MyInputter mm;

    int* i = mm.emit_cpp();

    ASSERT_TRUE(*i == 27)
      << "Fail: _emit_cpp method not called properly"
      << std::endl;

    /////////////////////////////////////////////////////
    MyInputter_maus_exception mm_s;
    try {
      mm_s.emit_cpp();
    }
    catch (NoInputException& e) {}  // A MAUS exception stops the inputter
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: MAUS::Exceptions::Exception should have been handled"
        << std::endl;
    }

    /////////////////////////////////////////////////////
    MyInputter_exception mm_e;
    try {
      mm_e.emit_cpp();
    }
    catch (NoInputException& e) {}  // A MAUS exception stops the inputter
    catch (...) {
      ASSERT_TRUE(false)
        << "Fail: MAUS::Exceptions::Exception should have been handled"
        << std::endl;
    }

    /////////////////////////////////////////////////////
    MyInputter_otherexcept mm_oe;
    try {
      mm_oe.emit_cpp();
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

    delete i;
  }

}// end of namespace
