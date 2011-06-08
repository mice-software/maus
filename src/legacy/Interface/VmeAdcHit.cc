// VmeAdcHit.cc
//
// A class describing a hit on an ADC channel in a VME module
//
// A. Fish - 12th September 2005

#include "VmeAdcHit.hh"

VmeAdcHit::VmeAdcHit( int crate, int board, std::string type, int chan, int adc ) : 
                    VmeBaseHit( crate, board, type, chan )
{
  miceMemory.addNew( Memory::VmeAdcHit );
  m_adc = adc;
}
