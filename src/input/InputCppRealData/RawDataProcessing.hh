#ifndef _MAUS_INPUTCPPREALDATA_RAWDATPROCESSING_HH_
#define _MAUS_INPUTCPPREALDATA_RAWDATPROCESSING_HH_

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <algorithm>
#include <fstream>
#include <sstream>

// do not increment inside following macro! (e.g.: MAX( ++a, ++b );
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////
class fADCDataProcessor
{
 public:

  fADCDataProcessor(){}
  ~fADCDataProcessor(){}

  int GetMaxPosition();
  int GetSignalArea(int&);
  int GetPedestalArea(int&);
  int	GetPoint(int i) const		{ return data[i]; }
  std::vector<int> GetData() const	{ return data; }
  int GetArea();
  int GetPedestal() const		{ return pedestal; }

  int GetCharge( int Algorithm = ceaPedMax );

  enum ChargeEstimationAlgorithm {
    ceaMinMax = 0, //Simplest algorithm
    ceaFractionDescriminatorThreshold = 1, // not implemented
    ceaPedMax = 3
  };

 protected:

  void SetPedestal();
  int ChargeMinMax();
  int ChargePedMax();
  int charge;
  std::vector<int> data;
  int pedestal;
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

//////////////////////////////////////////////////////////////////////////////////////////////

class DAQChannelMap
{
 public:
  DAQChannelMap(){}
  virtual ~DAQChannelMap();

  void InitFromFile(string filename);
  void InitFromCDB();
  DAQChannelKey* find(int ldc, int geo, int ch, int eqType);
  string detector(int ldc, int geo, int ch, int eqType);

 private:
  vector<DAQChannelKey*> _chKey;
};



#endif




