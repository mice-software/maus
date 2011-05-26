
//     Created :   17-DEC-2002  by Steve Kahn


#ifndef _TOFREC_HH_
#define _TOFREC_HH_  1

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

#include "Interface/DataBaseReader.hh"

#include "Recon/TOF/TofChannelMap.hh"
#include "Recon/TOF/TofCalibrationMap.hh"
#include "Recon/TOF/TofTrigger.hh"
#include "Recon/TOF/PmtKey.hh"
#include "Recon/TOF/TriggerKey.hh"

#include "Interface/VmeAdcHit.hh"
#include "Interface/VmefAdcHit.hh"
#include "Interface/VmeTdcHit.hh"

#include "Interface/TofDigit.hh"
#include "Recon/TOF/TofSlabHit.hh"
#include "Recon/TOF/TofSpacePoint.hh"
#include "Recon/TOF/TofTrack.hh"

using namespace std;


//!  TofRec class is used to reconstruct the TOF detectors responce.
//!  The class uses as input and output the MICEEvent object. The reconstruction
//!  can be executed over real data or over MonteCarlo data.
class TofRec
{
  public:
  //! Use this constructor if you do not want to connect to the Configuration Data Base. In this case do not forgot to set the trigger in the cards.in file.
  TofRec( MICEEvent&, MICERun* );

  //! Use this constructor if you want to connect to the Configuration Data Base.
  TofRec( MICEEvent&, MICERun*, DataBaseReader* );

  //! Destructor.
  ~TofRec();

  //! This function executes the reconstruction algorithm. To be called once in each event.
  bool Process();

  //! Returns the data member theTrigger.
  TofTrigger* 				GetTrigger()            { return theTrigger; };

  //! Returns the data member ch_map.
  TofChannelMap* 			GetChannelMap()         { return &ch_map; };

  //! Returns the data member calib_map.
  TofCalibrationMap* 	GetCalibrationMap()     { return &calib_map; };

  //! Returns the data member DBReader.
  DataBaseReader*			GetDBReader()				{ return DBReader; };

  //! Returns the data member DataType.
  int GetDataType() const  { return DataType; };

  //! Returns the serial numbers of all slab hits in the given station of the detector. Use these serial numbers when you extract TofSlabHits from the MICEEvent object. Do not call this function befor TofRec::Process().
  vector<int> GetSlabHits(int Station) 				const;

  //! Returns the serial numbers of all slab hits in the given station and plane of the detector. Use these serial numbers when you extract TofSlabHits from the MICEEvent object. Do not call this function befor TofRec::Process().
  vector<int> GetSlabHits(int Station, int Plane)	const {return SlabHits[Station][Plane];};

  //! Returns the serial numbers of all space points in the given station of the detector. Use these serial numbers when you extract TofSpacePoints from the MICEEvent object. Do not call this function befor TofRec::Process().
  vector<int> GetSpacePoints(int Station) const	{return SpacePoints[Station];};

  //! Returns matrix. More general version of GetSpacePoints(int Station). The first index correspondes to the station number.
  vector< vector <int> > GetSpacePoints() const	{return SpacePoints;};

  //! WARNING this is under developement !!! Do not use!
  vector<int> GetDownStreamTracks() 		const {return TOFTracks[1];};

  //! WARNING this is under developement !!! Do not use!
  vector<int> GetGVADigits()			const	{ return GVADigits; }

  //! WARNING this is under developement !!! Do not use!
  vector<int> GetGVASpacePoints()	const	{return GVASpacePoints;};

  //! WARNING this is under developement !!! Do not use!
  vector<int> GetUpStreamTracks() 	const	{return TOFTracks[0];};

  //! WARNING this is under developement !!! Do not use!
  vector<int> GetGVATracks()			const {return GVATracks;};

  //! The user can use this function to set the value of MakeSpacePiontCut in picoseconds. The default value of the cut is 500 ps.
  void SetSpacePiontCut(double cut)  { MakeSpacePiontCut = cut; };

  //! Returns the current value of the MakeSpacePiontCut in picoseconds.
  double GetSpacePiontCut() const { return MakeSpacePiontCut;};

  //! The user can use this function to set the value of FindTriggerPixelCut in picoseconds. The default value of the cut is 500 ps.
  void SetTriggerCut(double cut)  { FindTriggerPixelCut = cut; };

  //! Returns the current value of the FindTriggerPixelCut in picoseconds.
  double GetTriggerCut() const { return FindTriggerPixelCut;};

  //! Returns the Calibration Mode.
  bool GetCalibrationMode() const {return CalibrationMode;};

  //! Dumps some information about this digit to the standard output.
  void Print();

  //! Returns the efficiency of the reconstruction procedure. The efficiency is defined to be equal to the number of the successfully reconstructed events divided by the number of all recorded events. The event is counted as successfully reconstructed if the trigger hit is reconstructed.
  double GetEfficiency();

  //! Returns the inefficiency of the reconstruction procedure. The argument can be NoCalibration or UnknownTrigger.
  double GetInefficiency(int flag);

  //! Restarts the calculation of the efficiency.
  void ResetEfficiency();

  //! Returns the total number of successful reconstructed events since the last call of ResetEfficiency.
  int GetGoodEvents()		const { return NGoodEvents; };


  enum{
	 //! Data types
	 MICEData = 1,
	 KEKData = 2,
	 MCData = 3,

	 //! Inefficiency flags
	 NoCalibration = 4,
	 UnknownTrigger = 5
  };

  private:

  void Initialize();

  //! Resets the reconstruction before the beginning of the run.
  void OnRunBeginReset();

  //! Resets the reconstruction before the beginning of the event.
  void OnEventBeginReset();

  //! This function matches a VmefAdcHit to every VmeTdcHit and makes a TofDigit. To be used only for real data.
  void makeDigits();

  //! This function sort TofDigits. To be used only for MonteCarlo data.
  void sortDigits();

  //! This function matches TofDigits in same slabs and makes a TofSlabHit.
  void makeSlabHits();

  //! This function matches TofSlabHits in same station and makes a TofSpacePoint.
  void makeSpacePoints();


  void makeTracks();

  double addUpPe( TofSpacePoint* );

  MICEEvent& theEvent;
  MICERun* theRun;

  DataBaseReader* DBReader;
  int CurrentRunNum;

  TofChannelMap ch_map;
  TofCalibrationMap calib_map;

  int DataType;
  bool CalibrationMode;
  TofTrigger* theTrigger;

  double MakeSpacePiontCut;
  double FindTriggerPixelCut;

  vector<const MiceModule*> tofStationModules;
  vector<const MiceModule*> tofSlabModules;

  vector< int > GVADigits;
  vector< int > GVASpacePoints;
  vector< int > GVATracks;

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

  double ScintLightSpeed;
};


#endif

