#ifndef TOFCALIBRATIONMAP_HH
#define TOFCALIBRATIONMAP_HH

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

#include "Recon/TOF/PmtKey.hh"
#include "Recon/TOF/TriggerKey.hh"
#include "Interface/dataCards.hh"
#include "Config/MiceModule.hh"
#include "Interface/XMLMessage.hh"
#include "Interface/DataBaseReader.hh"



using namespace std;

//!  TofCalibrationMap class holds the values of the calibration constants for the
//!  TOF detectors and provides the calculation of the calibration corrections.
//!  The constants can be loaded from text files or from Configuration DB trough XML code.
class TofCalibrationMap {

public:
	//! Default constructor. The object is not ready to be used. Create PMTkays and apply some of the methods for initialization first.
	TofCalibrationMap();

   //! Constructor. Make one PmtKey for each channel of the detector. All PmtKeys are held in the data member _Pkey. The object is not ready to be used. Apply some of the methods for initialization first.
	TofCalibrationMap(vector<const MiceModule*> tofStations);

	//! Destructor.
	~TofCalibrationMap();

	//! Returns the T0 correction for the channel coded by the key.
	double T0( PmtKey key ,int &r);

	//! Returns the Trigger delay correction for the pixel coded by the key.
	double TriggerT0( TriggerKey key);

	//! Calculates the TimeWalk correction for the channel coded by the key and for given adc value.
	double TW( PmtKey key, int adc );

	//! Calculates the combined correction for the channel coded by Pkey, trigger pixel coded by the Tkey and for given adc value.
	double dT(PmtKey Pkey, TriggerKey Tkey, int adc);

   //! Make one PmtKey for each channel of the detector. All PmtKeys are held in the data member _Pkey.
	void MakePmtKeys(vector<const MiceModule*> tofStations);

	//! Initializes the map by using the Configuration Data Base. Do not forgot to use MakePmtKeys before doing this.
	bool InitializeFromDB(DataBaseReader* dbreader, int runNum);

   //! Loads calibration constants from XMLMessage.
	void LoadXML( XMLMessage calib );

	//! Initializes the map by using the text files specified in the cards and the vector of TOF station modules. The user is responsible to provided proper tofStationModules argument.
	void InitializeFromCards(dataCards* cards, vector<const MiceModule*> tofStationModules);

	//! Initializes the map by using the provided text files.
   void Initialize(string t0File, string twFile, string triggerFile);

	//! Calibration constants are exported to XML and are ready to be sent to the Configuration Data Base server. Use this functin together with void XMLMessage::NewTOFCalibRequest(string v1, string xml).
   string CalibMapToXML();

   //! Returns the data member TriggerStation. TriggerStation is automatically set during the initialization.
	int GetTriggerStation()                { return TriggerStation; };

	//! Value of the parameter is extracted from the input string "param".
	double GetParamValueDouble(string param);

	//! Value of the parameter is extracted from the input string "param".
	int GetParamValueInt(string param);

   //! Returns the data member Name.
   string GetName()	const	{return Name;};

	 //! Returns the data member Detector.
	string GetDetector()	const {return Detector;};

   //!  Prints the calibration map;
	void Print();


	enum{
	//! This value is returned when the correction can not be calculated.
	  NOCALIB = -99999
	};

private:
	//! Loads T0 constants from text file.
	void LoadT0File( string file );

   //! Loads Trigger delay constants from text file.
	void LoadTriggerFile( string file );

	//! Loads TimeWalk constants from text file.
	void LoadTWFile( string file );

	//! Convert a double parameter with name pName and value p to XML string.
	string CalibParDoubleToXML(string pName, double p);

   //! Convert an int parameter with name pName and value p to XML string
   string CalibParIntToXML(string name, int p);

	//!  The name and the type of the parameter are extracted from the input string "param".
   bool GetNameTypeFromXML(string param, string& name, string& type);

	//! Finds the position of the PMT key in the data member _Pkey.
	int FindPmtKey( PmtKey key );

	//! Finds the position of the trigger key in the data member _Tkey.
	int FindTriggerKey( TriggerKey key );

	//! Holds one PmtKey for each channel of the detector.
	vector<PmtKey> _Pkey;

	//! Holds the TimeWalk constants.\ IMPORTANT - the order of the entries here is the same as the order of the entries in _Pkey. This is used when the constants are read.
	vector< vector<double> > _twPar;

	//! Holds the T0 constants. IMPORTANT - the order of the entries here is the same as the order of the entries in _Pkey. This is used when the constants are read.
	vector<double> _t0;

	//! Holds the number of the refference bar. IMPORTANT - the order of the entries here is the same as the order of the entries in _Pkey. This is used when the constants are read.
	vector<int>_reff;

	//! Holds one TriggerKey for each calibrated pixel of the trigger station. IMPORTANT - uncalibrated pixels are not presented here.
	vector<TriggerKey> _Tkey;

	//! Holds the Trigger delay constants. IMPORTANT - the order of the entries here is the same as the order of the entries in _Tkey. This is used when the constants are read.
	vector<double> _Trt0;

   //! Number of the trigger station. It is automatically set during the initialization.
	int TriggerStation;

	//! Name of the calibration as in the CBD.
	string Name;

	//! Detector name as in the CBD.
	string Detector;

};

#endif
