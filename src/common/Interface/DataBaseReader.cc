//
//  Y.Karadzhov  Sep 2010
//


#include "Interface/DataBaseReader.hh"


DataBaseReader::DataBaseReader(dataCards* theCards)
{
  string host = theCards->fetchValueString( "DBServerHostName" );
  int port = theCards->fetchValueInt( "DBServerPort" );
  db_api = new DataBaseAPI(host, port);
  CurrentRun = -1;
}

DataBaseReader::~DataBaseReader()
{
  delete db_api;
}

XMLMessage DataBaseReader::GetRunInfo(int run)
{
  //! Firs chack do we have the information for this run.
  if( RunInfos.count(run) )
	 return RunInfos[run];

  //! If we do not have the information ask DB.
  XMLMessage request, response;

  //! Generate your question.
  request.MakeRunInfoRequest(run);

  //! Get your answare.
  response = db_api->sendQuery( request );

  //! Store the information
  RunInfos[run] = response;

  return response;
}

XMLMessage GetTofCalibration(int run, string detector)
{
  return XMLMessage( string("not implemented") );
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








