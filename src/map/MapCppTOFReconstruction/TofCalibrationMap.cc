#include "TofCalibrationMap.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

using CLHEP::picosecond;
using CLHEP::cm;
using CLHEP::c_light;

TofCalibrationMap::TofCalibrationMap()
{
  Name = "NoName";
  Detector = "NoDetector";
}

TofCalibrationMap::TofCalibrationMap(vector<const MiceModule*> tofStations)
{
  Name = "NoName";
  Detector = "NoDetector";
  MakePmtKeys(tofStations);
}

TofCalibrationMap::~TofCalibrationMap()
{
  _Pkey.clear();
  _Tkey.clear();
  _twPar.resize(0);
  _t0.resize(0);
  _reff.resize(0);
}

void TofCalibrationMap::InitializeFromCards(dataCards* cards, vector<const MiceModule*> tofStations)
{
  	//! Initialize the map by using the text files specified in the card
	//! TofCalib. Constants are load from ft0, ftw and ftr.
	//! Check for a possible conflict between the trigger station number in
	//! the text file and the trigger station number provided by the datacards is done.

  MakePmtKeys(tofStations);
  //for(unsigned int i=0;i<_Pkey.size();i++)
	 //cout<<_Pkey[i]<< endl;
  char* MICEFILES = getenv( "MICEFILES" );
  string calibfile = cards->fetchValueString( "TofCalib" );

  string ft0, ftw, ftr;
  ftw = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "TW.txt";

  int TriggerSt = cards->fetchValueInt( "TofTriggerStation" );

  if(TriggerSt==0)
  {
	 ft0 = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "T0_trTOF0.txt";
	 ftr = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "Trigger_trTOF0.txt";
  }
  if(TriggerSt==1)
  {
	 ft0 = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "T0_trTOF1.txt";
	 ftr = string( MICEFILES ) + "/Calibration/TOF/" + calibfile + "Trigger_trTOF1.txt";
  }

  try  { this->Initialize(ft0, ftw, ftr);}
  catch (PmtKey e)
  {
    cerr << "*** ERROR in TofCalibrationMap::InitializeFromCards() ***"<<endl;
    cerr << "*** "<< e << " is not presented in the current configuration ***"<<endl;
    cerr << "*** Unable to initialize TofCalibrationMap ***"<<endl;
    cerr << "*** exit ***"<< endl;
    exit(1);
  }

  if( TriggerSt != this->GetTriggerStation() )
  {
	 cerr << "*** ERROR in TofCalibrationMap::InitializeFromCards() ***"<<endl;
	 cerr <<"*** Trigger in TOF"<< TriggerSt <<"  ***"<<endl;
	 cerr <<"*** Wrong trigger delays file" << ftr <<"  ***"<< endl;
	 exit(1);
  }
}

bool TofCalibrationMap::InitializeFromDB(DataBaseReader* dbreader, int runNum)
{
  XMLMessage request, response;
  request.MakeCalibRequest("2010-06-11 22:42:44.0", "TOF");
  /*
  stringstream det;
  det<<"TOFtrigger"<<dbreader->GetTrigger(int runNum);
  string detectorName = det.str();
  request.MakeCalibRequest(runNim, detectorName);
  */
  response = dbreader->GetDataBaseAPI()->sendQuery( request );
  if( !dbreader->GetDataBaseAPI()->IsConnected() )
  {
    cerr << "*** ERROR in TofCalibrationMap::InitializeFromDB() ***"<<endl;
	 cerr << "*** No connection ***"<<endl;
	 cerr << "*** Unable to initialize TofCalibrationMap ***"<<endl;
    cerr << "*** exit ***"<< endl;
    exit(1);
  }
  string n, d;
  response.GetCalibHeader(n, d);
  if(Name==n && Detector==d)
  {
	 cout<<"TofCalibrationMap is already loaded."<<endl;
	 return true;
  }else
  {
	 Name = n;
	 Detector = d;
  }
  try{ this->LoadXML( response );}
  catch (PmtKey e)
  {
    cerr << "*** ERROR in TofCalibrationMap::InitializeFromDB() ***"<<endl;
    cerr << "*** "<< e << " is not presented in the current configuration ***"<<endl;
    cerr << "*** Unable to initialize TofCalibrationMap ***"<<endl;
    cerr << "*** exit ***"<< endl;
    exit(1);
  }
  cout<<"TofCalibrationMap is loaded from CDB server."<<endl;
  return true;
}

void TofCalibrationMap::Initialize(string t0File, string twFile, string triggerFile)
{
	LoadT0File(t0File);
	LoadTWFile(twFile);
	LoadTriggerFile(triggerFile);
}

