// VmefAdcHit.hh
//
// A class describing a hit on an fADC channel in a VME module
//
//Y. Karadzhov April 2008

#ifndef INTERFACE_VMEFADCHIT_H
#define INTERFACE_VMEFADCHIT_H

#include "VmeBaseHit.hh"
#include <algorithm>
#include <iostream>
#include <limits.h>

#include "Memory.hh"

enum ChargeEstimationAlgorithm { ceaMinMax = 0, //Simplest algorithm 
				 ceaFractionDescriminatorThreshold = 1, ceaPedMax };

class VmefAdcHit : public VmeBaseHit
{
  public :
    

   VmefAdcHit()			{ miceMemory.addNew( Memory::VmefAdcHit ); };
   ~VmefAdcHit() 		{ miceMemory.addDelete( Memory::VmefAdcHit ); };


   VmefAdcHit( int, int, std::string, int, std::vector<int> );
 

   int	GetPoint(int i) const		{ return data[i]; };
   std::vector<int> GetData() const	{ return data; };
   int  GetArea();
   int  GetPedestal() const		{ return pedestal; };
   void Print();
   int GetCharge( ChargeEstimationAlgorithm Algorithm = ceaPedMax );
   int GetMaxPosition();
   int GetSignalArea(int&);
   int GetPedestalArea(int&);
   	void SetTriggerTimeTag( int TTT ){ ttt = TTT; };
		int GetTriggerTimeTag(){ return ttt; };
  private :
  
   void SetPedestal();
   int ChargeMinMax();
   int ChargePedMax();
   int charge; 
   std::vector<int> data;
	 int ttt; // Trigger time tag
   int pedestal;
};

#endif

