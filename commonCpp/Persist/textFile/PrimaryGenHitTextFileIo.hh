// MCHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_MCHITTEXTFIILEIO_H
#define PERSIST_MCHITTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/MCHit.hh"

#include "CLHEP/Vector/ThreeVector.h"

class MCParticleTextFileIo;

class PrimaryGenHitTextFileIo : public TextFileIoBase
{
  public :

    PrimaryGenHitTextFileIo( PrimaryGenHit*, int );
    PrimaryGenHitTextFileIo( std::string );

    virtual ~PrimaryGenHitTextFileIo() {};

    std::string		storeObject();
    void		      restoreObject( std::string );

    PrimaryGenHit*		theHit() const { return m_hit; };
    PrimaryGenHit*		makePGHit();

    void		completeRestoration();

  private :

  PrimaryGenHit*		m_hit;

  int		   m_trackID;
  int		   m_pdg;
  double	 m_charge;
  double	 m_mass;
  Hep3Vector	m_position;
  Hep3Vector	m_momentum;
  double	 m_time;
  double	 m_edep;
  double	 m_energy;
  double   m_weight;
  long int m_seed;
  int      m_eventID;
   

  int m_particle_index;

  std::vector<MCParticleTextFileIo*>* m_particles;

};

#endif

