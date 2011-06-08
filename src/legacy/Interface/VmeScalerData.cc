// VmeScalerData.cc
//
// VME Scaler (CAEN V830) data container class
// EqId is 111
//
// V. Verguilov 30 January 2009

#include "VmeScalerData.hh"

#include <iostream>
//#include <fstream>

using namespace std;

VmeScalerData::VmeScalerData(){
	miceMemory.addNew( Memory::VmeScalerData );
	data = -99;
}

VmeScalerData::VmeScalerData( int crate,  int board, std::string type, int chan, int d ):
VmeBaseHit( crate, board, type, chan ), data( d )
{
	miceMemory.addNew( Memory::VmeScalerData );
}

int VmeScalerData::GetData(){
		return data;
}

void VmeScalerData::Print(){
	cout << "|= Scaler Data ================ " << endl;
	cout << "|  Channel : " << channel() << endl;
	cout << "|  Data : " << data << endl;

}

