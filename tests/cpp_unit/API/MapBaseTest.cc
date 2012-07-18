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
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "json/json.h"
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
// #include "Interface/Squeal.hh"
// #include "API/APIExceptions.hh"

namespace MAUS {


  class MyMapper : public MapBase<double, int> {
  public:
    MyMapper() : MapBase<double, int>("TestClass") {}
    MyMapper(const MyMapper& mm) : MapBase<double, int>(mm) {}
    virtual ~MyMapper() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual int* _process(double* t) const {
      return new int(*t);
    }

  private:
    FRIEND_TEST(MapBaseTest, TestConstructor);
    FRIEND_TEST(MapBaseTest, TestCopyConstructor);
  };

  class MyMapper_squeal : public MyMapper {
  public:
    MyMapper_squeal() : MyMapper() {}

  private:
    virtual int* _process(double* t) const {
      throw Squeal(Squeal::recoverable,
		   "Expected Test Squeal in _process",
		   "int* _process(double* t) const");
    }
  };

  class MyMapper_exception : public MyMapper {
  public:
    MyMapper_exception() : MyMapper() {}

  private:
    virtual int* _process(double* t) const {
      throw std::exception();
    }
  };

  class MyMapper_otherexcept : public MyMapper {
  public:
    MyMapper_otherexcept() : MyMapper() {}

  private:
    virtual int* _process(double* t) const {throw 17;}
  };

  class MyMapper_converter : public MapBase<Spill, int> {
  public:
    MyMapper_converter() : MapBase<Spill, int>("TestClass") {}
    MyMapper_converter(const MyMapper_converter& mm) : MapBase<Spill, int>(mm) {}
    virtual ~MyMapper_converter() {}

  private:
    virtual void _birth(const std::string&) {}
    virtual void _death() {}

    virtual int* _process(Spill* t) const {
      return 0;
    }
  };


  TEST(MapBaseTest, TestConstructor) {
    MyMapper m;

    ASSERT_FALSE(strcmp("TestClass", m._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(MapBaseTest, TestCopyConstructor) {
    MyMapper tc1;
    MyMapper tc2(tc1);

    ASSERT_FALSE(strcmp("TestClass", tc2._classname.c_str()))
      << "Fail: Copy Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(MapBaseTest, TestBirth) {
    MyMapper tc1;
    try {
      tc1.birth("TestConfig");
    }
    catch(...) {
      ASSERT_TRUE(false)
	<<"Fail: Birth function failed. Check ModuleBaseTest"
	<< std::endl;
    }
  }

  TEST(MapBaseTest, TestDeath) {
    MyMapper tc1;
    try {
      tc1.death();
    }
    catch(...) {
      ASSERT_TRUE(false)
	<<"Fail: Death function failed. Check ModuleBaseTest"
	<< std::endl;
    }
  }

  TEST(MapBaseTest, TestSameTypeProcess) {
    MyMapper mm;

    double* d = new double(27.3445);

    int* i = mm.process(d);

    ASSERT_TRUE(*i == 27)
      <<"Fail: _process method not called properly"
      <<std::endl;

    /////////////////////////////////////////////////////
    MyMapper mm2;
    try {
      double* dub = 0;
      mm2.process(dub);
      ASSERT_TRUE(false)
	<< "Fail: No exception thrown"
	<< std::endl;
    }
    catch(NullInputException& e) {}
    catch(...) {
      ASSERT_TRUE(false)
	<< "Fail: Expected exception of type NullInputException to be thrown"
	<< std::endl;
    }
    /////////////////////////////////////////////////////
    MyMapper_squeal mm_s;
    try {
      mm_s.process(d);
    }
    catch(...) {
      ASSERT_TRUE(false)
	<< "Fail: Squeal should have been handled"
	<< std::endl;
    }

    /////////////////////////////////////////////////////
    MyMapper_exception mm_e;
    try {
      mm_e.process(d);
    }
    catch(...) {
      ASSERT_TRUE(false)
	<< "Fail: Exception should have been handled"
	<< std::endl;
    }

    /////////////////////////////////////////////////////
    MyMapper_otherexcept mm_oe;
    try {
      mm_oe.process(d);
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

    delete d;
    delete i;
  }

  TEST(MapBaseTest, TestOtherTypeProcess) {
    Json::Value *jv = new Json::Value(Json::objectValue);
    Json::Reader r;
    std::stringstream ss;
    ss << getenv("MAUS_ROOT_DIR")
       << "/tests/cpp_unit/API/example_load_json_file.json";

    std::ifstream f(ss.str().c_str(),
		    std::ios::in|std::ios::binary);
    bool b = r.parse(f, *jv);
    ASSERT_TRUE(b)
      << "Fail: Problem parsing the test JSON file."
      << r.getFormatedErrorMessages()
      << std::endl;
    MyMapper_converter mm_c;
    ASSERT_FALSE(mm_c.process(jv) == 0)
      << "Fail: Processing of alternate input type failed possibly "
      << "as a result of conversion failure."
      << std::endl;
    delete jv;
  }

}// end of namespace
