#include "Squeal.hh"
#include "Squeak.hh"

#include <ostream>
#include <sstream>

#ifndef NO_STACKTRACE
#include <stdlib.h>
#include <execinfo.h>
#endif


std::ostream * Squeal::_debOut       = NULL;
std::ostream * Squeal::_recOut       = NULL;
std::ostream * Squeal::_nonRecOut    = NULL;
const size_t   Squeal::_maxStackSize = 100;

Squeal::Squeal() throw()
{Squeak::mout();}

Squeal::Squeal(exceptionLevel level, std::string errorMessage, std::string location) throw()
      : exception(), _message(errorMessage), _location(location), _stacktrace(MakeStackTrace(2)), _level(level)
{}

void Squeal::Print()
{
	mout(int(_level)) << _message << "\n";
	mout(-1)          << "Error at " << _location << "\n";
  if(_stacktrace != "")
    mout(-1)          << "Stack trace\n" << GetStackTrace() << "\n";
}


std::ostream&  Squeal::mout   (int level)
{
  switch( level )
  {
    case int(Squeal::recoverable):    return *_recOut;
    case int(Squeal::nonRecoverable): return *_nonRecOut;
    default:                          return *_debOut;
  }
}

void Squeal::setOutput(int level, std::ostream& out)
{
  switch( level )
  {
    case int(Squeal::recoverable):    _recOut    = &out;
    case int(Squeal::nonRecoverable): _nonRecOut = &out;
    default:                          _debOut    = &out;
  }

}

#ifndef NO_STACKTRACE
std::string Squeal::MakeStackTrace(size_t skipTrace) 
{
  size_t stackSize;
  void * stackAddress[_maxStackSize];
  char **stackNames;

  std::stringstream sstr;
  stackSize  = backtrace(stackAddress, _maxStackSize);
  stackNames = backtrace_symbols(stackAddress, stackSize);

  for (size_t i = skipTrace; i < stackSize; i++) sstr << stackNames[i] << std::endl;
  free(stackNames);
  return sstr.str();
}
#endif

#ifdef NO_STACKTRACE
std::string Squeal::MakeStackTrace() {return "";}
#endif
