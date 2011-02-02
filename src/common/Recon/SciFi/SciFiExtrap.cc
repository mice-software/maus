// SciFiExtrap.cc
//
// Class to hold an extrapolation for later use (analysis, event display, etc)
//
// M.Ellis - 17th September 2005

#include "SciFiExtrap.hh"
#include "SciFiKalTrack.hh"

SciFiExtrap::SciFiExtrap( SciFiKalTrack* trk , double z ) : m_track( trk )
{
   miceMemory.addNew( Memory::SciFiExtrap );
	
   //m_pos = m_track->positionAtZ( z );
   //m_mom = m_track->momentumAtZ( z );
}

