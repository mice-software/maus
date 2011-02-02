// SciFiExtrap.hh
//
// Class to hold an extrapolation for later use (analysis, event display, etc)
//
// M.Ellis - 17th September 2005

#ifndef RECON_SCIFIEXTRAP_H
#define RECON_SCIFIEXTRAP_H 1

#include "SciFiKalTrack.hh"

#include "Interface/Memory.hh" 

class SciFiExtrap
{
  public :

    SciFiExtrap()				{};

    SciFiExtrap( SciFiKalTrack*, double );

    ~SciFiExtrap()		{ miceMemory.addDelete( Memory::SciFiExtrap ); };

    double		x() const		{ return m_pos.x(); };

    double		y() const		{ return m_pos.y(); };

    double		z() const		{ return m_pos.z(); };

    Hep3Vector		pos() const		{ return m_pos; };

    SciFiKalTrack*	track() const		{ return m_track; };

    double		p() const		{ return m_mom.mag(); };

    double		pt() const		{ return sqrt( m_mom.x() * m_mom.x() + m_mom.y() * m_mom.y() ); };

    double		px() const		{ return m_mom.x(); };

    double		py() const		{ return m_mom.y(); };

    double		pz() const		{ return m_mom.z(); };

  private :

     Hep3Vector		m_pos;
     Hep3Vector 	m_mom;
     SciFiKalTrack*	m_track;
};

#endif

