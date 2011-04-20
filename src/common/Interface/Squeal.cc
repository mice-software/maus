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

#ifndef NO_STACKTRACE
#include <stdlib.h>
#include <execinfo.h>
#endif

#include <ostream>
#include <sstream>

#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"

const size_t   Squeal::_maxStackSize = 100;

Squeal::Squeal() throw() {
  Squeak::mout();
}

Squeal::Squeal(exceptionLevel level, std::string errorMessage,
                                                   std::string location) throw()
      : exception(), _message(errorMessage), _location(location),
                                 _stacktrace(MakeStackTrace(2)), _level(level) {
  Squeak::mout();  // make sure we initialise Squeak otherwise can get segv
}

void Squeal::Print() {
  Squeak::mout(_level) << _message << "\n";
  Squeak::mout(Squeak::debug) << "Error at " << _location << "\n";
  if (_stacktrace != "")
    Squeak::mout(Squeak::debug) << "Stack trace\n" << GetStackTrace() << "\n";
}

#ifndef NO_STACKTRACE
std::string Squeal::MakeStackTrace(size_t skipTrace) {
  size_t stackSize;
  void * stackAddress[_maxStackSize];
  char **stackNames;

  std::stringstream sstr;
  stackSize  = backtrace(stackAddress, _maxStackSize);
  stackNames = backtrace_symbols(stackAddress, stackSize);

  for (size_t i = skipTrace; i < stackSize; i++) sstr << stackNames[i]
                                                                   << std::endl;
  free(stackNames);
  return sstr.str();
}
#endif

#ifdef NO_STACKTRACE
std::string Squeal::MakeStackTrace() {return "";}
#endif

