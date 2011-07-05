


//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TOFDataProcessing.hh"

TOFChannelMap::~TOFChannelMap()
{
  for(unsigned int i=0;i<_tofKey.size();i++){
    delete _tofKey[i];
    delete _tdcKey[i];
    delete _fadcKey[i];
  }
  _tdcKey.resize(0);
  _fadcKey.resize(0);
  _tofKey.resize(0);

}

void TOFChannelMap::InitFromFile(string filename)
{
  ifstream stream( filename.c_str() );
  if( !stream )
  {
    cerr << "Can't TOF open cabling file " << filename << endl;
    exit(1);
  }
	stringstream key_s;
	TOFChannelKey* tofkey;
	DAQChannelKey* tdckey;
	DAQChannelKey* fadckey;

  while ( ! stream.eof() ){
		tofkey = new TOFChannelKey();
		tdckey = new DAQChannelKey();
		fadckey = new DAQChannelKey();
		stream >> *tofkey >> *fadckey >> *tdckey;
		_tofKey.push_back(tofkey);
		_tdcKey.push_back(tdckey);
		_fadcKey.push_back(fadckey);
	}
}

void TOFChannelMap::InitFromCDB()
{}

TOFChannelKey* TOFChannelMap::find(DAQChannelKey *daqch)
{
	if(daqch->eqType()==102)
	for(unsigned int i=0;i<_tofKey.size();i++)
    if( _tdcKey[i]->ldc()     == daqch->ldc() &&
        _tdcKey[i]->geo()     == daqch->geo() &&
        _tdcKey[i]->channel() == daqch->channel() ){
      return _tofKey[i];
    }

	if(daqch->eqType()==120)
	for(unsigned int i=0;i<_tofKey.size();i++)
    if( _fadcKey[i]->ldc()     == daqch->ldc() &&
        _fadcKey[i]->geo()     == daqch->geo() &&
        _fadcKey[i]->channel() == daqch->channel() ){
      return _tofKey[i];
    }
  return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool TOFChannelKey::operator== ( TOFChannelKey const key ) {
  if ( _station == key._station &&
       _plane == key._plane &&
       _slab == key._slab &&
       _pmt == key._pmt &&
       _detector == key._detector) {
		return true;
  }
  else return false;
}

ostream& operator<<( ostream& stream, TOFChannelKey key ) {
	stream << "TOFChannelKey " << key._station;
	stream << " " << key._plane;
	stream << " " << key._slab;
	stream << " " << key._pmt;
	stream << " " << key._detector;
	return stream;
}

istream& operator>>( istream& stream, TOFChannelKey &key ) {
	string xLabel;
	stream >> xLabel >> key._station >> key._plane >> key._slab >> key._pmt >> key._detector;
	return stream;
}

string TOFChannelKey::str()
{
	stringstream xConv;
	xConv<<(*this);
	return xConv.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool DAQChannelKey::operator== ( DAQChannelKey const key ) {
  if ( _ldcId == key._ldcId &&
       _geo == key._geo &&
       _channel == key._channel &&
       _eqType == key._eqType &&
       _detector == key._detector) {
		return true;
  }
  else return false;
}

ostream& operator<<( ostream& stream, DAQChannelKey key ) {
	stream << "DAQChannelKey " << key._ldcId;
	stream << " " << key._geo;
	stream << " " << key._channel;
	stream << " " << key._eqType;
	stream << " " << key._detector;
	return stream;
}

istream& operator>>( istream& stream, DAQChannelKey &key ) {
	string xLabel;
	stream >> xLabel >> key._ldcId >> key._geo >> key._channel >> key._eqType >> key._detector;
	return stream;
}

string DAQChannelKey::str()
{
	stringstream xConv;
	xConv<<(*this);
	return xConv.str();
}