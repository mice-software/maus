// CkovHitTextfileIo.hh
//
// M.Ellis 16/8/2006


#ifndef PERSIST_CKOVHITTEXTFILEIO_H
#define PERSIST_CKOVHITTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/CkovHit.hh"
#include "Interface/Memory.hh"
#include "Interface/MICEEvent.hh"

class MCParticleTextFileIo;

class CkovHitTextFileIo : public TextFileIoBase
{
  public :

    CkovHitTextFileIo( CkovHit*, int );
    CkovHitTextFileIo( std::string );

    virtual ~CkovHitTextFileIo() { miceMemory.addDelete( Memory::CkovHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    CkovHit*		theHit() const { return m_hit; };

    CkovHit*		makeCkovHit();

    void		setMCParticles(std::vector<MCParticleTextFileIo*>& );


    void		completeRestoration();

  private :

    CkovHit*		m_hit;

    int         	trackID;
    double      	charge;
    int         	pid;
    double      	mass;
    double      	edep;
    Hep3Vector  	pos;
    double      	time;
    Hep3Vector  	mom;
    double      	energy;

    int         	planeNo;
	 int         	pmtNo;
	 double      	wavelenght;
	 double      	Iangle;
	 double      	Pangle;
    Hep3Vector  	polarization;

    std::string		volumeName;
    MCParticle*	m_particle;

    int m_particle_index;

    std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif

