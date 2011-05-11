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

#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

/// Squeak class is designed to redirect output to std::out or file

/// Usage:
/// Squeak::mout(Squeak::debug) << "Blah blah" << std::endl;
///
/// Squeak is used to redirect output at run time to some output file or
/// standard outputs. Redirection is controlled by errorLevel:\n
///    debug - debugging information. Deliver information related to debugging
///            errors in the code; verbose developer level output\n
///    info - run control information. Deliver information related to run
///           information; aimed at users to give summary information about the
///           run (idea is to give a constant, but limited, stream of data to
///           info)\n
///    warning - information to warn the user of suspected errors in running\n
///    error - information to warn the user of certain errors in running\n
///    fatal - final information to tell the user if the code is crashing\n
///
/// We also redirect output according to exceptionLevel\n
///    recoverable - redirects to error\n
///    nonRecoverable - redirects to fatal
///
/// Squeak is a singleton class - so initialisation is done whenever a function
/// is called and the thing never destructs

class Squeak {
 public:
  /// error level that determines where the output goes
  enum errorLevel {debug, info, warning, error, fatal};

  /// Shorthand that returns output to Squeak::mout(Squeak::debug)
  static std::ostream & mout();

  /// Returns a reference to the ostream corresponding to the errorLevel
  static std::ostream & mout(errorLevel level);

  /// Returns a reference to the ostream corresponding to the exceptionLevel
  static std::ostream & mout(Squeal::exceptionLevel level);

  /// Set the ostream for a given error level (Squeal)
  static void setOutput(errorLevel level, std::ostream& out);

  /// Turn on/off std::cout, std::cerr, std::clog

  /// Set standard outputs to /dev/null depending on verboseLevel:
  ///   * if verboseLevel > int(Squeak::debug), set std::cout to /dev/null
  ///   * if verboseLevel > int(Squeak::info), set std::clog to /dev/null
  ///   * if verboseLevel > int(Squeak::warning), set std::cerr to /dev/null
  ///   * if verboseLevel < 0, take default from int datacard "VerboseLevel"
  /// GEANT4 has very verbose output that hides important run control info, so
  /// we can just turn it off by redirecting std::cout here.
  static void setStandardOutputs(int verboseLevel=-1);

  /// Activate std::cout

  /// If isActive is false, redirects std::cout to /dev/null; if isActive is
  /// true, redirects it to std::cout
  static void activateCout(bool isActive);

  /// Activate std::cerr

  /// If isActive is false, redirects std::clog to /dev/null; if isActive is
  /// true, redirects it to std::clog
  static void activateCerr(bool isActive);

  /// Activate std::clog

  /// If isActive is false, redirects std::clog to /dev/null; if isActive is
  /// true, redirects it to std::clog
  static void activateClog(bool isActive);

  /// Return ostream pointing to /dev/null (irrespective of verbose level)
  static std::ostream& nullOut();

  /// Return ostream pointing to standard output (irrespective of verbose level)
  static std::ostream& coutOut();

  /// Return ostream pointing to standard log (irrespective of verbose level)
  static std::ostream& clogOut();

  /// Return ostream pointing to standard error (irrespective of verbose level)
  static std::ostream& cerrOut();

 private:
  // constructor is called by any call to mout
  // defines std::map output
  Squeak();
  // associates an ostream with each error level
  static std::map<errorLevel, std::ostream*> output;
  // dummy ostream to send stuff to /dev/null
  static std::ostream*                       voidout;
  // ostream that points to std::out (so we can turn them back on after turning
  // them off)
  static std::ostream*                       stdout;
  static std::ostream*                       stdlog;
  static std::ostream*                       stderr;
  // pointer to the singleton instance of the class
  static Squeak * instance;
  // called automagically by mout
  static Squeak *   getInstance();
  // mout tries to find the datacards in the MICERun, otherwise it operates
  // verbose
  static dataCards* datacards;
  // Set outputs
  static void       initialiseOutputs();
  // Setup Squeal
  static void        setSqueal();
};



#endif

