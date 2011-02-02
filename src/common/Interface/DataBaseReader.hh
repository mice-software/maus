//
//  Y.Karadzhov  Sep 2010
//
#ifndef DBREADER_H
#define DBREADER_H 1

#include "XMLMessage.hh"
#include "DataBaseAPI.hh"
#include "dataCards.hh"

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
	 int GetTrigger(int run);
	 int GetCurrentRunNum()	const	{return CurrentRun;};

	 void OnRunBeginDo(const unsigned int);

  private:

	 DataBaseAPI* db_api;
	 map<int, XMLMessage> RunInfos;

	 int CurrentRun;  // to be set every time whene DATEReader starts a new run

};


#endif













