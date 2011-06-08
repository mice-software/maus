/** 
 * RunFooter
 *
 * Add run footer to output file and provide method to read in run footer.
 *
 * @author Alex Tapper 
 *  
 */

#ifndef RunFooter_h
#define RunFooter_h 1

#include <fstream>

using namespace std;

class RunFooter
{
 public:

/** Constructor for new run footer object using current settings */
  RunFooter();

  ~RunFooter();

/** Write run footer to output stream */
  void WriteFooter(ofstream&);
/** Read run footer from inout file stream */
  void ReadFooter(ifstream&);
/** Get date of run */
  string GetDate() const {return date;}
/** Get number of events in run */
  int    GetNevts() const {return nevts;}
/** Get random number seed */
  int    GetSeed() const {return seed;}

 private:

  const string header;
  const string footer;

  string date;
  int  nevts;
  int  seed;

};

#endif