void TofCalibrationMap::LoadXML( XMLMessage calib )
{
   vector<string> params;
	string element;

	string pName, pType;
	unsigned int T0entries=0, TWentries=0, Trentries=0, nparams=0;

	unsigned int elementNum = 0;

	//! Get the first element. The number of the T0 entries
	//! and the number of parameters in each entrie are recorded here.
   element = calib.GetCalibElement(elementNum, params);

	//!  Get the number of the entries for T0 calibration.
	GetNameTypeFromXML(params[0], pName, pType);
	if(pType=="int" && pName=="T0_entries")
	T0entries = GetParamValueInt(params[0]);

	//!  Get the number of parameters in each entrie.
   GetNameTypeFromXML(params[1], pName, pType);
	if(pType=="int" && pName=="NParams")
	nparams = GetParamValueInt(params[1]);

   //cout<<element<<endl;
	//cout<<T0entries<<endl;

	//! Read all T0 entries.
	while(elementNum < T0entries)
	{
	  params.resize(0);
	  elementNum++;
	  element = calib.GetCalibElement(elementNum, params);
	  PmtKey key;
	  //cout<<element<<endl;
	  //cout<<params.size()<<" "<<nparams<<endl;

	  //!This is not very clever. To be improved !!!
	  if(params.size()!=nparams || params.size()!=6)
	  {
        cerr << "*** ERROR in TofCalibrationMap::LoadXML() ***"<<endl;
        cerr << "*** Input XML code is corrupted ***"<<endl;
		  cerr << "*** Unable to initialize TofCalibrationMap ***"<<endl;
		  cerr << "*** exit ***"<< endl;
		  exit(1);
	  }
	  //cout<<"element "<<elementNum<<"  ";

	  //! Now set the PmtKey.
	  GetNameTypeFromXML(params[0], pName, pType);
     if(pName=="t0_station")
		 key.SetStation( GetParamValueInt(params[0]) );

	  GetNameTypeFromXML(params[1], pName, pType);
     if(pName=="t0_plane")
		 key.SetPlane( GetParamValueInt(params[1]) );

	  GetNameTypeFromXML(params[2], pName, pType);
     if(pName=="t0_slab")
		 key.SetSlab( GetParamValueInt(params[2]) );

	  GetNameTypeFromXML(params[3], pName, pType);
     if(pName=="t0_pmt")
		 key.SetPmt( GetParamValueInt(params[3]) );

	  //! Get the calibration constants.

	  double p0, reff;
	  GetNameTypeFromXML(params[4], pName, pType);
     if(pName=="t0_p0") p0 = GetParamValueDouble(params[4]);

	  GetNameTypeFromXML(params[5], pName, pType);
     if(pName=="t0_reffSlab") reff = GetParamValueInt(params[5]);

     //!Find the position of this key in the map.
	  int n = FindPmtKey( key );

     //! Set the values of the constants in the map.
     _t0[n] = p0;
     _reff[n] = reff;
	}

   //! Get the next element. The number of the TimeWalk entries
	//! and the number of parameters in each entrie are recorded here.
	elementNum++;
   params.resize(0);
   element = calib.GetCalibElement(elementNum, params);

	//!  Get the number of the entries for TW calibration.
	GetNameTypeFromXML(params[0], pName, pType);
	if(pType=="int" && pName=="TW_entries")
	   TWentries = GetParamValueInt(params[0]);

	//!  Get the number of parameters in each entrie.
   GetNameTypeFromXML(params[1], pName, pType);
	if(pType=="int" && pName=="NParams")
	   nparams = GetParamValueInt(params[1]);

	//cout<<element<<endl;
	//cout<<TWentries<<endl;

	//! Read all TW entries.
	_twPar.resize(TWentries);
	while(elementNum < (TWentries+T0entries+1))
	{
	  params.resize(0);
	  elementNum++;
	  element = calib.GetCalibElement(elementNum, params);
	  PmtKey key;
     //cout<<element<<endl;
	  //cout<<params.size()<<" "<<nparams<<endl;

	  //!This is not very clever. To be improved !!!
	  if(params.size()!=nparams || params.size()!=8)
	  {
        cerr << "*** ERROR in TofCalibrationMap::LoadXML() ***"<<endl;
        cerr << "*** Input XML code is corrupted ***"<<endl;
		  cerr << "*** Unable to initialize TofCalibrationMap ***"<<endl;
		  cerr << "*** exit ***"<< endl;
		  exit(1);
	  }
	  //cout<<"element "<<elementNum<<"  ";

	  //! Now set the PmtKey.
	  GetNameTypeFromXML(params[0], pName, pType);
     if(pName=="tw_station")
		 key.SetStation( GetParamValueInt(params[0]) );

	  GetNameTypeFromXML(params[1], pName, pType);
     if(pName=="tw_plane")
		 key.SetPlane( GetParamValueInt(params[1]) );

	  GetNameTypeFromXML(params[2], pName, pType);
     if(pName=="tw_slab")
		 key.SetSlab( GetParamValueInt(params[2]) );

	  GetNameTypeFromXML(params[3], pName, pType);
     if(pName=="tw_pmt")
		 key.SetPmt( GetParamValueInt(params[3]) );

	  //! Get the calibration constants.

	  double p0,p1,p2,p3;
	  GetNameTypeFromXML(params[4], pName, pType);
     if(pName=="tw_p0") p0 = GetParamValueDouble(params[4]);

	  GetNameTypeFromXML(params[5], pName, pType);
     if(pName=="tw_p1") p1 = GetParamValueDouble(params[5]);

	  GetNameTypeFromXML(params[6], pName, pType);
     if(pName=="tw_p2") p2 = GetParamValueDouble(params[6]);

	  GetNameTypeFromXML(params[7], pName, pType);
     if(pName=="tw_p3") p3 = GetParamValueDouble(params[7]);

	  //cout<<key<<endl;
     //!Find the position of this key in the map.
	  int n = FindPmtKey( key );

     //! Set the values of the constants in the map.
     _twPar[n].resize(4);
     _twPar[n][0] = p0;
     _twPar[n][1] = p1;
     _twPar[n][2] = p2;
     _twPar[n][3] = p3;
	}

   //! Get the next element. The number of the Trigger delay entries
	//! and the number of parameters in each entrie are recorded here.
	elementNum++;
   params.resize(0);
   element = calib.GetCalibElement(elementNum, params);

	//!  Get the number of the entries for Trigger calibration.
	GetNameTypeFromXML(params[0], pName, pType);
	if(pType=="int" && pName=="TrDelay_entries")
	   Trentries = GetParamValueInt(params[0]);

	//!  Get the number of parameters in each entrie.
   GetNameTypeFromXML(params[1], pName, pType);
	if(pType=="int" && pName=="NParams")
	   nparams = GetParamValueInt(params[1]);

	//cout<<element<<endl;
	//cout<<Trentries<<"  "<<nparams<<endl;

//! Read all Trigger delay entries.
	while(elementNum < (T0entries+TWentries+Trentries+2))
	{
	  params.resize(0);
	  elementNum++;
	  element = calib.GetCalibElement(elementNum, params);
	  TriggerKey key;
	  //cout<<element<<endl;
	  //cout<<params.size()<<" "<<nparams<<endl;

	  //!This is not very clever. To be improved !!!
	  if(params.size()!=nparams || params.size()!=4)
	  {
        cerr << "*** ERROR in TofCalibrationMap::LoadXML() ***"<<endl;
        cerr << "*** Input XML code is corrupted ***"<<endl;
		  cerr << "*** Unable to initialize TofCalibrationMap ***"<<endl;
		  cerr << "*** exit ***"<< endl;
		  exit(1);
	  }
	  //cout<<"element "<<elementNum<<"  ";

	  //! Now set the TriggerKey.
	  GetNameTypeFromXML(params[0], pName, pType);
     if(pName=="tr_station")
		 key.SetStation( GetParamValueInt(params[0]) );

	  GetNameTypeFromXML(params[1], pName, pType);
     if(pName=="tr_slabA")
		 key.SetSlabA( GetParamValueInt(params[1]) );

	  GetNameTypeFromXML(params[2], pName, pType);
     if(pName=="tr_slabB")
		 key.SetSlabB( GetParamValueInt(params[2]) );

	  //! Get the calibration constant.

	  double p0;
	  GetNameTypeFromXML(params[3], pName, pType);
     if(pName=="tr_p0") p0 = GetParamValueDouble(params[3]);
     //cout<<key<<" "<<p0<<endl;
		//! Add the kay for this pixel in the calibration map.
		_Tkey.push_back(key);
		//! Set the values of the constant in the map.
		_Trt0.push_back(p0);
	}

   //!  Use the last element to set the TriggerStation data member.
   TriggerStation = GetParamValueInt(params[0]);
}

