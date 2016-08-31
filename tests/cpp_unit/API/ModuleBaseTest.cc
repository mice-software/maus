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
#include <cstring>
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "src/common_cpp/API/APIExceptions.hh"
#include "src/common_cpp/API/ModuleBase.hh"
#include "Utils/Exception.hh"

namespace MAUS {

  class testclass : public ModuleBase {
  public:
    testclass(): ModuleBase("TestClass"), run_birth(false), run_death(false) {}
    testclass(const testclass& tc): ModuleBase(tc), run_birth(false), run_death(false) {}
  private:
    virtual void _birth(const std::string&) {run_birth = true;}
    virtual void _death() {run_death = true;}
    bool run_birth;
    bool run_death;

  private:
    FRIEND_TEST(ModuleBaseTest, TestConstructor);
    FRIEND_TEST(ModuleBaseTest, TestCopyConstructor);
    FRIEND_TEST(ModuleBaseTest, TestBirth);
    FRIEND_TEST(ModuleBaseTest, TestDeath);
  };

  class testclass_maus_exception : public ModuleBase {
  public:
    testclass_maus_exception(): ModuleBase("TestClass") {}
  private:
    virtual void _birth(const std::string&) {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
		   "Expected Test MAUS::Exceptions::Exception in _birth",
		   "void _birth (const std::string&)");
    }
    virtual void _death() {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
		   "Expected Test MAUS::Exceptions::Exception in _death",
		   "void _death ()");
    }

  private:
    FRIEND_TEST(ModuleBaseTest, TestBirth);
    FRIEND_TEST(ModuleBaseTest, TestDeath);
  };

  class testclass_exception : public ModuleBase {
  public:
    testclass_exception(): ModuleBase("TestClass") {}
  private:
    virtual void _birth(const std::string&) {
      throw std::exception();
    }
    virtual void _death() {
      throw std::exception();
    }

  private:
    FRIEND_TEST(ModuleBaseTest, TestBirth);
    FRIEND_TEST(ModuleBaseTest, TestDeath);
  };

  class testclass_otherexcept : public ModuleBase {
  public:
    testclass_otherexcept(): ModuleBase("TestClass") {}
  private:
    virtual void _birth(const std::string&) { throw 17;}
    virtual void _death() {throw 17;}

  private:
    FRIEND_TEST(ModuleBaseTest, TestBirth);
    FRIEND_TEST(ModuleBaseTest, TestDeath);
  };

  TEST(ModuleBaseTest, TestConstructor) {
    testclass tc;

    ASSERT_FALSE(strcmp("TestClass", tc._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(ModuleBaseTest, TestCopyConstructor) {
    testclass tc1;
    testclass tc2(tc1);

    ASSERT_FALSE(strcmp("TestClass", tc2._classname.c_str()))
      << "Fail: Copy Constructor failed, Classname not set properly"
      << std::endl;
  }

  TEST(ModuleBaseTest, TestBirth) {
    testclass tc;

    ASSERT_FALSE(tc.run_birth)
      << "Fail: Didn't set up the run_birth flag properly"
      << std::endl;

    tc.birth("TestConfig");

    ASSERT_TRUE(tc.run_birth)
      << "Fail: Didn't run _birth code."
      << std::endl;
  }

  TEST(ModuleBaseTest, TestDeath) {
    testclass tc;

    ASSERT_FALSE(tc.run_death)
      << "Fail: Didn't set up the run_birth flag properly"
      << std::endl;

    tc.death();

    ASSERT_TRUE(tc.run_death)
      << "Fail: Didn't run _birth code."
      << std::endl;

    /////////////////////////////////////////////////////
  }

}// end of namespace
