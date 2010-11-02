#ifndef Squeal_hh
#define Squeal_hh 1

#include <exception>
#include <string>

//! Exception handler class
//!
//! Exception handler has a severity (exceptionLevel), an error message and a location.
//! Also function to return the stack trace (gcc only).

class Squeal : public std::exception
{
public:
  //! ExceptionLevel enumerates the severity of the exception.
  //! I use an enumeration to distinguish between different error levels.
  //! nonRecoverable means the internal state of the programme is no longer well-defined
  //! i.e. some memory problem or so.
  //! recoverable means that in principle we could keep on running, although most of the time
  //! this results in end of run (usually indicates typo in an input file).
	enum exceptionLevel{recoverable, nonRecoverable};
  //constructor - with error level, error message and location
	Squeal(exceptionLevel level, std::string errorMessage, std::string location) throw();
  //constructor - does nothing
	Squeal() throw() {;}

  //destructor - does nothing
	~Squeal() throw() {;}
  //! Return char buffer message+" at "+location
	const char* what() const throw() {return (_message+" at "+_location).c_str();}
  //! Print the Message to Squeak::mout(Squeak::error) and Location to Squeak::mout(Squeak::debug)
	void Print();
  //! Get the severity of the exception
	exceptionLevel GetErrorLevel() {return _level;}
  //! Get the error message (as defined by constructor)
	std::string    GetMessage()    {return _message;}
  //! Get the location (as defined by constructor) of the error
	std::string    GetLocation()   {return _location;}
  //! Return the stack trace if it was stored
  std::string    GetStackTrace() {return _stacktrace;}
  //! Gcc-specific code to recover the stack trace as a string.
  //! Will skip traces below skipTrace (so, for example, if we want to know where the Squeal was thrown
  //! we might set skipTrace to 2, to skip MakeStackTrace and Squeal constructor).
  static std::string    MakeStackTrace(size_t skipTrace);

  friend class Squeak;

private:
  static std::ostream &  mout     (int level); //maps from exceptionLevel to out stream; default is debug
  static void            setOutput(int level, std::ostream& out); //Squeak controls handling of error messages
  static std::ostream *  _debOut;
  static std::ostream *  _recOut;
  static std::ostream *  _nonRecOut;

  static const size_t    _maxStackSize;

  std::string    _message;
	std::string    _location;
  std::string    _stacktrace;
	exceptionLevel _level;
};


#endif

