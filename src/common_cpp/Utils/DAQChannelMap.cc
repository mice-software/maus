#include "DAQChannelMap.hh"

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

bool DAQChannelKey::operator!= ( DAQChannelKey const key ) {
  if ( _ldcId == key._ldcId &&
       _geo == key._geo &&
       _channel == key._channel &&
       _eqType == key._eqType &&
       _detector == key._detector) {
		return false;
  }
  else return true;
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

//////////////////////////////////////////////////////////////////////////////////////////////////////



DAQChannelMap::~DAQChannelMap()
{
  for(unsigned int i=0;i<_chKey.size();i++)
    delete _chKey[i];

  _chKey.resize(0);
}

void DAQChannelMap::InitFromFile(string filename)
{

  ifstream stream( filename.c_str() );
  if( !stream )
  {
    std::cerr << "Can't DAQ open cabling file " << filename << std::endl;
    exit(1);
  }
	stringstream key_s;
	DAQChannelKey* key;

  while ( ! stream.eof() ){
		key = new DAQChannelKey();
		stream >> *key;
		_chKey.push_back(key);
	}
}

void DAQChannelMap::InitFromCDB()
{}

DAQChannelKey* DAQChannelMap::find(int ldc, int geo, int ch, int eqType)
{
	for(unsigned int i=0;i<_chKey.size();i++)
    if( _chKey[i]->ldc() == ldc &&
        _chKey[i]->geo() == geo &&
        _chKey[i]->channel() == ch &&
        _chKey[i]->eqType() == eqType )
      return _chKey[i];

  return NULL;
}

string DAQChannelMap::detector(int ldc, int geo, int ch, int eqType)
{
  DAQChannelKey* key = find(ldc, geo, ch, eqType);
  if( key )
    return key->detector();

  return "unknown";
}

