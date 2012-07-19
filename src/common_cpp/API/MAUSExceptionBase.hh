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
#ifndef _SRC_COMMON_CPP_API_MAUSEXCEPTIONBASE_
#define _SRC_COMMON_CPP_API_MAUSEXCEPTIONBASE_
#include <exception>
#include <string>
#include "gtest/gtest_prod.h"

namespace MAUS {

  class MAUSExceptionBase: public std::exception {

  public:
    explicit MAUSExceptionBase(const std::string& classname) :
      std::exception(), _classname(classname) {}
    virtual ~MAUSExceptionBase() throw() {}

  public:
    const char* what() const throw() {
      return _what();
    }

  protected:
    std::string _classname;

  private:
    virtual const char* _what() const throw() {
      std::string* ret = new std::string();
      *ret += "A MAUSException was thrown by '";
      *ret += _classname;
      *ret += "'";
      return ret->c_str();
    }
    FRIEND_TEST(MAUSExceptionBaseTest, TestMausExceptionBaseConstructor);
    FRIEND_TEST(MAUSExceptionBaseTest, TestMausExceptionBase_What);
    FRIEND_TEST(MAUSExceptionBaseTest, TestMausExceptionBaseWhat);
  };

}// end of namespace
#endif
