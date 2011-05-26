//Y.Karadzhov 15.10.2009

#ifndef TOFTRIGGER_HH
#define TOFTRIGGER_HH


#include "Interface/MICEEvent.hh"
#include "Recon/TOF/TriggerKey.hh"
#include "Recon/TOF/TofChannelMap.hh"
#include "Recon/TOF/TofCalibrationMap.hh"
#include "Interface/TofDigit.hh"
#include "Recon/TOF/TofSlabHit.hh"
#include "Recon/TOF/TofSpacePoint.hh"


//!  TofTrigger class holds the digital values of the time of the Particle trigger
//!  and  Particle trigger request signals as recorded by the TDC boards.
//!  The class provides procedure for identification of the trigger pixel.
class TofTrigger
{
  public :
  //! Constructor. Do not forgot to set the trigger station by SetStation(int)
  TofTrigger();

  //! Destructor.
  ~TofTrigger();

  //! The data from the previous event is erased.
  void Reset();

  //! Measured digital values of the time of the Trigger and Trigger Requests are set. Values are extracted from MICEEvent object. To be used in the beginning of the event reconstruction.
  void SetTriggerTimes( MICEEvent* theEvent, TofChannelMap* tofmap );

  //! Sets the value ot Station data member.
  void SetStation(int st)        { Station = st; };

  //! Sets the value of the TriggerTime data member.
  void SetTriggerTime(double t)  { TriggerTime = t; };

  //! The function searches for the pixel that is giving the trigger. The user have to provide pointers to the MICEEvent and the TofCalibrationMap. Also the serial numbers in the MICEEvent of the TofSlabHits in the trigger station have to be provided. Values of the time cut and the CalibMode have to be given.
  TriggerKey* ProcessTriggerKey(MICEEvent* theEvent,TofCalibrationMap* calib_map,vector< vector<int> > TrHits,double tCut,bool CalibMode);

  //! Returns the key of the reconstructed trigger pixel. Do not call this function befor TriggerKey::ProcessTriggerKey. Returs NULL if no trigger pixel is found in TriggerKey::ProcessTriggerKey.
  TriggerKey* GetTriggerKey()    { return theKey; };

  //! Digital value of the time of the Particle Trigger as recorded by board with number "boardNum" is returned.
  int GetTrigger(int boardNum);

  //! Digital value of the time of the first Particle Trigger Request as recorded by board with number "boardNum" is returned.
  int GetTriggerReq(int boardNum);

  //! Returns the number of the recorded Particle Trigger Requests.
  int GetNumberOfRequests()      { return p_trigger_request[1].size(); };

  //! Returns the reconstructed time of the trigger hit.
  int GetTriggerTime()           { return TriggerTime; };

  //! Returns the number of the trigger station.
  int GetStation()               { return Station; };

  //! Dumps some information about this digit to the standard output.
  void Print();


  private :
  //! Number of the TOF statin, used as a trigger.
  int Station;

  //! Digital values of the time of the Particle Trigger signal as measured by the TDC boards. The index corresponds to the TDC board number.
  vector< int > p_trigger;

  //! Digital values of the time of the Particle Trigger Request signals as measured by the TDC boards. The first index corresponds to the TDC board number. Be careful, we can have more than one Particle Trigger Request !!!
  vector< vector< int > > p_trigger_request;

  //! Key of the reconstructed trigger pixel. ProcessTriggerKey function is used to find this key (pixel).
  TriggerKey* theKey;

  //! Reconstructed time of the trigger hit. The value is set in ProcessTriggerKey function.
  double TriggerTime;
};

#endif