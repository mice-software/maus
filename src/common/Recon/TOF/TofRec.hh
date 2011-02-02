#ifndef _TOFREC_HH_
#define _TOFREC_HH_  1
//---------------------------------------------------------------------------
//
//     File :      TofRec.hh
//     Purpose :   Contain Tof digit/hits for reconstruction of a single
//                 Event.
//     Created :   17-DEC-2002  by Steve Kahn
//
//---------------------------------------------------------------------------

#include <iterator>
#include <list>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"
#include "Config/MiceModule.hh"

#include "TofChannelMap.hh"
#include "TofCalibrationMap.hh"
#include "TofTrigger.hh"
#include "PmtKey.hh"
#include "TriggerKey.hh"

#include "Interface/VmeAdcHit.hh"
#include "Interface/VmefAdcHit.hh"
#include "Interface/VmeTdcHit.hh"

#include "Interface/TofDigit.hh"
#include "TofSlabHit.hh"
#include "TofSpacePoint.hh"
#include "TofTrack.hh"
#include "Config/TofCable.hh"
#include "Calib/TofCalib.hh"

using namespace std;

class TofRec
{
  public:
  TofRec( MICEEvent& );
  TofRec( MICEEvent&, MICERun* );
  ~TofRec();

  bool Process();
  TriggerKey* ProcessTrigger(vector< vector<int> > TrSlabHits);

  TofTrigger* GetTrigger()                   { return theTrigger; };
  TofChannelMap* GetChannelMap()             { return &ch_map; };
  TofCalibrationMap* GetCalibrationMap()     { return &calib_map; };

  int GetDataType() const  { return DataType; };

  vector<int> GetSlabHits(int Station);
  vector< vector <int> > GetSpacePoints() {return SpacePoints;};
  vector<int> GetSpacePoints(int Station) {return SpacePoints[Station];};
  vector<int> GetUpStreamTracks() {return TOFTracks[0];};
  vector<int> GetDownStreamTracks() {return TOFTracks[1];};

  void SetSpacePiontCut(double cut)  { MakeSpacePiontCut = cut; };
  double GetSpacePiontCut() const { return MakeSpacePiontCut;};

  void SetTriggerCut(double cut)  { FindTriggerPixelCut = cut; };
  double GetTriggerCut() const { return FindTriggerPixelCut;};

  bool GetCalibrationMode() const {return CalibrationMode;};

  void Print();

  double GetEfficiency();
  double GetInefficiency(int flag);
  void ResetEfficiency();

  enum{
	 //data types
	 MICEData = 1,
	 KEKData = 2,
	 MCData = 3,

	 //inefficiency flags
	 NoCalibration = 4,
	 UnknownTrigger = 5
  };

  private:
  void Reset();
  void makeDigits( TofCalib*, TofCable*);
  void makeDigits();
  void sortDigits();
  void makeSlabHits();
  void makeSpacePoints();
  void makeTracks();

  double addUpPe( TofSpacePoint* );

  MICEEvent& theEvent;
  MICERun* theRun;

  TofChannelMap ch_map;
  TofCalibrationMap calib_map;

  int DataType;
  bool CalibrationMode;
  TofTrigger* theTrigger;

  double MakeSpacePiontCut;
  double FindTriggerPixelCut;

  std::vector<const MiceModule*> tofStationModules;
  std::vector<const MiceModule*> tofSlabModules;

  //UpDownStream/number of the TofSpacePoint in theEvent
  vector< vector <int> > TOFTracks;
  //station/number of the TofSpacePoint in theEvent
  vector< vector <int> > SpacePoints;
  //station/plane/number of the TofSlabHit in theEvent
  vector< vector< vector<int> > > SlabHits;

  //station/plane/ slab /  pmt / number of the TofDigit in theEvent
  vector< vector< vector< vector< vector <int> > > > > Digits;

  int NGoodEvents;
  int NBadEvents_NoCalib;
  int NBadEvents_NoTrigger;
  void AddGoodEvent();
  void AddBadEvent( int );
};
#endif

