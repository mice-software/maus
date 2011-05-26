// VlpcEventTime.hh
//
// The time the event was read out from the DAQ
//
// M.Ellis 4/10/2005

#ifndef VLPCEVENTTIME_H
#define VLPCEVENTTIME_H

#include <vector>

#include "Interface/Memory.hh"

class VlpcEventTime
{
public :

  VlpcEventTime()		{ miceMemory.addNew( Memory::VlpcEventTime ); };

  VlpcEventTime( int t1, int t2 )		{ miceMemory.addNew( Memory::VlpcEventTime ); m_time = (double) t1 + (double) t2 / 1e6; };

  VlpcEventTime(double time)                    { miceMemory.addNew( Memory::VlpcEventTime ); m_time = time; };

  ~VlpcEventTime() 		{ miceMemory.addDelete( Memory::VlpcEventTime ); };

  double	time() const			{ return m_time; };

private :

  double	m_time;
};

#endif
