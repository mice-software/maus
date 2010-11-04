// VirtualHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_VirtualHitTEXTFIILEIO_H
#define PERSIST_VirtualHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/VirtualHit.hh"

#include "Interface/Memory.hh"

class MCParticleTextFileIo;

class VirtualHitTextFileIo : public TextFileIoBase
{
  public :

    VirtualHitTextFileIo( VirtualHit*, int );
    VirtualHitTextFileIo( std::string );

    virtual ~VirtualHitTextFileIo() { miceMemory.addDelete( Memory::VirtualHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VirtualHit*		theHit() const { return m_hit; };

    VirtualHit*		makeVirtualHit();

    void                setMCParticles(std::vector<MCParticleTextFileIo*>& );


    void		completeRestoration();

  private :

    VirtualHit*		m_hit;

    int trackID;
    int stationNumber;
    Hep3Vector pos;
    Hep3Vector momentum;
    double time;
    double energy;
    double edep;
    int pid;
    double mass;
    double charge;
    Hep3Vector bfield;
    Hep3Vector efield;
  MCParticle*	m_particle;

  int m_particle_index;

  std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif

