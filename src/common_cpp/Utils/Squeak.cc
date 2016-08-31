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

#include <sstream>

#include "Utils/Squeak.hh"

namespace MAUS {

// Initialise the static member variables
std::ostream* Squeak::stdout    = NULL;
std::ostream* Squeak::stdlog    = NULL;
std::ostream* Squeak::stderr    = NULL;
std::ostream* Squeak::voidout   = NULL;
Squeak* Squeak::instance        = NULL;
const Squeak::errorLevel Squeak::default_error_level = Squeak::warning;
std::map<Squeak::errorLevel, std::ostream*> Squeak::output;

Squeak::Squeak() {
  // Do nothing
}

std::ostream & Squeak::mout() {
  getInstance();
  return Squeak::mout(debug);
}

std::ostream & Squeak::mout(errorLevel level) {
  getInstance();
  return (*output[level]);
}

std::ostream & Squeak::mout(Exceptions::exceptionLevel level) {
  getInstance();
  // Eventually a map or somesuch I suspect
  if (level == MAUS::Exceptions::recoverable)
    return (*output[Squeak::error]);
  else
    return (*output[Squeak::fatal]);
}

void  Squeak::setAnOutput(errorLevel level, std::ostream& out) {
  getInstance();
  output[level] = &out;
}

Squeak * Squeak::getInstance() {
  if (instance  == NULL) {
      instance = new Squeak();
      initialiseOutputs();
      setOutputs(default_error_level);
      setStandardOutputs(default_error_level);
  }
  return instance;
}

void Squeak::setOutputs(int verboseLevel) {
  getInstance();
  std::ostream* out[5] = {stdout, stdlog, stderr, stderr, stderr};
  for (int i = 0; i <= fatal; ++i) {
    if (i >= verboseLevel)
      output[Squeak::errorLevel(i)] = out[i];
    else
      output[Squeak::errorLevel(i)] = voidout;
  }
}

void Squeak::setStandardOutputs(int verboseLevel) {
  getInstance();
  activateCout(verboseLevel <= Squeak::debug);
  activateClog(verboseLevel <= Squeak::info);
  activateCerr(verboseLevel <= Squeak::error);
}

void Squeak::activateCout(bool isActive) {
  getInstance();
  if (isActive)
    std::cout.rdbuf(stdout->rdbuf());
  else
    std::cout.rdbuf(voidout->rdbuf());
}

bool Squeak::coutIsActive() {
  getInstance();
  return std::cout.rdbuf() != voidout->rdbuf();
}

void Squeak::activateCerr(bool isActive) {
  getInstance();
  if (isActive)
    std::cerr.rdbuf(stdlog->rdbuf());
  else
    std::cerr.rdbuf(voidout->rdbuf());
}

bool Squeak::cerrIsActive() {
  getInstance();
  return std::cerr.rdbuf() != voidout->rdbuf();
}

void Squeak::activateClog(bool isActive) {
  getInstance();
  if (isActive)
    std::clog.rdbuf(stdlog->rdbuf());
  else
    std::clog.rdbuf(voidout->rdbuf());
}

bool Squeak::clogIsActive() {
  getInstance();
  return std::clog.rdbuf() != voidout->rdbuf();
}

void Squeak::initialiseOutputs() {
  // assume they are all uninitialised if one is (we always initialise together)
  voidout = new std::ofstream();  // this points at /dev/null by default
  stdout  = new std::ofstream();
  stdout->rdbuf(std::cout.rdbuf());
  stdlog  = new std::ofstream();
  stdlog->rdbuf(std::clog.rdbuf());
  stderr  = new std::ofstream();
  stderr->rdbuf(std::cerr.rdbuf());
}

std::ostream& Squeak::nullOut() {
  getInstance();
  return *voidout;
}

std::ostream& Squeak::coutOut() {
  getInstance();
  return *stdout;
}

std::ostream& Squeak::clogOut() {
  getInstance();
  return *stdlog;
}

std::ostream& Squeak::cerrOut() {
  getInstance();
  return *stderr;
}
}