bool TofCalibrationMap::GetNameTypeFromXML(string param, string& name, string& type)
{
   size_t begin=0, end=0;
	string target1, target2;
	target1 = "name=&quot;";
	target2 = "&quot;";
	begin = param.find(target1);
	end = param.find( target2, begin+target1.size() );
	name = param.substr( begin+target1.size(), end-begin-target1.size() );
	if(begin == string::npos) return false;

	target1 = "type=&quot;";
	begin = param.find(target1);
	end = param.find( target2, begin+target1.size() );
	type = param.substr( begin+target1.size(), end-begin-target1.size() );
	if(begin == string::npos) return false;

	return true;
}

double TofCalibrationMap::GetParamValueDouble(string param)
{
   stringstream str;
	double value;
   size_t begin=0, end=0;
	string target1, target2;

	target1 = "value=&quot;";
	target2 = "&quot;";
	begin = param.find(target1);
	end = param.find( target2, begin+target1.size() );

	str << param.substr( begin+target1.size(), end-begin-target1.size() );
	str >> value;
	//cout<<"GetParamValueDouble  "<<str.str()<<"  "<<value<<endl;
   return value;
}

int TofCalibrationMap::GetParamValueInt(string param)
{
   stringstream str;
	int value;
   size_t begin=0, end=0;
	string target1, target2;

	target1 = "value=&quot;";
	target2 = "&quot;";
	begin = param.find(target1);
	end = param.find( target2, begin+target1.size() );

	str << param.substr( begin+target1.size(), end-begin-target1.size() );
	str >> value;
   return value;
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

		if(! stream.eof()){
		  TriggerKey Tkey(station, slabA, slabB);
		  _Tkey.push_back(Tkey);
		  _Trt0.push_back(dt);
		}
	 }
  TriggerStation = station;
}

