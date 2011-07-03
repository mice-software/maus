#include "RawDataProcessing.hh"

int fADCDataProcessor::GetArea()
{
    int area = 0;
    for( unsigned int ich = 0; ich < data.size(); ich++)
    area += abs( data[ich] - pedestal );
    return area;
}


void fADCDataProcessor::SetPedestal()
{
   int area = 0;
   unsigned int pedBins = 20;
	if( data.size() > pedBins)
   for(unsigned int i=0;i<pedBins;i++)
       area += data[i];
   pedestal = area/pedBins;
}

int fADCDataProcessor::ChargeMinMax()
{
  int min, max, d;
  d = -99;
  max = LONG_MIN;
  min = LONG_MAX;
  for( unsigned int i = 0; i < data.size(); ++i ){
    d = data[ i ];
    min = ( min > d )? d : min;
    max = ( max > d )? max : d;
  }

 return max - min;
}

int fADCDataProcessor::GetSignalArea(int& pos)
{
	vector<int>::iterator it = data.begin();
	vector<int>::iterator max;
	int area = 0;

	max = max_element(data.begin(), data.end());
	pos = distance(data.begin(),max);

	if(pos>10) it = max-10;

	while(it<max+20){
		area += *it - pedestal;
		it++;
	}

	return area;
}

int fADCDataProcessor::GetPedestalArea(int& pos)
{
	vector<int>::iterator it = data.begin();
	vector<int>::iterator max;
	int pedareaminus = 0;
	int pedareaplus = 0;

	max = max_element(data.begin(), data.end());
	pos = distance(data.begin(),max);

	if(pos>30 && pos<97) {
		it = max-30;
		while(it<max-10){
			pedareaminus += *it - pedestal;
			it++;
		}

		it = max+20;
		while(it<max+30){
			pedareaplus += *it - pedestal;
			it++;
		}
	}

	return pedareaminus + pedareaplus;
}


int fADCDataProcessor::GetMaxPosition()
{
	int pos = 0;
	vector<int>::iterator max;
	max = max_element(data.begin(), data.end());
	pos = distance(data.begin(),max);

	return pos;
}


int fADCDataProcessor::ChargePedMax()
{
	int max = 0;
 	max = *max_element(data.begin(), data.end());

	return max - pedestal;
}



int fADCDataProcessor::GetCharge( int Algorithm ){
  switch( Algorithm ){
  case ceaMinMax:
    charge = ChargeMinMax();
    break;
  case ceaPedMax:
    charge = ChargePedMax();
    break;
   default:
    charge = -99;
    break;
  }

  return charge;
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



