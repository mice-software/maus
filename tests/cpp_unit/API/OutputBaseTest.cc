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
#include "src/common_cpp/API/OutputBase.hh"

namespace MAUS {


  class MyOutputter : public OutputBase<int*> {
    public:
      MyOutputter() : OutputBase<int*>("TestClass") {}
      MyOutputter(const MyOutputter& mr) : OutputBase<int*>(mr) {}
      virtual ~MyOutputter() {}

    private:
      virtual void _birth(const std::string&) {}
      virtual void _death() {}

      virtual bool _save(int* i) {
        if (!i) { throw NullInputException(_classname); }
        return *i == 27? true : false;
      }

    private:
      FRIEND_TEST(OutputBaseTest, TestConstructor);
      FRIEND_TEST(OutputBaseTest, TestCopyConstructor);
  };

  class MyOutputter_exception : public MyOutputter {
    public:
      MyOutputter_exception() : MyOutputter() {}

    private:
      virtual bool _save(int* i) {
        throw Exception(Exception::recoverable,
           "Expected Test Exception in _save",
           "int* _save(int* t) const");
      }
  };

  class MyOutputter_exception : public MyOutputter {
    public:
      MyOutputter_exception() : MyOutputter() {}

    private:
      virtual bool _save(int* i) {
        throw std::exception();
      }
  };

  class MyOutputter_otherexcept : public MyOutputter {
    public:
      MyOutputter_otherexcept() : MyOutputter() {}

    private:
      virtual bool _save(int* i) {throw 17;}
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
    catch(...) {
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
    catch(...) {
      ASSERT_TRUE(false)
        <<"Fail: Death function failed. Check ModuleBaseTest"
        << std::endl;
    }
  }

  TEST(OutputBaseTest, TestSaveSpill) {
    MyOutputter mm;

    int* i1 = new int(27);
    int* i2 = new int(19);

    ASSERT_TRUE(mm.save(i1))
      <<"Fail: _save method not called properly didn't return 'true' for save(new int(27))"
      <<std::endl;

    ASSERT_FALSE(mm.save(i2))
      <<"Fail: _save method not called properly didn't return 'false' for save(new int(19))"
      <<std::endl;

    /////////////////////////////////////////////////////
    MyOutputter mm2;
    int* dub = 0;
    ASSERT_FALSE(mm2.save(dub));
    /////////////////////////////////////////////////////
    MyOutputter_exception mm_s;
    try {
      mm_s.save(i1);
    }
    catch(...) {
      ASSERT_TRUE(false)
        << "Fail: Exception should have been handled"
        << std::endl;
    }

    /////////////////////////////////////////////////////
    MyOutputter_exception mm_e;
    try {
      mm_e.save(i1);
    }
    catch(...) {
      ASSERT_TRUE(false)
        << "Fail: Exception should have been handled"
        << std::endl;
    }

    /////////////////////////////////////////////////////
    MyOutputter_otherexcept mm_oe;
    try {
      mm_oe.save(i1);
      ASSERT_TRUE(false)
        << "Fail: No exception thrown"
        << std::endl;
    }
    catch(UnhandledException& e) {}
    catch(...) {
      ASSERT_TRUE(false)
        << "Fail: Expected exception of type UnhandledException to be thrown"
        << std::endl;
    }

    delete i1;
    delete i2;
  }
}// end of namespace
