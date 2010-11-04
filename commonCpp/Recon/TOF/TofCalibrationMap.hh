#ifndef TOFCALIBRATIONMAP_HH
#define TOFCALIBRATIONMAP_HH

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

#include <vector>
#include <math.h>

#include "PmtKey.hh"
#include "TriggerKey.hh"

class TofCalibrationMap {

public:
	TofCalibrationMap();
	~TofCalibrationMap();
	double T0( PmtKey key ,int &r);
	double TriggerT0( TriggerKey key);
	double TW( PmtKey key, int adc );
	double dT(PmtKey Pkey, TriggerKey Tkey, int adc);
	
	void Initialize(string t0File, string twFile, string triggerFile);
	void LoadT0File( string file );
	void LoadTWFile( string file );
	void LoadTriggerFile( string file );
	void MakePmtKeys();
	int FindPmtKey( PmtKey key );
	int FindTriggerKey( TriggerKey key );
	int GetTriggerStation()                { return TriggerStation; };

	enum{ NOCALIB = -99999 };	

private:
	vector<PmtKey> _key;
	vector< vector<double> > _twPar;
	vector<double> _t0;
	vector<int>_reff;

	vector<TriggerKey> _Tkey;
	vector<double> _Trt0;
	int TriggerStation;

};

#endif
