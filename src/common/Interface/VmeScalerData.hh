// VmeScalerData.hh
//
// VME Scaler (CAEN V830) data container class
// EqId is 111
//
// V. Verguilov 30 January 2009

#ifndef INTERFACE_VMESCALERDATA_H
#define INTERFACE_VMESCALERDATA_H

//Includes
#include "Interface/VmeBaseHit.hh"
#include <iostream>
#include "Interface/Memory.hh"

//Interface

class VmeScalerData : public VmeBaseHit {
	public:
		
		VmeScalerData ();
		virtual ~VmeScalerData () { miceMemory.addDelete( Memory::VmeScalerData ); }
		
		VmeScalerData( int crate,  int board, std::string type, int chan, int d );
		
		int GetData();
		void SetData( int aData ){ data = aData; };
		void Print();
	
	private:
		int data;
};

#endif

