#include "Squeak.hh"
#include "dataCards.hh"
#include "MICERun.hh"
#include <sstream>

std::ostream*                               Squeak::stdout    = NULL;
std::ostream*                               Squeak::stdlog    = NULL;
std::ostream*                               Squeak::stderr    = NULL;
std::ostream*                               Squeak::voidout   = NULL;
std::map<Squeak::errorLevel, std::ostream*> Squeak::output;
dataCards*                                  Squeak::datacards = NULL;
Squeak *                                    Squeak::instance  = NULL;

std::ostream & Squeak::mout()
{
	getInstance();
	return Squeak::mout(warning);
}

std::ostream & Squeak::mout(errorLevel level)
{
	getInstance();
	return (*output[level]);
}

std::ostream & Squeak::mout(Squeal::exceptionLevel level)
{
	getInstance();
	//eventually a map or somesuch I suspect
	if(level == Squeal::recoverable) return (*output[Squeak::error]);
	else return (*output[Squeak::fatal]);
}

void  Squeak::setOutput(errorLevel level, std::ostream& out)
{
	output[level] = &out;
}


Squeak * Squeak::getInstance()
{
  if(datacards == NULL)  
  {
    datacards = MICERun::getInstance()->DataCards; //keep trying to find datacards
    if(datacards != NULL && instance != NULL) {delete instance; instance = new Squeak();} //if I use mout before datacards are defined, should still work; but then re-initialise once datacards defined to set the verbose level properly
  }
  if(instance  == NULL)  instance  = new Squeak();
  return instance;
}

Squeak::Squeak()
{
	errorLevel verboseLevel = debug; //verboseLevel defaults to debug if datacards not defined (safest)
	if(datacards != NULL)
		verboseLevel = errorLevel(datacards->fetchValueInt("VerboseLevel"));
  initialiseOutputs();

  for(int i=0; i<=fatal; i++)
	{
		errorLevel error = errorLevel(i);
		if(i>=verboseLevel) output[error] = stdout;
		else                output[error] = voidout;
	}
	output[log] = voidout;
}

void Squeak::mout(Squeal theException) 
{
	mout(Squeak::warning) << theException.GetMessage() << " ";
	mout(Squeak::debug)   << theException.GetLocation();
}

void Squeak::setStandardOutputs(int verboseLevel)
{
  getInstance();
  if(verboseLevel < 0 && datacards != NULL)
    verboseLevel = datacards->fetchValueInt("VerboseLevel");
  activateCout(verboseLevel <= Squeak::debug);
  activateClog(verboseLevel <= Squeak::info);
  activateCerr(verboseLevel <= Squeak::warning);
}

void Squeak::activateCout(bool isActive)
{
  getInstance();
  if(isActive) std::cout.rdbuf(stdout->rdbuf());
  else         std::cout.rdbuf(voidout->rdbuf());
}

void Squeak::activateCerr(bool isActive)
{
  getInstance();
  if(isActive) std::clog.rdbuf(stdlog->rdbuf());
  else         std::clog.rdbuf(voidout->rdbuf());
}


void Squeak::activateClog(bool isActive)
{
  getInstance();
  if(isActive) std::cerr.rdbuf(stderr->rdbuf());
  else         std::cerr.rdbuf(voidout->rdbuf());
}

void Squeak::initialiseOutputs()
{
  if(voidout == NULL) //assume they are all uninitialised
  {
    voidout = new std::ofstream(); //this points at /dev/null by default
    stdout  = new std::ofstream(); stdout->rdbuf(std::cout.rdbuf());
    stdlog  = new std::ofstream(); stdlog->rdbuf(std::clog.rdbuf());
    stderr  = new std::ofstream(); stderr->rdbuf(std::cerr.rdbuf());
    Squeal::setOutput(-1,                     mout(debug)   );
    Squeal::setOutput(Squeal::recoverable,    mout(warning) );
    Squeal::setOutput(Squeal::nonRecoverable, mout(error)   );
  }
  
}


