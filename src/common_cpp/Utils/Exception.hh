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

#ifndef SRC_COMMON_CPP_UTILS_EXCEPTION_HH
#define SRC_COMMON_CPP_UTILS_EXCEPTION_HH 1

#include <cstdio>
#include <cstring>

#include <exception>
#include <string>
#include <vector>

#include "Utils/ExceptionLevel.hh"


namespace MAUS {
namespace Exceptions {
/** MAUS exception class
 *
 *  Exception has a severity (exceptionLevel), an error message and a
 *  location. Also function to return the stack trace (gcc only).
 * 
 *  If you aren't using gcc, you need to define NO_STACKTRACE at compile time
 * 
 *  Note the throw() directives are there to explicitly declare that we cannot
 *  throw an exception in this stuff
 */
class Exception : public std::exception {
 public:

  /** constructor - with error level, error message and location
   *
   *  note this makes a stack trace which can be slow.
   */
  Exception(exceptionLevel level, std::string errorMessage, std::string location) throw();
  /** constructor - does nothing */
  Exception() throw();

  /** destructor - does nothing */
  ~Exception() throw() {}

  /** Return char buffer of  message+" at "+location 
   *
   *  Memory remains owned by Exception
   */
  const char* what() const throw() {return &_what[0];}

  /** Print the error message and location
   *
   *  Prints to error message to Squeak::mout(Squeak::error) and Location to
   *  Squeak::mout(Squeak::debug)
   */
  void Print();

  /** Get the severity of the exception */
  inline exceptionLevel GetErrorLevel() const {return _level;}

  /** Get the error message (as defined by constructor) */
  inline std::string GetMessage() const {return _message;}

  /** Set the error message */
  inline void SetMessage(std::string new_message);

  /** Get the location (as defined by Exception constructor) of the error */
  std::string GetLocation() const {return _location;}

  /** Return the stack trace if it was stored */
  std::string GetStackTrace() const {return _stacktrace;}

  /** Gcc-specific code to recover the stack trace as a string.
   *
   *  Will skip traces below skipTrace (so, for example, if we want to know
   *  where the Exception was thrown we might set skipTrace to 2, to skip
   *  MakeStackTrace and Exception constructor).
   */
  static std::string MakeStackTrace(size_t skipTrace);

  /** Set that it makes a stack trace on exception */
  static void SetWillDoStackTrace(bool willDoStackTrace);

  /** Return whether makes a stack trace on exception */
  static bool GetWillDoStackTrace();

 private:
  inline void SetWhat(std::string what_str);

  static const size_t _maxStackSize;
  static bool _willDoStackTrace;

  std::string       _message;
  std::string       _location;
  std::string       _stacktrace;
  std::vector<char> _what;
  exceptionLevel    _level;
};

void Exception::SetMessage(std::string new_message) {
    _message = new_message;
    SetWhat(_message+" at "+_location);
}

void Exception::SetWhat(std::string what_str) {
    _what = std::vector<char>(what_str.size()+1);
    snprintf(&_what[0], what_str.size()+1, "%s", what_str.c_str());
}
}
}

#endif
