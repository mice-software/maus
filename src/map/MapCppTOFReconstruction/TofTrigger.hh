//Y.Karadzhov 15.10.2009

#ifndef TOFTRIGGER_HH
#define TOFTRIGGER_HH


#include "Interface/MICEEvent.hh"
#include "TriggerKey.hh"
#include "TofChannelMap.hh"
#include "TofCalibrationMap.hh"
#include "TofDigit.hh"
#include "TofSlabHit.hh"
#include "TofSpacePoint.hh"

//!  TofTrigger class holds the digital values of the time of the Particle trigger
//!  and  Particle trigger request signals as recorded by the TDC boards.
//!  The class provides procedure for identification of the trigger pixel.


class TofTrigger
{
  public :

  TofTrigger();
  //! Constructor. Do not forgot to set the trigger station by SetStation(int)

  ~TofTrigger();
  //! Destructor.

  void Reset();
  //! The data from the previous event is erased.

  void SetTriggerTimes( MICEEvent* theEvent, TofChannelMap* tofmap );
  //! Measured digital values of the time of the Trigger and Trigger Requests are set.
  //! Values are extracted from MICEEvent object.
  //! To be used in the beginning of the event reconstruction.

  void SetStation(int st)        { Station = st; };
  //! Sets the value ot Station data member.

  void SetTriggerTime(double t)  { TriggerTime = t; };
  //! Sets the value of the TriggerTime data member.

  TriggerKey* ProcessTriggerKey(MICEEvent* theEvent,TofCalibrationMap* calib_map,vector< vector<int> > TrHits,double tCut,bool CalibMode);
  //! The function searches for the pixel that is giving the trigger.
  //! The user have to provide pointers to the MICEEvent and the TofCalibrationMap.
  //! Also the serial numbers in the MICEEvent of the TofSlabHits in the trigger station have to be provided.
  //! Values of the time cut and the CalibMode have to be given.

  TriggerKey* GetTriggerKey()    { return theKey; };
  //! Returns the key of the reconstructed trigger pixel.

  int GetTrigger(int boardNum);
  //! Digital value of the time of the Particle Trigger
  //! as recorded by board with number "boardNum" is returned.

  int GetTriggerReq(int boardNum);
  //! Digital value of the time of the first Particle Trigger Request
  //! as recorded by board with number "boardNum" is returned.

  int GetNumberOfRequests()      { return p_trigger_request[1].size(); };
  //! Returns the number of the recorded Particle Trigger Requests.

  int GetTriggerTime()           { return TriggerTime; };
  //! Returns the reconstructed time of the trigger hit.

  int GetStation()               { return Station; };
  //! Returns the number of the trigger station.

  void Print();


  private :

  int Station;
  //! Number of the TOF statin, used as a trigger.

  vector< int > p_trigger;
  //! Digital values of the time of the Particle Trigger signal as measured by the TDC boards.
  //! The index corresponds to the TDC board number.

  vector< vector< int > > p_trigger_request;
  //! Digital values of the time of the Particle Trigger Request signals as measured by the TDC boards.
  //! The first index corresponds to the TDC board number.
  //! Be careful, we can have more than one Particle Trigger Request !!!


  TriggerKey* theKey;
  //! Key of the reconstructed trigger pixel.
  //! ProcessTriggerKey function is used to find this key (pixel).

  double TriggerTime;
  //! Reconstructed time of the trigger hit.
  //! The value is set in ProcessTriggerKey function.
};

#endif