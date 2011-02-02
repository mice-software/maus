//
//  Y.Karadzhov  Sep 2010
//


#include "DataBaseReader.hh"


DataBaseReader::DataBaseReader(dataCards* theCards)
{
  string host = theCards->fetchValueString( "DBServerHostName" );
  int port = theCards->fetchValueInt( "DBServerPort" );
  db_api = new DataBaseAPI(host, port);
}

DataBaseReader::~DataBaseReader()
{
  delete db_api;
}

XMLMessage DataBaseReader::GetRunInfo(int run)
{
  //firs chack do we have the information for this run
  if( RunInfos.count(run) )
	 return RunInfos[run];

  //if we do not have the information ask DB
  XMLMessage request, response;

  // generate your question
  request.MakeRunInfoRequest(run);

  // get your answare
  response = db_api->sendQuery( request );

  //store the information
  RunInfos[run] = response;

  return response;
}

int DataBaseReader::GetTrigger(int run)
{
  string trigger = GetRunInfo( run ).GetValueString("trigger");
  if(trigger.size()!=4) return -99999;

  stringstream str;
  int tr;
  str << trigger[3];
  str >> tr;

  return tr;
}

void DataBaseReader::OnRunBeginDo(const unsigned int rn)
{
  GetRunInfo(rn);
  CurrentRun = rn;
}








