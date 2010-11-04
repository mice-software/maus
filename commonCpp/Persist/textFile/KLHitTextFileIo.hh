// KLHit.hh
//


#ifndef PERSIST_KLHitTEXTFILEIO_H
#define PERSIST_KLHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/KLHit.hh"
#include <vector>
#include "Interface/Memory.hh" 

class MCParticleTextFileIo;

class KLHitTextFileIo : public TextFileIoBase
{
  public :

    KLHitTextFileIo( KLHit*, int );
    KLHitTextFileIo( std::string );

    virtual ~KLHitTextFileIo() { miceMemory.addDelete( Memory::KLHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    KLHit*		theHit() const { return m_hit; };

    KLHit*		makeKLHit();

    void		completeRestoration();

    void    setMCParticles(std::vector<MCParticleTextFileIo*>& );


  private :

    KLHit*   m_hit;

    // ***** variables go here ******
    double      _edep;
    Hep3Vector  _position;
    int         _copyNumber;
    int         _cellNumber;
    int         _layerNumber;
    int         _trackID;
    int         _pid;
    double      _time;
    Hep3Vector  _mom;
    double      _energy;
  MCParticle*	m_particle;

  int m_particle_index;

  double _charge;
  double _mass;

  std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif

