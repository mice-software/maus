// MCHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_MCHITTEXTFIILEIO_H
#define PERSIST_MCHITTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/MCHit.hh"

#include "CLHEP/Vector/ThreeVector.h"

class MCParticleTextFileIo;

class MCHitTextFileIo : public TextFileIoBase
{
  public :

    MCHitTextFileIo( MCHit*, int );
    MCHitTextFileIo( std::string );

    virtual ~MCHitTextFileIo() {};

    std::string		storeObject();
    void		restoreObject( std::string );

    MCHit*		theHit() const { return m_hit; };

    MCHit*		makeMCHit();

    void		completeRestoration();
  
    void                setMCParticles(std::vector<MCParticleTextFileIo*>& );

  private :

  MCHit*		m_hit;

  int		m_trackID;
  int		m_pdg;
  double	m_charge;
  double	m_mass;
  Hep3Vector	m_position;
  Hep3Vector	m_momentum;
  double	m_time;
  double	m_edep;
  double	m_energy;
  MCParticle*	m_particle;

  int m_particle_index;

  std::vector<MCParticleTextFileIo*>* m_particles;

};

#endif

