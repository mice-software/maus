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

#ifndef Squeak_hh
#define Squeak_hh 1

#include <ostream>
#include <iostream>
#include <fstream>
#include <map>

#include "json/value.h"

#include "Utils/Exception.hh"
#include "Utils/ExceptionLevel.hh"

namespace MAUS {

/** @class Squeak Designed to redirect output to std::out or file
 *
 *  Usage:
 *  Squeak::mout(Squeak::debug) << "Blah blah" << std::endl;
 *
 *  Squeak is used to redirect output at run time to some output file or
 *  standard outputs. Redirection is controlled by errorLevel:
 *    debug - debugging information. Deliver information related to debugging
 *            errors in the code; verbose developer level output
 *    info - run control information. Deliver information related to run
 *           information; aimed at users to give summary information about the
 *           run (idea is to give a constant, but limited, stream of data to
 *           info)
 *    warning - information to warn the user of suspected errors in running
 *    error - information to warn the user of certain errors in running
 *    fatal - final information to tell the user if the code is crashing
 *
 *  We also redirect output according to exceptionLevel
 *    recoverable - redirects to error
 *    nonRecoverable - redirects to fatal
 *
 *  Squeak is a singleton class - so initialisation is done whenever a function
 *  is called and the thing never destructs
 */
class Squeak {
 public:
  /** Error level that determines where the output goes */
  enum errorLevel {debug, info, warning, error, fatal};

  /** Shorthand that returns output to Squeak::mout(Squeak::debug) */
  static std::ostream & mout();

  /** Returns a reference to the ostream corresponding to the errorLevel */
  static std::ostream & mout(errorLevel level);

  /** Returns a reference to the ostream corresponding to the exceptionLevel */
  static std::ostream & mout(Exceptions::exceptionLevel level);

  /** Set the ostream for a given error level (MAUS::Exceptions::ExceptionHandling::Exception) */
  static void setAnOutput(errorLevel level, std::ostream& out);

  /** Set the ostream for all items below "verboseLevel" to /dev/null
   *  If verboseLevel is less than or equal to
   *   - Squeak::debug then mout(Squeak::debug) redirects to std::cout
   *   - Squeak::info then mout(Squeak::info) redirects to std::clog
   *   - Squeak::warning then mout(Squeak::warning) redirects to std::cerr
   *   - Squeak::error then mout(Squeak::error) redirects to std::cerr
   *   - Squeak::fatal then mout(Squeak::fatal) redirects to std::cerr
   *  Note that the redirection is independent of setStandardOutputs status
   */
  static void setOutputs(int verboseLevel);

  /** Turn on/off std::cout, std::cerr, std::clog
   * Set standard outputs to /dev/null depending on verboseLevel:
   * - if verboseLevel > int(Squeak::debug), set std::cout to /dev/null
   * - if verboseLevel > int(Squeak::info), set std::clog to /dev/null
   * - if verboseLevel > int(Squeak::warning), set std::cerr to /dev/null
   * GEANT4 has very verbose output that hides important run control info, so
   * we can just turn it off by redirecting std::cout here.
   */
  static void setStandardOutputs(int verboseLevel);

  /** Activate std::cout
   *  If isActive is false, redirects std::cout to /dev/null; if isActive is
   *  true, redirects it to std::cout
   */
  static void activateCout(bool isActive);

  /** Return false if std::cout points to voidout; else true */
  static bool coutIsActive();

  /** Activate std::cerr
   *  If isActive is false, redirects std::cerr to /dev/null; if isActive is
   *  true, redirects it to std::cerr
   */
  static void activateCerr(bool isActive);

  /** Return false if std::cerr points to voidout; else true */
  static bool cerrIsActive();

  /** Activate std::clog
   *  If isActive is false, redirects std::clog to /dev/null; if isActive is
   *  true, redirects it to std::clog
   */
  static void activateClog(bool isActive);

  /** Return false if std::clog points to voidout; else true */
  static bool clogIsActive();

  /** Return ostream pointing to /dev/null (irrespective of verbose level) */
  static std::ostream& nullOut();

  /** Return ostream pointing to standard output (irrespective of verb level) */
  static std::ostream& coutOut();

  /** Return ostream pointing to standard log (irrespective of verbose level) */
  static std::ostream& clogOut();

  /** Return ostream pointing to standard error (irrespective of verb level) */
  static std::ostream& cerrOut();

 private:
  /** Constructor is called by any call to mout. Defines std::map output */
  Squeak();

  /** Associates an ostream with each error level */
  static std::map<errorLevel, std::ostream*> output;

  /** dummy ostream to send stuff to /dev/null */
  static std::ostream* voidout;

  /** ostream that points to std::out (so we can turn them back on after turning
   * them off)
   */
  static std::ostream* stdout;

  /** ostream that points to std::log */
  static std::ostream* stdlog;

  /** ostream that points to std::err */
  static std::ostream* stderr;

  static const errorLevel default_error_level;

  /** Pointer to the singleton instance of the class */
  static Squeak* instance;

  /** Called automagically by mout to ensure class is always instantiated
   *  whenever needed.
   *  @returns Pointer to the singleton class instance
   */
  static Squeak* getInstance();

  /** Set outputs */
  static void initialiseOutputs();

  /** Setup Exception */
  static void setException();
};
}

#endif

