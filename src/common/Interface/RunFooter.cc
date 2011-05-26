/** 
 * RunFooter
 *
 * Add run footer to output file and provide method to read in run footer.
 *
 * @author Alex Tapper 
 *  
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <cstdlib>

#include "Interface/RunFooter.hh"
#include "Interface/dataCards.hh"

using namespace std;

RunFooter::RunFooter():
  header("StartRunFooter"),
  footer("EndRunFooter")
{
  // Get time and date
  time_t rawtime;
  time (&rawtime);
  date = asctime(gmtime(&rawtime));

  // Get nevts and seed from data cards
  nevts = MyDataCards.fetchValueInt("numEvts");
  seed = (int ) MyDataCards.fetchValueInt("RandomSeed");
}

RunFooter::~RunFooter() {}

void RunFooter::WriteFooter(ofstream &fp)
{
  fp<<header<<endl;
  fp<<"date="<<date; // Date string from ctime already has endl char.
  fp<<"nevts="<<nevts<<endl; 
  fp<<"seed="<<seed<<endl;
  fp<<footer<<endl;
}

void RunFooter::ReadFooter(ifstream &fp)
{
  string line;

  if(!fp.is_open())
    {
      cerr<<"RunFooter::ReadFooter file not open"<<endl;
      return;
    }

  do 
    {
      getline(fp,line);
      if (!fp.good()) return;
    }
  while (line!=header);

  do 
    {
      getline(fp,line);
      if (!fp.good()) return;

      istringstream iss(line);
      string key;
      getline(iss, key, '='); // This will only work if there is no white space around the keyword i.e. key=value
 
      if (key=="date") {getline(iss,date);
	continue;}

      if (key=="nevts") {iss>>nevts;
	continue;}

      if (key=="seed") {iss>>seed;
	continue;}

      if(key==footer)
	break;

      cout << "Warning in RunFooter::ReadFooter: Unknown keyword "<< key << endl;

    }      
  while (line!=footer);
}


