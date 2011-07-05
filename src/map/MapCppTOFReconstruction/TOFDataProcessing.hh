#ifndef _MAUS_INPUTCPPREALDATA_TOFDATPROCESSING_HH_
#define _MAUS_INPUTCPPREALDATA_TOFDATPROCESSING_HH_

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
class TOFChannelKey
{
 public:
  TOFChannelKey()
  :_station(-999),_plane(-999),_slab(-999),_pmt(-999),_detector("unknown"){}

  TOFChannelKey(int l, int g, int ch, int e, string d)
  :_station(l),_plane(g),_slab(ch),_pmt(e),_detector(d){}
	virtual ~TOFChannelKey(){}

  bool operator==( TOFChannelKey key );
  friend ostream& operator<< ( ostream& stream, TOFChannelKey key );
  friend istream& operator>> ( istream& stream, TOFChannelKey &key );

	string detector() {return _detector;}
	string str();
  int station()     {return _station;}
  int plane()     {return _plane;}
  int slab() {return _slab;}
  int pmt()  {return _pmt;}
	int make_TOFChannelKey_id()
	{ return _station*1e7 + _plane*1e5 + _slab*1e3 + _pmt; }

  int _station;
  int _plane;
  int _slab;
  int _pmt;
	string _detector;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class DAQChannelKey
{
 public:
  DAQChannelKey()
  :_ldcId(-999),_geo(-999),_channel(-999),_eqType(-999),_detector("unknown"){}

  DAQChannelKey(int l, int g, int ch, int e, string d)
  :_ldcId(l),_geo(g),_channel(ch),_eqType(e),_detector(d){}
	virtual ~DAQChannelKey(){}

  bool operator==( DAQChannelKey key );
  friend ostream& operator<< ( ostream& stream, DAQChannelKey key );
  friend istream& operator>> ( istream& stream, DAQChannelKey &key );

	string detector() {return _detector;}
	string str();

  int ldc()     {return _ldcId;}
  int geo()     {return _geo;}
  int channel() {return _channel;}
  int eqType()  {return _eqType;}
	int make_DAQChannelKey_id()
	{ return _ldcId*1e7 + _geo*1e5 + _channel*1e3 + _eqType; }

  int _ldcId;
  int _geo;
  int _channel;
  int _eqType;
	string _detector;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class TOFChannelMap
{
 public:
  TOFChannelMap(){}
  virtual ~TOFChannelMap();

  void InitFromFile(string filename);
  void InitFromCDB();
  TOFChannelKey* find(DAQChannelKey* daqch);

 private:
  vector<TOFChannelKey*> _tofKey;
  vector<DAQChannelKey*> _tdcKey;
	vector<DAQChannelKey*> _fadcKey;
};

#endif




