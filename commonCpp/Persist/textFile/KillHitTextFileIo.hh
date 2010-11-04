#ifndef PERSIST_KILLHITTEXTFIILEIO_H
#define PERSIST_KILLHITTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/KillHit.hh"

#include "CLHEP/Vector/ThreeVector.h"

class KillParticleTextFileIo;

class KillHitTextFileIo : public TextFileIoBase
{
  public :

    KillHitTextFileIo( KillHit*, int );
    KillHitTextFileIo( std::string );

    virtual ~KillHitTextFileIo() {};

    std::string		storeObject();
    void		restoreObject( std::string );

    KillHit*		theHit() const { return m_hit; };

    KillHit*		makeKillHit();

    void		completeRestoration() {;}
  
  private :

  KillHit*		m_hit;

  int		m_trackID;
  int		m_pdg;
  double	m_charge;
  double	m_mass;
  Hep3Vector	m_position;
  Hep3Vector	m_momentum;
  double	m_time;
  double	m_energy;
  std::string   m_reason;
};

#endif

