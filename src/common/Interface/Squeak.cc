// Copyright 2007-2011 Chris Rogers
//
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#include <sstream>

#include "Interface/Squeak.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICERun.hh"

std::ostream*                               Squeak::stdout    = NULL;
std::ostream*                               Squeak::stdlog    = NULL;
std::ostream*                               Squeak::stderr    = NULL;
std::ostream*                               Squeak::voidout   = NULL;
std::map<Squeak::errorLevel, std::ostream*> Squeak::output;
dataCards*                                  Squeak::datacards = NULL;
Squeak *                                    Squeak::instance  = NULL;

std::ostream & Squeak::mout() {
  getInstance();
  return Squeak::mout(debug);
}

std::ostream & Squeak::mout(errorLevel level) {
  getInstance();
  return (*output[level]);
}

std::ostream & Squeak::mout(Squeal::exceptionLevel level) {
  getInstance();
  // eventually a map or somesuch I suspect
  if (level == Squeal::recoverable)
    return (*output[Squeak::error]);
  else
    return (*output[Squeak::fatal]);
}

void  Squeak::setOutput(errorLevel level, std::ostream& out) {
  getInstance();
  output[level] = &out;
}


Squeak * Squeak::getInstance() {
  // verbose level is set by datacards. If I use mout before datacards are
  // defined, should still work; but then re-initialise once datacards defined
  // to set the verbose level properly
  if (datacards == NULL) {  // keep trying to find datacards
    datacards = MICERun::getInstance()->DataCards;
    if (datacards != NULL && instance != NULL) {
      delete instance;
      instance = new Squeak();
    }
  }
  if (instance  == NULL) instance  = new Squeak();
  return instance;
}

Squeak::Squeak() {
  // verboseLevel defaults to debug if datacards not defined (safest)
  errorLevel verboseLevel = debug;
  if (datacards != NULL)
    verboseLevel = errorLevel(datacards->fetchValueInt("VerboseLevel"));
  initialiseOutputs();

  for (int i = 0; i <= fatal; ++i) {
    errorLevel error = errorLevel(i);
    if (i >= verboseLevel) output[error] = stdout;
    else                output[error] = voidout;
  }
}

void Squeak::setStandardOutputs(int verboseLevel) {
  getInstance();
  if (verboseLevel < 0 && datacards != NULL)
    verboseLevel = datacards->fetchValueInt("VerboseLevel");
  activateCout(verboseLevel <= Squeak::debug);
  activateClog(verboseLevel <= Squeak::info);
  activateCerr(verboseLevel <= Squeak::warning);
}

void Squeak::activateCout(bool isActive) {
  getInstance();
  if (isActive) std::cout.rdbuf(stdout->rdbuf());
  else         std::cout.rdbuf(voidout->rdbuf());
}

void Squeak::activateCerr(bool isActive) {
  getInstance();
  if (isActive) std::cerr.rdbuf(stdlog->rdbuf());
  else          std::cerr.rdbuf(voidout->rdbuf());
}


void Squeak::activateClog(bool isActive) {
  getInstance();
  if (isActive) std::clog.rdbuf(stdlog->rdbuf());
  else         std::clog.rdbuf(voidout->rdbuf());
}

void Squeak::initialiseOutputs() {
  // assume they are all uninitialised if one is (we always initialise together)
  if (voidout == NULL) {
    voidout = new std::ofstream();  // this points at /dev/null by default
    stdout  = new std::ofstream();
    stdout->rdbuf(std::cout.rdbuf());
    stdlog  = new std::ofstream();
    stdlog->rdbuf(std::clog.rdbuf());
    stderr  = new std::ofstream();
    stderr->rdbuf(std::cerr.rdbuf());
  }
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

