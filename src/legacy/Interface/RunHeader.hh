/** 
 * RunHeader
 *
 * Add run header to output file and provide method to read in run header.
 *
 */

#ifndef RunHeader_h
#define RunHeader_h 1

#include <fstream>

using namespace std;

class RunHeader
{
 public:

/** Constructor for new run header object using current settings */
  RunHeader();

  ~RunHeader();

/** Write run header to output stream */
  void WriteHeader(ofstream&);
/** Read run header from inout file stream */
  void ReadHeader(ifstream&);
/** Get version of G4MICE used for run*/         
  string GetVersion() const {return version;}
/** Get date of run */
  string GetDate() const {return date;}
/** Get description of run */
  string GetDescription() const {return description;}
/** Get run number */
  int    GetRun_No() const {return run_no;}
/** Get number of events in run */
  int    GetNevts() const {return nevts;}
/** Get random number seed */
  int    GetSeed() const {return seed;}
/** Get machine name */
  string GetMachine() const {return machine;}
/** Get operating system */
  string GetOS() const {return os;}
/** Get user */
  string GetUser() const {return user;}

 private:

  const string header;
  const string footer;

  string application;
  string version;
  string date;
  string description;
  int  run_no;
  int  nevts;
  int  seed;
  string machine;
  string os;
  string user;

  string EnvString( const char* e) { char* tmp; return string( (tmp=getenv(e))==NULL ? "unknown" : tmp );} 
  
};

#endif
