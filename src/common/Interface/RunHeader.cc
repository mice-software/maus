/** 
 * RunHeader
 *
 * Add run header to output file and provide method to read in run header.
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

#include "Interface/RunHeader.hh"
#include "Interface/dataCards.hh"

using namespace std;

RunHeader::RunHeader():
  header("StartRunHeader"),
  footer("EndRunHeader")
{
  // CVS tags for version of package
  application = MyDataCards.getClient();
  static string cvs_rev = "$Revision: 1.6 $"; // source file version
  static string cvs_tag = "$Name:  $"; // tag
  static string cvs_id  = "$Id: RunHeader.cc,v 1.6 2009-05-11 14:41:33 ellis Exp $"; // more info just in case
  version = cvs_rev + cvs_tag + cvs_id;

  // Get time and date
  time_t rawtime;
  time (&rawtime);
  date = asctime(gmtime(&rawtime));

  // Get description, run_no and nevts from data cards
  description = MyDataCards.fetchValueString("Description");
  run_no = MyDataCards.fetchValueInt("runNumber");
  nevts = MyDataCards.fetchValueInt("numEvts");
  seed = (int ) MyDataCards.fetchValueInt("RandomSeed");

  // Get machine, os and user from ENV
  machine = EnvString( "HOST" );
  os = EnvString( "OSTYPE" );
  user = EnvString( "USER" );  
}

RunHeader::~RunHeader() {}

void RunHeader::WriteHeader(ofstream &fp)
{
  fp<<header<<endl;
  fp<<"application="<<application<<endl;
  fp<<"version="<<version<<endl;
  fp<<"date="<<date; // Date string from ctime already has endl char.
  fp<<"description="<<description<<endl;
  fp<<"run_no="<<run_no<<endl;
  fp<<"nevts="<<nevts<<endl;
  fp<<"seed="<<seed<<endl;
  fp<<"machine="<<machine<<endl;
  fp<<"os="<<os<<endl;
  fp<<"user="<<user<<endl;  
  fp<<footer<<endl;
}

void RunHeader::ReadHeader(ifstream &fp)
{
  string line;

  if(!fp.is_open())
    {
      cerr<<"RunHeader::ReadHeader file not open"<<endl;
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

          if (key=="application") {getline(iss,application);
	continue;}

          if (key=="version") {getline(iss,version);
	continue;}

          if (key=="date") {getline(iss,date);
	continue;}

	  if (key=="description") {getline(iss,description);
	continue;}

	  if (key=="run_no") {iss>>run_no;
	continue;}

	  if (key=="nevts") {iss>>nevts;
	continue;}

	  if (key=="seed") {iss>>seed;
	continue;}

	  if (key=="machine") {getline(iss,machine);
	continue;}

	  if (key=="os") {getline(iss,os);
	continue;}

	  if (key=="user") {getline(iss,user);
	continue;}

      if(key==footer)
	break;

      cout << "Warning in RunHeader::ReadHeader: Unknown keyword "<< key << endl;

    }      
  while (line!=footer);
}


