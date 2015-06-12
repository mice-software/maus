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
#include "src/common_cpp/API/ReduceBase.hh"

namespace MAUS {


  class MyReducer : public ReduceBase<int, size_t> {
  public:
    MyReducer() : ReduceBase<int, size_t>("TestClass") {}
    MyReducer(const MyReducer& mr) : ReduceBase<int, size_t>(mr) {}
    virtual ~MyReducer() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual size_t* _process(int* t) {
      return new size_t(abs(*t));
  }

  private:
    FRIEND_TEST(ReduceBaseTest, TestConstructor);
    FRIEND_TEST(ReduceBaseTest, TestCopyConstructor);
  };

  class MyReducer_maus_exception : public MyReducer {
  public:
    MyReducer_maus_exception() : MyReducer() {}

  private:
    virtual size_t* _process(int* t) {
      throw MAUS::Exception(MAUS::Exception::recoverable,
		   "Expected Test MAUS::Exception in _process",
		   "int* _process(int* t) const");
    }
  };

  class MyReducer_exception : public MyReducer {
  public:
    MyReducer_exception() : MyReducer() {}

  private:
    virtual size_t* _process(int* t) {
      throw std::exception();
    }
  };

  class MyReducer_otherexcept : public MyReducer {
  public:
    MyReducer_otherexcept() : MyReducer() {}

  private:
    virtual size_t* _process(int* t) {throw 17;}
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

    int* i = new int(-27);

    size_t* i2 = mm.process(i);

    ASSERT_TRUE(*i2 == 27)
      <<"Fail: _process method not called properly"
      << *i2 << std::endl;


    /////////////////////////////////////////////////////
    MyReducer mm2;
    try {
      int* dub = 0;
      mm2.process(dub);
      ASSERT_TRUE(false)
	<< "Fail: No exception thrown"
	<< std::endl;
    }
    catch (NullInputException& e) {}
    catch (...) {
      ASSERT_TRUE(false)
	<< "Fail: Expected exception of type NullInputException to be thrown"
	<< std::endl;
    }
    /////////////////////////////////////////////////////
    MyReducer_maus_exception mm_s;
    try {
      mm_s.process(i);
    }
    catch (...) {
      ASSERT_TRUE(false)
	<< "Fail: MAUS::Exception should have been handled"
	<< std::endl;
    }

    /////////////////////////////////////////////////////
    MyReducer_exception mm_e;
    try {
      mm_e.process(i);
    }
    catch (...) {
      ASSERT_TRUE(false)
	<< "Fail: MAUS::Exception should have been handled"
	<< std::endl;
    }

    /////////////////////////////////////////////////////
    MyReducer_otherexcept mm_oe;
    try {
      mm_oe.process(i);
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
    delete i2;
  }

}// end of namespace
