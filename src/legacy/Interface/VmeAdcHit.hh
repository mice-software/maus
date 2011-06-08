// VmeAdcHit.hh
//
// A class describing a hit on an ADC channel in a VME module
//
// A. Fish - 12th September 2005

#ifndef INTERFACE_VMEADCHIT_H
#define INTERFACE_VMEADCHIT_H

#include "VmeBaseHit.hh"
#include "Memory.hh" 

class VmeAdcHit : public VmeBaseHit
{
  public :

   VmeAdcHit()			{ miceMemory.addNew( Memory::VmeAdcHit ); };

   VmeAdcHit( int, int, std::string, int, int );
   
   ~VmeAdcHit() 		{ miceMemory.addDelete( Memory::VmeAdcHit ); };
   
   int		adc() const	{ return m_adc; };

  private :

     int	m_adc;
};

#endif

