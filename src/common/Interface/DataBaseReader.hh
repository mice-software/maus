//
//  Y.Karadzhov  Sep 2010
//
#ifndef DBREADER_H
#define DBREADER_H 1

#include "Interface/XMLMessage.hh"
#include "Interface/DataBaseAPI.hh"
#include "Interface/dataCards.hh"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

class DataBaseReader
{
  public:

	 DataBaseReader(dataCards* theCards);
	 ~DataBaseReader();

	 DataBaseAPI* GetDataBaseAPI() const 		{ return db_api; };

	 XMLMessage GetRunInfo(int run);
	 XMLMessage GetTofCalibration(int run, string detector);

	 int GetTrigger(int run);
	 int GetCurrentRunNum()	const	{return CurrentRun;};

	 void OnRunBeginDo(const unsigned int);

  private:

	 DataBaseAPI* db_api;
	 map<int, XMLMessage> RunInfos;
	 map<string,XMLMessage> TofCalibrations;

	 int CurrentRun;  // to be set every time whene DATEReader starts a new run

};


#endif













