#include "TofCalibrationMap.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
using CLHEP::picosecond;
using CLHEP::cm;
using CLHEP::c_light;

TofCalibrationMap::TofCalibrationMap() 
{
	 MakePmtKeys();
	 _twPar.resize(120);
	 _t0.resize(120);
	 _reff.resize(120);
}

TofCalibrationMap::~TofCalibrationMap() 
{
	 _key.clear();
	 _twPar.resize(0);
	 _t0.resize(0);
	 _reff.resize(0);
}

void TofCalibrationMap::Initialize(string t0File, string twFile, string triggerFile)
{
	LoadT0File(t0File);
	LoadTWFile(twFile);
	LoadTriggerFile(triggerFile);
}

void TofCalibrationMap::LoadT0File( string file )
{
	ifstream stream( file.c_str() );
	if( !stream )
   {
      std::cerr << "Can't open T0 calibration file " << file << std::endl;
      exit(1);
   }
	
	int station, plane, slab, pmt, reff;
	double d;
	while ( ! stream.eof() ) {
		stream >> station;
		stream >> plane;
		stream >> slab;
		stream >> pmt;
		stream >> d;
		stream >> reff;

		PmtKey key(station,plane,slab,pmt);
		int n = FindPmtKey( key );
		_t0[n] = d;
		_reff[n] = reff;
	}
}

void TofCalibrationMap::LoadTWFile( string file )
{
	ifstream stream( file.c_str() );
	if( !stream )
   {
      std::cerr << "Can't open TW calibration file " << file << std::endl;
      exit(1);
   }	
	
	int station, plane, slab, pmt;
	double p0, p1, p2, p3;
	while ( ! stream.eof() ) {
		stream >>dec>> station;
		stream >>dec>> plane;
		stream >>dec>> slab;
		stream >>dec>> pmt;
		stream >>dec>> p0;
		stream >>dec>> p1;
		stream >>dec>> p2;
		stream >>dec>> p3;

		PmtKey key(station,plane,slab,pmt);
		int n = FindPmtKey( key );
		_twPar[n].resize(4);
		_twPar[n][0] = p0;
		_twPar[n][1] = p1;
		_twPar[n][2] = p2;
		_twPar[n][3] = p3;
	}
}

void TofCalibrationMap::LoadTriggerFile( string file )
{
	ifstream stream( file.c_str() );
	if( !stream )
   {
      std::cerr << "Can't open Trigger calibration file " << file << std::endl;
      exit(1);
   }
	
	int station, slabA, slabB;
	double dt;
	while ( ! stream.eof() ) {
	   stream >> station;
		stream >> slabA;
		stream >> slabB;
		stream >> dt;
		TriggerKey Tkey(station, slabA, slabB);
		_Tkey.push_back(Tkey);
		_Trt0.push_back(dt);
	 }
  TriggerStation = station;
}

void TofCalibrationMap::MakePmtKeys()
{
	for(int st=0;st<3;st++)
	 for(int pl=0;pl<2;pl++)
		for(int sl=0;sl<10;sl++)
		  for(int pmt=0;pmt<2;pmt++) 
			 _key.push_back( PmtKey(st,pl,sl,pmt) ); 
}

int TofCalibrationMap::FindPmtKey( PmtKey key )
{
	for ( unsigned int i=0; i<_key.size(); ++i )
		if ( _key.at(i) == key )
			 return i;

	 return NOCALIB;
}

int TofCalibrationMap::FindTriggerKey( TriggerKey key )
{
	for ( unsigned int i=0; i<_Tkey.size(); ++i )
		if ( _Tkey.at(i) == key )
			 return i;

	 return NOCALIB;
}

double TofCalibrationMap::T0( PmtKey key ,int &r) 
{
	int n = FindPmtKey( key ); 

	if ( n != NOCALIB) 
	{
		r = _reff[n];
		if( _t0[n] ) return _t0[n];
	}
	
	//cout<<"TofCalibrationMap -> No T0 calibration for "<<key<<endl;
	return NOCALIB;
}

double TofCalibrationMap::TriggerT0( TriggerKey key)
{
	int n = FindTriggerKey( key );
	if ( n != NOCALIB)
		return _Trt0[n];
	
   //cout<<"TofCalibrationMap -> No Trigger calibration for "<<key<<endl;
	return n;
}

double TofCalibrationMap::TW( PmtKey key, int adc ) 
{
	int n = FindPmtKey( key ); 

	if ( n != NOCALIB)
	{
		double x = adc + _twPar[n][0];
		double x2 = x*x;
		double p1 = _twPar[n][1];
		double p2 = _twPar[n][2]/x;
		double p3 =_twPar[n][3]/x2;
		double dt_tw = p1 + p2 + p3;
		if(_twPar[n][0] && _twPar[n][1] && _twPar[n][2] && _twPar[n][3])
		  return dt_tw;
	} 
	//cout<<"TofCalibrationMap -> No TW calibration for "<<key<<endl;
   return NOCALIB;
}

double TofCalibrationMap::dT(PmtKey Pkey, TriggerKey TrKey, int adc)
{
    int reffSlab;
	 double tw = TW(Pkey, adc);
	 double t0 = T0(Pkey, reffSlab);
	 double trt0 = TriggerT0(TrKey);

	 if (tw==NOCALIB || t0==NOCALIB || trt0==NOCALIB){ return NOCALIB; }
	 
	 double dt = tw - t0 + trt0;
	 //cout<<"TofCalibrationMap -> tw = "<<tw<<"  t0 = "<<-t0<<"  trt0 = "<<trt0<<" "<<dt<<endl;
	 if(Pkey.station() == TriggerStation)
	 {
		if(Pkey.plane()==0) 
		{
		  TriggerKey refTr(TriggerStation, Pkey.slab(), reffSlab);
		  if( TriggerT0(refTr)==NOCALIB ) return NOCALIB;
		  dt -= TriggerT0(refTr);
		}
		else
		{
		  TriggerKey refTr(TriggerStation, reffSlab, Pkey.slab());
		  if( TriggerT0(refTr)==NOCALIB ) return NOCALIB;
		  dt -= TriggerT0(refTr);
		}
	 }
	 //cout<<"TofCalibrationMap -> dT = "<<-dt<<endl;
	 return - dt;
}