void TofCalibrationMap::MakePmtKeys( vector<const MiceModule*> tofStationModules )
{
   //! Makes one PmtKey for each channel of the TOF detector.
	//! The detector description is provided by tofStationModules argument.
	//! The size of _t0, _reff and _twPar is set here.

   int nStation = tofStationModules.size();
	for(int st=0;st<nStation;st++)
	  for(int pl=0;pl<tofStationModules[st]->daughters();pl++)
       if(tofStationModules[st]->daughter(pl)->propertyExistsThis( "Plane", "int" ))
		 {
	      int nSlabs = tofStationModules[st]->daughter(pl)->propertyInt( "numSlabs" );
			for(int sl=0;sl<nSlabs;sl++)
		     for(int pmt=0;pmt<2;pmt++)
			    _Pkey.push_back( PmtKey(st,pl,sl,pmt) );
		 }
	int nChannels = _Pkey.size();
   _t0.resize( nChannels );
	_reff.resize( nChannels );
	_twPar.resize( nChannels );
}

int TofCalibrationMap::FindPmtKey( PmtKey key )
{
  for ( unsigned int i=0; i<_Pkey.size(); ++i )
	 if ( _Pkey.at(i) == key )
		return i;

  throw key;
  return NOCALIB;
}

int TofCalibrationMap::FindTriggerKey( TriggerKey key )
{
	for ( unsigned int i=0; i<_Tkey.size(); ++i )
	  if ( _Tkey.at(i) == key )
		 return i;

  //throw key;
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
	 //cout<<"TofCalibrationMap -> tw = "<<tw<<"  t0 = "<<-t0<<"  trt0 = "<<trt0<<" tot = "<<dt<<endl;
	 //cout<<"PMTSt = "<<Pkey.station()<<"   TrSt = "<<TriggerStation<<endl;
	 if(Pkey.station() == TriggerStation)
	 {
		if(Pkey.plane()==0)
		{
		  TriggerKey refTr(TriggerStation, Pkey.slab(), reffSlab);
		  if( TriggerT0(refTr)==NOCALIB ) return NOCALIB;
		  dt -= TriggerT0(refTr);
		  //cout<<refTr<<"  dt = "<<TriggerT0(refTr)<<endl;
		}
		else
		{
		  TriggerKey refTr(TriggerStation, reffSlab, Pkey.slab());
		  if( TriggerT0(refTr)==NOCALIB ) return NOCALIB;
		  dt -= TriggerT0(refTr);
		  //cout<<refTr<<"  dt = "<<TriggerT0(refTr)<<endl;
		}
	 }
	 //cout<<"TofCalibrationMap -> dT = "<<-dt<<endl;
	 return - dt;
}

string TofCalibrationMap::CalibParDoubleToXML(string name, double p)
{
   std::stringstream xml ;
   xml  << "<CalibrationParameter name=\""<< name <<"\" value=\" "<< p <<"\" type=\"double\">" << endl;
	xml  << "</CalibrationParameter>" << endl;
	return xml.str();
}

