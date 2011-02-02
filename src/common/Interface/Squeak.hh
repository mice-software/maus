//Squeak class allows the user to define ostreams for different levels of error at run time
//Maps an error level to an ostream object via static function mout(errorLevel).
//Squeak is a singleton class, constructor is called by mout functions

#ifndef Squeak_hh
#define Squeak_hh 1

#include <iostream>
#include <ostream>
#include <map>
#include <fstream>

#include "Squeal.hh"
#include "dataCards.hh"

class Squeak
{
public:
  //Usage: Squeak::mout(Squeak::debug) << "Blah blah" << std::endl;
  //e.g. will give output to std::out if VerboseLevel datacard is set to 0; or send to /dev/null if VerboseLevel is > 0
	//errorLevel is an enum to make adding extra errorLevels easy and with error checking at compile-time
	//log by default redirects /dev/null; 
	//other errorLevels redirect to std::out or /dev/null depending on VerboseLevel datacard
  //Squeak looks for datacards in the MICERun - they should be accessible, otherwise just prints everything!
	enum errorLevel{debug, info, warning, error, fatal, log};
	//returns a reference to the default output object
	static std::ostream & mout();
	//returns a reference to the ostream corresponding to the errorLevel
	static std::ostream & mout(errorLevel level);
	static std::ostream & mout(Squeal::exceptionLevel level);
	static void           mout(Squeal theException);
	//set the ostream for a given level
	static void           setOutput(errorLevel level, std::ostream& out);
  //Turn on/off std::cout, etc
  static void           setStandardOutputs(int verboseLevel=-1);
  static void           activateCout(bool isActive);
  static void           activateCerr(bool isActive);
  static void           activateClog(bool isActive);

private:
	//constructor is called by any call to mout
	//defines std::map output
	Squeak();
	//associates an ostream with each error level
	static std::map<errorLevel, std::ostream*> output;
	//dummy ostream to send stuff to /dev/null
	static std::ostream*                       voidout;
	//ostream that points to std::out
	static std::ostream*                       stdout;
	static std::ostream*                       stdlog;
	static std::ostream*                       stderr;
	//pointer to the singleton instance of the class
	static Squeak * instance;
	//called automagically by mout
	static Squeak *   getInstance();
  //mout tries to find the datacards in the MICERun, otherwise it operates verbose
  static dataCards* datacards;
  //Set outputs
  static void       initialiseOutputs();
  //Setup Squeal
  static void        setSqueal();
};



#endif

