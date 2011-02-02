//Y.Karadzhov 15.10.2009

#ifndef TOFTRIGGER_HH
#define TOFTRIGGER_HH


#include "Interface/MICEEvent.hh"
#include "TriggerKey.hh"
#include "TofChannelMap.hh"
#include "TofCalibrationMap.hh"
#include "Interface/TofDigit.hh"
#include "TofSlabHit.hh"
#include "TofSpacePoint.hh"

class TofTrigger
{
  public :
	 
  TofTrigger();
  ~TofTrigger();
  void Reset();
  void SetTriggerTimes( MICEEvent theEvent, TofChannelMap* tofmap );
  void SetStation(int st)        { Station = st; };
  void SetTriggerTime(double t)  { TriggerTime = t; };
  
  TriggerKey* ProcessTriggerKey(MICEEvent theEvent,TofCalibrationMap* calib_map,vector< vector<int> > TrHits,double tCut,bool CalibMode);
  TriggerKey* GetTriggerKey()    { return theKey; };
  
  int GetTrigger(int board)      { return p_trigger[ board ]; };
  int GetTriggerReq(int board)   { return p_trigger_request[ board ][0]; };
  int GetNumberOfRequests()      { return p_trigger_request[1].size(); };
  int GetTriggerTime()           { return TriggerTime; };
  int GetStation()               { return Station; };
  
  void Print();
  
  private :

  vector< int > p_trigger;
  vector< vector< int > > p_trigger_request;

  int Station;
  TriggerKey* theKey;
  double TriggerTime;
};

#endif
