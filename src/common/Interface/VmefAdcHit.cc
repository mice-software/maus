// VmefAdcHit.cc
//
// A class describing a hit on an fADC channel in a VME module
//
// Y. Karadzhov April 2008

#include "Interface/VmefAdcHit.hh"

#include <iostream>
#include <fstream>

using namespace std;

VmefAdcHit::VmefAdcHit( int crate,  int board, std::string type, int chan, std::vector<int> d )
:VmeBaseHit( crate, board, type, chan )
{
	ttt = 0;
  miceMemory.addNew( Memory::VmefAdcHit );
  data = d;
  SetPedestal();
}

int VmefAdcHit::GetArea()
{
    int area = 0;
    for( unsigned int ich = 0; ich < data.size(); ich++)
    area += abs( data[ich] - pedestal );
    return area;
}

void VmefAdcHit::Print()
{
	cout << "____________________________"   << endl;
	cout << "  VmefAdcHit"                   << endl;
	cout << "  Crate Id : " << crate()       << endl;
	cout << "  Board Id : "<< board()        << endl;
	cout << "  Board Type : "<< moduleType() << endl;
	cout << "  Channel : " << channel()      << endl;
	cout << "  Total area : " << GetArea()   << endl;
	cout << "  Charge : " << GetCharge()     << endl;
	cout << "  Pedestal : " << pedestal     << endl;
	cout << "  Trig. Time Tag : " << ttt << endl;
//	for( unsigned int ich = 0; ich < data.size(); ich++)
//	   cout << ich << "   " <<data[ich] << "  " <<data[ich] - pedestal << "  "<<endl;
//  	cout << "____________________________"   << endl;
}

void VmefAdcHit::SetPedestal()
{
   int area = 0;
   unsigned int pedBins = 20;
	if( data.size() > pedBins)
   for(unsigned int i=0;i<pedBins;i++)
       area += data[i];
   pedestal = area/pedBins;
}

int VmefAdcHit::ChargeMinMax()
{
  vector<int>::iterator max = max_element( data.begin(), data.end() );
  vector<int>::iterator min = min_element( data.begin(), data.end() );
  return (*max) - (*min);
}

int VmefAdcHit::GetSignalArea(int& pos)
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

int VmefAdcHit::GetPedestalArea(int& pos)
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


int VmefAdcHit::GetMaxPosition()
{
	int pos = 0;
	vector<int>::iterator max;
	max = max_element(data.begin(), data.end());
	pos = distance(data.begin(),max);

	return pos;
}


int VmefAdcHit::ChargePedMax()
{
	int max = 0;
 	max = *max_element(data.begin(), data.end());

	return max - pedestal;
}



int VmefAdcHit::GetCharge( ChargeEstimationAlgorithm Algorithm ){
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


