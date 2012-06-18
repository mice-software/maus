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
#ifndef _MAUS_API_EXCEPTIONS
#define _MAUS_API_EXCEPTIONS
#include <exception>
#include <string>
#include "gtest/gtest_prod.h"

namespace MAUS {

  class NullInputException: public std::exception {

  public:
    explicit NullInputException(const std::string& classname) :
      std::exception(), _classname(classname) {}

    virtual ~NullInputException() throw() {}

  public:
    virtual const char* what() const throw() {
      std::string* ret = new std::string();
      *ret += "The input to '";
      *ret += _classname;
      *ret += "' was a null pointer";
      return ret->c_str();
    }

  protected:
    std::string _classname;

  private:
    FRIEND_TEST(APIExceptionsTest, TestNullInputExceptionConstructor);
    FRIEND_TEST(APIExceptionsTest, TestNullInputExceptionWhat);
  };

  class UnhandledException: public std::exception {

  public:
    explicit UnhandledException(const std::string& classname) :
      std::exception(), _classname(classname) {}

    virtual ~UnhandledException() throw() {}

  public:
    virtual const char* what() const throw() {
      std::string* ret = new std::string();
      *ret +="An unhandled exception was thrown by '";
      *ret += _classname;
      *ret += "' which was not of type 'Squeal' or deriving from 'std::exception'";
      return ret->c_str();
    }

  protected:
    std::string _classname;

  private:
    FRIEND_TEST(APIExceptionsTest, TestUnhandledExceptionConstructor);
    FRIEND_TEST(APIExceptionsTest, TestUnhandledExceptionWhat);
  };

}// end of namespace

#endif
