// SciFiHitTextfileIo.hh
//
// M.Ellis 22/8/2005


#ifndef PERSIST_SCIFIHITTEXTFILEIO_H
#define PERSIST_SCIFIHITTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/SciFiHit.hh"
#include "Interface/Memory.hh"

class MCParticleTextFileIo;

class SciFiHitTextFileIo : public TextFileIoBase
{
  public :

    SciFiHitTextFileIo( SciFiHit*, int );
    SciFiHitTextFileIo( std::string );

    virtual ~SciFiHitTextFileIo() { miceMemory.addDelete( Memory::SciFiHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiHit*		theHit() const { return m_hit; };

    SciFiHit*		makeSciFiHit();

    void                setMCParticles(std::vector<MCParticleTextFileIo*>& );


    void		completeRestoration();

  private :

    SciFiHit*		m_hit;

    int         	trackID;
    double      	charge;
    int         	pid;
    double      	mass;
    double      	edep;
    Hep3Vector  	pos;
    double      	time;
    Hep3Vector  	mom;
    double      	energy;
    int         	trackerNo;
    int         	stationNo;
    int         	planeNo;
    int         	fiberNo;
    std::string		volumeName;
    MCParticle*		m_particle;

  int m_particle_index;

  std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif

