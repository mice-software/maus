// MAUS WARNING: THIS IS LEGACY CODE.
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

#ifndef Squeal_hh
#define Squeal_hh 1

#include <cstdio>
#include <cstring>

#include <exception>
#include <string>
#include <vector>

/// Exception handler class
///
/// Exception handler has a severity (exceptionLevel), an error message and a
/// location. Also function to return the stack trace (gcc only).
///
/// If you aren't using gcc, you need to define NO_STACKTRACE

// note the throw() directives are there to explicitly declare that we cannot
// throw an exception in this stuff

class Squeal : public std::exception {
 public:
  /// ExceptionLevel enumerates the severity of the exception.

  /// I use an enumeration to distinguish between different error levels.
  /// * nonRecoverable means the internal state of the programme is no longer
  ///   well-defined i.e. some memory problem or so.
  /// * recoverable means that in principle we could keep on running, although
  ///   most of the time this results in end of run (usually indicates typo in
  ///   an input file).
  /// If we start doing GUIs, then the distinction becomes important. Most stuff
  /// should be recoverable.
  enum exceptionLevel {recoverable, nonRecoverable};

  /// constructor - with error level, error message and location

  /// note this makes a stack trace which can be slow.
  Squeal(exceptionLevel level, std::string errorMessage, std::string location)
                                                                        throw();
  /// constructor - does nothing
  Squeal() throw();

  /// destructor - does nothing
  ~Squeal() throw() {}
  /// Return char buffer of  message+" at "+location
  const char* what() const throw() {return &_what[0];}
  /// Print the Message to Squeak::mout(Squeak::error) and Location to
  /// Squeak::mout(Squeak::debug)
  void Print();
  /// Get the severity of the exception
  exceptionLevel GetErrorLevel() const {return _level;}
  /// Get the error message (as defined by constructor)
  std::string    GetMessage() const {return _message;}
  /// Set the error message
  void           SetMessage(std::string new_message) {
    _message = new_message;
    SetWhat(_message+" at "+_location);
  }
  /// Get the location (as defined by constructor) of the error
  std::string    GetLocation() const {return _location;}
  /// Return the stack trace if it was stored
  std::string    GetStackTrace() const {return _stacktrace;}
  /// Gcc-specific code to recover the stack trace as a string.

  /// Will skip traces below skipTrace (so, for example, if we want to know
  /// where the Squeal was thrown we might set skipTrace to 2, to skip
  /// MakeStackTrace and Squeal constructor).
  static std::string    MakeStackTrace(size_t skipTrace);

 private:

  void SetWhat(std::string what_str) {
    _what = std::vector<char>(what_str.size()+1);
    snprintf(&_what[0], what_str.size()+1, "%s", what_str.c_str());
  }

  static const size_t    _maxStackSize;

  std::string       _message;
  std::string       _location;
  std::string       _stacktrace;
  std::vector<char> _what;
  exceptionLevel    _level;
};


#endif

