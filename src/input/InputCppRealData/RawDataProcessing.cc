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