string TofCalibrationMap::CalibParIntToXML(string name, int p)
{
   std::stringstream xml ;
   xml  << "<CalibrationParameter name=\""<< name <<"\" value=\" "<< p <<"\" type=\"int\">" << endl;
	xml  << "</CalibrationParameter>" << endl;
   return xml.str();
}

string TofCalibrationMap::CalibMapToXML()
{
   std::stringstream xml ;
   int n_tr = _Tkey.size();
	int n_pmt = _Pkey.size();

	xml  << "<CalibrationParameterSet element=\""<< 0 <<"\">" << endl;
   xml  << CalibParIntToXML( "T0_entries", _Pkey.size() );
	xml  << CalibParIntToXML( "NParams", 6 );
	xml  << "</CalibrationParameterSet>" << endl;

   for(int i=0;i<n_pmt;i++)
	{
	   xml  << "<CalibrationParameterSet element=\""<< i+1 <<"\">" << endl;
      xml  << CalibParIntToXML( "t0_station", _Pkey[i].station() );
		xml  << CalibParIntToXML( "t0_plane", _Pkey[i].plane() );
		xml  << CalibParIntToXML( "t0_slab", _Pkey[i].slab() );
		xml  << CalibParIntToXML( "t0_pmt", _Pkey[i].pmt() );
		xml  << CalibParDoubleToXML( "t0_p0", _t0[i] );
		xml  << CalibParIntToXML( "t0_reffSlab", _reff[i] );
		xml  << "</CalibrationParameterSet>" << endl;
	}

	xml  << "<CalibrationParameterSet element=\""<< n_pmt + 1 <<"\">" << endl;
   xml  << CalibParIntToXML( "TW_entries", _Pkey.size() );
	xml  << CalibParIntToXML( "NParams", 8 );
	xml  << "</CalibrationParameterSet>" << endl;

   for(int i=0;i<n_pmt;i++)
	{
	   xml  << "<CalibrationParameterSet element=\""<< n_pmt+2+i <<"\">" << endl;
      xml  << CalibParIntToXML( "tw_station", _Pkey[i].station() );
		xml  << CalibParIntToXML( "tw_plane", _Pkey[i].plane() );
		xml  << CalibParIntToXML( "tw_slab", _Pkey[i].slab() );
		xml  << CalibParIntToXML( "tw_pmt", _Pkey[i].pmt() );
		xml  << CalibParDoubleToXML( "tw_p0", _twPar[i][0] );
      xml  << CalibParDoubleToXML( "tw_p1", _twPar[i][1] );
		xml  << CalibParDoubleToXML( "tw_p2", _twPar[i][2] );
		xml  << CalibParDoubleToXML( "tw_p3", _twPar[i][3] );
		xml  << "</CalibrationParameterSet>" << endl;
	}

	xml  << "<CalibrationParameterSet element=\""<<  2*n_pmt+2 <<"\">" << endl;
   xml  << CalibParIntToXML( "TrDelay_entries", _Tkey.size() );
	xml  << CalibParIntToXML( "NParams", 4 );
	xml  << "</CalibrationParameterSet>" << endl;

   for(int i=0;i<n_tr;i++)
	{
	   xml  << "<CalibrationParameterSet element=\""<< 2*n_pmt+3+i <<"\">" << endl;
      xml  << CalibParIntToXML( "tr_station", _Tkey[i].station() );
		xml  << CalibParIntToXML( "tr_slabA", _Tkey[i].slabA() );
		xml  << CalibParIntToXML( "tr_slabB", _Tkey[i].slabB() );
		xml  << CalibParDoubleToXML( "tr_p0", _Trt0[i] );
		xml  << "</CalibrationParameterSet>" << endl;
	}

   return xml.str();
}

void TofCalibrationMap::Print()
{
  cout<<"====================== TofCalibrationMap ============================="<<endl;
  cout<<" Name : "<< Name <<endl;
  cout<<" Trigger in TOF"<< TriggerStation << endl;
  cout<<" Number of channels : "<<_Pkey.size()<<endl;
  cout<<" Number of calibrated pixels in the trigger station : "<<_Tkey.size()<<endl;
  for(unsigned int i=0;i<_Pkey.size();i++){
	 cout<<_Pkey[i]<<" T0 :"<< _t0[i] <<", "<<_reff[i];
    cout<<"  TW: "<<_twPar[i][0]<<", "<<_twPar[i][1]<<", "<<_twPar[i][2]<<", "<<_twPar[i][3]<<endl;
  }
  for(unsigned int i=0;i<_Tkey.size();i++)
    cout<<_Tkey[i]<<"  "<< _Trt0[i]<<endl;
  cout<<"======================================================================="<<endl;
}






