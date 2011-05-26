// VmeTdcHit.hh
//
// A class describing a hit on an TDC channel in a VME module
//
// A. Fish - 12th September 2005

#ifndef INTERFACE_VMETDCHIT_H
#define INTERFACE_VMETDCHIT_H

#include "Interface/VmeBaseHit.hh"

#include "Interface/Memory.hh"

class VmeTdcHit : public VmeBaseHit
{
  public :

  VmeTdcHit()			{ miceMemory.addNew( Memory::VmeTdcHit ); };

  ~VmeTdcHit() 			{ miceMemory.addDelete( Memory::VmeTdcHit ); };

  VmeTdcHit( int, int, std::string, int, int, int );
  VmeTdcHit( int, int, std::string, int, int );
   int		tdc() const	{ return m_Ltdc; };
   int		TrailingTdc() const	{ return m_Ttdc; };
   void     Print();
   void SetTriggerTimeTag( int TTT ){ m_TTT = TTT; };
   int GetTriggerTimeTag(){ return m_TTT; };
   void SetBunchId( int BunchId  ){ m_BunchId = BunchId; };
   int GetBunchId(){ return m_BunchId; };
 
  private :

     int	m_Ltdc;
     int	m_Ttdc;
     int  m_TTT;
		 int  m_BunchId;
};

#endif

